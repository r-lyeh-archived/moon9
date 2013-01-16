#pragma once

#include <glm/gtc/type_ptr.hpp>

#include <moon9/render/gl.hpp>
#include <moon9/render/nocopy.hpp>

#include <moon9/spatial/vec.hpp>

namespace moon9
{
    namespace matrix
    {
        struct rotate_x : nocopy
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

        struct rotate_y : nocopy
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

        struct rotate_z : nocopy
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

        struct rotate : nocopy
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

        struct scale : nocopy
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

        struct translate : nocopy
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
        struct offset : nocopy
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

        struct inc_rotate_x : nocopy
        {
            explicit inc_rotate_x( const float &angle_360 )
            {
                glRotatef( angle_360, 1.f, 0.f, 0.f );
            }
        };
        struct inc_rotate_y : nocopy
        {
            explicit inc_rotate_y( const float &angle_360 )
            {
                glRotatef( angle_360, 0.f, 1.f, 0.f );
            }
        };
        struct inc_rotate_z : nocopy
        {
            explicit inc_rotate_z( const float &angle_360 )
            {
                glRotatef( angle_360, 0.f, 0.f, 1.f );
            }
        };
        struct inc_scale : nocopy
        {
            explicit inc_scale( const float &factor )
            {
                glScalef( factor, factor, factor );
            }
        };
        struct inc_translate : nocopy
        {
            explicit inc_translate( const moon9::vec3 &v )
            {
                glTranslatef( v.x, v.y, v.z );
            }
        };

        struct push : nocopy   // preserve
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

        struct transform : nocopy
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
        struct load : nocopy
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
        struct ortho : nocopy
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
}

