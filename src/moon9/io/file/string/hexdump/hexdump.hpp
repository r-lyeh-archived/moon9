#pragma once

#include <string>
#include "format/format.hpp"

namespace
{
    template<typename T>
    std::string hexdump( const T& obj )
    {
        //todo: obj.size() is num of elements. obj.size() * sizeof(*obj.begin()) is num of bytes!!
        //todo: maxwidth != 80 doesnt work
        int maxwidth = 80;
        int width = 32; //ascii width
        int width_offset_block = (8 + 1);
        int width_chars_block  = (width + 1);
        int width_hex_block    = (width * 2 + 1);
        int width_padding = maxwidth - ( width_offset_block + width_chars_block + width_hex_block );
        int blocks = width_padding / ( width_chars_block + width_hex_block ) ;

        size_t dumpsize = ( obj.size() < width * 16 ? obj.size() : width * 16 ); //16 lines max

        std::string result;

        result += moon9::format( "%-*s %-.*s hexdump .this=%p .size=%d\n", width_offset_block - 1, "offset", width_chars_block - 1, "ascii56789ABCDEF0123456789ABCDEF", &obj, obj.size() );

        if( !obj.size() )
            return result;

        blocks++;

        const unsigned char *p = reinterpret_cast<const unsigned char *>( obj.data() );
        size_t i = 0;

        while( i < dumpsize )
        {
            //offset block
            result += moon9::format("%p ", (size_t)(p + i) ); //%08x, %08zx

            //chars blocks
            for( int b = 0; b < blocks; b++)
            {
                for( int c = 0 ; c < width ; c++ )
                    result += moon9::format("%c", i + c >= dumpsize ? '.' : p[i + c] < 32 || p[i + c] >= 127 ? '.' : p[i + c]);

                result += ' ';
            }

            //hex blocks
            for( int b = 0; b < blocks; b++)
            {
                for( int c = 0; c < width ; c++)
                    result += moon9::format( i + c < dumpsize ? "%02x" : "??", p[i + c]);

                result += ' ';
            }

            //next line
            result += '\n';
            i += width * blocks;
        }

        return result;
    }
}
