/*
 * Simple and handy endianness conversion routines

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

 * To do:
 * - NUXI support (useful?)
 * - Get auto_test() out of template type, so it does not instance once per type

 * - rlyeh
 */

#pragma once

#if (__cplusplus > 199711) || (defined(_MSC_VER) && _MSC_VER >= 1600) || (defined(__GNUC__))
#include <type_traits> // <-- std c++0x
namespace moon9
{
    using std::is_pod;
}
#else
#include <boost/type_traits.hpp> // non c++0x compilers
namespace moon9
{
    using boost::is_pod;
}
#endif

#include <cassert>

#include "directives/directives.hpp"

#if kMoon9TargetByteOrder == 2143
#   error NUXI/middle endian is not supported
#endif

#include <iostream>

namespace moon9
{
    namespace endian
    {
        enum
        {
            is_debug = true,

            is_target_little = ( kMoon9TargetByteOrder == 1234 ),
            is_target_big = ( kMoon9TargetByteOrder != 1234 )
        };

        template<typename T>
        T swap( const T &t )
        {
            static struct auto_test
            {
                auto_test()
                {
                    if( is_debug )
                    {
                        union endianness
                        {
                            short word;
                             char byte[ sizeof(short) ];
                        }
                        test = { 0x0100 };

                        if( test.byte[0] )
                        {
                            assert( "<moon9/os/endian.hpp> says: autodetection resolved to big endian!" && kMoon9TargetByteOrder == 4321 );
                        }
                        else
                        {
                            assert( "<moon9/os/endian.hpp> says: autodetection resolved to little endian!" && kMoon9TargetByteOrder == 1234 );
                        }
                    }
                }
            } _;

            T out = t;
            char *ptr;

            if( moon9::is_pod<T>::value ) // <-- std c++0x
            switch( sizeof( T ) )
            {
                case 0:
                case 1:
                    break;
                case 2:
                    ptr = reinterpret_cast<char *>(&out);
                    std::swap( ptr[0], ptr[1] );
                    break;
                case 4:
                    ptr = reinterpret_cast<char *>(&out);
                    std::swap( ptr[0], ptr[3] );
                    std::swap( ptr[1], ptr[2] );
                    break;
                case 8:
                    ptr = reinterpret_cast<char *>(&out);
                    std::swap( ptr[0], ptr[7] );
                    std::swap( ptr[1], ptr[6] );
                    std::swap( ptr[2], ptr[5] );
                    std::swap( ptr[3], ptr[4] );
                    break;
                case 16:
                    ptr = reinterpret_cast<char *>(&out);
                    std::swap( ptr[0], ptr[15] );
                    std::swap( ptr[1], ptr[14] );
                    std::swap( ptr[2], ptr[13] );
                    std::swap( ptr[3], ptr[12] );
                    std::swap( ptr[4], ptr[11] );
                    std::swap( ptr[5], ptr[10] );
                    std::swap( ptr[6], ptr[9] );
                    std::swap( ptr[7], ptr[8] );
                    break;
                default:
                    std::cerr << "<moon9/os/endian.hpp> says: Warning! 256-bit? bigger?" << std::endl;
                    break;
            }
            else
            {
                std::cerr << "<moon9/os/endian.hpp> says: Cannot reverse non-POD types" << std::endl;
            }

            return out;
        }

        template<typename T>
        T little_to_big( const T &in )
        {
            return swap( in );
        }
        template<typename T>
        T big_to_little( const T &in )
        {
            return swap( in );
        }

        template<typename T>
        T little_to_native( const T &in )
        {
            return kMoon9TargetByteOrder == 1234 ? in : swap( in );
        }
        template<typename T>
        T native_to_little( const T &in )
        {
            return kMoon9TargetByteOrder == 1234 ? in : swap( in );
        }

        template<typename T>
        T big_to_native( const T &in )
        {
            return kMoon9TargetByteOrder == 4321 ? in : swap( in );
        }
        template<typename T>
        T native_to_big( const T &in )
        {
            return kMoon9TargetByteOrder == 4321 ? in : swap( in );
        }
    }
}
