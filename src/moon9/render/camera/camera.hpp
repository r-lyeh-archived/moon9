/*
 * Simple camera class
 * Based on quaternion camera class by Vic Hollis
 * - rlyeh
 *
 *
 * Features:
 * - Safe reticles
 * - Frustum clipping

 *
 * Real-Time Cameras: A guide for game designers and developers
 * 2009, Mark Haigh-Hutchinson
 * p22:
 * - lens type: anamorphic, zoom, fisheye
 * - film stock: 35mm, 70mm, film grain, light sensitivity
 * - capture rate: 24fps, 30 fps, 48fps, 96fps...
 * - exposure (shutter speed): overexposed, underexposed
 * - aspect ratio
 * p30
 * - first person camera / third person camera
 * - cinematic / dynamic
 * - look at / position / orientation / frustum / viewport / field of view / aspect ratio
 * p34
 * - safe frame : 10/15%
 * - common display aspect ratios : 1.33:1 (tv 4:3), 1.78:1 (hdtv 16:9), 2.35:1 (anamorphic), 1.85:1 (cinemascope)
 * - motion blur
 * p39
 * - transitions
 *   behavioral
 *   interpolations: from state a to state b in t seconds using tw tween
 *   jump cuts ( interpolations where t = 0 )
 *   wipes and fades ( special fx to do transitions )
 * p40
 * - constraints
 *   motion : distance, character-relative position, path, splines, surface, acceleration/velocity limits
 *   orientation : limit pitch (~disorientation), soft limits / hard limits, acceleration restrictions (lowpass) ...
 * p41
 * - presentation
 *   orthographic 2d
 *   perspective 3d
 *   hybrid 2.5d
 * p64 predicition, ai, pathfinding
 * voy por p80, cinematography
 */

#pragma once

#include <cmath>

#include <moon9/render/os.hpp>
#include <moon9/spatial/vec.hpp>

#include <moon9/time/dt.hpp>
#include <moon9/string/format.hpp>

#include "frustum/frustum.hpp"
#include <moon9/spatial/frustum.hpp>

#if 0

// fps bar (25 hz)
// take
// frame, scene

#pragma once

// fov
// dov
// orbit
// advance
// link_target
// link_position
// link_up
// tween
// project
// unproject

// bokeh
// speed
// acceleration
// shaking
// set_working_area
// set_wall_mask
//
// ...

#endif


// moon9::vec2 projection( moon9::vec3 )
// rect3 inverse_projection( moon9::vec2 )
// set_pitch, set_roll
// rotate_yaw

#include <moon9/render/render.hpp>

// camera coordinate system follows opengl basis internally
//
//  .z+
//  |  y+
//  | /
//  |/
//  *---- x+
//
// beware when retrieving cam_pos or cam_basis!
//
//
//
//

namespace moon9
{
    class glass;
}


/* Using GLM for our transformation matrices */
#include <glm/glm.hpp> // 4minicam
#include <glm/gtc/matrix_transform.hpp> // 4minicam
#include <glm/gtc/type_ptr.hpp>

namespace moon9
{


    struct minicam
    {
        // input values
        moon9::vec3 position;
        moon9::vec3 offset;

        moon9::vec3 angle;        // radians (yaw,roll,pitch)
        moon9::vec3 up;
        float yaw()   const { return angle.x; }
        float roll()  const { return angle.y; }
        float pitch() const { return angle.z; }

        moon9::vec3 noise;        // disturbs direction

        glm::vec4 viewport;     // viewport (x,y,w,h) ; (x:0, y:0) is bottom-left

        float distance;
        float fov;              // degrees

        // output values, after update()
        glm::mat4 projection, view, view90, mvp, mvp90;
        moon9::vec3 forward, right, direction;
        moon9::frustum frustum;

        // final output tuples are:
        // final_pos is { (position + offset) }
        // final_dir is { (direction + noise) }
        // final relative axis are { right, forward, up }

        // custom values (for your own purposes)
        bool   does_collision;
        size_t objs_count, objs_rendered;
        size_t tris_count, tris_rendered;
        std::string name;

        minicam( float w = 640, float h = 480 )
        {
            position = moon9::vec3(0, 0, 10);
            offset = moon9::vec3(0, 0, 0);
            noise = moon9::vec3(0, 0, 0);
            angle = moon9::vec3(0, 0, 0);
            up = moon9::vec3(0, 0, 1);

            resize( w, h );

            distance = 1023.f;
            fov = 75.f * 0.5f; //37.5f;

            does_collision = false;
            objs_count = objs_rendered = 0;
            tris_count = tris_rendered = 0;
            name = "undefined";

            update();
        }

        void update()
        {
            // Refresh forward, right and direction vectors
            float sinx = sinf( angle.x ), sinz = sinf( angle.z ), cosx = cosf( angle.x ), cosz = cosf( angle.z );

            forward = moon9::vec3( sinx, cosx, 0 );
            right = moon9::vec3( cosx, -sinx, 0 );
            direction = moon9::vec3( sinx * cosz, cosx * cosz, sinz );

            // Refresh matrices
            view = glm::lookAt( ( position + offset ), ( position + offset ) + ( direction + noise ), up );

                // classic distance depth
                projection = glm::perspective( fov, viewport.z / viewport.w, 1.f, 1.f + distance );

                // reverse distance depth!
                // projection = glm::perspective( fov, viewport.z / viewport.w, 1.f, 0.f );

                // logarithmic zbuffer. helps getting rid of near/far distance problems.
                // [ref] http://outerra.blogspot.com.es/2009/08/logarithmic-z-buffer.html
                // projection = glm::perspective( fov, viewport.z / viewport.w, 0.0001f, 100000000.f ); // znear = 0.0001 inch, zfar = 39370.0 * 100000.0 inches = 100K kilometers
                // requires following code at the very end of your fragment shader
                //        const float C = 1.0;
                //        const float far = 1000000000.0;
                //        const float offset = 1.0;
                //        gl_FragDepth = (log(C * gl_TexCoord[6].z + offset) / log(C * far + offset));

            mvp = projection * view;

            // Alternative matrix (right,forward,up) instead of (right,up,forward)
            view90 = glm::rotate( view, -90.f, moon9::vec3(-1.0f, 0.0f, 0.0f) );
            mvp90 = projection * view90;

            if( 0 )
            {
                if( 1 ) // near ~0, far 1
                {
                    //glClearDepthdNV( distance );
                    glClearDepth( distance );
                    //glDepthRangedNV( 0.0, distance );
                    glDepthBoundsEXT( 0.0, distance );
                }
                else
                {
                    // inverted: near 1, far 0

                    //glClearDepthdNV( 0.0 );
                    glClearDepth( 0.0 );
                    //glDepthRangedNV( distance, 0.0);
                    glDepthBoundsEXT( distance, 0.0 );
                }
            }

            // Update frustum
            frustum.setup( fov, viewport.z / viewport.w, 1.f, 1.f + distance );
            frustum.update( position + offset, ( position + offset ) + ( direction + noise ), up );
        }

        void resize( int w, int h )
        {
            viewport = glm::vec4(0,0,w,h);

            update();
        }

        void lookdir( const moon9::vec3 &dir /*, moon9::vec3 focus_dof */ )
        {
            moon9::vec3 _dir = glm::normalize( dir.as<glm::vec3>() );

            angle.x = atan2( _dir.x, _dir.y );
            angle.z = atan2( _dir.z, sqrt( _dir.x * _dir.x + _dir.y * _dir.y ) );

            update(); // recompute direction vector (and matrices)
        }

        void lookat( const moon9::vec3 &target /*, moon9::vec3 focus_dof */ )
        {
            lookdir( target - ( position + offset ) );
        }

        void rotate( float yaw, float roll, float pitch )
        {
            angle.x -= -yaw;
            angle.y -= roll;
            angle.z -= pitch;

            const float pi = 3.1415926535897932384626433832795f;

            if( angle.x < -pi )           angle.x += pi * 2;
            if( angle.x >  pi )           angle.x -= pi * 2;
            //if( angle.y < -pi )           angle.y += pi * 2;
            //if( angle.y >  pi )           angle.y -= pi * 2;
            if( angle.z < -pi * 0.49 )    angle.z = -pi * 0.49;
            if( angle.z >  pi * 0.49 )    angle.z =  pi * 0.49;

            update();
        }

        //@todo
        //void lock_pos();
        //void lock_dir( bool forward, bool right, bool up ); dir * vec3().110()

        void apply() const
        {
            // Refresh viewport
            glViewport( viewport.x, viewport.y, viewport.z, viewport.w );

            // Apply matrices to fixed pipeline
            glMatrixMode( GL_PROJECTION );
            glLoadMatrixf( glm::value_ptr( projection ) );

            glMatrixMode( GL_MODELVIEW );
            glLoadMatrixf( glm::value_ptr( view ) );

            // CCW
            glFrontFace(GL_CCW);
        }

        void apply90() const
        {
            apply();

            // to xzy
            glScalef( 1.f, 1.f, -1.f );
            glRotatef(  90.f, 0.f, 0.f, 1.f );
            glRotatef( -90.f, 1.f, 0.f, 0.f );

            // CW
            glFrontFace(GL_CW);
        }

        moon9::vec3 unproject( float screen_x, float screen_y ) const //at_screen()
        {
            //apply();

            float depth;
            glReadPixels( screen_x, viewport.w - screen_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth );

            moon9::vec3 wincoord = moon9::vec3( screen_x, screen_y, depth );
            //moon9::vec3 wincoord = moon9::vec3( screen_x, screen_y, depth );
            moon9::vec3 ret = glm::unProject( wincoord, view, projection, viewport );
            return moon9::vec3( ret.x, ret.y, -ret.z );
        }

        // typical fps controllers

        void wheel( float inc )
        {
            fov += -inc;
        }

        void rfu( float inc_right, float inc_fwd, float inc_up )
        {
            position += right * inc_right;
            position += forward * inc_fwd;
            position += up * inc_up;
        }

        void ruf( float inc_right, float inc_up, float inc_fwd )
        {
            rfu( inc_right, inc_fwd, inc_up );
        }

        void fru( float inc_fwd, float inc_right, float inc_up )
        {
            rfu( inc_right, inc_fwd, inc_up );
        }

        void fur( float inc_fwd, float inc_up, float inc_right )
        {
            rfu( inc_right, inc_fwd, inc_up );
        }

        void urf( float inc_up, float inc_right, float inc_fwd )
        {
            rfu( inc_right, inc_fwd, inc_up );
        }

        void ufr( float inc_up, float inc_fwd, float inc_right )
        {
            rfu( inc_right, inc_fwd, inc_up );
        }

        void wsad( float w, float s, float a, float d )
        {
            rfu( d-a, w-s, 0.f );
        }

        void wasd( float w, float a, float s, float d )
        {
            rfu( d-a, w-s, 0.f );
        }

        void wsadec( float w, float s, float a, float d, float e, float c )
        {
            rfu( d-a, w-s, e-c );
        }

        void wasdec( float w, float a, float s, float d, float e, float c )
        {
            rfu( d-a, w-s, e-c );
        }
    };

    typedef minicam camera;

#if 0
class camera1
{
    // based on code by switfless   http://www.swiftless.com/tutorials/opengl/camera2.html
    // based on code by nathan reed http://reedbeta.com/graphics.html

    protected:

    friend class moon9::glass;
    friend class moon9::camera;

public:

    /* 45deg fov */
    // outerra uses 37.5 vert, 66.6 horiz
    float cam_yaw, cam_pitch, cam_fov, cam_focus;
    nathan::vec3 cam_pos;
    nathan::basis cam_basis;

    enum { forward, strafe, elevation, axes };

    float factor[axes];

    nathan::mat3 cam_rotation;

    double w, h;

    bool is_2d;

    public:

    moon9::frustum frustum;

    camera1( double _w, double _h ) : is_2d(false), cam_fov(75.f * 0.5f), cam_focus(10000.f)
    {
        resize( _w, _h );

        using namespace nathan;
        using nathan::vec3;

        cam_rotation = mat3::identity();
        cam_pos = vec3(0,0,0);
        cam_pitch = -10; cam_yaw = 135;

        factor[forward] =
        factor[strafe] =
        factor[elevation] = 0.f;
    }

    virtual ~camera1()
    {}

    virtual void resize( double _w, double _h )
    {
        w = _w <= 0 ? 0 : _w;
        h = _h <= 0 ? 1 : _h;
    }

    virtual void set_perspective() // note to self: cant be update() camera while rendering!
    {
        if( is_2d )
        {
            glMatrixMode ( GL_PROJECTION) ;
            glLoadIdentity ();
            glOrtho ( 0.0, w, 0.0, h, -1.0, 1.0 );
            glMatrixMode(GL_MODELVIEW);
            /*
            // 0,0 -> width,height
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glScalef(2.0f / (float)w, -2.0f / (float)h, 1.0f);
            glTranslatef(-((float)w / 2.0f), -((float)h / 2.0f), 0.0f);
            glViewport(0, 0, w, h); // viewport size in pixels
            */
        }
        else
        {
            glViewport(0, 0, w, h);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(cam_fov, w/h, 1, 1 + cam_focus);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            moon9::vec3 eye( 0, 3, -13 );
            moon9::vec3 pos( 0, 0, 0 );
            moon9::vec3 upv( 0, 1, 0 );

            gluLookAt( eye.x, eye.y, eye.z,
                       pos.x, pos.y, pos.z,
                       upv.x, upv.y, upv.z );
        }
    }

    double get_fov() const
    {
        return cam_fov;
    }

    double get_aspect() const
    {
        return w/h;
    }

    double get_near() const
    {
        return 1;
    }

    double get_far() const
    {
        return cam_focus;
    }

    virtual void update()
    {
        // Update cam logic
        update_stuff();
    }

    virtual void apply()
    {
        // Update the projection matrix
        set_perspective();

        // Setup camera matrix : rotation and translation
        apply_rotation();
        apply_translation();

        // Update frustum
        frustum.setup( get_fov(), get_aspect(), get_near(), get_far() );
        frustum.update( get_position(), get_position() + get_lookat(), get_up() );
    }

    void apply_rotation()
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(nathan::mat4(cam_rotation).transpose().data());
    }

    void apply_translation()
    {
        glTranslatef(-cam_pos[0], -cam_pos[1], -cam_pos[2]);
    }

    void set_height( float h )
    {
        cam_pos[2] = h;
    }

    moon9::vec3 get_lookat() const
    {
        return moon9::vec3( -cam_rotation[2][0], -cam_rotation[2][1], -cam_rotation[2][2] );
    }

    moon9::vec3 get_position() const
    {
        return moon9::vec3( cam_pos[0], cam_pos[1], cam_pos[2] );
    }

    // @todo: ok?
    moon9::vec3 get_up() const
    {
        nathan::mat3 cam = cam_rotation.inverse();

        return moon9::vec3( cam[0][1], cam[1][1], cam[2][1] );
    }

    // @todo: lock axis {x and/or y and/or z}

    moon9::vec3 get_rotation() const
    {
        return moon9::vec3( cam_pitch, 0, cam_yaw );
    }

    void set_rotation( const moon9::vec3 &v )
    {
        cam_pitch = v.x, cam_yaw = v.z;
    }

    void set_position( const moon9::vec3 &pos, const float &damp = 1.00f )
    {
        cam_pos = cam_pos * (1.0 - damp) + nathan::vec3( pos.x, pos.y, pos.z ) * damp;
    }

    void set_lookat( const moon9::vec3 &lookat, const float &damp = 1.00f )
    {
        moon9::vec3 dir = ( lookat - get_position() ).normalize();

        // assert lookat.z > 0
        float new_cam_yaw = 90.f + rad2deg(-atan2( dir.x, dir.y )); //atan2[-180..+180]

        float pitch = deg2rad(dir.z*90.f);
        if( pitch > 1.0f ) pitch = 1.f;
        else if( pitch < -1.0f ) pitch = -1.f;
        float new_cam_pitch = rad2deg( asin( pitch ) );

        if (new_cam_yaw   >= 360.0f) new_cam_yaw  -= 360.0f;
        if (new_cam_yaw    <   0.0f) new_cam_yaw  += 360.0f;

        float diff = std::abs( new_cam_yaw ) - std::abs( cam_yaw );

        if( diff <= -180.f )
            new_cam_yaw += 360.f;
        else
        if( diff >= +180.f )
            new_cam_yaw -= 360.f;

        float ncam_yaw = cam_yaw * (1.0 - damp) + new_cam_yaw * (damp);
        float ncam_pitch = cam_pitch * (1.0 - damp) + new_cam_pitch * (damp);

        // Wrap yaw and clamp pitch
        if (ncam_yaw   > 360.0f) ncam_yaw  -= 360.0f;
        if (ncam_yaw   <   0.0f) ncam_yaw  += 360.0f;
        if (ncam_pitch >  90.0f) ncam_pitch =  90.0f;
        if (ncam_pitch < -90.0f) ncam_pitch = -90.0f;

        cam_yaw = ncam_yaw;
        cam_pitch = ncam_pitch;
    }

    void mouse_abs( const moon9::vec3 &lookat, const float &damp = 1.00f ) //lookat given moon9::vec3 pos
    {
        set_lookat( lookat, damp );
    }

    void mouse( const float &diffx, const float &diffy, const float &sensitivity = 0.2f ) //lookat incremental from current pos
    {
        // Change main camera view angles
        cam_yaw -= diffx * sensitivity; //MOUSE_CAMERA_ANGLE_FACTOR;
        cam_pitch += diffy * sensitivity; //MOUSE_CAMERA_ANGLE_FACTOR;

        // Wrap yaw and clamp pitch
        if (cam_yaw   > 360.0f) cam_yaw  -= 360.0f;
        if (cam_yaw   <   0.0f) cam_yaw  += 360.0f;
        if (cam_pitch >  90.0f) cam_pitch =  90.0f;
        if (cam_pitch < -90.0f) cam_pitch = -90.0f;
    }

    void mouse_wheel( const float &diff, const float &sensitivity = 1.0f / 12 )
    {
        cam_fov -= diff * sensitivity; //MOUSEWHEEL_CAMERA_FOV_FACTOR
        if (cam_fov < 15.0f) cam_fov = 15.0f;
        if (cam_fov > 175.0f) cam_fov = 175.0f;
    }

    void keyboard( const char &key, float pressure = 0.1f )
    {
        switch(key)
        {
            case 'C': case 'c': pressure = -pressure;
            case 'E': case 'e': factor[ elevation ] = pressure; return;

            case 'S': case 's': pressure = -pressure;
            case 'W': case 'w': factor[ forward ] = pressure; return;

            case 'A': case 'a': pressure = -pressure;
            case 'D': case 'd': factor[ strafe ] = pressure; return;

            default:
                break;
        }
    }

    protected:

    void update_stuff() // note to self: cant be update() camera while rendering!
    {
        using namespace nathan;
        using nathan::vec3;

        cam_basis = basis();
        float cos_cam_yaw, sin_cam_yaw, cos_cam_pitch, sin_cam_pitch;

        // W, S, A, D for movement
        // Calculate sines and cosines of angles
        cos_cam_yaw = cosf(pi / 180.0f * cam_yaw);
        sin_cam_yaw = sinf(pi / 180.0f * cam_yaw);
        cos_cam_pitch = cosf(pi / 180.0f * cam_pitch);
        sin_cam_pitch = sinf(pi / 180.0f * cam_pitch);

        // Calculate back vector
        cam_basis.v3 = -vec3(cos_cam_pitch * cos_cam_yaw,
            cos_cam_pitch * sin_cam_yaw,
            sin_cam_pitch);

        // Calculate up vector
        if ((cam_pitch != 90.0f) && (cam_pitch != -90.0f))
        {
            cam_basis.v2 = vec3(0, 0, 1);
        }
        else
        {
            float sign = (cam_pitch > 0) ? -1.0f : 1.0f;
            cam_basis.v2 = vec3(cos_cam_yaw * sign,
                sin_cam_yaw * sign,
                0);
        }

        // Calculate right vector
        // Normalized cross product of up and back
        cam_basis.v1 = cross(cam_basis.v2, cam_basis.v3).normalize();
        // Recalculate proper up vector, as cross product of back and right
        cam_basis.v2 = cross(cam_basis.v3, cam_basis.v1).normalize();

        cam_pos += factor[strafe] * cam_basis.v1;
        cam_pos += factor[elevation] * cam_basis.v2;
        cam_pos -= factor[forward] * cam_basis.v3;

        factor[strafe] = factor[elevation] = factor[forward] = 0.f;

        // Update the camera rotation matrix
        cam_rotation = cam_basis.from_standard().L;
    }
};

}


namespace moon9
{
    class camera : public moon9::camera1
    {
        //info {
        struct reticle
        {
            // ~~ black sabbath / heaven and hell

            reticle()
            {}
            reticle( const std::string &_name, const float &_aspect_ratio, const bool &_safe_guide = false )
                : name(_name), aspect_ratio(_aspect_ratio), safe_guide(_safe_guide)
            {}

            void resize( const float &_w, const float &_h )
            {
                //std::cout << moon9::echo( name, _w, _h );

                float xoff, yoff, w, h;
                float viewport_aspect_ratio = _w / _h;

                // if current viewport aspect_ratio is smaller than reticle aspect ratio
                if( viewport_aspect_ratio < aspect_ratio )
                {
                    //current viewport: 640 x 480 -> 1.33
                    //reticle: 1920 x 1080 style -> 1.78
                    w = _w;
                    h = _w / aspect_ratio;
                    xoff = 0;
                    yoff = ( _h - h ) * 0.5;
                }
                // else if current viewport aspect_ratio is bigger than reticle aspect ratio
                else
                if( viewport_aspect_ratio > aspect_ratio )
                {
                    //current viewport: 1920 x 1080 -> 1.78
                    //reticle: 640 x 480 style -> 1.33
                    w = _h * aspect_ratio;
                    h = _h;
                    xoff = ( _w - w ) * 0.5;
                    yoff = 0;
                }
                // else equal.
                else
                {
                    // be sure to make it visible
                    xoff = 1, yoff = 1, w = _w - 1, h = _h - 1;
                }

                //std::cout << moon9::echo( name, xoff, yoff, w, h );

                if( safe_guide )
                {
                    // safe area = 15% smaller area
                    // make every side to pad 7.5%

                    moon9::vec2 top_left( w * 0.075, h * 0.075 );
                    moon9::vec2 bottom_right( w * 0.925, h * 0.925 );

                    guide.clear();
                    guide.push_back( moon9::vec3( top_left.x, top_left.y ) );
                    guide.push_back( moon9::vec3( bottom_right.x, top_left.y ) );
                    guide.push_back( moon9::vec3( bottom_right.x, bottom_right.y ) );
                    guide.push_back( moon9::vec3( top_left.x, bottom_right.y ) );
                    guide.push_back( moon9::vec3( top_left.x, top_left.y ) );
                }
                else
                {
                    moon9::vec2 top_left( xoff, yoff );
                    moon9::vec2 bottom_right( xoff+w-1, yoff+h-1 );

                    guide.clear();
                    guide.push_back( moon9::vec3( top_left.x, top_left.y ) );
                    guide.push_back( moon9::vec3( bottom_right.x, top_left.y ) );
                    guide.push_back( moon9::vec3( bottom_right.x, bottom_right.y ) );
                    guide.push_back( moon9::vec3( top_left.x, bottom_right.y ) );
                    guide.push_back( moon9::vec3( top_left.x, top_left.y ) );
                }
            }

            bool safe_guide;
            std::string name;
            float aspect_ratio;
            std::vector< moon9::vec3 > guide;
        };
        std::vector< reticle > reticles;
        std::vector< moon9::vec3 > crosshairv, crosshairh;
        moon9::dt dt;
        // }

        GLdouble projection_matrix_ro[16];                     // Where The 16 Doubles Of The Projection Matrix Are To Be Stored
        GLdouble modelview_matrix_ro[16];                      // Where The 16 Doubles Of The Modelview Matrix Are To Be Stored

        public:

        enum { debug = true };

        camera( int w = 640, int h = 480 ) : camera1(w,h)
        {
            // http://en.wikipedia.org/wiki/Aspect_ratio_(image)

            reticles.push_back( reticle(      "current", float(w) / float(h) ) );
            reticles.push_back( reticle( "current safe", float(w) / float(h), true ) );
            reticles.push_back( reticle(       "tv 4:3", 1.33f ) );
            reticles.push_back( reticle(    "hdtv 16:9", 1.78f ) );
            reticles.push_back( reticle(   "theatrical", 1.85f ) );
            reticles.push_back( reticle(   "anamorphic", 2.35f ) ); //cinemascope

            resize(w, h);
        }

        const GLdouble* get_projection_matrix()
        {
            glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix_ro);     // Retrieve The Projection Matrix
            return projection_matrix_ro;
        }

        const GLdouble* get_modelview_matrix()
        {
            glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix_ro);        // Retrieve The Modelview Matrix
            return modelview_matrix_ro;
        }

        void resize( int w, int h)
        {
            camera1::resize( w, h );

            if( debug )
            {
                // update "current" and "current safe" reticle aspect_ratios
                reticles[ 0 ].aspect_ratio = float(w) / float(h);
                reticles[ 1 ].aspect_ratio = float(w) / float(h);

                // update all reticles with new given dimensions
                for( size_t i = 0; i < reticles.size(); ++i )
                    reticles[ i ].resize( w, h );

                // update crosshair
                float segment = std::sqrtf( w*w + h*h ) * 0.05; // 10% of hypotenuse
                crosshairh.clear();
                crosshairh.push_back( moon9::vec3( w/2 - segment, h/2 ) );
                crosshairh.push_back( moon9::vec3( w/2 + segment, h/2 ) );
                crosshairv.clear();
                crosshairv.push_back( moon9::vec3( w/2, h/2 - segment ) );
                crosshairv.push_back( moon9::vec3( w/2, h/2 + segment ) );
            }
        }

        void render_info()
        {
            if( dt.s() > 1.00 )
                dt.reset();

            int frame = int( dt.s() * 100.0 );

            moon9::matrix::ortho flat;

            for( size_t i = 0; i < reticles.size(); ++i )
            {
                if( reticles[i].safe_guide )
                {
                    moon9::style::dashed line_style;
                    moon9::style::green line_color;
                    moon9::geometry::lines( reticles[i].guide );
                }
                else
                {
                    moon9::style::red line_color;
                    moon9::geometry::lines( reticles[i].guide );
                }

                moon9::style::yellow text_color;
                moon9::geometry::text2d( reticles[i].guide[3].x + 5, reticles[i].guide[3].y - 20, reticles[i].name );
            }

            moon9::style::white crosshair_color;
            moon9::geometry::lines h( crosshairh );
            moon9::geometry::lines v( crosshairv );
            moon9::geometry::text2d( crosshairh[0].x, crosshairh[0].y, moon9::format("frame %02d", frame) );
        }
    };

#endif

}
