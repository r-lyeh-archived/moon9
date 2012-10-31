#pragma once

#include <string>
#include <sstream>

namespace moon9
{
    namespace
    {
        class custom : public std::string
        {
            public:

            custom() : std::string()
            {}

            template<typename T>
            custom( const T &t ) : std::string()
            {
                std::stringstream ss;
                ss.precision( 20 );
                if( ss << t )
                    this->assign( ss.str() );
            }

            template<>
            custom( const bool &boolean ) : std::string( boolean ? "true" : "false" )
            {}

            custom( const void *address ) : std::string()
            {
                std::stringstream ss;
                if( ss << address )
                    this->assign( ss.str() );
            }
        };
    }
}
