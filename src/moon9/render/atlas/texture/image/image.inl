#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace moon9
{
    namespace // taken from <moon9/render/image.cpp>
    {
        std::vector<std::string> tokenize( const std::string &text, const std::string &chars )
        {
            std::string map( 256, '\0' );

            for( auto it = chars.begin(), end = chars.end(); it != end; ++it )
                map[ *it ] = '\1';

            std::vector<std::string> tokens;

            tokens.push_back( std::string() );

            for( int i = 0, end = text.size(); i < end; ++i )
            {
                unsigned char c = text.at(i);

                std::string &str = tokens.back();

                if( !map.at(c) )
                    str.push_back( c );
                else
                if( str.size() )
                    tokens.push_back( std::string() );
            }

            while( tokens.size() && !tokens.back().size() )
                tokens.pop_back();

            return tokens;
        }

        template<typename T>
        T as( const std::string &text )
        {
            T t;
            std::stringstream ss( text );
            return ss >> t ? t : (T)(as<bool>(text));
        }

        template<>
        bool as( const std::string &text )
        {
            return text.size() > 0 && text != "0" && text != "false";
        }

        class custom : public std::string
        {
            public:

            template<typename T>
            explicit
            custom( const T &t ) : std::string()
            {
                std::stringstream ss;
                if( ss << t )
                    this->assign( ss.str() );
            }

            template <typename T1>
            custom( const char *fmt, const T1 &t1 ) : std::string()
            {
                std::string t[] = { std::string(), custom(t1) };
                for( ;*fmt; fmt++ )
                {
                    if( *fmt == '\1' )
                        t[0] += t[1];
                    else
                        t[0] += *fmt;
                }
                this->assign( t[0] );
            }
        };
    }
}
