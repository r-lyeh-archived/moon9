// http://stuff.mit.edu/afs/sipb/project/python-lib/src/wxPython-src-2.5.3.1/src/common/dynlib.cpp
// http://www.marcosgomezplaton.com/?p=811

// win
// TEXT("kernel32.dll"), TEXT("lstrlenA")

// unix
// _T("/lib/libc-2.0.7.so"), _T("strlen")

#pragma once

//#include <moon9/os/directives.hpp>

#if defined(_WIN32)

#   ifndef NOMINMAX
#       define NOMINMAX
#   endif
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif
#   include <winsock2.h>

#   include <DbgHelp.h>
#   pragma comment(lib,"dbghelp.lib")

#   define kMoon9DllHandle(x)                     HMODULE x
#   define kMoon9DllOpenCurrent(x)                { kMoon9DllHandle(this_process); GetModuleHandleEx(0,0,&this_process); x = GetModuleHandle(0); }
#   define kMoon9DllOpen(x,lib)                   x = LoadLibraryA( lib )
#   define kMoon9DllHasLoaded(x)                  ( x != NULL )
#   define kMoon9DllClose(x)                      FreeLibrary(x)

#   define kMoon9SymHandle(x)                     FARPROC x
#   define kMoon9SymLoad(x, lib, fn)              proc = GetProcAddress( lib, fn )
#   define kMoon9SymHasLoaded(x)                  ( x != NULL )
#   define kMoon9SymMakeFunc(FN,PROC,RET,...)     typedef RET (*function_type)(__VA_ARGS__); \
                                                  function_type FN = (function_type) PROC;
#else

#   include <dlfcn.h>
#   include <stdio.h>

#   define kMoon9DllHandle(x)                     void * x
#   define kMoon9DllOpenCurrent(x)                x = dlopen(NULL,0)
#   define kMoon9DllOpen(x,lib)                   x = dlopen( lib, RTLD_LAZY )
#   define kMoon9DllHasLoaded(x)                  ( x != NULL )
#   define kMoon9DllClose(x)                      dlclose(x)

#   define kMoon9SymHandle(x)                     void * x
#   define kMoon9SymLoad(x, lib, fn)              proc = dlsym( lib, fn )
#   define kMoon9SymHasLoaded(x)                  ( x != NULL )
#   define kMoon9SymMakeFunc(FN,PROC,RET,...)     typedef RET (*function_type)(__VA_ARGS__); \
                                                  /*function_type init_func = *((function_type) &proc); */ \
                                                  union { function_type fn; void * obj; } alias; \
                                                  alias.obj = PROC; \
                                                  function_type FN = alias.fn;

#endif

#include <map>
#include <string>

namespace moon9
{

    class dll
    {
        bool ok;

        kMoon9DllHandle( handle );
        kMoon9SymHandle( proc );

    public:

        dll( const char *fn_name ) : ok(false)
        {
#if defined(WIN32) || defined(_WIN32)

            SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);

            HANDLE hProcess = GetCurrentProcess();

            SymInitialize(hProcess, NULL, TRUE);

            char szSymbolName[MAX_SYM_NAME];
            ULONG64 buffer[(sizeof(SYMBOL_INFO) +
                MAX_SYM_NAME * sizeof(TCHAR) +
                sizeof(ULONG64) - 1) /
                sizeof(ULONG64)];
            PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;

            strcpy(szSymbolName, fn_name ); //TEXT("strlen"));
            pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            pSymbol->MaxNameLen = MAX_SYM_NAME;

            if (SymFromName(hProcess, szSymbolName, pSymbol))
            {
                // SymFromName returned success
                proc = (FARPROC) pSymbol->Address;
                ok = true;
            }

            return;

#else

            static void *rtldDefault= 0;

            if( !rtldDefault )
                rtldDefault = dlopen(0, RTLD_NOW | RTLD_GLOBAL);

            proc = dlsym(rtldDefault, fn_name);

            if( proc )
                ok = true;

            return;

#endif

            kMoon9DllOpenCurrent( handle );

            if( kMoon9DllHasLoaded( handle ) )
            {
                kMoon9SymLoad( proc, handle, fn_name );

                if( kMoon9SymHasLoaded( proc ) )
                {
                    ok = true;
                }
            }
        }

        dll( const char *module, const char *fn_name ) : ok(false)
        {
            kMoon9DllOpen( handle, module );

            if( kMoon9DllHasLoaded( handle ) )
            {
                kMoon9SymLoad( proc, handle, fn_name );

                if( kMoon9SymHasLoaded( proc ) )
                {
                    ok = true;
                }
            }
        }

        operator const bool () const
        {
            return ok;
        }

        ~dll()
        {
            if( kMoon9DllHasLoaded( handle ) )
            {
                kMoon9DllClose( handle );
            }
        }

        template< typename RET >
        RET call()
        {
            kMoon9SymMakeFunc(func,proc,RET,void);
            return (*func)();
        }

        template< typename RET, typename T0 >
        RET call( T0 arg0 )
        {
            kMoon9SymMakeFunc(func,proc,RET,T0);
            return (*func)( arg0 );
        }

        template< typename RET, typename T0, typename T1 >
        RET call( T0 arg0, T1 arg1 )
        {
            kMoon9SymMakeFunc(func,proc,RET,T0,T1);
            return (*func)( arg0, arg1 );
        }

    };

    // following class is not thread safe!

    class sym
    {
        void *proc;

    public:

        sym() : proc(0)
        {}

        explicit
        sym( const std::string &fn_name )
        {
            static std::map< std::string, void * > map;
            std::map< std::string, void * >::iterator it = map.find( fn_name );

            if( it != map.end() )
            {
                proc = it->second;
                return;
            }

#if defined(WIN32) || defined(_WIN32)

            HANDLE hProcess = GetCurrentProcess();

            SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
            SymInitialize(hProcess, NULL, TRUE);

            const int size = (sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR) + sizeof(ULONG64) - 1) / sizeof(ULONG64);
            ULONG64 buffer[ size ];
            PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;

            pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            pSymbol->MaxNameLen = MAX_SYM_NAME;

            if (SymFromName(hProcess, fn_name.c_str(), pSymbol))
            {
                // SymFromName returned success
                map[ fn_name ] = ( proc = (void *)pSymbol->Address );
            }
            else
            {
                proc = 0;
            }

#else

            static void *rtldDefault= 0;

            if( !rtldDefault )
                rtldDefault = dlopen( 0, RTLD_NOW | RTLD_GLOBAL );

	   proc = ::dlsym( rtldDefault, fn_name.c_str() );

            if( proc )
                map[ fn_name ] = proc;

#endif
        }

        bool exist() const
        {
            return proc != 0;
        }

        operator const bool() const
        {
            return exist();
        }

        template< typename RET >
        inline RET call()
        {
            kMoon9SymMakeFunc(func,proc,RET,void);
            return (*func)();
        }

        template< typename RET, typename T0 >
        inline RET call( T0 &arg0 )
        {
            kMoon9SymMakeFunc(func,proc,RET,T0);
            return (*func)( arg0 );
        }

        template< typename RET, typename T0, typename T1 >
        inline RET call( T0 &arg0, T1 &arg1 )
        {
            kMoon9SymMakeFunc(func,proc,RET,T0,T1);
            return (*func)( arg0, arg1 );
        }

        template< typename RET, typename T0, typename T1, typename T2 >
        inline RET call( T0 &arg0, T1 &arg1, T2 &arg2 )
        {
            kMoon9SymMakeFunc(func,proc,RET,T0,T1,T2);
            return (*func)( arg0, arg1, arg2 );
        }

        template< typename RET, typename T0, typename T1, typename T2, typename T3 >
        inline RET call( T0 &arg0, T1 &arg1, T2 &arg2, T3 &arg3 )
        {
            kMoon9SymMakeFunc(func,proc,RET,T0,T1,T2,T3);
            return (*func)( arg0, arg1, arg2, arg3 );
        }

        template< typename RET >
        inline RET operator()()
        {
            kMoon9SymMakeFunc(func,proc,RET,void);
            return (*func)();
        }

        template< typename RET, typename T0 >
        inline RET operator()( T0 &arg0 )
        {
            kMoon9SymMakeFunc(func,proc,RET,T0);
            return (*func)( arg0 );
        }

        template< typename RET, typename T0, typename T1 >
        inline RET operator()( T0 &arg0, T1 &arg1 )
        {
            kMoon9SymMakeFunc(func,proc,RET,T0,T1);
            return (*func)( arg0, arg1 );
        }

        template< typename RET, typename T0, typename T1, typename T2 >
        inline RET operator()( T0 &arg0, T1 &arg1, T2 &arg2 )
        {
            kMoon9SymMakeFunc(func,proc,RET,T0,T1,T2);
            return (*func)( arg0, arg1, arg2 );
        }

        template< typename RET, typename T0, typename T1, typename T2, typename T3 >
        inline RET operator()( T0 &arg0, T1 &arg1, T2 &arg2, T3 &arg3 )
        {
            kMoon9SymMakeFunc(func,proc,RET,T0,T1,T2,T3);
            return (*func)( arg0, arg1, arg2, arg3 );
        }
    };
}

#undef kMoon9DllHandle
#undef kMoon9DllOpenCurrent
#undef kMoon9DllOpen
#undef kMoon9DllHasLoaded
#undef kMoon9DllClose

#undef kMoon9SymHandle
#undef kMoon9SymLoad
#undef kMoon9SymHasLoaded
#undef kMoon9SymMakeFunc

