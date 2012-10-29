#pragma once

#include <deque>
#include <string>

namespace moon9
{
    namespace
    {
        std::deque<std::string> split( const std::string &str, char sep )
        {
            std::deque<std::string> tokens;

            tokens.push_back( std::string() );

            for( std::string::const_iterator it = str.begin(), end = str.end(); it != end; ++it )
            {
                if( *it == sep )
                {
                    tokens.push_back( std::string() + sep );
                    tokens.push_back( std::string() );
                }
                else
                {
                    tokens.back() += *it;
                }
            }

            return tokens;
        }
    }
}
