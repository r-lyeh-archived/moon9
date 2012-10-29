#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace moon9
{
    namespace
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
    }
}
