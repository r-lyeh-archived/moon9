// (Not so) simple RAII render, wip
// -rlyeh

// prio #1
// use pushattrib y pushmatrix, etc para restaurar donde sea posible
// un push/pop viewport tb

// prio #2
// todo!! split! ie, moon9::2d::raster and moon9::3d::translate
// split visually whats 2d and 3d commands. i spent too much time trying to do an matrix::ortho and render::cube!
// ortho is only for 2d bitmap operations, and cube is a 3d thing. argh. its not clear enough

// todo: this file should be an interface (w/ pImpl maybe) and fully agnostic {

// proposal 2d coord axis: -1,-1 <-> 1,1 safe margins; >1 || <-1 visibility not guaranteed
// -1,-1   |
//         |0,0
// --------+----------
//         |
//         |       1,1

//www.eng.cam.ac.uk/help/tpl/languages/C++/namespaces.html
//http://winterdom.com/dev/cpp/nspaces
//shivavg (opengl), maestrovg(sw)
//http://sourceforge.net/projects/shivavg/
//http://code.google.com/p/maestrovg/

// to check:
// http://altdevblogaday.org/2011/06/23/improving-opengl-error-messages/
// https://github.com/harthur/color

/*
    this is a safer approach

    disable::lighting()
        glPushAttrib(GL_LIGHTING_BIT);
        glDisable(GL_LIGHTING);
    disable::~lighting()
        glPopAttrib();

    enable::lighting()
        glPushAttrib(GL_LIGHTING_BIT);
        glEnable(GL_LIGHTING);
    enable::~lighting()
        glPopAttrib();

*/

#pragma once

#include "os.hpp"
#include <GL/freeglut.h>

#include <vector>
#include <deque>
#include <map>
#include <string>
#include <cassert>
#include <iostream>
#include <mutex>

//#include <moon9/os/mutex.hpp>
//#include <moon9/string/format.hpp>

#include "shader.hpp"
#include "texture.hpp"

#include <moon9/spatial/vec.hpp>
#include <moon9/spatial/spline.hpp>

#include <nanosvg/nanosvg.h>


#include <glm/gtc/type_ptr.hpp>

#include "texture.hpp"



namespace moon9
{
    namespace render_detail
    {
        struct nocopy { nocopy() {} protected: nocopy( const nocopy &other ); nocopy operator=( const nocopy &other ); };

        struct glQuadric //: nocopy
        {
            GLUquadricObj *quadric;

            glQuadric()
            {
                quadric = gluNewQuadric();
                //gluQuadricNormals(quadric, GLU_SMOOTH);
                //gluQuadricTexture(quadric, GL_TRUE);
                //gluQuadricOrientation( quadric, GLU_OUTSIDE ); //GLU_INSIDE
                gluQuadricDrawStyle( quadric, GLU_SILHOUETTE /*GLU_FILL*/ ); //GLU_LINE, GLU_SILHOUETTE, GLU_POINT
            }
            ~glQuadric()
            {
                gluDeleteQuadric(quadric);
            }

            GLUquadricObj *operator()() const
            {
                return quadric;
            }
        };
    }

    namespace matrix
    {
        struct rotate_x : render_detail::nocopy
        {
            explicit rotate_x( const float &degree_360 ) : angle( degree_360 )
            {
                glRotatef(  angle, 1.f, 0.f, 0.f );
            }
            ~rotate_x()
            {
                glRotatef( -angle, 1.f, 0.f, 0.f );
            }
            protected: float angle;
        };

        struct rotate_y : render_detail::nocopy
        {
            explicit rotate_y( const float &degree_360 ) : angle( degree_360 )
            {
                glRotatef(  angle, 0.f, 1.f, 0.f );
            }
            ~rotate_y()
            {
                glRotatef( -angle, 0.f, 1.f, 0.f );
            }
            protected: float angle;
        };

        struct rotate_z : render_detail::nocopy
        {
            explicit rotate_z( const float &degree_360 ) : angle( degree_360 )
            {
                glRotatef(  angle, 0.f, 0.f, 1.f );
            }
            ~rotate_z()
            {
                glRotatef( -angle, 0.f, 0.f, 1.f );
            }
            protected: float angle;
        };

        struct rotate : render_detail::nocopy
        {
            explicit rotate( const float &x360, const float &y360, const float &z360 ) : x(x360), y(y360), z(z360)
            {
                glRotatef( x, 1.f, 0.f, 0.f );
                glRotatef( y, 0.f, 1.f, 0.f );
                glRotatef( z, 0.f, 0.f, 1.f );
            }
            ~rotate()
            {
                glRotatef( -z, 0.f, 0.f, 1.f );
                glRotatef( -y, 0.f, 1.f, 0.f );
                glRotatef( -x, 1.f, 0.f, 0.f );
            }
            protected: float x,y,z;
        };

        struct scale : render_detail::nocopy
        {
            explicit scale( const float &_factor ) : v( _factor, _factor, _factor ), is_valid( v.x != 0.f && v.y != 0.f && v.z != 0.f )
            {
                if( is_valid )
                    glScalef( v.x, v.y, v.z );
            }
            scale( float x, float y, float z ) : v(x,y,z), is_valid( v.x != 0.f && v.y != 0.f && v.z != 0.f )
            {
                if( is_valid )
                    glScalef( v.x, v.y, v.z );
            }
            explicit scale( const moon9::vec3 &_v ) : v(_v), is_valid( v.x != 0.f && v.y != 0.f && v.z != 0.f )
            {
                if( is_valid )
                    glScalef( v.x, v.y, v.z );
            }
            ~scale()
            {
                if( is_valid )
                    glScalef( 1.f / v.x, 1.f / v.y, 1.f / v.z );
            }
            protected: moon9::vec3 v; bool is_valid;
        };

        struct translate : render_detail::nocopy
        {
            explicit translate( const moon9::vec3 &_v ) : v( _v )
            {
                glTranslatef( v.x, v.y, v.z );
                position += v;
            }
            explicit translate( const float &x, const float &y, const float &z ) : v( x,y,z )
            {
                glTranslatef( v.x, v.y, v.z );
                position += v;
            }
            ~translate()
            {
                position -= v;
                glTranslatef( -v.x, -v.y, -v.z );
            }
            static moon9::vec3 get()
            {
                return position;
            }
            protected: moon9::vec3 v;
            static moon9::vec3 position;
        };

#if 0
        struct offset : render_detail::nocopy
        {
            offset( const moon9::vec3 &v )
            {
                GLfloat matrix[16];
                glGetFloatv(GL_PROJECTION_MATRIX, matrix);

                org.x = matrix[13];
                org.y = matrix[14];
                org.z = matrix[15];

                off = org + v;

                glTranslatef( off.x, off.y, off.z );
            }
            ~offset()
            {
                glTranslatef( -off.x, -off.y, -off.z );
            }
            protected: moon9::vec3 off, org;
        };
#endif

        struct inc_rotate_x : render_detail::nocopy
        {
            explicit inc_rotate_x( const float &angle_360 )
            {
                glRotatef( angle_360, 1.f, 0.f, 0.f );
            }
        };
        struct inc_rotate_y : render_detail::nocopy
        {
            explicit inc_rotate_y( const float &angle_360 )
            {
                glRotatef( angle_360, 0.f, 1.f, 0.f );
            }
        };
        struct inc_rotate_z : render_detail::nocopy
        {
            explicit inc_rotate_z( const float &angle_360 )
            {
                glRotatef( angle_360, 0.f, 0.f, 1.f );
            }
        };
        struct inc_scale : render_detail::nocopy
        {
            explicit inc_scale( const float &factor )
            {
                glScalef( factor, factor, factor );
            }
        };
        struct inc_translate : render_detail::nocopy
        {
            explicit inc_translate( const moon9::vec3 &v )
            {
                glTranslatef( v.x, v.y, v.z );
            }
        };

        struct push : render_detail::nocopy   // preserve
        {
            push()
            {
                glPushMatrix();
            }
            ~push()
            {
                glPopMatrix();
            }
        };

        struct transform : render_detail::nocopy
        {
            explicit transform( float *m )
            {
                glPushMatrix();
                glMultMatrixf(m);
            }

            explicit transform( const glm::mat4 &matrix )
            {
                glPushMatrix();
                glMultMatrixf(/*(GLfloat*)*/glm::value_ptr(matrix) );
            }

            ~transform()
            {
                glPopMatrix();
            }
        };

/*
        struct load : render_detail::nocopy
        {
            load( float *m )
            {
                glPushMatrix();
                glLoadMatrixf(m);
            }

            ~load()
            {
                glPopMatrix();
            }
        };
*/

        // proposal: cartesian axis 2d, x[-1,1] y[1,-1] o[0,0]
        // right now: from (0,0) to (w,h), o[w/2,h/2]
        struct ortho : render_detail::nocopy
        {
            explicit ortho( const bool &topleft = true ) // false = bottom-left
            {
//                glPushAttrib( GL_TRANSFORM_BIT );

                struct { GLint x, y, width, height; } viewport;
                glGetIntegerv(GL_VIEWPORT, &viewport.x);

                glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                glLoadIdentity();

                #if 1
                if( topleft )
                glOrtho(0,viewport.width,viewport.height,0,0,1);
                else
	            glOrtho(0,viewport.width,0,viewport.height,0,1);
                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                glLoadIdentity();
                //glTranslatef(0,0,0);
                #else
                gluOrtho2D( viewport.x, viewport.width, viewport.y, viewport.height );
                #endif
            }

            ~ortho()
            {
                glMatrixMode(GL_MODELVIEW);
                glPopMatrix();
                glMatrixMode(GL_PROJECTION);
                glPopMatrix();

//                glPopAttrib();
            }
        };
    } //matrix


    namespace invert
    {
        struct normals : render_detail::nocopy
        {
            normals()
            {
                glGetIntegerv( GL_FRONT_FACE, &mode );
                glFrontFace( mode == GL_CW ? GL_CCW : GL_CW );
            }

            ~normals()
            {
                glFrontFace( mode );
            }

            protected: GLint mode;
        };
    }

    namespace disable
    {
        struct scissor : render_detail::nocopy
        {
             scissor() { glDisable( GL_SCISSOR_TEST ); }
            ~scissor() {  glEnable( GL_SCISSOR_TEST ); }
        };

        struct blending : render_detail::nocopy
        {
            blending() { glDisable(GL_BLEND); }
           ~blending() { glEnable(GL_BLEND); }
        };

        struct depth : render_detail::nocopy
        { //bool on; GLboolean glIsEnabled( GLenum      cap);
               depth() { glDisable( GL_DEPTH_TEST ); }
              ~depth() {  glEnable( GL_DEPTH_TEST ); }
        };

        struct lighting : render_detail::nocopy
        {
               lighting() { glDisable( GL_LIGHTING ); }
              ~lighting() {  glEnable( GL_LIGHTING ); }
        };

        struct culling : render_detail::nocopy
        {
               culling() { glDisable( GL_CULL_FACE ); }
              ~culling() {  glEnable( GL_CULL_FACE ); }
        };

        struct texturing : render_detail::nocopy
        {
               texturing() { glDisable( GL_TEXTURE_2D ); }
              ~texturing() {  glEnable( GL_TEXTURE_2D ); }
        };

        struct fog : render_detail::nocopy
        {
               fog() { glDisable( GL_FOG ); }
              ~fog() {  glEnable( GL_FOG ); }
        };
    }

    namespace enable
    {
        struct alpha : render_detail::nocopy
        {
             explicit alpha( float value = 0.5f ) { glAlphaFunc(GL_GREATER, value); glEnable(GL_ALPHA_TEST); }
            ~alpha()                     { glDisable(GL_ALPHA_TEST); }
        };

        struct depth : render_detail::nocopy
        {
             depth() { glEnable(GL_DEPTH_TEST); }
            ~depth() { glDisable(GL_DEPTH_TEST); }
        };

        struct scissor : render_detail::nocopy
        {
             scissor(float x, float y, float w, float h) { glScissor( x, y, w, h ); glEnable(GL_SCISSOR_TEST); }
             scissor() { glEnable(GL_SCISSOR_TEST); }
            ~scissor() { glDisable(GL_SCISSOR_TEST); }
        };

        struct lighting : render_detail::nocopy
        {
               lighting() {  glEnable( GL_LIGHTING ); }
              ~lighting() { glDisable( GL_LIGHTING ); }
        };

        struct blending : render_detail::nocopy
        {
            // FIXME
            // http://www.opengl.org/resources/faq/technical/transparency.htm

            blending()
            {
                glEnable (GL_BLEND);
                glDisable(GL_DEPTH_TEST);

                //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glBlendFunc( GL_SRC_ALPHA, GL_ONE );
                //glBlendFunc(GL_ONE, GL_ONE);

                glColor4f( 1.f, 1.f, 1.f, 0.5f );
            }

            ~blending()
            {
                glDisable( GL_BLEND );
                glEnable( GL_DEPTH_TEST );

                //glBlendFunc( GL_SRC_ALPHA, GL_ONE );

                glColor4f( 1.f, 1.f, 1.f, 1.f );
            }
        };

        struct billboard : render_detail::nocopy
        {
            explicit billboard( const bool &bSpherical = true )
            {
                if( bSpherical ) // cheap spherical billboard
                {
                    float modelview[16];

                    // save the current modelview matrix
                    glPushMatrix();

                    // get the current modelview matrix
                    glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

                    // undo all rotations
                    // beware all scaling is lost as well
                    /*
                    for( int i=0; i<3; i++ )
                    for( int j=0; j<3; j++ ) {
                    if ( i==j )
                    modelview[i*4+j] = 1.0;
                    else
                    modelview[i*4+j] = 0.0;
                    }
                    */

                    modelview[0*4+0] = 1.0;
                    modelview[0*4+1] = 0.0;
                    modelview[0*4+2] = 0.0;

                    modelview[1*4+0] = 0.0;
                    modelview[1*4+1] = 1.0;
                    modelview[1*4+2] = 0.0;

                    modelview[2*4+0] = 0.0;
                    modelview[2*4+1] = 0.0;
                    modelview[2*4+2] = 1.0;

                    // set the modelview with no rotations
                    glLoadMatrixf(modelview);
                }
                else
                    if( 1 ) // cheap cylindrical billboard
                    {
                        float modelview[16];

                        // save the current modelview matrix
                        glPushMatrix();

                        // get the current modelview matrix
                        glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

                        /*
                        for( int i=0; i<3; i+=2 )
                        for( int j=0; j<3; j++ ) {
                        if ( i==j )
                        modelview[i*4+j] = 1.0;
                        else
                        modelview[i*4+j] = 0.0;
                        }
                        */

                        modelview[0*4+0] = 1.0;
                        modelview[0*4+1] = 0.0;
                        modelview[0*4+2] = 0.0;

                        modelview[2*4+0] = 0.0;
                        modelview[2*4+1] = 0.0;
                        modelview[2*4+2] = 0.0;

                        // set the modelview matrix
                        glLoadMatrixf(modelview);
                    }

                    // add missing ones from here
                    // http://www.lighthouse3d.com/opengl/billboarding/index.php?billCyl

            }
            ~billboard()
            {
                // restore the previously
                // stored modelview matrix
                glPopMatrix();
            }
        };
    };

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

        struct texture : render_detail::nocopy
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

        struct debug : render_detail::nocopy
        {
            disable::texturing t;
            disable::lighting l;
            disable::depth d;

             debug() {}
            ~debug() {}
        };

        struct zdebug : render_detail::nocopy
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
        struct dotted/*_edge*/ : render_detail::nocopy
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

        struct dotdashed/*_edge*/ : render_detail::nocopy
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

        struct dashed/*_edge*/ : render_detail::nocopy
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

        struct thick/*_edge*/ : render_detail::nocopy
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

        struct smooth/*_edge*/ : render_detail::nocopy
        {
            //http://fly.cc.fer.hr/~unreal/theredbook/chapter07.html
        };
        //}

        //namespace fill {
        struct solid/*_fill*/ : render_detail::nocopy
        {
             solid();
            ~solid();
        };
        //cull
        //{}
        struct wireframe : render_detail::nocopy
        {
            wireframe()
            {
                glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
                glPolygonMode(GL_FRONT, GL_LINE);
            }
            ~wireframe()
            {
                glPolygonMode(GL_FRONT, polygonMode);
            }
            GLint polygonMode;
        };
        //}

        namespace light
        {
        }

        namespace depth
        {
        }

        struct color : render_detail::nocopy
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
    } //style


    namespace geometry
    {
        static const float  zero[] = { 0.f, 0.f, 0.f };
        static const float halfx[] = { 0.5f, 0.f, 0.f };
        static const float halfy[] = { 0.f, 0.5f, 0.f };
        static const float halfz[] = { 0.f, 0.f, 0.5f };
        static const float  onex[] = { 1.f, 0.f, 0.f };
        static const float  oney[] = { 0.f, 1.f, 0.f };
        static const float  onez[] = { 0.f, 0.f, 1.f };
        static const float nonex[] = { -1.f, 0.f, 0.f };
        static const float noney[] = { 0.f, -1.f, 0.f };
        static const float nonez[] = { 0.f, 0.f, -1.f };

        struct point : moon9::render_detail::nocopy
        {
            point( const int size = 16 )
            {
                glPointSize( size );
                glEnable(GL_POINT_SMOOTH);

                glBegin( GL_POINTS );
                glVertex3fv( zero );
                glEnd();

                glDisable(GL_POINT_SMOOTH);
                glPointSize( 1 );
            }
        };

        struct line : moon9::render_detail::nocopy
        {
            explicit
            line( const moon9::vec3 &v = moon9::float110() )
            {
                glBegin(GL_LINES);
                    glVertex3f(0,0,0);
                    glVertex3f(v[0],v[1],v[2]);
                glEnd();
            }
        };

        struct normal : moon9::render_detail::nocopy
        {
            normal()
            {
                glBegin(GL_LINES);
                    glVertex3f(0,0,0);
                    glVertex3f(0,0,1);
                glEnd();
            }
        };

        struct points : moon9::render_detail::nocopy
        {
            template< typename T, const size_t N >
            explicit points( const T (&pointList)[ N ], int size = 16 )
            {
                if( N < 1 ) return;

                glPointSize( size );
                glEnable(GL_POINT_SMOOTH);

                glBegin( GL_POINTS );
                    for( size_t i = 0, end = N/3; i < end; ++i )
                        glVertex3fv( &pointList[i] );
                glEnd();

                glDisable(GL_POINT_SMOOTH);
                glPointSize( 1 );
            }

            explicit points( const float *pointList, size_t N, int size = 16 )
            {
                assert( pointList != 0 );
                assert( N > 0 );

                glPointSize( size );
                glEnable(GL_POINT_SMOOTH);

                glBegin( GL_POINTS );
                    for( size_t i = 0, end = N/3; i < end; ++i )
                        glVertex3fv( &pointList[i*3] );
                glEnd();

                glDisable(GL_POINT_SMOOTH);
                glPointSize( 1 );
            }

            explicit points( const std::vector< moon9::vec3 > &pointList, int size = 16 )
            {
                size_t N = pointList.size();
                if( !N ) return;

                glPointSize( size );
                glEnable(GL_POINT_SMOOTH);

                glBegin( GL_POINTS );
                    for( size_t i = 0; i < N; ++i )
                        glVertex3fv( pointList[i].data() );
                glEnd();

                glDisable(GL_POINT_SMOOTH);
                glPointSize( 1 );
            }
        };

        struct lines : moon9::render_detail::nocopy
        {
            // arrow line

            template< typename T, const size_t N >
            explicit lines( const T (&pointList)[ N ] )
            {
                if( N < 2 ) return;

                glBegin( GL_LINE_STRIP );

                    for( size_t i = 0, end = N/3; i < end; ++i )
                        glVertex3fv( &pointList[i] );

                glEnd();
            }

            explicit lines( const float *pointList, size_t N )
            {
                assert( pointList != 0 );
                assert( N >= 3 );

#if 1
                if( N < 3 ) return;

                glBegin( GL_LINE_STRIP );

                    for( size_t i = 0, end = N/3; i < end; ++i )
                        glVertex3fv( &pointList[i*3] );

                glEnd();
#else
                // activate and specify pointer to vertex array
                glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(3, GL_FLOAT, 0, pointList);

                // draw a cube
                glDrawArrays(GL_TRIANGLES, 0, N/3);

                // deactivate vertex arrays after drawing
                glDisableClientState(GL_VERTEX_ARRAY);
#endif
            }

            explicit lines( const std::vector< moon9::vec3 > &pointList )
            {
                size_t N = pointList.size();

                if( N < 2 ) return;

                glBegin( GL_LINE_STRIP );

                    for( size_t i = 0; i < N; ++i )
                        glVertex3fv( pointList[i].data() );

                glEnd();
            }
        };

        struct curve : moon9::render_detail::nocopy // spliner
        {
            // arrow spline
            // pointlist, true, detail

            explicit
            curve( const moon9::spline<moon9::vec3> &knotList, const float &detail01 = 0.5f )
            {
                assert( knotList.size() >= 4 );

                const int STEPS = knotList.size() * int(detail01 * 7.0f + 1.f); //0..1 -> 1..8 -> size..size*8

                glBegin( GL_LINE_STRIP );

                    for (int i = 0; i <= STEPS; ++i)
                    {
                        float dt01 = i / (float)STEPS;

                        moon9::vec3 point = knotList.atdt( dt01 );

                        glVertex3fv( &point.x );
                    }

                glEnd();
            }
        };

        struct pyramid : moon9::render_detail::nocopy
        {
            pyramid()
            {
                glutSolidTetrahedron();
            }
        };

        struct square : moon9::render_detail::nocopy
        {
            square()
            {
                glutSolidSphere( 1.0, 2, 2 );
            }
        };

        struct cube : moon9::render_detail::nocopy
        {
            cube()
            {
#if 1
                glutSolidCube( 1.0 );
#else
                float pointList[] =
                {
                    +0.5f, +0.5f, +0.5f, //9 up
                    +0.5f, -0.5f, +0.5f, //3 up
                    -0.5f, -0.5f, +0.5f, //1 up
                    -0.5f, +0.5f, +0.5f, //7 up
                    +0.5f, +0.5f, +0.5f, //9 up

                    +0.5f, +0.5f, -0.5f, //9 down
                    +0.5f, -0.5f, -0.5f, //3 down
                    -0.5f, -0.5f, -0.5f, //1 down
                    -0.5f, +0.5f, -0.5f, //7 down
                    +0.5f, +0.5f, -0.5f  //9 down
                };

                lines ln( pointList, sizeof(pointList) / sizeof(pointList[0]) );

                glBegin(GL_LINES);
                    glVertex3f(+0.5f,-0.5f,+0.5f); // 3 up
                    glVertex3f(+0.5f,-0.5f,-0.5f); // 3 down
                    glVertex3f(-0.5f,-0.5f,+0.5f); // 1 up
                    glVertex3f(-0.5f,-0.5f,-0.5f); // 1 down
                    glVertex3f(-0.5f,+0.5f,+0.5f); // 7 up
                    glVertex3f(-0.5f,+0.5f,-0.5f); // 7 down
                glEnd();

#endif
            }
        };

        struct semicircle : moon9::render_detail::nocopy
        {
            explicit semicircle( bool closed, const float &detail01 = 0.5f )
            {
                //detail -> [16..32] segments
                int segments = int( detail01 * 16 + 16.0 );

                float theta = 2.f * 3.1415926f / float( segments );
                float c = std::cosf( theta );
                float s = std::sinf( theta );
                float t;

                float x = 1.0;//we start at angle = 0
                float y = 0;

                segments = ( segments / 2 ) + 1;

                glBegin( closed ? GL_LINE_LOOP : GL_LINE_STRIP );
                for( int i = 0; i < segments; i++ )
                {
                    glVertex3f(x, y, 0);//output vertex

                    //apply the rotation matrix
                    t = x;
                    x = c * x - s * y;
                    y = s * t + c * y;
                }
                glEnd();
            }
        };

        struct circle : moon9::render_detail::nocopy
        {
            explicit circle( const float &detail01 = 0.5f )
            {
                //glutSolidSphere( 3.0, 2, 6 );

                //detail -> [16..32] segments
                int segments = int( detail01 * 16 + 16.f );

                float theta = 2.f * 3.1415926f / float( segments );
                float c = std::cosf( theta );
                float s = std::sinf( theta );
                float t;

                float x = 1.0;//we start at angle = 0
                float y = 0;

                glBegin(GL_LINE_LOOP);
                for( int i = 0; i < segments; i++ )
                {
                    glVertex3f(x, y, 0);//output vertex

                    //apply the rotation matrix
                    t = x;
                    x = c * x - s * y;
                    y = s * t + c * y;
                }
                glEnd();
            }
        };

        // capsule2d
        struct capsule2d : moon9::render_detail::nocopy
        {
            capsule2d( float radius, float height, const float &detail01 = 0.5f )
            {
                    float qx = 0.f, qy = 0.f;
                    float px = 0.f, py = height, r = radius;
                    float dx = qx-px;
                    float dy = qy-py;
                    float d = 1.0f/sqrtf(dx*dx+dy*dy);
                    dx*=d; dy*=d;
                    py-=r/2; qy-=r/2;
                    float nx = dy;
                    float ny = -dx;
                    int hn = ( detail01 * 16 + 16 )/2;
                    glBegin(GL_LINE_LOOP);
                    for (int i = 0; i < hn; ++i)
                    {
                            const float u = (float)i/(float)(hn-1)*(float)3.14159f; //M_PI;
                            const float rx = cosf(u)*r;
                            const float ry = sinf(u)*r;
                            float x = qx + rx*nx + ry*dx;
                            float y = qy + rx*ny + ry*dy;
                            glVertex2f(x,y);
                    }
                    for (int i = 0; i < hn; ++i)
                    {
                            const float u = 3.14159f /*M_PI*/ + (float)i/(float)(hn-1)*(float)3.14159f; //M_PI;
                            const float rx = cosf(u)*r;
                            const float ry = sinf(u)*r;
                            float x = px + rx*nx + ry*dx;
                            float y = py + rx*ny + ry*dy;
                            glVertex2f(x,y);
                    }
                    glEnd();
            }
        };

        struct arrow : moon9::render_detail::nocopy
        {
            explicit arrow( const moon9::vec2 &v = moon9::vec2(1,1) )
            {
                    const float px = 0.f, py = 0.f, qx = v.x, qy = v.y, w = sqrt( qx * qx + qy * qy ) * 0.5f;
                    float dx = qx-px;
                    float dy = qy-py;
                    float d = 1.0f/sqrtf(dx*dx+dy*dy);
                    dx*=d; dy*=d;
                    glBegin(GL_LINES);
                    glVertex2f(px,py);
                    glVertex2f(qx,qy);
                    glVertex2f(qx,qy);
                    glVertex2f(qx-dx*w-dy*w/2,qy-dy*w+dx*w/2);
                    glVertex2f(qx,qy);
                    glVertex2f(qx-dx*w+dy*w/2,qy-dy*w-dx*w/2);
                    glEnd();
            }
        };

        struct diamond : moon9::render_detail::nocopy
        {
            diamond()
            {
                glutSolidSphere( 1.0, 4, 2 );
            }
        };

        struct teapot : moon9::render_detail::nocopy
        {
            teapot()
            {
                glutSolidTeapot( 1.0 );
            }
        };

        struct convexhull : moon9::render_detail::nocopy
        {
            convexhull()
            {
            }
        };

        // http://zeuxcg.org/2010/10/17/aabb-from-obb-with-component-wise-abs/#more-268

        struct aabb : moon9::render_detail::nocopy
        {
            aabb()
            {
            }
        };

        struct obb : moon9::render_detail::nocopy
        {
            obb()
            {
            }
        };

        // useful?

        struct triangle : moon9::render_detail::nocopy
        {
            triangle()
            {
            }
        };

        struct axes : moon9::render_detail::nocopy
        {
            explicit axes( bool draw_negative_axes = true )
            {
                disable::lighting l;
                //glLineWidth (2.0);

                glPushMatrix();

                    if( draw_negative_axes )
                    {
                        style::dotted d;
                        glBegin (GL_LINES);
                            style::red   r; glVertex3fv( zero ); glVertex3fv( nonex );
                            style::green g; glVertex3fv( zero ); glVertex3fv( noney );
                            style::blue  b; glVertex3fv( zero ); glVertex3fv( nonez );
                        glEnd ();
                    }

                    glBegin (GL_LINES);
                        style::red   r; glVertex3fv( zero ); glVertex3fv( onex );
                        style::green g; glVertex3fv( zero ); glVertex3fv( oney );
                        style::blue  b; glVertex3fv( zero ); glVertex3fv( onez );
                    glEnd ();

                    glRasterPos3f( 0.0f, 0.0f, 1.1f );
                    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, 'z' );

                    style::green _g;
                    glRasterPos3f( 0.0f, 1.1f, 0.0f );
                    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, 'y' );

                    style::red _r;
                    glRasterPos3f( 1.1f, 0.0f, 0.0f );
                    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, 'x' );

                glPopMatrix();
            }
        };

        struct coord3
        {
            coord3( const vec3 &pt )
            {
                style::dotted line;
                disable::blending noshading;

                std::vector<vec3> v( 2, pt );

                style::green color1;
                v[0] = pt * float110();
                v[1] = v[0] * float100();
                geometry::lines axis1( v );

                style::red color2;
                v[0] = pt * float110();
                v[1] = v[0] * float010();
                geometry::lines axis2( v );

                style::cyan color3;
                v[0] = pt;
                v[1] = v[0] * float110();
                geometry::lines axis3( v );
            }
        };

        struct sphere : moon9::render_detail::nocopy
        {
            explicit sphere( const float &detail01 = 0.5f )
            {
#if 0
                static render_detail::glQuadric quadric;
                int idetail = int(detail01 * 10) + 6; //0..1 = 4..16
                gluSphere(quadric(),1.0f,idetail,idetail);
#else
                geometry::circle cl1( detail01 );
                matrix::rotate_x rx( 90.f );
                geometry::circle cl2( detail01 );
                matrix::rotate_y ry( 90.f );
                geometry::circle cl3( detail01 );

                enable::billboard bl( true );
                geometry::circle outer( detail01 );

#endif
            }
        };

        struct semisphere : moon9::render_detail::nocopy
        {
            explicit semisphere( const float &detail01 = 0.5f )
            {
                geometry::semicircle scl1( false, detail01 );
                matrix::rotate_y ry( 90.f );
                geometry::semicircle scl2( false, detail01 );
                matrix::rotate_x rx( 90.f );

                geometry::circle cl1( detail01 );
            }
        };

        struct capsule : moon9::render_detail::nocopy
        {
            capsule( float radius, float height, const float &detail01 = 0.5f )
            {
                // xyz
                {
                    capsule2d c1( radius, height, detail01 );
                    matrix::rotate_y ry1( 60 );
                    capsule2d c2( radius, height, detail01 );
                    matrix::rotate_y ry2( 60 );
                    capsule2d c3( radius, height, detail01 );
                }
                {
                    matrix::translate tr( 0, height/2, 0 );
                    enable::billboard bd( true );
                    geometry::circle cl( detail01 );
                }
                {
                    matrix::translate tr( 0, -height/2, 0 );
                    enable::billboard bd( true );
                    geometry::circle cl( detail01 );
                }
            }
        };

        struct cylinder : moon9::render_detail::nocopy
        {
            explicit cylinder( const float &detail01 = 0.5f )
            {
                static render_detail::glQuadric quadric;
                int idetail = int(detail01 * 10) + 6; //0..1 = 4..16
                gluCylinder(quadric(),1.0f,1.0f,1.0f,idetail,idetail);
            }
        };

        struct cone : moon9::render_detail::nocopy
        {
            explicit cone( const float &detail01 = 0.5f )
            {
                static render_detail::glQuadric quadric;
                int idetail = int(detail01 * 10) + 6; //0..1 = 4..16
                gluCylinder(quadric(),1.0f,0.0f,1.0f,idetail,idetail);
            }
        };

        struct sector : moon9::render_detail::nocopy
        {
            sector( int degree, const float &detail01 = 0.5f )
            {
                static render_detail::glQuadric quadric;
                int idetail = int(detail01 * 10) + 6; //0..1 = 4..16
                gluPartialDisk(quadric(),0.5f,1.5f,idetail,idetail, 180-degree/2, degree );
            }
        };

        struct disk : moon9::render_detail::nocopy
        {
            explicit disk( const float &detail01 = 0.5f )
            {
                //sector( 360 );
                static render_detail::glQuadric quadric;
                int idetail = int(detail01 * 10) + 6; //0..1 = 4..16
                gluDisk(quadric(),0.5f,1.5f,idetail,idetail);
            }
        };

        struct pacman : moon9::render_detail::nocopy
        {
            explicit pacman( const float &detail01 = 0.5f )
            {
                //sector( 360 );
                static render_detail::glQuadric quadric;
                int idetail = int(detail01 * 10) + 6; //0..1 = 4..16
                gluPartialDisk( quadric(), 0.0, 0.8, 30, 1, -45, 270 );
            }
        };

        /*

        to study:

        // the gluDisk based shapes
        gluDisk (q, 0.0, 0.8, 30, 1);  // Solid Circle
        gluDisk (q, 0.6, 0.8, 30, 1);  // ring
        gluDisk (q, 0.6, 0.8, 4, 1);   // hollow square
        gluDisk (q, 0.6, 0.8, 3, 1);   // hollow triangle
        gluDisk (q, 0.0, 0.8, 3, 1);   // solid triangle
        gluDisk (q, 0.0, 0.8, 4, 1);   // solid square
        gluDisk (q, 0.0, 0.8, 6, 1);   // solid Hexagon
        gluDisk (q, 0.6, 0.8, 6, 1);   // hollow hexagon (think beehive)
        Now the Sphere using gluSphere.

        // the gluSphere based ones...
        gluSphere (q, 0.8, 20, 20);    // a sphere
        gluSphere (q, 0.8, 20, 20);    // a smartie ...when scaled of course
        gluSphere (q, 0.8, 20, 20);    // an ellipsoid...same here, fix later
        gluSphere (q, 0.8, 3, 20);     // a seed like shape?
        gluSphere (q, 0.8, 4, 20);     // an ikea paper lamp?
        gluSphere (q, 0.8, 4, 2);      // diamond
        gluSphere (q, 0.8, 3, 3);      // triangular crystal
        gluSphere (q, 0.8, 6, 3);      // quartz crystal
        Thirdly the Cylinder using gluCylinder.

        // some interesting cylinders
        gluCylinder (q, 0.4, 0.4, 0.8, 20, 20);  // cylinder
        gluCylinder (q, 0.4, 0.4, 0.8, 4, 20);   // square tube
        gluCylinder (q, 0.4, 0.4, 0.8, 3, 20);   // triangular tube
        gluCylinder (q, 0.4, 0.4, 0.8, 6, 20);   // hexagonal tube
        gluCylinder (q, 0.4, 0.0, 0.8, 20, 20);  // cone
        gluCylinder (q, 0.4, 0.0, 0.8, 4, 20);   // square base pyramid
        gluCylinder (q, 0.4, 0.0, 0.8, 3, 20);   // triangular based pyramid
        gluCylinder (q, 0.4, 0.0, 0.8, 6, 20);   // hexagonal based pyramid
        Lastly gluPartialDisk.

        // some partial disk shapes.
        gluPartialDisk (q, 0.0, 0.8, 30, 1, -45, 270);  // pacman
        gluPartialDisk (q, 0.6, 0.8, 30, 1,0,180);      // half ring
        gluPartialDisk (q, 0.6, 0.8, 3, 1,0,270);       // square ring partial
        gluPartialDisk (q, 0.6, 0.8, 2, 1,0, 240);      // triangle ring partial
        gluPartialDisk (q, 0.0, 0.8, 2, 1, 0, 240);     // arrow head
        gluPartialDisk (q, 0.0, 0.8, 3, 1,0,270);       // square partial
        gluPartialDisk (q, 0.0, 0.8, 5, 1,0,300);       // Hexagon partial
        gluPartialDisk (q, 0.6, 0.8, 5, 1,0,300);       // Hexagon ring partial



        void mesh( mn::string modelPath, size_t mask )
        {
            bool ret = mesh::load_and_render( modelPath, mask );

            mn::debug::check( ret );
        }

        bool mesh_md2( mn::string modelPath, mn::string anim, float dt01, bool interpolated )
        {
            mn::debug::check( dt01 >= 0.f && dt01 <= 1.f );

            static mn::map< mn::string, MD2Obj > modelMgr;

            MD2Obj *pObj, Obj;

            if( modelMgr.find( modelPath ) == modelMgr.end() )
            {
                // Load our Object
                if( Obj.Load( modelPath.c_str() ) != MD2_OK )
                {
                    std::cout<<"Unable to load Object!" << std::endl;
                    return false;
                }

                modelMgr[ modelPath ] = Obj;

                pObj = &Obj;
            }
            else
            {
                pObj = &modelMgr[ modelPath ];
            }

            // Draw our Object
            matrix::rotate_x< -90 > m;
            matrix::rotate_z< -90 > n;

            if( interpolated )
                pObj->DrawInterpolated( anim, dt01 );
            else
                pObj->Draw( anim, dt01 );

            return true;
        }
        */

#if 0

        struct translate2d_rel
        {
                // -1,+1     |
                //           |
                //           |0,0
                // ----------+----------
                //           |
                //           |
                //           |     +1,-1
        };

        struct translate2d_px : moon9::render_detail::nocopy
        {
            translate2d_px( const vec2 &pos ) : backup(world2scr())
            {
                glRasterPos2f( pos.x, pos.y );
            }

            ~translate2d_px()
            {
                glRasterPos2f( backup.x, backup.y );
            }

            protected:

            vec2 backup;
            vec2 world2scr()
            {
                vec2 pos;

                glRasterPos2f( 0, 0 );

                float pos[4] = { 0,0,0,0 };
                glGetFloatv( GL_CURRENT_RASTER_POSITION, pos );

                struct { GLint x, y, width, height; } viewport;
                glGetIntegerv(GL_VIEWPORT, &viewport.x);

                pos[1] = viewport.height - pos[1];

                return pos;
            }
        };

#endif

        struct text2d : moon9::render_detail::nocopy
        {
            text2d( int x, int y, const std::string &str )
            {
                // pos: position in physical pixels
                // fixme1: evaluate position according to screen window
                // fixme2: evaluate position in normalized 2d coords
                // -1,+1     |     +1,+1
                //           |
                //           |0,0
                // ----------+----------
                //           |
                //           |
                // -1,-1     |     +1,-1

                matrix::ortho flat;

                float data[4];
                glGetFloatv( GL_CURRENT_RASTER_POSITION, data );

#if 0
                // glut
                moon9::strings strs = moon9::string( str ).tokenize("\r\n");

                for( size_t i = 0; i < strs.size(); ++i )
                {
                    glRasterPos2f( pos.x, pos.y + 12 + 12 * i );
                    for( const char *string = strs[i].c_str(); *string; )
                        glutBitmapCharacter( GLUT_BITMAP_8_BY_13, *string++ );
                }
#else
                // freeglut
                glRasterPos4i( int(x), int(y + 12), 0, 1 );
                glutBitmapString( GLUT_BITMAP_8_BY_13, (const unsigned char *)str.c_str() );
#endif

                glRasterPos4fv( data );
            }

            #if 0
            text2d( const std::string &str )
            {
                /* world2scr pos { */
                glRasterPos2f( 0, 0 );

                float pos[4] = { 0,0,0,0 };
                glGetFloatv( GL_CURRENT_RASTER_POSITION, pos );

                struct { GLint x, y, width, height; } viewport;
                glGetIntegerv(GL_VIEWPORT, &viewport.x);

                pos[1] = viewport.height - pos[1];
                /* } */

                text2d( str, vec2( pos[0], pos[1] ) );
            }
            #endif
        };

        struct text3d : moon9::render_detail::nocopy
        {
            text3d( const std::string &str, const float scale_factor = 0.0015f )
            {
                matrix::scale s(scale_factor);
                matrix::push p;           // because glTranslatef is used to advance width of character

                for( const char *string = str.c_str(); *string; )
                    glutStrokeCharacter( GLUT_STROKE_ROMAN, *string++ );
            }
        };

        //vectors?

        struct vector : moon9::render_detail::nocopy
        {
            explicit vector( const moon9::vec3 &direction )
            {
                glBegin( GL_LINES );

                    glVertex3fv( zero );
                    glVertex3fv( direction.data() );

                glEnd();
            }
        };

        struct plane : moon9::render_detail::nocopy
        {
            explicit plane( float texture_scale = 1.f )
            {
                glBegin( GL_QUADS );

                    glTexCoord2f( 0.f,  0.f );
                    glVertex3f( -1.f, -0.0001f, -1.f );

                    glTexCoord2f( 0.f, texture_scale );
                    glVertex3f(  -1.f, -0.0001f, 1.f );

                    glTexCoord2f( texture_scale, texture_scale );
                    glVertex3f(  1.f, -0.0001f,  1.f );

                    glTexCoord2f( texture_scale, 0.f );
                    glVertex3f( 1.f, -0.0001f,  -1.f );

                glEnd();
            }
        };

        struct quad : moon9::render_detail::nocopy //rect,square,quad
        {
            explicit quad( float texture_scale = 1.f )
            {
                glBegin( GL_QUADS );

                    glTexCoord2f( -texture_scale, 0.f );
                    glVertex2f( -1.f, -1.f );

                    glTexCoord2f( -texture_scale, -texture_scale );
                    glVertex2f( -1.f, 1.f );

                    glTexCoord2f( 0.f, -texture_scale );
                    glVertex2f( 1.f, 1.f );

                    glTexCoord2f( 0.f,  0.f );
                    glVertex2f( 1.f, -1.f );

                glEnd();
            }
        };

        struct svg : moon9::render_detail::nocopy
        {
            explicit svg( const std::string &pathFile )
            {
                // Load
                struct SVGPath* plist;
                plist = svgParseFromFile( pathFile.c_str() ); //"fg.svg"); //bg.svg"); tiger.svg");

                /*
                glColor3fv(color);
                glPolygonMode(MaterialFace.Front, PolygonMode.Fill);
                glBegin(BeginMode.Polygon);
                for (int j = 0; j < sides; j++)
                {
                glVertex2f(xPoints[j], yPoints[j]);
                }
                glEnd();
                glFinish();
                */

                moon9::disable::texturing no_texture;

                glPolygonMode( GL_FRONT, GL_FILL );

                // Use...
                for (SVGPath* it = plist; it; it = it->next)
                {
                        if( it->hasFill )
                        {
                            unsigned int glColor = it->fillColor;
                            glColor3ub( (glColor >> 16) & 0xFF, (glColor >> 8) & 0xFF, glColor & 0xFF );
                        }

                        glBegin( it->closed ? GL_LINE_LOOP : GL_LINE_STRIP);
                        for( int i = 0; i < it->npts; ++i)
                                glVertex3f(it->pts[i*2], it->pts[i*2+1], 160.0f );
                        if( it->npts )
                        {
                            glVertex3f( it->pts[ (it->npts-1)*2], it->pts[(it->npts-1)*2+1], 160.0f );
                            glVertex3f( it->pts[ (it->npts-1)*2], it->pts[(it->npts-1)*2+1], 160.0f );
                        }
                        glEnd();
                }

                // Delete
                svgDelete(plist);
            }
        };

        struct camera : moon9::render_detail::nocopy
        {
            camera()
            {
                if( 0 )
                {
                    // rlyeh's art :o)
                    {
                        matrix::rotate_z z(-90);
                        matrix::scale scy( moon9::vec3(1,2,1) );
                        cone c( 0.f );
                    }
                    glTranslatef( 1.5f, 0.f, 0.f );
                    {
                        matrix::scale scx( moon9::vec3(2,1,1) );
                        cube box;
                    }
                    matrix::scale s(0.75f);
                    glTranslatef( -0.5f, 1.f, 0.f );
                    circle c1;
                    glTranslatef(   1.f, 0.f, 0.f );
                    circle c2;
                    glTranslatef( -2.0f, -1.f, 0.f );
                }
                else
                {
                    // original artwork by Alexandri Zavodny

                    moon9::matrix::scale sc( moon9::vec3(1,2,0.75) );
                    moon9::matrix::rotate_x rx( -180.f );
                    moon9::matrix::scale sc2( moon9::vec3(1,-1,1) );

                    glutWireCube(1.0f);

                    //draw the reels on top of the camera...
                    for(int currentReel = 0; currentReel < 2; currentReel++)
                    {
                        float radius    = 0.25f;
                        int resolution  = 32;
                        moon9::vec3 reelCenter = moon9::vec3(0, -0.25 + (currentReel==0?0:0.5), -0.5);
                        glBegin(GL_LINES);
                            moon9::vec3 s = reelCenter - moon9::vec3(0,0.25,0);
                            glVertex3f(s.x, s.y, s.z);
                            for(int i = 0; i < resolution; i++)
                            {
                                float ex    = -cosf( i / (float)resolution * 3.1415926535897932384626433832795f);
                                float why   =  sinf( i / (float)resolution * 3.1415926535897932384626433832795f);
                                moon9::vec3 p = moon9::vec3(0, ex*radius, -why*radius*3) + reelCenter;
                                glVertex3f(p.x, p.y, p.z);  //end of this line...
                                glVertex3f(p.x, p.y, p.z);  //and start of the next
                            }
                            moon9::vec3 f = reelCenter + moon9::vec3(0,0.25,0);
                            glVertex3f(f.x, f.y, f.z);
                        glEnd();
                    }

                    //and just draw the lens shield manually because
                    //i don't want to think about shear matrices.
                    //clockwise looking from behind the camera:
                    float lensOff = 0.3f;
                    float lensOut = 0.2f;
                    moon9::vec3 v0    =   moon9::vec3( 0.5,  0.5, -0.5);
                    moon9::vec3 v1    =   moon9::vec3(-0.5,  0.5, -0.5);
                    moon9::vec3 v2    =   moon9::vec3(-0.5,  0.5,  0.5);
                    moon9::vec3 v3    =   moon9::vec3( 0.5,  0.5,  0.5);

                    moon9::vec3 l0    =   v0 + moon9::vec3( lensOut,0,0) + vec3(0,lensOut,0) + vec3(0,0,-lensOff);
                    vec3 l1    =   v1 + vec3(-lensOut,0,0) + vec3(0,lensOut,0) + vec3(0,0,-lensOff);
                    vec3 l2    =   v2 + vec3(-lensOut,0,0) + vec3(0,lensOut,0) + vec3(0,0,lensOff);
                    vec3 l3    =   v3 + vec3( lensOut,0,0) + vec3(0,lensOut,0) + vec3(0,0,lensOff);


                    glBegin(GL_LINE_STRIP);
                        glVertex3f(l0.x, l0.y, l0.z);
                        glVertex3f(l1.x, l1.y, l1.z);
                        glVertex3f(l2.x, l2.y, l2.z);
                        glVertex3f(l3.x, l3.y, l3.z);
                        glVertex3f(l0.x, l0.y, l0.z);
                    glEnd();

                    //and connect the two
                    glBegin(GL_LINES);
                        glVertex3f(v0.x, v0.y, v0.z);  glVertex3f(l0.x, l0.y, l0.z);
                        glVertex3f(v1.x, v1.y, v1.z);  glVertex3f(l1.x, l1.y, l1.z);
                        glVertex3f(v2.x, v2.y, v2.z);  glVertex3f(l2.x, l2.y, l2.z);
                        glVertex3f(v3.x, v3.y, v3.z);  glVertex3f(l3.x, l3.y, l3.z);
                    glEnd();
                }
            }
        };

        struct manipulator : moon9::render_detail::nocopy
        {
            manipulator()
            {
                moon9::matrix::scale s1(1.0);  // to do: matrix::fixed size.. no matter camera distance
                moon9::disable::lighting no_lights;
                moon9::style::thick thicker;

                float detail01 = 1.f;
                {
                    moon9::style::blue b;
                    moon9::geometry::circle( detail01 );
                    {
                    moon9::style::green g;
                    moon9::matrix::rotate_x x( 90 );
                    moon9::geometry::circle( detail01 );
                    }
                    {
                    moon9::style::red r;
                    moon9::matrix::rotate_y y( 90 );
                    moon9::geometry::circle( detail01 );
                    }
                }
                moon9::matrix::scale s2(1.25);
                moon9::geometry::axes ax;
            }
        };

        /*
	        void Arrow()
	        {
		        glBegin( GL_TRIANGLES );

		        // shadow
		        glColor4f( 0.0f, 0.0f, 0.0f, 0.5f );
		        glVertex3f( -0.5f, 0.0, 0.f  );
		        glVertex3f( +0.5f, 0.0, 0.f  );
		        glVertex3f(  0.0f, 0.0, 1.75f);

		        // arrow
		        glColor4f( 1.0f, 0.0f, 0.0f, 1.f );
		        glVertex3f( +0.5f, 0.05f, 0.f );

		        glColor4f( 0.0f, 1.0f, 0.0f, 1.f );
		        glVertex3f( -0.5f, 0.05f, 0.f );

		        glColor4f( 0.0f, 0.0f, 1.0f, 1.f );
		        glVertex3f( 0.0f, 0.05f, 1.75f );

		        glEnd();
	        }
        */

    } //geometry

    namespace util
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

        struct check : render_detail::nocopy
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

    using namespace geometry;
    using namespace util;

} //roo



// memoizated spline class
// it constructs a bspline/catmull-rom spline from a list of M points, then
// splits the spline into a list of N uniformly distrubted knots. for every delta
// t = [0..1] at this point we lerp values from the memoizated knotlist.
// should be faster than bsplines/catmull rom splines and curve accuracy is
// selectable. also we get an uniformly distributed knotlist for free.

class mspline
{
    public:

    float road, roaderr;
    moon9::spline<moon9::vec3> highquality;   // - render
    moon9::spline<moon9::vec3>::vector pos;  // knotlist for positions

    enum type_enumeration
    {
        CATMULL,
        BSPLINE
    };

    //pseudo_spline( std::vector<float> &pointlist, type_enumeration _type = CATMULL ) : length(0), type(_type)
    mspline( const std::vector<moon9::vec3> &pointlist, size_t segments /* >= 1 */, type_enumeration _type = CATMULL ) : road(0), roaderr(0), type(_type)
    {

        // create spline

        {
            moon9::spline<moon9::vec3> &spline( highquality );

            //@todo: optimize following for

            for( size_t i = 0; i < pointlist.size(); ++i )
                spline.push_back( pointlist[i] );

            moon9::vec3 A, B;

            for (size_t i = 0; i < segments; ++i)
            {
                float dt01 = i / (float)segments;

                A = spline.atdt( dt01 );
                B = spline.atdt( dt01 + ( 1.f / float(segments) ) );

                road += len( moon9::vec3( B - A ) );

                pos.push_back( A );
            }

            pos.push_back( B );
        }

        // reparametrice spline with "~equally distant" knots
        // (done in a horrible way)
        // @todo: improve in a order magnitude, or two

        if( 1 )
        {
            moon9::spline<moon9::vec3> spline;

            for( size_t i = 0; i < pos.size(); ++i )
                spline.push_back( pos[i] );

            std::vector<moon9::vec3> pos2;

            moon9::vec3 A, B;

            float segment = road / segments;
            float step = 0.01f;

            pos2.push_back( pos.front() );

            for( float dta = 0, dtb = step; dta + dtb < 1; )
            {
                A = spline.atdt( dta );
                B = spline.atdt( dta + dtb );

                if( len( moon9::vec3(B-A) ) < segment )
                {
                    dtb += step;
                }
                else
                {
                    roaderr += len( moon9::vec3( B - A ) );
                    pos2.push_back( B ),
                    dta += dtb,
                    dtb = step;
                }
            }

            pos2.push_back( pos.back() );

            std::swap( pos, pos2 );

            std::cout << "spline aprox err: " << road - roaderr << std::endl;

            // optimize top
        }

        // redistribute knots by creating a new spline

        {
            moon9::spline<moon9::vec3> spline;

            for( size_t i = 0; i < pos.size(); ++i )
                spline.push_back( pos[i] );

            pos.clear();

            // optimize top

            moon9::vec3 A, B;

            for (size_t i = 0; i < segments; ++i)
            {
                float dt01 = i / (float)segments;

                A = spline.atdt( dt01 );
                B = spline.atdt( dt01 + ( 1.f / float(segments) ) );

                pos.push_back( A );
            }

            pos.push_back( B );
        }

    }

    #if 0
    mspline segment( float dt01 = 0.5 )
    {}

    mspline segment( size_t segments /* >= 1 */ )
    {}
    #endif

    ~mspline()
    {}

    float length() const
    {
        return road;
    }

    moon9::vec3 at_lq( float dt01 )
    {
        /* example
        knotlist.size() -> = 8
        input dt01 = 0.345
        status map
        0------1------2-|-----3---|---4------5------6------7
        0              0.33      0.5                       1
        (8-1) * dt01 = 2.415 -> segment = floorf(dt) = 2 // dt - floorf(dt) = .415
        A = segment
        B = segment+1
        B-A = dtBA
        return lerp = A + (B-A) * dt
        */

        if( dt01 <= 0 )
            return *pos.begin();

        if( dt01 >= 1 )
            return *(pos.end() - 1);

        float dt = ( pos.size() - 1 ) * dt01;
        int segment = floorf(dt);
        dt -= segment;

        moon9::vec3 A = pos[segment];
        moon9::vec3 B = pos[segment+1];

        return A + ( B - A ) * dt;
    }

    moon9::vec3 at_hq(float dt01)
    {
        return highquality.atdt( dt01 );
    }

    moon9::vec3 tan(float dt01)
    {
        return highquality.tandt( dt01 );
    }

    // [temp]
    void draw_lq( bool draw_knots = false )
    {
        glBegin( GL_LINE_STRIP );

            for (size_t i = 0; i < pos.size(); ++i)
                glVertex3fv( pos[i].data() );

        glEnd();

        if( draw_knots )
        {
            glPointSize( 16 );
            glEnable(GL_POINT_SMOOTH);

                glBegin( GL_POINTS );
                    for (size_t i = 0; i < pos.size(); ++i)
                    {
                        glVertex3fv( pos[i].data() );
                    }
                glEnd();

            glDisable(GL_POINT_SMOOTH);
            glPointSize( 1 );
        }
    }

    void draw_hq( bool draw_knots = false )
    {
        moon9::curve spl( highquality );

        if( draw_knots )
        {
            glPointSize( 16 );
            glEnable(GL_POINT_SMOOTH);

                glBegin( GL_POINTS );
                    for (size_t i = 0; i < pos.size(); ++i)
                    {
                        glVertex3fv( &pos[i].x );
                    }
                glEnd();

            glDisable(GL_POINT_SMOOTH);
            glPointSize( 1 );
        }
    }


    private:

    type_enumeration type;

    //moon9::vector<moon9::vec3> tan; // knotlist for tangents
};


/*

moon9::vec3 unproject(int x, int y)    // world2screen
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    return moon9::vec3(posX, posY, posZ);
}

*/


//ref: http://www.opengl.org/wiki/Skybox

//ref: http://sidvind.com/wiki/Skybox_tutorial
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

namespace moon9
{
    namespace geometry
    {
        class textured_cube : render_detail::nocopy
        {
            public:

            template<typename T>
            textured_cube( const T textures[6], float texture_scale = 1.0f, float halfcube = 0.5f )
            {
                // Render the quads in order: front, left, back, right, top and bottom

                {
                    moon9::style::texture tx( textures[0] );
                    glBegin(GL_QUADS);
                        glTexCoord2f(           0.f,           0.f ); glVertex3f( +halfcube, -halfcube, +halfcube );
                        glTexCoord2f( texture_scale,           0.f ); glVertex3f( -halfcube, -halfcube, +halfcube );
                        glTexCoord2f( texture_scale, texture_scale ); glVertex3f( -halfcube, -halfcube, -halfcube );
                        glTexCoord2f(           0.f, texture_scale ); glVertex3f( +halfcube, -halfcube, -halfcube );
                    glEnd();
                }
                {
                    moon9::style::texture tx( textures[1] );
                    glBegin(GL_QUADS);
                        glTexCoord2f(           0.f,           0.f ); glVertex3f( +halfcube, +halfcube, +halfcube );
                        glTexCoord2f( texture_scale,           0.f ); glVertex3f( +halfcube, -halfcube, +halfcube );
                        glTexCoord2f( texture_scale, texture_scale ); glVertex3f( +halfcube, -halfcube, -halfcube );
                        glTexCoord2f(           0.f, texture_scale ); glVertex3f( +halfcube, +halfcube, -halfcube );
                    glEnd();
                }
                {
                    moon9::style::texture tx( textures[2] );
                    glBegin(GL_QUADS);
                        glTexCoord2f(           0.f,           0.f ); glVertex3f( -halfcube, +halfcube, +halfcube );
                        glTexCoord2f( texture_scale,           0.f ); glVertex3f( +halfcube, +halfcube, +halfcube );
                        glTexCoord2f( texture_scale, texture_scale ); glVertex3f( +halfcube, +halfcube, -halfcube );
                        glTexCoord2f(           0.f, texture_scale ); glVertex3f( -halfcube, +halfcube, -halfcube );
                    glEnd();
                }
                {
                    moon9::style::texture tx( textures[3] );
                    glBegin(GL_QUADS);
                        glTexCoord2f(           0.f,           0.f ); glVertex3f( -halfcube, -halfcube, +halfcube );
                        glTexCoord2f( texture_scale,           0.f ); glVertex3f( -halfcube, +halfcube, +halfcube );
                        glTexCoord2f( texture_scale, texture_scale ); glVertex3f( -halfcube, +halfcube, -halfcube );
                        glTexCoord2f(           0.f, texture_scale ); glVertex3f( -halfcube, -halfcube, -halfcube );
                    glEnd();
                }
                {
                    moon9::style::texture tx( textures[4] );
                    glBegin(GL_QUADS);
                        glTexCoord2f(           0.f,           0.f ); glVertex3f( -halfcube, -halfcube, +halfcube );
                        glTexCoord2f( texture_scale,           0.f ); glVertex3f( +halfcube, -halfcube, +halfcube );
                        glTexCoord2f( texture_scale, texture_scale ); glVertex3f( +halfcube, +halfcube, +halfcube );
                        glTexCoord2f(           0.f, texture_scale ); glVertex3f( -halfcube, +halfcube, +halfcube );
                    glEnd();
                }
                {
                    moon9::style::texture tx( textures[5] );
                    glBegin(GL_QUADS);
                        glTexCoord2f(           0.f,           0.f ); glVertex3f( -halfcube, -halfcube, -halfcube );
                        glTexCoord2f( texture_scale,           0.f ); glVertex3f( -halfcube, +halfcube, -halfcube );
                        glTexCoord2f( texture_scale, texture_scale ); glVertex3f( +halfcube, +halfcube, -halfcube );
                        glTexCoord2f(           0.f, texture_scale ); glVertex3f( +halfcube, -halfcube, -halfcube );
                    glEnd();
                }
            }
        };

        class skybox : render_detail::nocopy
        {
            public:

            explicit skybox( const char *textures[6] )
            {
                moon9::matrix::scale sc(500.f);
                moon9::matrix::translate t(moon9::vec3(0.f,0.f,0.f));
                //moon9::disable::depth no_depth;
                moon9::disable::lighting no_lights;
                //moon9::disable::blending no_blend;
                moon9::style::white color;

                // Render the quads in order: front, left, back, right, top and bottom

                moon9::invert::normals textured_inside;
                textured_cube cube(textures);
            }
        };
    }
}


namespace moon9
{
    class capturer;

    class surface
    {
        // @todo: fix top horiz line glitch & scale (half?) bug

        std::vector<unsigned char> colorBits;
        const int w, h; //size of texture
        const bool capture_depth, sharp; //sharp/blocky or blurry

        unsigned int id;

        int viewport[4];

        public:

        surface( int _w = 256, int _h = 256 ) : w( _w ), h( _h ), capture_depth( false ), sharp( true ), id( 0 )
        {
            //new array
            colorBits.resize( w * h * 3 );

            //texture creation..
            glGenTextures( 1, &id );
            glBindTexture( GL_TEXTURE_2D, id );

            if(!capture_depth)
                glTexImage2D( GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, colorBits.data() );
            else
                glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0 );

            //you can set other texture parameters if you want
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        }

        ~surface()
        {
            if( id )
            {
                GLuint uid = id;
                glDeleteTextures( 1, &uid );
            }
        }

        void capture_begin() //capture @rect(x,y)!! //void predraw() || operator bool?
        {
            //save viewport and set up new one
            glGetIntegerv( GL_VIEWPORT, (int*)viewport );
            glViewport( 0, 0, w, h );
        }

        void capture_end() //void postdraw()
        {
            //save data to texture using glCopyTexImage2D
            glBindTexture( GL_TEXTURE_2D, id );

            if( !capture_depth )
                glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 0,0, w, h, 0 );
            else
                glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, w, h );

            //restore viewport
            glViewport( viewport[0], viewport[1], viewport[2], viewport[3] );
        }

        void display( const moon9::vec2 &pos = moon9::vec2(0,0), float scale = 1.f ) //x,y
        {
            moon9::disable::lighting dl;

            // locate (0,0 is top-left if using default viewport)
            moon9::matrix::ortho flat;
            moon9::matrix::translate tr( moon9::vec3(pos.x,pos.y,0) );
            moon9::matrix::scale sc( moon9::vec3( w * scale, h * scale, scale ) );

            // draw center of quad at given position
            moon9::style::texture tx( id );
            moon9::quad sprite( 1.f );
        }
    };

    class capturer : render_detail::nocopy
    {
        surface &sf;

        public:

        explicit capturer( surface &_surface ) : sf(_surface)
        {
            sf.capture_begin();
        }

        ~capturer()
        {
            sf.capture_end();
        }
    };

    // DEPTH TO TEXTURE
    /*

    int shadowMapWidth = 512;
    int shadowMapHeight = 512;
    glGenTextures(1, &m_depthTexture);

    glBindTexture(GL_TEXTURE_2D, m_depthTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, 512,512);

    */
}

#include "camera.hpp"
void render( const moon9::camera &cam, const float &projection = 0.f ); // [0 = position .. 1 = lookat, 2 = lookat*2, ...]

#include "frustum.hpp"
void render( const moon9::frustum &fr, float _farPlane = 0.f );

#include "render-vertex-array.inl"
