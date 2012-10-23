#include <cassert>

#include <iostream>
#include <vector>
#include <string>

#include "hexdump.hpp"
#include "string.hpp"
#include "pak.hpp"
#include "file.hpp"

#include <miniz/miniz.h>

namespace moon9
{
    bool pakfile::has_field( const std::string &property ) const
    {
        return this->find( property ) != this->end();
    }

    bool pakfile::has_operator( const std::string &plugin, const std::string &opcode ) const
    {
        return has_field("operators") && ( get_value<moon9::string>("operators").find( std::string(",") + plugin + ':' + opcode + ',' ) != std::string::npos );
    }

    std::string pakfile::debug( const std::string &format12 ) const
    {
        moon9::string out;
        for( const_iterator it = this->begin(), end = this->end(); it != end; ++it )
            if( it->first != "content" )
            out << moon9::string( format12, it->first, it->second );
            else
            out << moon9::string( format12, it->first, std::string("\n") + hexdump( it->second ) );
        return out;
    }

    // binary serialization

    bool pak::bin( const std::string &bin_import ) //const
    {
        this->clear();
        std::vector<pakfile> result;

        if( const bool parse_zip = true && bin_import.size() > 0 )
        {
            // Try to open the archive.
            mz_zip_archive zip_archive;
            memset(&zip_archive, 0, sizeof(zip_archive));

            mz_bool status = mz_zip_reader_init_mem( &zip_archive, (void *)bin_import.c_str(), bin_import.size(), 0 );

            if( !status )
                return "mz_zip_reader_init_file() failed!", false;

            // Get and print information about each file in the archive.
            for( unsigned int i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++ )
            {
                mz_zip_archive_file_stat file_stat;

                if( !mz_zip_reader_file_stat( &zip_archive, i, &file_stat ) )
                {
                    mz_zip_reader_end( &zip_archive );
                    return "mz_zip_reader_file_stat() failed!", false;
                }

                result.push_back( pakfile() );

                result.back()["filename"] = file_stat.m_filename;
                result.back()["comment"] = file_stat.m_comment;
                result.back()["size"] = (unsigned int)file_stat.m_uncomp_size;
                result.back()["size_z"] = (unsigned int)file_stat.m_comp_size;
                //result.back()["modify_time"] = ze.mtime;
                //result.back()["access_time"] = ze.atime;
                //result.back()["create_time"] = ze.ctime;
                //result.back()["attributes"] = ze.attr;

                if( const bool decode = true )
                {
                    // Try to extract file to the heap.
                    size_t uncomp_size;

                    void *p = mz_zip_reader_extract_file_to_heap(&zip_archive, file_stat.m_filename, &uncomp_size, 0);

                    if( !p )
                    {
                        mz_zip_reader_end(&zip_archive);
                        return "mz_zip_reader_extract_file_to_heap() failed!", false;
                    }

                    // Make sure the extraction really succeeded.
                    /*
                    if ((uncomp_size != strlen(s_pStr)) || (memcmp(p, s_pStr, strlen(s_pStr))))
                    {
                    free(p);
                    mz_zip_reader_end(&zip_archive);
                    return "mz_zip_reader_extract_file_to_heap() failed to extract the proper data", false;
                    }
                    */

                    result.back()["content"].resize( uncomp_size );
                    memcpy( (void *)result.back()["content"].data(), p, uncomp_size );

                    free(p);
                }
            }

            // We're done.
            mz_zip_reader_end(&zip_archive);
        }

        this->resize( result.size() );
        std::copy( result.begin(), result.end(), this->begin() );

        return true;
    }

    std::string pak::bin() //const
    {
        mz_zip_archive zip_archive;
        memset( &zip_archive, 0, sizeof(zip_archive) );

        mz_bool status = mz_zip_writer_init_heap( &zip_archive, 128 * 1024, 128 * 1024 );
        assert( status );
        if( !status )
            return "mz_zip_writer_init_heap() failed!", std::string();

        for( iterator it = this->begin(); it != this->end(); ++it )
        {
            const char *filename = it->operator[]("filename").c_str();
            const char *content = it->operator[]("content").c_str();
            const size_t bufsize = it->operator[]("content").size();

            status = mz_zip_writer_add_mem( &zip_archive, filename, content, bufsize, MZ_DEFAULT_COMPRESSION );
            if( !status )
                return "mz_zip_writer_add_mem() failed!", std::string();
        }

        void *pBuf;
        size_t pSize;

        status = mz_zip_writer_finalize_heap_archive( &zip_archive, &pBuf, &pSize);
        if( !status )
            return "mz_zip_writer_finalize_heap_archive() failed!", std::string();

        // Ends archive writing, freeing all allocations, and closing the output file if mz_zip_writer_init_file() was used.
        // Note for the archive to be valid, it must have been finalized before ending.
        status = mz_zip_writer_end( &zip_archive );
        assert( status );
        if( !status )
            return "mz_zip_writer_end() failed!", std::string();

        std::string result;
        result.resize( pSize );
        memcpy( &result.at(0), pBuf, pSize );

        free( pBuf );

        return result;
    }

    // debug

    std::string pak::debug( const std::string &format1 ) const
    {
        moon9::string out;
        for( const_iterator it = this->begin(); it != this->end(); ++it )
            out << moon9::string( format1, it->debug() );
        return out;
    }
}

