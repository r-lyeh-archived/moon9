// returns an hex dump
// - rlyeh

#pragma once

//todo: maxwidth != 80 doesnt work

#include <string>
#include "format.hpp"

namespace
{
    std::string hexdump( const void *data, size_t num_bytes, const void *self = 0 )
    {
        int maxwidth = 80;
        int width = 16; //column width
        int width_offset_block = (8 + 1);
        int width_chars_block  = (width * 3 + 1) + sizeof("asc");
        int width_hex_block    = (width * 3 + 1) + sizeof("hex");
        int width_padding = maxwidth - ( width_offset_block + width_chars_block + width_hex_block );
        int blocks = width_padding / ( width_chars_block + width_hex_block ) ;

        size_t dumpsize = ( num_bytes < width * 16 ? num_bytes : size_t( width * 16 ) ); //16 lines max

        std::string result;

        result += moon9::format( "%-*s %-.*s [ptr=%p sz=%d]\n", width_offset_block - 1, "offset", width_chars_block - 1, "00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F", self ? self : data, num_bytes );

        if( !num_bytes )
            return result;

        blocks++;

        const unsigned char *p = reinterpret_cast<const unsigned char *>( data );
        size_t i = 0;

        while( i < dumpsize )
        {
            //offset block
            result += moon9::format("%p ", (size_t)(p + i) ); //%08x, %08zx

            //chars blocks
            for( int b = 0; b < blocks; b++)
            {
                for( int c = 0 ; c < width ; c++ )
                    result += moon9::format(" %c ", i + c >= dumpsize ? '.' : p[i + c] < 32 || p[i + c] >= 127 ? '.' : p[i + c]);

                result += "asc\n";
            }

            //offset block
            result += moon9::format("%p ", (size_t)(p + i) ); //%08x, %08zx

            //hex blocks
            for( int b = 0; b < blocks; b++)
            {
                for( int c = 0; c < width ; c++)
                    result += moon9::format( i + c < dumpsize ? "%02x " : "?? ", p[i + c]);

                result += "hex\n";
            }

            //next line
            //result += '\n';
            i += width * blocks;
        }

        return result;
    }

    std::string hexdump( const char *obj )
    {
        return hexdump( obj, strlen(obj) );
    }

    template<typename T>
    std::string hexdump( const T& obj )
    {
        return hexdump( obj.data(), obj.size() * sizeof(*obj.begin()), &obj );
    }
}
