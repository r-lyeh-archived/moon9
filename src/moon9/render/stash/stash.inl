#pragma once

#include <deque>
#include <sstream>
#include <string>

namespace moon9
{
    namespace // taken from <moon9/render/image.cpp>
    {
        template<typename T>
        T as( const std::string &text )
        {
            T t;
            std::stringstream ss( text );
            return ss >> t ? t : (T)( as<bool>(text) );
        }

        template<>
        bool as( const std::string &text )
        {
            return text.size() > 0 && text != "0" && text != "false";
        }

        std::deque< std::string > split( const std::string &text, const std::string& delimiters ) // tokenize_incl_separators
        {
            std::deque< std::string > tokens;
            std::string str;

            for( size_t i = 0; i < text.size(); ++i )
            {
                char c = text.at(i);

                if( delimiters.find_first_of( c ) == std::string::npos )
                {
                    str += c;
                }
                else
                {
                    if( str.size() )
                        tokens.push_back( str );

                    tokens.push_back( std::string() + c );

                    str = "";
                }
            }

            tokens.push_back( str );
            return tokens;
        }
    }
}
