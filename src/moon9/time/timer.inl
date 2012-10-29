#pragma once

#include <sstream>
#include <string>

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
            custom( const T& t ) : std::string()
            {
                std::stringstream ss;
                if( ss << t )
                    this->assign( ss.str() );
            }

            template<typename T>
            T as() const
            {
                std::stringstream ss;
                T t;
                return ss >> t ? t : T();
            }
        };
    }
}
