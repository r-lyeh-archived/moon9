/*
 * C-ish formatted std::strings
 * - rlyeh
 */

// @todo: is format::list() broken? seems so in moon9/io/xml/query or i'm unable to get it to work :s

/*
 * - according amd 64bit guide
     printf("%"PRIi64"\n", myint64 ); --> inttypes.h
     printf("%z", size_t );
     printf("%p", pointer );

     http://en.wikipedia.org/wiki/Printf :
        hh  For integer types, causes printf to expect an int sized integer argument which was promoted from a char.
        h   For integer types, causes printf to expect a int sized integer argument which was promoted from a short.
        l   For integer types, causes printf to expect a long sized integer argument.
        ll  For integer types, causes printf to expect a long long sized integer argument.
        L   For floating point types, causes printf to expect a long double argument.
        z   For integer types, causes printf to expect a size_t sized integer argument.
        j   For integer types, causes printf to expect a intmax_t sized integer argument.
        t   For integer types, causes printf to expect a ptrdiff_t sized integer argument.
 */

#pragma once

#include <cstdarg>
#include <cstdio>
#include <string>

#pragma warning(push)
#pragma warning(disable:4996)

#if 0

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#if defined(_MSC_VER)
//#   include <msinttypes/inttypes.h>
#else
//#   include <inttypes.h>
#endif

#if defined(_MSC_VER) && _MSC_VER < 1700
#   include <moon9/deps/msinttypes/stdint.h>
#else
#   include <stdint.h>
#endif

#endif

namespace moon9
{
    // @todo try this
    // std::cout << "hello " << decimate(1,4,pi) << " worlds"; // hello 3.1415 worlds
    // or...
    // std::cout << "hello " << f1_4(pi) << " worlds"; // hello 3.1415 worlds
    // ie,
    // std::string f1_2( const number &n ) { return moon9::format("%1.2f", n); }
    // std::string fn20( const char *str ) { return moon9::format("%-20.*s", str); }
    // etc...

    class format : public std::string
    {
        public:

            static std::string list( const char *fmt, va_list args )
            {
                const int defaultBufferSize = 256;

                char *buffer = 0;
                int   result, size = defaultBufferSize;

                // try stack first (saves heap allocation/deallocation)
                {
                    char stackBuffer[ defaultBufferSize ];
                    {
                    using namespace std;
                    result = vsnprintf( stackBuffer, defaultBufferSize, fmt, args );
                    }
                    if (result > -1 && result < size)
                    {
                        return std::string(stackBuffer);
                    }
                }

                /* Else try again with more space. */
                if (result > -1)    /* glibc 2.1 */
                    size = result+1; /* precisely what is needed */
                else                /* glibc 2.0 */
                    size *= 2;       /* twice the old size */

                // try heap now
                do
                {
                    if( buffer )
                        delete [] buffer;

                    buffer = new char [ size /* *= 2 */ ];
                    //std::memset(buffer, 0, size);	//useful?
                    {
                    using namespace std;
                    result = vsnprintf(buffer, size, fmt, args);
                    }


                    /* If that worked, return the string. */
                    if (result > -1 && result < size)
                    {
                        std::string s(buffer);
                        delete [] buffer;
                        return s;
                    }

                    /* Else try again with more space. */
                    if (result > -1)    /* glibc 2.1 */
                        size = result+1; /* precisely what is needed */
                    else                /* glibc 2.0 */
                        //size *= result;  /* twice the old size */
                        size *= 2;

                }
                while( size <= 1024*1024 );

                std::string s(buffer);
                delete [] buffer;
                return s;
            }

            /*
            format()
            {}

            format( const std::string &s ) : std::string( s )
            {}
            */

            explicit format( const char *fmt, ... ) : std::string()
            {
                if( fmt )
                {
                    va_list args;
                    va_start( args, fmt );

                    this->assign( format::list( fmt, args ) );

                    va_end( args );
                }
            }

        private:

            // This constructor is intentionally private to prevent potential bugs

            format( const char * );

            /*
            // Copy-constructor
            format( const format & );
            // Copy-assignment
            format& operator=( const format & );
            */
    };
}


#pragma warning(pop)


#if 0

// is this impl faster? is my implementation less safe?

// http://tdistler.com/2010/01/11/cc-writing-printf-style-functions

#include <cstdarg>

#ifdef _WIN32
#define kBooVSNPrintf _vsnprintf
#else
#define kBooVSNPrintf  vsnprintf
#endif

// The function
explicit format( const char* fmt ... )
{
    // Parse the argument list
    va_list args;
    va_start( args, format );

    // Calculate the final length of the formatted string
    //va_list args_copy;
    //va_copy( args_copy, args );
    int len = kBooVSNPrintf( 0, 0, fmt, args );
    //va_end( args_copy );

    // Allocate a buffer (including room for null termination)
    char* target_string = new char[++len];

    // Generate the formatted string
    kBooVSNPrintf( target_string, len, fmt, args );

    // <Do something with the formatted string>
    this->assign( target_string );

    // Clean up
    delete [] target_buffer;
    va_end( args );
}

#undef kBooVSNPrintf

#endif
