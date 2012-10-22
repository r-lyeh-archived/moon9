#include <cstdlib>
#include <string>

#ifdef _WIN32
#   include <Windows.h>
#   pragma comment(lib, "kernel32.lib")
#endif

#include "errorbox.hpp"
#include "callstack.hpp"

void die( const std::string &reason, int errorcode )
{
    if( reason.size() )
    {
        std::string append = moon9::callstack().str();

        if( append.size() )
        {
            fprintf( stderr, "%s\n%s", reason.c_str(), append.c_str() );
            errorbox( reason + '\n' + append, "Error" );
        }
        else
        {
            fprintf( stderr, "%s", reason.c_str() );
            errorbox( reason, "Error" );
        }
    }

#ifdef _WIN32
    FatalExit( errorcode );
#endif

    // standard ; thread friendly?
    std::exit( errorcode );
}

void die( int errorcode )
{
    die( std::string(), errorcode );
}

