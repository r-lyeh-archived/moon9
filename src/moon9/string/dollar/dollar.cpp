// Simple string interpolation and string casting macros. MIT licensed.
// - rlyeh

#include "string/string.hpp"

#include <vector>
#include <map>
#include <string>

namespace moon9
{
    std::map< std::string, moon9::string > map;

    moon9::string &locate( const moon9::string &text )
    {
        return ( map[ text ] = map[ text ] );
    }

    moon9::string translate( const moon9::string &dollartext, const moon9::string &parent )
    {
        moon9::string out, id;

        for( auto &it : dollartext )
        {
            if( id.size() )
            {
                if( it >= 'a' && it <= 'z' )
                    id += it;
                else
                if( it >= 'A' && it <= 'Z' )
                    id += it;
                else
                if( it >= '0' && it <= '9' )
                    id += it;
                else
                if( it == '-' || it == '_' )
                    id += it;
                else
                {
                    if( map.find(id) != map.end() && id != parent )
                        out << translate(map.find(id)->second, id) << it;
                    else
                        out << id << it;

                    id = std::string();
                }
            }
            else
            {
                if( it == '$' )
                    id += it;
                else
                    out << it;
            }
        }

        if( id.size() )
        {
            if( map.find(id) != map.end() && id != parent )
                out << translate(map.find(id)->second, id);
            else
                out << id;
        }

        return out;
    }

    std::vector< std::string > extract( const moon9::string &dollartext, char sep0, char sep1 )
    {
        std::string id;
        std::vector< std::string > out;

        for( auto &it : dollartext )
        {
            if( id.size() )
            {
                if( it >= 'a' && it <= 'z' )
                    id += it;
                else
                if( it >= 'A' && it <= 'Z' )
                    id += it;
                else
                if( it >= '0' && it <= '9' )
                    id += it;
                else
                if( it == '-' || it == '_' )
                    id += it;
                else
                {
                    out.push_back( id );

                    id = std::string();
                }
            }
            else
            {
                if( it == sep0 || it == sep1 )
                    id += it;
            }
        }

        if( id.size() )
            out.push_back( id );

        return out;
    }
}
