#pragma once

#include <deque>
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

            template <typename T>
            custom( const T &t ) : std::string()
            {
                std::stringstream ss;
                if( ss << t )
                    this->assign( ss.str() );
            }

            template<typename T>
            T as() const
            {
                T t;
                std::stringstream ss;
                ss << *this;
                if( ss >> t )
                    return t;
                return T();
            }

            std::deque<custom> split( char separator ) const
            {
                std::deque<custom> cs;

                cs.push_back( custom() );

                for( auto it = this->begin(), end = this->end(); it != end; ++it )
                {
                    if( *it == separator )
                    {
                        cs.push_back( custom() );
                        ++it;
                    }
                    else
                    {
                        cs.back() += *it;
                    }
                }

                return cs;
            }
        };
    }
}
