// Simple any-key/value map. MIT licensed.
// - rlyeh

#pragma once

#include <map>
#include <functional>
#include <string>

namespace moon9
{
    template<typename VALUE>
    class nmap : public std::map< size_t, VALUE >
    {
        public:

        nmap()
        {}

        template<typename T>
        VALUE &operator []( const T &t )
        {
            size_t hash = std::hash<T>()(t);
            std::map< size_t, VALUE > &map = *this;
            return ( map[ hash ] = map[ hash ] );
        }

        VALUE &operator []( const char *t )
        {
            size_t hash = std::hash<std::string>()(t);
            std::map< size_t, VALUE > &map = *this;
            return ( map[ hash ] = map[ hash ] );
        }

        template<typename T>
        typename std::map< size_t, VALUE >::iterator find( const T &t )
        {
            size_t hash = std::hash<T>()(t);
            std::map< size_t, VALUE > &map = *this;
            return map.find( hash );
        }

        typename std::map< size_t, VALUE >::iterator find( const char *t )
        {
            size_t hash = std::hash<std::string>()(t);
            std::map< size_t, VALUE > &map = *this;
            return map.find( hash );
        }
    };
}
