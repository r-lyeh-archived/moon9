// (Not so) simple RAII render, wip
// -rlyeh

#pragma once

#include <cassert>

#include <string>
#include <map>
#include <mutex>
#include <iostream>

#include "gl.hpp"
#include "texture.hpp" // texture, texturemap
#include "disable.hpp"
#include <moon9/render/nocopy.hpp>

#include <moon9/spatial/vec.hpp>

namespace moon9
{
    namespace style
    {
        namespace todo
        {
            namespace glow
            {
            }

            namespace outline
            {
                // == fill.empty() + edge.solid() ?
            }
        }

        struct texture : nocopy
        {
            moon9::texture t;

            explicit
            texture( const std::string &pathFile, const bool high_quality = true ) : id( 0 )
            {
#if 0
                id = manager( true, pathFile, high_quality );

                glEnable(GL_TEXTURE_2D);
#else
                if( !pathFile.size() )
                    return;

                static moon9::texturemap<std::string> map;

                if( map.find( pathFile ) )
                {
                    id = map.found().id;
                }
                else
                {
                    moon9::texture &tx = ( map[ pathFile ] = map[ pathFile ] );

                    tx.load( pathFile ); //, mirror_w, mirror_h );
                    tx.submit();

                    id = tx.id;
                }

                glEnable(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, id );

                config_filtering( high_quality );
#endif
            }

            explicit
            texture( size_t texture_id ) : id( texture_id )
            {
                glEnable(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, id );
            }

            explicit
            texture( const moon9::texture &t ) : id(t.id)
            {
                glEnable(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, id );
            }

            ~texture()
            {
                #if 0
                unbind();
                #endif

                glDisable(GL_TEXTURE_2D);
            }

            static void unbind( const std::string &pathFile )
            {
                manager( false, pathFile );
            }

            static size_t create( int num )
            {
                GLuint id;
                glGenTextures( 1, &id );
                return id;
            }

            static void bind( size_t _id )
            {
                GLuint id = _id;
                glBindTexture(GL_TEXTURE_2D, id );
                //std::cout << "texture #" << id << std::endl;
            }

            static void apply( const std::string &pathFile, bool mirror_w = false, bool mirror_h = false )
            {
                if( !pathFile.size() )
                    return;

                std::cout << "Registering texture: " << pathFile << std::endl;

                static std::map< std::string, moon9::texture > map;

                // decode
                moon9::texture &tx = ( map[ pathFile ] = map[ pathFile ] );
                tx = moon9::texture();
                tx.load( pathFile, mirror_w, mirror_h );

                // submit decoded data
                tx.submit();
            }

            static void config_filtering( bool high_quality )
            {
#if 0

                // Nearest Filtered Texture
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
                glTexImage2D(...)

                // Linear Filtered Texture
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                glTexImage2D(...)

                // MipMapped Texture
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
                gluBuild2DMipmaps(...)

#endif

                /* blurry/blocky

                ref: http://gregs-blog.com/2008/01/17/opengl-texture-filter-parameters-explained/
                MAG_FILTER/MIN_FILTER                 Bilinear Filtering          Mipmapping
                                                        Near   Far
                GL_NEAREST / GL_NEAREST_MIPMAP_NEAREST  Off    Off                Standard
                GL_NEAREST / GL_LINEAR_MIPMAP_NEAREST   Off    On                 Standard (* chars)
                GL_NEAREST / GL_NEAREST_MIPMAP_LINEAR   Off    Off                Use trilinear filtering
                GL_NEAREST / GL_LINEAR_MIPMAP_LINEAR    Off    On                 Use trilinear filtering (* chars)
                GL_NEAREST / GL_NEAREST                 Off    Off                None
                GL_NEAREST / GL_LINEAR                  Off    On                 None (* scene)
                GL_LINEAR / GL_NEAREST_MIPMAP_NEAREST   On     Off                Standard
                GL_LINEAR / GL_LINEAR_MIPMAP_NEAREST    On     On                 Standard
                GL_LINEAR / GL_NEAREST_MIPMAP_LINEAR    On     Off                Use trilinear filtering
                GL_LINEAR / GL_LINEAR_MIPMAP_LINEAR     On     On                 Use trilinear filtering
                GL_LINEAR / GL_NEAREST                  On     Off                None
                GL_LINEAR / GL_LINEAR                   On     On                 None

                (*) intended for prjx

                @todo
                enum { near_blurry, near_blocky }
                enum { far_blurry, far_blocky }
                enum { mipmap_none, mipmap_std, mipmap_trilinear };
                mask -> near_blocky | far_blurry | mipmap_std

                */

                if( high_quality ) {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
                }
                else {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
                }
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            }

            size_t id;

            private:

            static size_t manager( bool acquire, const std::string &pathFile, const bool high_quality = true )
            {
                static std::map< std::string, GLuint > map;
                static std::mutex mutex;
                std::lock_guard<std::mutex> locker( mutex );

                std::map< std::string, GLuint >::iterator id_it = map.find( pathFile );
                bool found = ( id_it != map.end() );

                // delete ?
                if( !acquire )
                {
                    assert( found );
                    glDeleteTextures( 1, &id_it->second );
                    map.erase( id_it );
                    return 0;
                }

                // if acquire...

                // previously acquired?
                if( found )
                {
                    //std::cout << "Caching texture" << std::endl;

                    assert( id_it->second != 0 );
                    bind( id_it->second );
                    return id_it->second;
                }

                GLuint id = create(1);
                bind( id );
                apply( pathFile );
                config_filtering( high_quality );

                // cache insertion
                map[ pathFile ] = id;

                return id;
            }


            /*
                Contrast
                Crop
                Extract alpha channel
                Convert to grayscale
                Intensity
                Lightness
                Color quantization (conversion of true-color images to 256 colors)
                Bilinear resize
                Gaussian blur
                Threshold

                blend(texture &t, float01)
                or(texture &t, float01) //(saturated)
                and(texture &t, float01)

                conversion policies:
                onSize: clip/stretch, scale/bilinear/trilinear/anisotropic
                onColor: uniform/palettized/etc
            */
        };

        struct debug : nocopy
        {
            disable::texturing t;
            disable::lighting l;
            disable::depth d;

             debug() {}
            ~debug() {}
        };

        struct zdebug : nocopy
        {
            disable::texturing t;
            disable::lighting l;

             zdebug() {}
            ~zdebug() {}
        };

        //namespace edge {
        //glLineStipple( 1, 0xFFF0 ); //0b1111111111110000 );
        //glLineStipple( 1, 0xFF00 ); //0b1111111100000000 );
        //glLineStipple( 1, 0xF0F0 ); //0b1111000011110000 );
        //glLineStipple( 1, 0xCCCC ); //0b1100110011001100 );
        //glLineStipple( 1, 0xAAAA ); //0b1010101010101010 );
        struct dotted/*_edge*/ : nocopy
        {
            dotted()
            {
                glEnable( GL_LINE_STIPPLE );
                glLineStipple(1, 0xCCCC);
            }
            ~dotted()
            {
                glDisable( GL_LINE_STIPPLE );
            }
        };

        struct dotdashed/*_edge*/ : nocopy
        {
            dotdashed()
            {
                glEnable( GL_LINE_STIPPLE );
                glLineStipple(1, 0xC0F0);
            }
            ~dotdashed()
            {
                glDisable( GL_LINE_STIPPLE );
            }
        };

        struct dashed/*_edge*/ : nocopy
        {
            dashed()
            {
                glEnable( GL_LINE_STIPPLE );
                //glLineStipple(1, 0xFF00);

                static unsigned short mask = 0xFF00;
                unsigned short bitN = ( mask & 0x8000 ) > 0;
                unsigned short bit0 = ( mask & 1 );

                if( 1 ) //if anim left
                {
                    mask <<= 1;
                    mask |= ( bitN << 0 );
                }
                else    //else anim right.
                {
                    mask >>= 1;
                    mask |= ( bit0 << 31 );
                }
                glLineStipple(1, mask);
            }
            ~dashed()
            {
                glDisable( GL_LINE_STIPPLE );
            }
        };

        struct thick/*_edge*/ : nocopy
        {
            explicit thick( float mag = 3.0 )
            {
                glLineWidth(mag);
            }
            ~thick()
            {
                glLineWidth(1.0);
            }
        };

        struct smooth/*_edge*/ : nocopy
        {
            //http://fly.cc.fer.hr/~unreal/theredbook/chapter07.html
        };
        //}

        //namespace fill {
        struct solid/*_fill*/ : nocopy
        {
             solid();
            ~solid();
        };
        //cull
        //{}
        struct wireframe : nocopy
        {
            wireframe()
            {
                glGetIntegerv( GL_POLYGON_MODE, previous );
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // GL_FRONT
            }
            ~wireframe()
            {
                glPolygonMode( GL_FRONT, previous[0] );
                glPolygonMode( GL_BACK,  previous[1] );
            }
            GLint previous[2];
        };
        //}

        namespace light
        {
        }

        namespace depth
        {
        }

        struct color : nocopy
        {
             //explicit color( const moon9::vec4 &c ) { glColor4f( c.x, c.y, c.z, c.a ); }
             explicit color( const moon9::vec3 &c, float a ) { glColor4f( c.x, c.y, c.z, a ); }
             explicit color( const moon9::vec3 &c ) { glColor4f( c.x, c.y, c.z, 1.f ); }
             color( float r, float g, float b, float a = 1.f ) { glColor4f( r, g, b, a ); }
             color( unsigned long rgb, float a ) { glColor4f( ((rgb >> 16) & 0xff ) / 255.f, ((rgb >>  8) & 0xff ) / 255.f, ((rgb >> 0) & 0xff ) / 255.f, a ); }
             explicit color( unsigned long rgba ) { glColor4f( ((rgba >> 24) & 0xff ) / 255.f, ((rgba >> 16) & 0xff ) / 255.f, ((rgba >> 8) & 0xff ) / 255.f, (rgba & 0xff) / 255.f ); }
            ~color() { glColor3f( 1.f, 1.f, 1.f ); }
        };

        namespace arne_happymonster_color_scheme // based on arne v20 + happy monster v2 16-color palettes
        {
            struct       black : color {       black () : color(   0.f/255.f,   0.f/255.f,   0.f/255.f ) {} }; //0
            struct  light_gray : color {  light_gray () : color( 157.f/255.f, 157.f/255.f, 157.f/255.f ) {} }; //1
            struct       white : color {       white () : color( 255.f/255.f, 255.f/255.f, 255.f/255.f ) {} }; //2
            struct         red : color {         red () : color( 190.f/255.f,  38.f/255.f,  51.f/255.f ) {} }; //3
            struct        pink : color {        pink () : color( 224.f/255.f, 111.f/255.f, 139.f/255.f ) {} }; //4
            struct  dark_brown : color {  dark_brown () : color(  73.f/255.f,  60.f/255.f,  43.f/255.f ) {} }; //5
            struct light_brown : color { light_brown () : color( 164.f/255.f, 100.f/255.f,  34.f/255.f ) {} }; //6
            struct      orange : color {      orange () : color( 235.f/255.f, 137.f/255.f,  49.f/255.f ) {} }; //7
            struct      yellow : color {      yellow () : color( 247.f/255.f, 226.f/255.f, 107.f/255.f ) {} }; //8
            struct   dark_blue : color {   dark_blue () : color(  37.f/255.f,  55.f/255.f,  64.f/255.f ) {} }; //9
            struct  dark_green : color {  dark_green () : color(  37.f/255.f,  55.f/255.f,  64.f/255.f ) {} }; //9
            struct       green : color {       green () : color(  68.f/255.f, 137.f/255.f,  26.f/255.f ) {} }; //10
            struct light_green : color { light_green () : color( 163.f/255.f, 206.f/255.f,  39.f/255.f ) {} }; //11
            struct      purple : color {      purple () : color(  98.f/255.f,  50.f/255.f, 150.f/255.f ) {} }; //12
            struct        blue : color {        blue () : color(   0.f/255.f,  87.f/255.f, 132.f/255.f ) {} }; //13
            struct  light_blue : color {  light_blue () : color(  49.f/255.f, 162.f/255.f, 242.f/255.f ) {} }; //14
            struct        cyan : color {        cyan () : color( 178.f/255.f, 220.f/255.f, 239.f/255.f ) {} }; //15
        }

        namespace moon9_color_scheme
        {
            // default roo colour scheme
            // all components always makes ~255+192 to keep tone balance.
            // purple, red and yellow tweak a little bit to fit better with gray colours.

            // intensity+alpha are combined for ease of use when specifing colors at debugging

            struct transparent : color { transparent () : color( 0, 0, 0, 0 ) {} };
            struct       black : color {       black (float i = 1.0) : color(     0.f/255.f,     0.f/255.f,     0.f/255.f, i ) {} }; //0
            struct       white : color {       white (float i = 1.0) : color( i*255.f/255.f, i*255.f/255.f, i*255.f/255.f, i ) {} }; //2

            struct        gray : color {        gray (float i = 1.0) : color(   i*149/255.f,   i*149/255.f,   i*149/255.f, i ) {} };
            struct         red : color {         red (float i = 1.0) : color(   i*255/255.f,    i*48/255.f,    i*48/255.f, i ) {} };
            struct        pink : color {        pink (float i = 1.0) : color(   i*255/255.f,    i*48/255.f,   i*144/255.f, i ) {} };
            struct      orange : color {      orange (float i = 1.0) : color(   i*255/255.f,   i*144/255.f,    i*48/255.f, i ) {} };
            struct      yellow : color {      yellow (float i = 1.0) : color(   i*255/255.f,   i*224/255.f,     i*0/255.f, i ) {} };
            struct       green : color {       green (float i = 1.0) : color(   i*144/255.f,   i*255/255.f,    i*48/255.f, i ) {} };
            struct      purple : color {      purple (float i = 1.0) : color(   i*178/255.f,   i*128/255.f,   i*255/255.f, i ) {} };
            struct        blue : color {        blue (float i = 1.0) : color(     i*0/255.f,   i*192/255.f,   i*255/255.f, i ) {} };
            struct        cyan : color {        cyan (float i = 1.0) : color(    i*48/255.f,   i*255/255.f,   i*144/255.f, i ) {} };
        }


        namespace solarized_color_scheme
        {
            // http://ethanschoonover.com/solarized
            // intensity+alpha are combined for ease of use when specifing colors at debugging

            struct transparent : color { transparent () : color( 0, 0, 0, 0 ) {} };
            struct       black : color {       black (float i = 1.0) : color(     0.f/255.f,     0.f/255.f,     0.f/255.f, i ) {} }; //0
            struct       white : color {       white (float i = 1.0) : color( i*255.f/255.f, i*255.f/255.f, i*255.f/255.f, i ) {} }; //2

            struct        gray : color {        gray (float i = 1.0) : color( 0x002b36, i ) {} }; // base03
            struct         red : color {         red (float i = 1.0) : color( 0xdc322f, i ) {} };
            struct        pink : color {        pink (float i = 1.0) : color( 0xd33682, i ) {} };
            struct      orange : color {      orange (float i = 1.0) : color( 0xcb4b16, i ) {} };
            struct      yellow : color {      yellow (float i = 1.0) : color( 0xb58900, i ) {} };
            struct       green : color {       green (float i = 1.0) : color( 0x859900, i ) {} };
            struct      purple : color {      purple (float i = 1.0) : color( 0x6c71c4, i ) {} };
            struct        blue : color {        blue (float i = 1.0) : color( 0x268bd2, i ) {} };
            struct        cyan : color {        cyan (float i = 1.0) : color( 0x2aa198, i ) {} };
        }

        using namespace moon9_color_scheme;

        namespace alpha
        {
            namespace
            {
                struct preserve
                {
                    preserve() {
                        glGetIntegerv( GL_BLEND_SRC, &src );
                        glGetIntegerv( GL_BLEND_DST, &dst );
                        active = glIsEnabled( GL_BLEND );
                        glEnable( GL_BLEND );
                    }
                    ~preserve() {
                        glBlendFunc( src, dst );
                        if( active ) glEnable( GL_BLEND ); else glDisable( GL_BLEND );
                    }
                    private: GLint src, dst; GLboolean active;
                };
            }

            struct blend : preserve
            {
                blend() { glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); }
            };

            struct saturate : preserve
            {
                saturate() { glBlendFunc( GL_SRC_ALPHA, GL_ONE ); }
            };
        }

    } //style
} // moon9
