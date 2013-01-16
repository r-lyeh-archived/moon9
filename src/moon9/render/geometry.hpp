#pragma once

#include <cmath>

#include <vector>
#include <string>

#include <moon9/render/gl.hpp>

#include <GL/freeglut.h>
#include <nanosvg/nanosvg.h>

#include <moon9/render/nocopy.hpp>
#include <moon9/spatial/vec.hpp>
#include <moon9/spatial/spline.hpp>

#include "enable.hpp"
#include "disable.hpp"
#include "style.hpp"
#include "matrix.hpp"
#include "invert.hpp"
#include "nocopy.hpp"


namespace moon9
{
//ref: http://www.opengl.org/wiki/Skybox

//ref: http://sidvind.com/wiki/Skybox_tutorial
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    namespace
    {
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

        struct point : nocopy
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

        struct line : nocopy
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

        struct normal : nocopy
        {
            normal()
            {
                glBegin(GL_LINES);
                    glVertex3f(0,0,0);
                    glVertex3f(0,0,1);
                glEnd();
            }
        };

        struct points : nocopy
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

        struct lines : nocopy
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

        struct curve : nocopy // spliner
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

        struct pyramid : nocopy
        {
            pyramid()
            {
                glutSolidTetrahedron();
            }
        };

        struct square : nocopy
        {
            square()
            {
                glutSolidSphere( 1.0, 2, 2 );
            }
        };

        struct cube : nocopy
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

        struct semicircle : nocopy
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

        struct circle : nocopy
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
        struct capsule2d : nocopy
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

        struct arrow : nocopy
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

        struct diamond : nocopy
        {
            diamond()
            {
                glutSolidSphere( 1.0, 4, 2 );
            }
        };

        struct teapot : nocopy
        {
            teapot()
            {
                glutSolidTeapot( 1.0 );
            }
        };

        struct convexhull : nocopy
        {
            convexhull()
            {
            }
        };

        // http://zeuxcg.org/2010/10/17/aabb-from-obb-with-component-wise-abs/#more-268

        struct aabb : nocopy
        {
            aabb()
            {
            }
        };

        struct obb : nocopy
        {
            obb()
            {
            }
        };

        // useful?

        struct triangle : nocopy
        {
            triangle()
            {
            }
        };

        struct axes : nocopy
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

        struct sphere : nocopy
        {
            explicit sphere( const float &detail01 = 0.5f )
            {
#if 0
                static glQuadric quadric;
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

        struct semisphere : nocopy
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

        struct capsule : nocopy
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

        struct cylinder : nocopy
        {
            explicit cylinder( const float &detail01 = 0.5f )
            {
                static glQuadric quadric;
                int idetail = int(detail01 * 10) + 6; //0..1 = 4..16
                gluCylinder(quadric(),1.0f,1.0f,1.0f,idetail,idetail);
            }
        };

        struct cone : nocopy
        {
            explicit cone( const float &detail01 = 0.5f )
            {
                static glQuadric quadric;
                int idetail = int(detail01 * 10) + 6; //0..1 = 4..16
                gluCylinder(quadric(),1.0f,0.0f,1.0f,idetail,idetail);
            }
        };

        struct sector : nocopy
        {
            sector( int degree, const float &detail01 = 0.5f )
            {
                static glQuadric quadric;
                int idetail = int(detail01 * 10) + 6; //0..1 = 4..16
                gluPartialDisk(quadric(),0.5f,1.5f,idetail,idetail, 180-degree/2, degree );
            }
        };

        struct disk : nocopy
        {
            explicit disk( const float &detail01 = 0.5f )
            {
                //sector( 360 );
                static glQuadric quadric;
                int idetail = int(detail01 * 10) + 6; //0..1 = 4..16
                gluDisk(quadric(),0.5f,1.5f,idetail,idetail);
            }
        };

        struct pacman : nocopy
        {
            explicit pacman( const float &detail01 = 0.5f )
            {
                //sector( 360 );
                static glQuadric quadric;
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

        struct translate2d_px : nocopy
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

        struct text2d : nocopy
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

        struct text3d : nocopy
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

        struct vector : nocopy
        {
            explicit vector( const moon9::vec3 &direction )
            {
                glBegin( GL_LINES );

                    glVertex3fv( zero );
                    glVertex3fv( direction.data() );

                glEnd();
            }
        };

        struct plane : nocopy
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

        struct quad : nocopy //rect,square,quad
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

        struct svg : nocopy
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

        struct camera : nocopy
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

        struct manipulator : nocopy
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

                    class textured_cube : nocopy
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

        class skybox : nocopy
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
    } //geometry
} //moon9

namespace moon9
{
    using namespace geometry;
}
