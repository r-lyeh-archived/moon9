// Simple multi-any-key/value (N-keys/1-value) map. Key order is commutative. MIT licensed.
// This is a std compatible-ish container in spirit of std::multimap, whereas it handles
// multiple-keys/single-value as opposed to single-key/multiple-values.
// - rlyeh

// /* usage: */
// moon9::multinmap<char> mnm;
// /* direct access */
// mnm["helloworld"][true]['a'][-123][3.1459f][100.00].get() = 'X';
// assert( mnm["helloworld"][true]['a'][-123][3.1459f][100.00].get() == 'X' );
// assert( mnm['a'][true][3.1459f][100.00]["helloworld"][-123].get() == 'X' );
// assert( mnm[true][-123][100.00]['a'][3.1459f]["helloworld"].get() == 'X' );
// /* standard find */
// assert( mnm[true][-123][100.00]['a'][3.1459f]["helloworld"].find() != mnm.end() );
// /* custom find */
// assert( mnm[true][-123][100.00]['a'][3.1459f]["helloworld"].findit() == true );
// assert( mnm[true][-123][100.00]['a'][3.1459f]["helloworld"].found() == 'X' );

#pragma once

#include <map>
#include <set>
#include <functional>
#include <string>
//#include <sstream>

namespace moon9
{
    template<typename VALUE>
    class multinmap : public std::map< size_t, VALUE >
    {
        mutable struct hashes_t : public std::set< size_t >
        {
            hashes_t()
            {}

            size_t retrieve()
            {
                // merge hashes into a final hash
#           if 0
                std::stringstream ss;
                for( auto &it : *this )
                    ss << it;
                this->clear();
                return std::hash<std::string>()(ss.str());
#           else
                size_t hash = 0;
                for( auto &it : *this )
                    hash += it;
                this->clear();
                return hash;
#           endif
            }
        } hashes;

        mutable typename std::map< size_t, VALUE >::iterator cursor;

        public:

        multinmap()
        {
            cursor = this->end();
        }

        // access
        template<typename T>
        multinmap &operator []( const T &t )
        {
            hashes.insert( std::hash<T>()(t) );
            return *this;
        }

        multinmap &operator []( const char *t )
        {
            hashes.insert( std::hash<std::string>()(t) );
            return *this;
        }

        const VALUE &get() const
        {
            size_t hash = hashes.retrieve();
            std::map< size_t, VALUE > &map = *this;

            return ( map[ hash ] = map[ hash ] );
        }
        VALUE &get()
        {
            size_t hash = hashes.retrieve();
            std::map< size_t, VALUE > &map = *this;

            return ( map[ hash ] = map[ hash ] );
        }

        // standard search (c++ way)
        typename std::map< size_t, VALUE >::iterator find()
        {
            size_t hash = hashes.retrieve();
            std::map< size_t, VALUE > &map = *this;

            return map.find( hash );
        }

        // custom search (my way)
        bool findit()
        {
            size_t hash = hashes.retrieve();
            std::map< size_t, VALUE > &map = *this;

            return ( ( cursor = map.find( hash ) ) != map.end() );
        }
        const VALUE &found() const
        {
            return cursor->second;
        }
        VALUE &found()
        {
            return cursor->second;
        }
    };
}
