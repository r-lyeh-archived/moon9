/*
 * String to Id conversion

 * Copyright (c) 2010 Mario 'rlyeh' Rodriguez
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

 * Note:
 * Following code is very sensitive to compiler version and compilation settings!
 * Please ensure your final disassembly is fully unrolled (~using in-place immediates).
 *
 * Based on idea by Stefan 'Tivolo' Reinalter. Thanks dude! : )
 * http://www.humus.name/index.php?page=Comments&ID=296
 * http://www.humus.name/index.php?page=Comments&ID=299
 * http://www.gamedev.net/community/forums/topic.asp?topic_id=550505

 * - rlyeh ~~ listening to Radiohead / Karma Police
 */

#pragma once

#include <cassert>

#include <map>
#include <string>
#include <iostream>

namespace moon9
{
    namespace detail
    {
        struct ids
        {
            static void check_for_collision( size_t hash, const char *str )
            {
                static std::map< size_t, std::string > map;

                if( map.find( hash ) != map.end() )
                {
                    std::cerr << "Error! '" << map[ hash ] << "' and '" << str << "' hashes just collided!" << std::endl;
                    assert( !"moon::id() collision!" );
                }

                map.insert( std::pair< size_t, std::string >( hash, str ) );
            }
        };
    };

    template <size_t N>
    static inline size_t id( const char (&str)[N] )
    {
        size_t hash = 5381;                   //unrolled djb2 hash algorithm

        for( int kIter = 0; kIter < 128 ; kIter++ )
            if( kIter < N ) hash = (hash * 33) ^ str[ kIter ];

//        kBooRepeat( 128, if( kIter < N ) hash = (hash * 33) ^ str[ kIter ]; );

#if !defined(NDEBUG) && !defined(_NDEBUG)
        // do collision check
        detail::ids::check_for_collision( hash, str );
#endif

        return hash;
    }
}

