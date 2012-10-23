/*
 * Simple intrusive serialization library

 * Copyright (c) 2011 Mario 'rlyeh' Rodriguez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.

 * Features:
 * - Macroless
 * - XML based

 * To do:
 * - STL containers auto exposition
 * - More serialization formats: JSON? BSON? ASN.1? YAML? S-Expressions? (useful?)
 * - hierarchy, ref: http://altdevblogaday.com/2011/07/26/building-your-gui-with-gwen/

 * - rlyeh ~~ listening to David Bowie / Moonage daydream
 */

// lugaru hace pack/unpack con %b(yte) %s(hort) %i(nt) %l(ong) %f(loat) %d(ouble)
// con los modificadores L(ittle-endian) B(big-endian) N(etwork endian) H(ost endian)
// es muy interesante:
//
// void serialize( serializer &s, const int version )
// {
//      if( version < 2 ) ;
//
//      s.serial( "%Lb", &test );
//
// }

// check this too:
// http://www.gamedev.net/page/resources/_//feature/fprogramming/a-simple-c-object-loader-r2698

#pragma once

#include <map>
#include <string>

#include "string.hpp"
#include "xml.hpp"

namespace moon9
{
    class serial : public std::map< std::string, moon9::string >
    {
        public:

        serial()
        {}

        explicit serial( bool is_saving ) : saving( is_saving )
        {}

        // internal transit interface

        template<typename T>
        serial &protected_data( const std::string &key, const T &t )
        {
            if( saving )
                this->operator[](key) = t;

            return *this;
        }

        template<typename T>
        serial &public_data( const std::string &key, T &t )
        {
            saving ? (*this)[key] = t
              : t = (*this)[key].as<T>();

            return *this;
        }

        // stringization

        std::string str() const
        {
            moon9::xml doc;

            doc.push("class");

            for( const_iterator it = this->begin(); it != this->end(); ++it )
                doc.node( it->first, it->second );

            doc.back();

            return doc.str();
        }

        serial &str( std::string data )
        {
            struct predicate
            {
                moon9::string *last;
                serial &self;

                predicate( serial &_self ) : last( 0 ), self( _self )
                {}

                virtual bool operator()( const int &depth, const std::string &type, const std::string &name, const std::string &value )
                {
                    if( depth > 0 && type == "element"        )  last = &self[ name ], *last = "";
                    if( depth > 0 && type == "pcdata" && last ) *last = value;
                    return true; // continue traversal
                }
            } p(*this);

            this->clear();                         // clear previous variable map

            moon9::xml doc;
            doc.str( data );                       // build xml tree
            doc.walker( p );                       // populate with predicate (~import)

            return *this;
        }

        // public transit interface

        template <typename T>
        static void load( T &t, const std::string &str )
        {
            // 1) create serial to be loaded, then
            // 2) populate empty serial with string content, then
            // 3) fill t class
            t.serialize( moon9::serial(false).str(str) );
        }

        template <typename T>
        static std::string save( T &t )
        {
            // 1) create serial to be saved, then
            // 2) populate empty serial thru t class, then
            // 3) stringize filled serial
            return t.serialize( moon9::serial(true) ).str();
        }

        template <typename T>
        static void print( T &t )
        {
            std::cout << save(t);
        }

        // @todo: (quick)load/(quick)save

        protected:

        bool saving;
    };
}
