#include <cstdlib>
#include <cassert>

#include <stdexcept>

#ifdef _WIN32
#   include <Windows.h>
#   pragma comment(lib,"kernel32.lib")
#endif

#include "errorbox.hpp"

#include <iostream>

bool debugger( const std::string &reason )
{
    if( reason.size() > 0 )
        errorbox( reason );

// os based

    #ifdef _WIN32
    if( IsDebuggerPresent() )
        DebugBreak();
    #endif

    #ifdef __linux__
    raise(SIGTRAP); //POSIX
    #endif

// compiler based

    //msvc
    #ifdef _MSC_VER
    // "With native code, I prefer using int 3 vs DebugBreak, because the int 3 occurs right in the same stack frame and module as the code in question, whereas DebugBreak occurs one level down in another DLL, as shown in this callstack:"
    // [ref] http://blogs.msdn.com/b/calvin_hsia/archive/2006/08/25/724572.aspx
    // __debugbreak();
    #endif

    // gcc
    #ifdef __GNUC__
    __builtin_trap();
    #endif

// standard

    //assert( !"debugger() requested" );
    // still here? duh, maybe we are in release mode...

// host based

    //macosx: asm {trap}            ; Halts a program running on PPC32 or PPC64.
    //macosx: __asm {int 3}         ; Halts a program running on IA-32.

    #ifdef _M_X86
    //__asm int 3;
    #endif

    errorbox( "<moon9/debug/debugger.hpp> says:\n\nDebugger invokation failed.\nPlease attach a debugger now.", "Error!");
    return false;
}
