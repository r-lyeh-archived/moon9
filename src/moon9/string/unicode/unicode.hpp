/*
 * Simple unicode class (WIP)
 * Copyright (c) 2010 Mario 'rlyeh' Rodriguez
 *
 * Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
 * See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

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

 * To do:
 * - replace uint8_t, uint16_t, uint32_t back? what about MSVC<2010?
 * - document class & unicode

 * - rlyeh
 */

#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// After reading http://en.wikipedia.org/wiki/Plane_(Unicode) and some other websites:
//
// Code points are for people and code units are for computers.
// It takes five code points to say Hello, and they are: 72(H), 101(e), 108(l), 108(l), 111(o)
// Each code unit format has its own structure.
// UTF-8: 0x48, 0x65, 0x6c, 0x6c, 0x6f
// UTF-16: 0x0048, 0x0065, 0x006c, 0x006c, 0x006f
// UTF-32: 0x00000048, 0x00000065, 0x0000006c, 0x0000006c, 0x0000006f
//
// Files in unicode start with BOM hopefully (see *Unicode byte order mark*)
// that is EFBBBF UTF8 / FEFF UTF16-LE/ FFFE UTF16-BE / 0000FEFF UTF32-BE / FFFE0000 UTF32-LE
//
// Just deal with these files with wstreams:
//
// #include <fstream>
// #include <iostream>
//
// wchar_t *file_content;
// ifstream::pos_type file_size;
// wifstream file_stream;
// string file_temporary;
//
// file_stream.open( file_temporary.c_str(), ios_base::in | ios_base::binary | ios_base::ate );
// if ( file_stream.is_open() ) {
//    file_size = file_stream.tellg();
//    file_content = new wchar_t[file_size];
//    file_stream.seekg( 0, ios_base::beg );
//    file_stream.read( file_content, (streamsize)file_size );
//    file_stream.close();
// }

//
// Mi recomendacion es:
// - Evitar UTF16, wchar_t y todo lo relacionado *
// - Seguir usando std::string() en tu programa
// - Para las cadenas que tienen internacionalizacion:
//   - Importar y exportar en UTF8+BOM **
//   - Procesar internamente en UTF32 *** ****
//
// *: UTF16 está bien si vamos a estar todo el programa en el mismo plano (generalmente plano 0)
// pero habra complicaciones si el idioma está en otro plano. Diria que solo funciona bien para
// la mayoria de lenguajes western.
// **: Almacenar y serializar las cadenas unicode en UTF8+BOM, ya que no tiene problemas de
// endianess, y en la mayoria de casos son la representacion mas pequeña posible (no siempre).
// ***: Porque sus 32 bits contienen los 21 bits maximos que hay y habrá durante mucho tiempo, y
// porque code points y code units coinciden (actualmente).
// ****: En UTF8 el BOM no es necesario pero me parece bien incluirlo para detectar dinamicamente en
// un programa si un std::string contiene una cadena UTF8 o ASCII.
//


namespace moon9
{
    // unicode class as vector of codepoints

    class unicode : public std::vector< unsigned int >
    {
        public:

        static const bool verbose = true;

        unicode()
        {}

        unicode( const unicode &t )
        {
            operator=( t );
        }

        unicode &operator=( const unicode &t )
        {
            this->resize( t.size() );
            std::copy( t.begin(), t.end(), this->begin() );
            return *this;
        }

        unicode( const std::string &utf8_string )
        {
            unsigned int codepoint, state = 0;

            for( unsigned char *s = (unsigned char *)utf8_string.c_str(); *s; )
            {
                //printf("%c-%02x", *s, *s );
                if( detail::decode_utf8( &state, &codepoint, *s++ ) == detail::UTF8_ACCEPT )
                    this->push_back( codepoint ); //, printf(" >> U+%04X (%d)\n", codepoint, state);
                //else printf("(%d),", state );
            }

            if( state != detail::UTF8_ACCEPT )
            {
                std::cerr << "ERROR: Not an UTF8 string" << std::endl;
                this->clear();
            }

            if( verbose )
                for( size_t i = 0; i < this->size(); ++i )
                    std::cout << moon9::format("U+%04X", this->operator[](i) ) << std::endl;
        }

        std::wstring wstr()
        {
            std::wstring ws = L"";

            for( size_t i = 0; i < this->size(); ++i )
            {
                unsigned int codepoint = this->operator[]( i );

                if (codepoint > 0xffff)
                {
                    ws += (unsigned short)(0xD7C0 + (codepoint >> 10));
                    ws += (unsigned short)(0xDC00 + (codepoint & 0x3FF));
                }
                else
                {
                    ws += (unsigned short)codepoint;
                }
            }

            return ws;
        }

        private:

        struct detail
        {
            static const unsigned int UTF8_ACCEPT = 0;
            static const unsigned int UTF8_REJECT = 12;

            static unsigned int decode_utf8( unsigned int* state, unsigned int* codep, unsigned int byte )
            {
                static const unsigned char utf8d[] =
                {
                    // The first part of the table maps bytes to character classes that
                    // to reduce the size of the transition table and create bitmasks.
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
                    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                    8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
                    10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

                    // The second part is a transition table that maps a combination
                    // of a state of the automaton and a character class to a state.
                     0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
                    12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
                    12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
                    12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
                    12,36,12,12,12,12,12,12,12,12,12,12
                };

                unsigned int type = utf8d[byte];
                *codep = (*state != UTF8_ACCEPT) ?
                    (byte & 0x3fu) | (*codep << 6) :
                    (0xff >> type) & (byte);
                return *state = utf8d[256 + *state + type];
            }
        };

    };
}

