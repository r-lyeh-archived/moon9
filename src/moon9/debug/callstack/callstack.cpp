/* Simple callstack class. Based on code by Magnus Norddahl (ClanLib)
 * - rlyeh
 */

#ifdef _WIN32

#   include <mutex>
#   include <thread>

#   include <Windows.h>
#   if defined(DEBUG) || defined(_DEBUG)
#       include <crtdbg.h>
#   endif
#   include <dbghelp.h>
#   pragma comment(lib, "dbghelp.lib")

#   ifdef _MSC_VER
#       pragma message( "<moon9/debug/callstack.cpp> says: do not forget /Zi, /Z7 or /C7 compiler settings! /Oy- also helps" )
#   endif

#endif

#ifndef _WIN32

#   include <unistd.h>
#   include <sys/time.h>
#   include <sys/types.h>

#   if defined(HAVE_SYS_SYSCTL_H) && \
        !defined(_SC_NPROCESSORS_ONLN) && !defined(_SC_NPROC_ONLN)
#       include <sys/param.h>
#       include <sys/sysctl.h>
#   endif

#   ifndef __APPLE__
#       include <execinfo.h>
#   endif

#   include <cxxabi.h>
#   include <cstring>
#   include <cstdlib>

#   ifdef __GNUC__
#       warning "<moon9/debug/callstack.cpp> says: do not forget -g -rdynamic compiler settings!"
#   endif

#endif

#include <sstream>
#include <string>
#include <vector>
#include <mutex>

#include "callstack.hpp"

namespace moon9
{
    namespace
    {
        namespace detail
        {
            class string : public std::string
            {
                public:

                string() : std::string()
                {}

                template <typename T>
                explicit string( const T &t ) : std::string()
                {
                    std::stringstream ss;
                    if( ss << t )
                        this->assign( ss.str() );
                }

                template <typename T1, typename T2>
                explicit string( const char *fmt12, const T1 &_t1, const T2 &_t2 ) : std::string()
                {
                    string t1( _t1 );
                    string t2( _t2 );

                    string &s = *this;

                    while( *fmt12 )
                    {
                        if( *fmt12 == '\1' )
                            s += t1;
                        else
                        if( *fmt12 == '\2' )
                            s += t2;
                        else
                            s += *fmt12;

                        fmt12++;
                    }
                }

                size_t count( const std::string &substr ) const
                {
                    size_t n = 0;
                    std::string::size_type pos = 0;
                    while( (pos = this->find( substr, pos )) != std::string::npos ) {
                        n++;
                        pos += substr.size();
                    }
                    return n;
                }
            };

            typedef std::vector<std::string> strings;
        }

        size_t capture_stack_trace(int frames_to_skip, int max_frames, void **out_frames, unsigned int *out_hash = 0)
        {
        #ifdef _WIN32

                if (max_frames > 32)
                        max_frames = 32;

                unsigned short capturedFrames = 0;

                // RtlCaptureStackBackTrace is only available on Windows XP or newer versions of Windows
                typedef WORD(NTAPI FuncRtlCaptureStackBackTrace)(DWORD, DWORD, PVOID *, PDWORD);

                static struct raii
                {
                    raii() : module(0), ptrRtlCaptureStackBackTrace(0)
                    {
                        module = LoadLibraryA("kernel32.dll");
                        if( module )
                            ptrRtlCaptureStackBackTrace = (FuncRtlCaptureStackBackTrace *)GetProcAddress(module, "RtlCaptureStackBackTrace");
                    }
                    ~raii() { if(module) FreeLibrary(module); }

                    HMODULE module;
                    FuncRtlCaptureStackBackTrace *ptrRtlCaptureStackBackTrace;
                } module;

                if (module.ptrRtlCaptureStackBackTrace)
                        capturedFrames = module.ptrRtlCaptureStackBackTrace(frames_to_skip+1, max_frames, out_frames, (DWORD *) out_hash);

                if (capturedFrames == 0 && out_hash)
                        *out_hash = 0;

                return capturedFrames;

        #elif !defined(__APPLE__)
                // Ensure the output is cleared
                memset(out_frames, 0, (sizeof(void *)) * max_frames);

                if (out_hash)
                        *out_hash = 0;

                return (backtrace(out_frames, max_frames));

        #else
                return 0;

        #endif
        }

        detail::strings get_stack_trace(void **frames, int num_frames)
        {
#ifdef _WIN32
            // this mutex is used to prevent race conditions.
            // however, it is constructed with heap based plus placement-new just to meet next features:
            // a) ready to use before program begins.
            // our memtracer uses callstack() and mutex is ready to use before the very first new() is made.
            // b) ready to use after program ends.
            // our memtracer uses callstack() when making the final report after the whole program has finished.
            // c) allocations free: memory is taken from heap, and constructed thru placement new
            // we will avoid recursive deadlocks that would happen in a new()->memtracer->callstack->new()[...] scenario.
            // d) leaks free: zero global allocations are made.
            // we don't polute memmanager/memtracer reports with false positives.
            static std::mutex *mutex = 0;
            if( !mutex )
			{
                static char placement[ sizeof(std::mutex) ];
                mutex = (std::mutex *)placement; // no leak and no memory traced :P
                new (mutex) std::mutex();        // memtraced recursion safe; we don't track placement-news
			}

			mutex->lock();

            BOOL result = SymInitialize(GetCurrentProcess(), NULL, TRUE);
            if (!result)
                return mutex->unlock(), detail::strings();

            detail::strings backtrace_text;
            for( int i = 0; i < num_frames; i++ )
            {
                char buffer[sizeof(IMAGEHLP_SYMBOL64) + 128];
                IMAGEHLP_SYMBOL64 *symbol64 = reinterpret_cast<IMAGEHLP_SYMBOL64*>(buffer);
                memset(symbol64, 0, sizeof(IMAGEHLP_SYMBOL64) + 128);
                symbol64->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
                symbol64->MaxNameLength = 128;

                DWORD64 displacement = 0;
                BOOL result = SymGetSymFromAddr64(GetCurrentProcess(), (DWORD64) frames[i], &displacement, symbol64);
                if (result)
                {
                    IMAGEHLP_LINE64 line64;
                    DWORD displacement = 0;
                    memset(&line64, 0, sizeof(IMAGEHLP_LINE64));
                    line64.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
                    result = SymGetLineFromAddr64(GetCurrentProcess(), (DWORD64) frames[i], &displacement, &line64);
                    if (result)
                    {
                        std::string line = detail::string(symbol64->Name) + " (" + line64.FileName + ", line " + detail::string(line64.LineNumber) + ")";
                        backtrace_text.push_back( line );
                    }
                    else
                    {
                        backtrace_text.push_back(symbol64->Name);
                    }
                }
            }

            SymCleanup(GetCurrentProcess());
            return mutex->unlock(), backtrace_text;

#elif !defined(__APPLE__)

            char **strings;
            strings = backtrace_symbols(frames, num_frames);
            if (!strings)
            {
                return detail::strings();
            }

            detail::strings backtrace_text;

            for( int cnt = 0; cnt < num_frames; cnt++ )
            {
                // Decode the strings
                char *ptr = strings[cnt];
                char *filename = ptr;
                const char *function = "";

                // Find function name
                while(*ptr)
                {
                    if (*ptr=='(')  // Found function name
                    {
                        *(ptr++) = 0;
                        function = ptr;
                        break;
                    }
                    ptr++;
                }

                // Find offset
                if (function[0])        // Only if function was found
                {
                    while(*ptr)
                    {
                        if (*ptr=='+')  // Found function offset
                        {
                            *(ptr++) = 0;
                            break;
                        }
                        if (*ptr==')')  // Not found function offset, but found, end of function
                        {
                            *(ptr++) = 0;
                            break;
                        }
                        ptr++;
                    }
                }

                int status;
                char *new_function = abi::__cxa_demangle(function, 0, 0, &status);
                if (new_function)       // Was correctly decoded
                {
                    function = new_function;
                }

                backtrace_text.push_back( detail::string(function) + " (" + detail::string(filename) + ")" );

                if (new_function)
                {
                    free(new_function);
                }
            }

            free (strings);
            return backtrace_text;

#else

            return detail::strings();

#endif
        }
    }
}

namespace moon9
{
    namespace
    {
        bool is_callstack_available = false;
    }

    callstack::callstack() // save
    {
        // When calling callstack().str() from a secondary thread will retrieve empty stack until a callstack is retrieved from main thread.
        // This happens on my machine (Win7, 64bit, msvc 2011) under some circumstances. So I've added '100 tries' instead of 'try once'.
        // @Todo: examine & fix issue

        static size_t tries = 0;
        if( (!is_callstack_available) && tries < 100 )
        {
            ++tries;

            num_frames = moon9::capture_stack_trace( 0, max_frames, frames );
            for( int i = num_frames; i < max_frames; ++i ) frames[ i ] = 0;

            detail::strings stack_trace = moon9::get_stack_trace( frames, num_frames );
            detail::string out;

            for( size_t i = 0; i < stack_trace.size(); i++ )
                out += stack_trace[i] + '|';

            is_callstack_available = ( out.count("callstack") > 0 );
        }

        if( !is_callstack_available )
            return;

        num_frames = moon9::capture_stack_trace( 1, max_frames, frames );
        for( int i = num_frames; i < max_frames; ++i ) frames[ i ] = 0;
    }

    std::string callstack::str( const char *format12, size_t skip_initial )
    {
        detail::string out;

        if( !is_callstack_available )
            return out;

        detail::strings stack_trace = moon9::get_stack_trace( frames, num_frames );

        for( size_t i = skip_initial; i < stack_trace.size(); i++ )
            out += detail::string( format12, (int)i, stack_trace[i] );

        return out;
    }
}
