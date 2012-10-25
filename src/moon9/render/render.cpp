#include "render.hpp"

namespace moon9
{
    namespace matrix
    {
        moon9::vec3 translate::position = moon9::vec3(0,0,0);
    }
}

#include "render.hpp"
#include "camera.hpp"
#include "frustum.hpp"

void render( const moon9::camera &cam, const float &projection ) // [0 = position .. 1 = lookat, 2 = lookat*2, ...]
{
    moon9::vec3 pos = cam.position + cam.direction * projection;

    moon9::disable::lighting nl;
    moon9::matrix::translate t( pos );

    if( moon9::compile axesgeo = moon9::compile("camera.render_itself()/[axes]") )
    {
        moon9::matrix::scale sc( moon9::vec3(0.75,0.75,0.75) );
        moon9::style::pink color;
        moon9::geometry::point();
        moon9::geometry::axes();
    }

    moon9::matrix::rotate_z rcz( cam.yaw() - 90 );
    moon9::matrix::rotate_x rcx( cam.pitch() );

    if( moon9::compile camgeo = moon9::compile("camera.render_itself()/[outlines+camera]"))
    {
        {
            moon9::style::thick th(3.0);
            moon9::style::black color;
            moon9::geometry::camera cam;
        }
        {
            moon9::style::green color;
            moon9::geometry::camera cam;
        }
    }

    moon9::matrix::rotate_x rcx2( 90 );

    render( cam.frustum, 10.f ); // should be get_far()
}

#include "render.hpp"
#include "frustum.hpp"

//@todo: fix blending at exit
void render( const moon9::frustum &fr, float _farPlane )
{
    // [ref] http://www.songho.ca/opengl/gl_transform.html

    float fovY = fr.fov;
    float aspectRatio = fr.aspect;
    float nearPlane = fr.neard;
    float farPlane = _farPlane ? _farPlane : fr.fard;

    float tangent = tanf( deg2rad(fovY*0.5) );

    float nearHeight = nearPlane * tangent;
    float nearWidth = nearHeight * aspectRatio;
    float farHeight = farPlane * tangent;
    float farWidth = farHeight * aspectRatio;

    // compute 8 vertices of the frustum
    float vertices[8][3];
    // near top right
    vertices[0][0] = nearWidth;     vertices[0][1] = nearHeight;    vertices[0][2] = -nearPlane;
    // near top left
    vertices[1][0] = -nearWidth;    vertices[1][1] = nearHeight;    vertices[1][2] = -nearPlane;
    // near bottom left
    vertices[2][0] = -nearWidth;    vertices[2][1] = -nearHeight;   vertices[2][2] = -nearPlane;
    // near bottom right
    vertices[3][0] = nearWidth;     vertices[3][1] = -nearHeight;   vertices[3][2] = -nearPlane;
    // far top right
    vertices[4][0] = farWidth;      vertices[4][1] = farHeight;     vertices[4][2] = -farPlane;
    // far top left
    vertices[5][0] = -farWidth;     vertices[5][1] = farHeight;     vertices[5][2] = -farPlane;
    // far bottom left
    vertices[6][0] = -farWidth;     vertices[6][1] = -farHeight;    vertices[6][2] = -farPlane;
    // far bottom right
    vertices[7][0] = farWidth;      vertices[7][1] = -farHeight;    vertices[7][2] = -farPlane;

    float colorLine1[4] = { 0.7f, 0.7f, 0.7f, 0.7f };
    float colorLine2[4] = { 0.2f, 0.2f, 0.2f, 0.7f };
    float colorPlane[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

    moon9::disable::lighting nolit;
    //moon9::disable::culling nocull;
    glDisable(GL_CULL_FACE);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //beautiful :D

    // draw the edges around frustum
    glBegin(GL_LINES);
    glColor4fv(colorLine2);
    glVertex3f(0, 0, 0);
    glColor4fv(colorLine1);
    glVertex3fv(vertices[4]);

    glColor4fv(colorLine2);
    glVertex3f(0, 0, 0);
    glColor4fv(colorLine1);
    glVertex3fv(vertices[5]);

    glColor4fv(colorLine2);
    glVertex3f(0, 0, 0);
    glColor4fv(colorLine1);
    glVertex3fv(vertices[6]);

    glColor4fv(colorLine2);
    glVertex3f(0, 0, 0);
    glColor4fv(colorLine1);
    glVertex3fv(vertices[7]);
    glEnd();

    glColor4fv(colorLine1);
    glBegin(GL_LINE_LOOP);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[5]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[7]);
    glEnd();

    glColor4fv(colorLine1);
    glBegin(GL_LINE_LOOP);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[3]);
    glEnd();

    // draw near and far plane
    glColor4fv(colorPlane);
    glBegin(GL_QUADS);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[5]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[7]);
    glEnd();

    glEnable(GL_CULL_FACE);
}
