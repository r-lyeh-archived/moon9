/* Simple factory class
 * - rlyeh
 */

#pragma once

#include <map>
#include <string>

#include "any/any.hpp"

namespace moon9
{
    class factory
    {
        public:

        factory()
        {}

        template <class T> //inscribe, register, store, add
        void inscribe( const std::string &given_name, const T& t )
        {
            map[ given_name ] = map[ given_name ];
            map[ given_name ] = moon9::any( t );
        }

        template <class T> // create, copy, clone
        bool clone( const std::string &name, T& t ) const
        {
            std::map< std::string, moon9::any >::const_iterator it = map.find( name );

            if( it == map.end() )
                return false;

            t = it->second.as<T>();
            return true;
        }

        protected:
        std::map< std::string, moon9::any > map;
    };
}

