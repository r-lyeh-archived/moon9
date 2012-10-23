/* C formatted std::strings
 * Based on code by Tom Distler. [ref] http://tdistler.com/2010/01/11/cc-writing-printf-style-functions
 * - rlyeh
 */

#pragma once

#include <stdarg.h>
#include <cstdarg>
#include <string>

#ifdef _MSC_VER
#define kMoon9VSNPrintf _vsnprintf
#else
#define kMoon9VSNPrintf  vsnprintf
#endif

namespace moon9
{
    class format : public std::string
    {
        public:

        format() : std::string()
        {}

        explicit
        format( const char *fmt, ... ) : std::string()
        {
            int len;
            using namespace std;

            // Calculate the final length of the formatted string
            {
                va_list args;
                va_start( args, fmt );
                len = kMoon9VSNPrintf( 0, 0, fmt, args );
                va_end( args );
            }

            // Allocate a buffer (including room for null termination)
            char* target_string = new char[++len];

            // Generate the formatted string
            {
                va_list args;
                va_start( args, fmt );
                kMoon9VSNPrintf( target_string, len, fmt, args );
                va_end( args );
            }

            // Assign the formatted string
            this->assign( target_string );

            // Clean up
            delete [] target_string;
        }

        protected:

        // This constructor is intentionally private to prevent potential bugs
        format( const char * );

        // Copy-constructor
        // format( const format & );
        // Copy-assignment
        // format& operator=( const format & );
    };
}

#undef kMoon9VSNPrintf

/*  @todo: useful?

    static std::string dec( int digits, int decimals, double number, char filler = ' ' ) {
        return format( "%*.*f", digits, decimals, number );
    }

    static std::string hex( int digits, double f, char filler = '0' ) {
    }

    static std::string left( int n, const std::string &arg, char filler = ' ' ) {
        int amount = n - arg.size();
        return amount >= 0 ? arg + std::string( amount, filler ) : std::string();
    }

    static std::string right( int n, const std::string &arg, char filler = ' ' ) {
        int amount = n - arg.size();
        return amount >= 0 ? std::string( amount, filler ) + arg : std::string();
    } */
