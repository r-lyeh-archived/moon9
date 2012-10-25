/*
 * Simple file handling classes

 * Copyright (c) 2010-2011 Mario 'rlyeh' Rodriguez
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

 * todo:
 *
 * add stream() << >> chunk()
 * or
 * add subread( from, to ), subwrite( from, to ), chunk(), next(), tell(),

 * - rlyeh ~~ listening to Alice in chains / Nutshell
 */

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <chrono>
#include <thread>

#ifdef _WIN32
#   include <winsock2.h>
#   include <windows.h>
#   include <ctime>
#   include <sys/stat.h>
#else
#   include <sys/time.h>
#   include <sys/stat.h>
#endif

#include <sys/utime.h>

#include "file.hpp"
#include "string/string.hpp"

namespace moon9
{
    pathfile::pathfile() : std::string(), subdirs(false)
    {}

    pathfile::pathfile( const std::string &os_path, bool include_subdirs ) : std::string(os2ioo(os_path)), subdirs(include_subdirs)
    {}

    pathfile::pathfile( const std::string &ioo_extended_path ) : std::string( ioo_extended_path ), subdirs( ioo2ioo(ioo_extended_path).second )
    {}

    std::string pathfile::to_os() const
    {
        return ioo2os( *this ).first;
    }

    std::string pathfile::to_ioo() const
    {
        return *this;
    }

    std::pair<moon9::iostring,bool> pathfile::ioo2ioo( const moon9::iostring &dir ) const /* /g/Boot/ -> g:\\Boot\\ with no subfolders ; also /g/Boot// -> g:\Boot\ with subfolders */
    {
        bool subdirs = false;

        int idx = dir.find_last_of('/');
        if( idx != std::string::npos )
            if( idx > 0 && dir.at(idx-1) == '/' ) //&& dir.find_last_of('*') > idx && dir.find_last_of('*') < std::string::npos )
                subdirs = true;

        return std::pair<moon9::iostring,bool>(dir,subdirs);
    }

    std::pair<moon9::iostring,bool> pathfile::ioo2os( const moon9::iostring &dir ) const /* /g/Boot/ -> g:\\Boot\\ with no subfolders ; also /g/Boot// -> g:\Boot\ with subfolders */
    {
        bool subdirs = false;

        int idx = dir.find_last_of('/');
        if( idx != std::string::npos )
            if( idx > 0 && dir.at(idx-1) == '/' ) //&& dir.find_last_of('*') > idx && dir.find_last_of('*') < std::string::npos )
                subdirs = true;

    #if defined(WIN32) || defined(_WIN32)
        if( dir.size() > 2 && dir.at(0) == '/' && dir.at(2) == '/' &&
         (( dir.at(1) >= 'a' && dir.at(1) <= 'z' ) || ( dir.at(1) >= 'A' && dir.at(1) <= 'Z' )) )
            return std::pair<std::string,bool>( moon9::iostring( "\1:\2", dir.at(1), dir.replace("/","\\").substr(2) ), subdirs );
        else
            return std::pair<std::string,bool>( dir.replace("/", "\\"), subdirs );
    #else
            return std::pair<std::string,bool>( dir, subdirs );
    #endif
    }
    moon9::iostring pathfile::os2ioo( const moon9::iostring &dir ) const /* g:\\Boot\\ -> /g/Boot/ */
    {
    #if defined(_WIN32)
        if( dir.size() > 2 && dir.at(1) == ':' && dir.at(2) == '\\' &&
         (( dir.at(0) >= 'a' && dir.at(0) <= 'z' ) || ( dir.at(0) >= 'A' && dir.at(0) <= 'Z' )) )
            return moon9::iostring( "/\1\2", dir.at(0), dir.replace("\\","/").substr(2) );
        return dir.replace("\\", "/");
    #else
        return dir;
    #endif
    }




    void file::sleep( double seconds )
    {
        std::chrono::microseconds duration( (int)(seconds * 1000000) );
        std::this_thread::sleep_for( duration );
    }

    const std::string file::get_temp_path()
    {
#if defined(_WIN32) // \st823.1 becomes .\st823.1, or w:\users\user9612\temp~1\st823.1
        static const std::string temp_path = getenv("TEMP") ? getenv("TEMP") : ( getenv("TMP") ? getenv("TMP") : "" );
        return temp_path;
#else
        return "";
#endif
    }

    file::file( const std::string &_pathfile, sorting_type defaults ) :
        is_temp_name( _pathfile.size() ? false : true ),
        pathfile( _pathfile.size() ? _pathfile : std::tmpnam(0) ),
        sorting(defaults)
    {
        if( is_temp_name )
        {
            pathfile = get_temp_path() + pathfile;
        }

        // metadata here
        // ie, committed_by, date_created, last_open, last_access, vertex_info, bitrate, etc

        // valid prefixes:
        // b_ = bool
        // d_ = double
        // s_ = std::string
        // z_ = size_t
    }

    file::~file()
    {
        if( is_temp_name )
            remove();
    }

    std::string file::name() const
    {
        return pathfile;
    }

    std::string file::ext() const
    {
        std::string n = name();

        auto npos = n.end();

        for( auto it = n.begin(), end = n.end(); it != end; ++it )
        {
            if( *it == '.' )
                npos = ++it;
            else
            if( *it == '/' || *it == '\\' )
                npos = n.end();
        }

        return npos == n.end() ? std::string() : &n.c_str()[ npos - n.begin() ];
    }

    moon9::pathfile file::path() const
    {
        return pathfile;
    }

    size_t file::size() const
    {
        //should we return std::streamoff instead and avoid casting?
        std::ifstream is( pathfile.c_str(), std::ios::in|std::ios::binary|std::ios::ate );

        //is.seekg(0, std::ios::end);
        size_t length = static_cast<size_t>( is.tellg() );
        is.seekg(0, std::ios::beg);
        length -= static_cast<size_t>( is.tellg() );

        return length;
    }

    std::string file::read() const
    {
        size_t length = size();

        std::string buffer;

        if( length > 0 )
        {
            buffer.resize( length );

            std::ifstream is( pathfile.c_str(), std::ios::in|std::ios::binary );
            is.read( reinterpret_cast< char * >( &buffer[0] ), length );

            // std::ifstream ifs( pathfile.c_str(), std::ios::in|std::ios::binary );
            // buffer = std::string( std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>() );
        }

        return buffer;
    }

    bool file::overwrite( const std::string &content ) const
    {
        return overwrite( content.c_str(), content.size() );
    }

    bool file::overwrite( const void *data, size_t size ) const
    {
        // is trunc flag needed?
        std::ofstream is( pathfile.c_str(), std::ios::out|std::ios::binary|std::ios::trunc );

        if( is.is_open() && is.good() )
            is.write( reinterpret_cast<const char *>( data ), size );

        return !is.fail();
    }

    bool file::append( const std::string &content ) const
    {
        return append( content.c_str(), content.size() );
    }

    bool file::append( const void *data, size_t size ) const
    {
        std::fstream is( pathfile.c_str(), std::ios::out|std::ios::binary|std::ios::app|std::ios::ate );

        if( is.is_open() && is.good() )
            is.write( reinterpret_cast<const char *>( data ), size );

        return !is.fail();
    }

    bool file::remove() const
    {
        for( int i = 0; i < 512; ++i )
        {
            if( !exist() )
                return true;
            if( !std::remove( pathfile.c_str() ) )
                return true;
            sleep(0.001);
        }
        return false;
    }

    bool file::rename( const std::string &newpathfile )
    {
        for( int i = 0; i < 512; ++i )
        {
            if( !exist() )
                return false;
            if( !std::rename( pathfile.c_str(), newpathfile.c_str() ) )
                { pathfile = newpathfile; return true; }
            sleep(0.001);
        }
        return false;
    }

    bool file::exist() const // may fail due to permissions (check errno)
    {
        struct stat fileInfo;
        return stat( pathfile.c_str(), &fileInfo ) == 0 ? true : false;
    }

    bool file::is_dir() const // may return false due to permissions (check errno)
    {
        struct stat fileInfo;
        if( stat( pathfile.c_str(), &fileInfo ) < 0 )
            return false;
        return (( fileInfo.st_mode & S_IFMT ) == S_IFDIR );
    }

    bool file::is_file() const // may return true due to permissions (check errno)
    {
        return !is_dir();
    }

    time_t file::date() const
    {
        struct stat fileInfo;
        if( stat( pathfile.c_str(), &fileInfo ) < 0 )
            return ::time(0);
        return fileInfo.st_mtime;
    }

    std::string file::timestamp( const char *format ) const // defaults to mysql date format
    {
        struct stat fileInfo;
        time_t mtime = stat( pathfile.c_str(), &fileInfo ) < 0 ? (::time( 0 )) : fileInfo.st_mtime;

        struct tm *ts = localtime( &mtime );

        char buffer[64];
        strftime(buffer, sizeof(buffer), format, ts);
        return buffer;
    }

    bool file::touch( const time_t &modtime ) // const // may fail due to sharing violations (check errno)
    {
        struct stat fileInfo;
        if( stat( pathfile.c_str(), &fileInfo ) < 0 )
            return false;

        struct utimbuf tb;
        tb.actime = fileInfo.st_atime;  /* keep atime unchanged */
        tb.modtime = modtime;           /* set mtime to current time */

        return utime( pathfile.c_str(), &tb ) != -1 ? true : false;
    }

    bool file::has_changed() // check for external changes on file
    {
        static std::map< std::string, time_t > cache;
        if( cache.find( pathfile ) == cache.end() )
        {
            ( cache[ pathfile ] = cache[ pathfile ] ) = date();
        }

        time_t modtime = cache[ pathfile ];
        time_t curtime = date();

        bool changed = std::abs( std::difftime( modtime, curtime ) ) > 2.5;  // fat32 minimal lapse is 2 seconds; others filesystems are close to zero

        if( changed )
            cache[ pathfile ] = curtime;

        return changed;
    }

    // metadata api

    moon9::iostring file::get( const moon9::iostring &property ) const
    {
        //@todo: mutex
        metadata[ property ] = metadata[ property ];
        return metadata[ property ];
    }

    void file::set( const moon9::iostring &property, const moon9::iostring &value )
    {
        //@todo: mutex
        metadata[ property ] = metadata[ property ];
        metadata[ property ] = value;
    }

    std::string file::debug( const char *format12 )
    {
        moon9::iostring out;
        for( std::map< moon9::iostring, moon9::iostring >::iterator it = metadata.begin(); it != metadata.end(); ++it )
            out << moon9::iostring(format12, it->first, it->second);
        return out;
    }

    // sorting

    void file::sort_by( sorting_type sorter )
    {
        sorting = sorter;
    }

    int file::compare_with( const file &other, sorting_type sorter )
    {
        sorting_type bkp = sorting;
        sorting = sorter;
        if( operator <( other ) ) return sorting = bkp, -1;
        if( operator==( other ) ) return sorting = bkp,  0;
        return sorting = bkp, +1;
    }

    bool file::operator==( const file &other ) const
    {
        if( sorting == by_size )
        return size() == other.size();

        if( sorting == by_date )
        return date() == other.date();

        if( sorting == by_extension )
        return ext() == other.ext();

        if( sorting == by_type )
        return is_dir() == other.is_dir();

        //if( sorting == by_name )
        return pathfile == other.pathfile;
    }

    bool file::operator<( const file &other ) const
    {
        if( sorting == by_size )
        return size() < other.size();

        if( sorting == by_date )
        return date() < other.date();

        if( sorting == by_extension ) //@todo: lower!, buggy
        return ext() < other.ext();

        if( sorting == by_type )
        return is_dir() < other.is_dir();

        //if( sorting == by_name )
        return pathfile < other.pathfile;
    }

    bool file::patch( const std::string &patch_data, bool delete_tempfile ) const
    {
        if( !patch_data.size() )
            return false;

        bool success = false;

        std::string unpatched_file = pathfile;
        std::string tempfile = unpatched_file + ".old"; // @todo: use tmpname()

        //try to remove
        file( tempfile ).remove();

        //try to rename & smash
        if( file( unpatched_file ).rename( tempfile ) )
        {
            // try to patch
            if( file( unpatched_file ).overwrite( patch_data ) )
            {
                success = true;
            }
            else
            {
                // try to rollback
                if( !file( tempfile ).rename( unpatched_file ) )
                {
                    //argh! what do we do now? :(
                }
            }
        }

        //try to remove
        if( delete_tempfile )
            file( tempfile ).remove();

        return success;
    }

    std::string file::sanitize( moon9::iostring path )
    {
        #if defined(_WIN32)
        return path.replace("/", "\\");
        #else
        return path.replace("\\", "/");
        #endif
    }


    files::files()
    {}

    void files::include( const std::string &path, const std::string &mask, bool scan_subdirs ) // addition: beware duplicates (set!)
    {
        add_recursively( *this, path, mask, scan_subdirs );
    }

    void files::exclude( const std::string &path, const std::string &mask, bool scan_subdirs ) // subtraction: beware duplicates (set!)
    {
        files target, to_remove;
        add_recursively( to_remove, path, mask, scan_subdirs );
        for( const_iterator it = this->begin(); it != this->end(); ++it )
            if( to_remove.find( file( it->name() ) ) == to_remove.end() )
                target.insert( file( it->name() ) );
        std::swap( *this, target );
    }

    files files::sort( file::sorting_type sorting ) const
    {
        files result;
        for( const_iterator it = this->begin(); it != this->end(); ++it )
            result.insert( file( it->name(), sorting ) );
        return result;
    }

    std::string files::str( const char *format1 ) const
    {
        moon9::iostring out;
        for( const_iterator it = this->begin(); it != this->end(); ++it )
            out << moon9::iostring( format1, it->name() );
        return out;
    }

    bool files::add_recursively( files &self, const std::string &sDir, const std::string &mask, bool recursive )
    {
#if defined(_WIN32)

        WIN32_FIND_DATAA fdFile;
        HANDLE hFind = NULL;

        std::string sPath = sDir + "\\" + mask;

        if((hFind = FindFirstFileA(sPath.c_str(), &fdFile)) == INVALID_HANDLE_VALUE)
        {
            std::cout << "Path not found: " << sDir << std::endl;
            return false;
        }

        do
        {
            // Ignore . and .. folders
            if(strcmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0)
            {
                // Rebuild path
                sPath = std::string(sDir) + "\\" + std::string(fdFile.cFileName);

                // Scan recursively if needed
                if( fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                {
                    if( recursive )
                        add_recursively( self, sPath, mask, recursive );
                    else
                        self.insert( file( sPath ) );
                }
                else
                {
                    self.insert( file( sPath ) );
                }
            }
        }
        while( FindNextFileA( hFind, &fdFile ) );

        FindClose( hFind );

        return true;

#else

        moon9::iostring out;

        FILE *fp;
        if( recursive)
            fp = popen( moon9::iostring( "find \1 -type d -or -type f -name '\2'", sDir, mask ).c_str(), "r" );
        else
            fp = popen( moon9::iostring( "find \1 -maxdepth 1 -type d -or -type f -name '\2'", sDir, mask ).c_str(), "r" );
        if( fp )
        {
            while( !feof(fp) ) out << (unsigned char)(fgetc(fp));
            fclose(fp);
        }
        moon9::iostrings found = out.tokenize("\n\r");
        for( size_t i = 0; i < found.size(); ++i )
        {
            file entry( found[i] );
            if( entry.exist() )
                self.insert( entry );
        }
        return true;

#endif

    }
}
