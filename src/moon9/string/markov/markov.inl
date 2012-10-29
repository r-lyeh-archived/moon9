#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

namespace moon9
{
    namespace
    {
        std::string strip( const std::string &text, char stripped )
        {
            const char *begin = text.c_str();
            const char *end = begin + text.size() - 1;

            const char *bpos = begin;
            const char *epos = end;

            while( epos > begin && *epos == stripped )
                epos--;

            while( bpos < end && *bpos == stripped )
                bpos++;

            return text.substr( bpos - begin, epos - bpos + 1 );
        }

        std::string lowercase( std::string s )
        {
            std::transform( s.begin(), s.end(), s.begin(), (int(*)(int)) std::toupper );

            return s;
        }

        std::string uppercase( std::string s )
        {
            std::transform( s.begin(), s.end(), s.begin(), (int(*)(int)) std::toupper );

            return s;
        }

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
    }
}
