#pragma once

#include <deque>
#include <sstream>
#include <string>

namespace moon9
{
    namespace // from <moon9/render/image.cpp>
    {
        template<typename T>
        std::string to_string( const T &t )
        {
            std::stringstream ss;
            return ss << t ? ss.str() : std::string();
        }

        template<typename T1>
        std::string to_string( const char *fmt, const T1 &t1 )
        {
            std::string t[] = { std::string(), to_string(t1) };
            for( ; *fmt ; fmt++ )
            {
                if( *fmt == '\1' )
                    t[0] += t[1];
                else
                    t[0] += *fmt;
            }
            return t[0];
        }

        template<typename T1, typename T2>
        std::string to_string( const char *fmt, const T1 &t1, const T2 &t2 )
        {
            std::string t[] = { std::string(), to_string(t1), to_string(t2) };
            for( ; *fmt ; fmt++ )
            {
                if( *fmt == '\1' )
                    t[0] += t[1];
                else
                if( *fmt == '\2' )
                    t[0] += t[2];
                else
                    t[0] += *fmt;
            }
            return t[0];
        }

        template<typename T1, typename T2, typename T3>
        std::string to_string( const char *fmt, const T1 &t1, const T2 &t2, const T3 &t3 )
        {
            std::string t[] = { std::string(), to_string(t1), to_string(t2), to_string(t3) };
            for( ; *fmt ; fmt++ )
            {
                if( *fmt == '\1' )
                    t[0] += t[1];
                else
                if( *fmt == '\2' )
                    t[0] += t[2];
                else
                if( *fmt == '\3' )
                    t[0] += t[3];
                else
                    t[0] += *fmt;
            }
            return t[0];
        }

        std::deque<std::string> tokenize( const std::string &text, const std::string &chars )
        {
            std::string map( 256, '\0' );

            for( auto it = chars.begin(), end = chars.end(); it != end; ++it )
                map[ *it ] = '\1';

            std::deque<std::string> tokens;

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

        bool starts_with( const std::string &text, const std::string &head )
        {
            return text.substr( 0, head.size() ) == head;
        }
    }
}
