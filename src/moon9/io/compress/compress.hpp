#pragma once

#include <vector>
#include <cstdio>

//#include <fastlz/fastlz.h>
#include <lz4/lz4.h>
#include <lz4/lz4hc.h>

#include <iostream>
#include <iomanip>
#include <sstream>

namespace moon9
{
#if 0
    namespace fastlz
    {
        template < class T1, class T2 >
        static bool compress( T2 &buffer_out, const T1 &buffer_in, bool highest_compression = true )
        {
            static const bool verbose = false;

            bool ret = false;

            if( buffer_in.size() >= 70 )
            {
                buffer_out.resize( std::max<size_t>( size_t(buffer_in.size() * 1.05), 70 ) );

                size_t compressed_size = fastlz_compress_level( highest_compression ? 2 : 1, &buffer_in.at(0), buffer_in.size(), &buffer_out.at(0) );

                ret = ( compressed_size > 0 && compressed_size < buffer_in.size() );

                if( ret )
                {
                    buffer_out.resize( compressed_size );
                }

                if( verbose )
                {
//                    std::cout << moon9::echo( ret, compressed_size, buffer_in.size() );
                }
            }

            return ret;
        }

        // note: destination buffer size must be resized properly before doing the actual decompression
        template < class T1, class T2 >
        static bool decompress( T2 &buffer_out, const T1 &buffer_in )
        {
            static const bool verbose = false;

            size_t decompressed_size = fastlz_decompress( &buffer_in.at(0), buffer_in.size(), &buffer_out.at(0), buffer_out.size() );

            bool ret = ( decompressed_size == buffer_out.size() );

            if( verbose )
            {
//                std::cout << moon9::echo( ret, decompressed_size, buffer_out.size() );
            }

            return ret;
        }
    }
#endif

    namespace lz4
    {
        template < class T1, class T2 >
        static bool compress( T2 &buffer_out, const T1 &buffer_in, bool highest_compression = true )
        {
            static const bool verbose = false;

            bool ret = false;

            if( 1 )
            {
                // resize to worst case
                buffer_out.resize( LZ4_compressBound((int)(buffer_in.size())) );

                // compress
                size_t compressed_size = highest_compression ?
                    LZ4_compressHC( &buffer_in.at(0), &buffer_out.at(0), buffer_in.size() )
                    :
                    LZ4_compress( &buffer_in.at(0), &buffer_out.at(0), buffer_in.size() );

                ret = ( compressed_size > 0 && compressed_size < buffer_in.size() );

                if( ret )
                {
                    // if ok, resize properly to unused space
                    buffer_out.resize( compressed_size );
                }

                if( verbose )
                {
//                    std::cout << moon9::echo( ret, compressed_size, buffer_in.size() );
                }
            }

            return ret;
        }

        // note: destination buffer size must be resized properly before doing the actual decompression
        template < class T1, class T2 >
        static bool decompress( T2 &buffer_out, const T1 &buffer_in )
        {
            static const bool verbose = false;

            size_t bytes_read = LZ4_uncompress( &buffer_in.at(0), &buffer_out.at(0), buffer_out.size() );

            bool ret = ( bytes_read == buffer_in.size() );

            if( verbose )
            {
//                std::cout << moon9::echo( ret, decompressed_size, buffer_out.size() );
            }

            return ret;
        }
    }

    // compression methods

    bool is_z( const std::string &self );
    bool is_unz( const std::string &self );

    std::string z( const std::string &self, bool highest_compression = false );
    std::string unz( const std::string &self );
}
