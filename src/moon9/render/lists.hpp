#pragma once

#include <cassert>

#include <iostream>
#include <string>
#include <map>

#include <moon9/render/gl.hpp>
#include <moon9/render/nocopy.hpp>

namespace moon9
{
    namespace lists
    {
        #define MOON9_COMPILE_LIST moon9::compile __comp__ = moon9::compile(__FILE__,__LINE__)

        class compile
        {
            /* usage:

                if( moon9::compile comp = moon9::compile("sphere1") )
                {
                    moon9::matrix::position p( vec3(1, 10, 0) );
                    moon9::sphere sph;
                }

                or

                if( moon9::compile comp = moon9::compile(__FILE__,__LINE__) )
                {
                    moon9::matrix::position p( vec3(1, 10, 0) );
                    moon9::sphere sph;
                }

                or

                if( MOON9_COMPILE_LIST )
                {
                    ...
                }

            */

            const char *file;
            int line;
            bool compiled;
            void go( bool construct )
            {
                typedef std::pair<std::string,int> key_t;
                typedef std::map<key_t, int> map_t;

                static map_t map;
                key_t id = key_t( file, line );
                map_t::iterator it = map.find( id );

                if( construct )
                {
                    compiled = ( it != map.end() );
                    if( !compiled )
                    {
                        static int list = 0;
                        list++;
                        map.insert( std::pair<key_t,int>( id, list) );
                        glNewList( list, GL_COMPILE );
                    }
                }
                else
                {
                    int list = it->second;
                    if( !compiled )
                    {
                        glEndList();
                        //std::cout << "compiled list #" << list << std::endl;
                    }
                    //std::cout << "calling list #" << list << std::endl;
                    glCallList( list );
                }
            }

            public:

            explicit
            compile( const char *_id ) : file(_id), line(0), compiled(false)
            {
                go( true );
            }

            explicit
            compile( const char *_file, int _line ) : file(_file), line(_line), compiled(false)
            {
                go( true );
            }

            ~compile()
            {
                go( false );
            }

            operator const bool() const
            { return !compiled; }
        };

        struct check : nocopy
        {
            check( const char *_file, int _line ) : file(_file), line(_line)
            {
                if( glGetError() != GL_NO_ERROR )
                {
                    std::cerr << gluErrorString( glGetError() ) << std::endl;
                    assert( !"OpenGL Error" );
                }
            }

            ~check()
            {
                if( glGetError() != GL_NO_ERROR )
                {
                    std::cerr << gluErrorString( glGetError() ) << std::endl;
                    assert( !"OpenGL Error" );
                }
            }

            private:

            const char *file;
            int line;
        };
    }
}

namespace moon9
{
    using namespace lists;
}
