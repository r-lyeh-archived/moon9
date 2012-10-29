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
            custom( const T &t ) : std::string()
            {
                std::stringstream ss;
                if( ss << t )
                    this->assign( ss.str() );
            }

            std::deque<custom> tokenize( const std::string &chars ) const
            {
                std::string map( 256, '\0' );

                for( auto it = chars.begin(), end = chars.end(); it != end; ++it )
                    map[ *it ] = '\1';

                std::deque<custom> tokens;

                tokens.push_back( custom() );

                for( int i = 0, end = this->size(); i < end; ++i )
                {
                    unsigned char c = at(i);

                    std::string &str = tokens.back();

                    if( !map.at(c) )
                        str.push_back( c );
                    else
                    if( str.size() )
                        tokens.push_back( custom() );
                }

                while( tokens.size() && !tokens.back().size() )
                    tokens.pop_back();

                return tokens;
            }

            template <typename T>
            T as() const
            {
                T t;
                std::stringstream ss;
                ss << *this;
                return ss >> t ? t : T();
            }
        };
    }
}
