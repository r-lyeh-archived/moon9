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

#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>

#include <sys/utime.h>

#include "string/string.hpp"

namespace moon9
{
    class pathfile : public std::string
    {
        public:

        const bool subdirs;

        pathfile();

        pathfile( const std::string &os_path, bool include_subdirs );

        pathfile( const std::string &ioo_extended_path );

        std::string to_os() const;

        std::string to_ioo() const;

        protected:

        std::pair<moon9::iostring,bool> ioo2ioo( const moon9::iostring &dir ) const;

        std::pair<moon9::iostring,bool> ioo2os( const moon9::iostring &dir ) const;

        moon9::iostring os2ioo( const moon9::iostring &dir ) const;
    };

    class file
    {
        moon9::iostring pathfile;
        bool is_temp_name;

        static void sleep( double seconds );

        static const std::string get_temp_path();

    public:

        enum sorting_type { by_name, by_size, by_date, by_extension, by_type } sorting;

        file( const std::string &_pathfile = std::string(), sorting_type defaults = by_name );

        ~file();

        std::string name() const;
        std::string ext() const;

        moon9::pathfile path() const;

        size_t size() const;

        std::string read() const;

        bool overwrite( const std::string &content ) const;
        bool overwrite( const void *data, size_t size ) const;

        bool append( const std::string &content ) const;

        bool append( const void *data, size_t size ) const;

        bool remove() const;

        bool rename( const std::string &newpathfile );

        bool exist() const; // may fail due to permissions (check errno)

        bool is_dir() const; // may return false due to permissions (check errno)

        bool is_file() const; // may return true due to permissions (check errno)

        time_t date() const;

        std::string timestamp( const char *format = "%Y-%m-%d %H:%M:%S" ) const; // defaults to mysql date format

        bool touch( const time_t &modtime = (::time(0)) ); // const // may fail due to sharing violations (check errno)

        bool has_changed(); // check for external changes on file

        // metadata api

        moon9::iostring get( const moon9::iostring &property ) const;

        void set( const moon9::iostring &property, const moon9::iostring &value );

        std::string debug( const char *format12 = "\t.\1=\2\n" );

        // sorting

        void sort_by( sorting_type sorter );

        int compare_with( const file &other, sorting_type sorter );

        bool operator==( const file &other ) const;

        bool operator<( const file &other ) const;

        bool patch( const std::string &patch_data, bool delete_tempfile = true ) const;

        private: mutable std::map< moon9::iostring /*property*/, moon9::iostring /*value*/ > metadata;

        std::string sanitize( moon9::iostring path );
    };

    class files : public std::set< file >
    {
    public:

        files();

        void include( const std::string &path, const std::string &mask, bool scan_subdirs ); // addition: beware duplicates (set!)
        void exclude( const std::string &path, const std::string &mask, bool scan_subdirs ); // subtraction: beware duplicates (set!)

        files sort( file::sorting_type sorting ) const;

        std::string str( const char *format1 = "\1\n" ) const;

        private:

        static bool add_recursively( files &self, const std::string &sDir, const std::string &mask, bool recursive );
    };
}


#if 0

// filesystem = ~xml

#include <moon9/io/xml.hpp>

namespace moon9
{
    class filesystem
    {
        moon9::xml xml;

    public:

        filesystem()
        {}

        ~filesystem()
        {}

        virtual files search()
        {}

        //useful when having ::files ?
        //virtual void mount( id, files, ro/rw ) // unmount
        //{}

        //cwd,pwd,pushd,popd
        //root,defaults
        //read,overwrite,append,compress,uncompress
        //stream

        //import_file
        //import_folder

        //remove (from fs)

        //bool update()
        //-> get_additions
        //-> get_removals

        virtual void unmount( id )
        {}

        virtual void patch( id, files, ro/rw )
        {}
    };
}

#endif
