#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "chdir.hpp"

namespace moon9
{
    chdir::chdir( const std::string &_path ) : base(get())
    {
        std::string path = _path + '/';

        for( size_t b = 0, i = 0, e = path.size(); i < e; ++i )
        {
            char ch = path[i];

            if( ch == '\\' || ch == '/' )
            {
                set( path.substr( b, i - b ) );
                b = i + 1;
            }
        }
    }

    chdir::~chdir()
    {
        set( base );
    }

    void chdir::set( const std::string &path ) const
    {
#ifdef _WIN32
        ::_chdir( path.c_str() );
#else
        ::chdir( path.c_str() );
#endif
    }

    std::string chdir::get() const
    {
        char cd[256];

#ifdef _WIN32
        ::_getcwd( cd, 256 );
#else
        ::getcwd( cd, 256 );
#endif

        return cd;
    }
}
