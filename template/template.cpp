#include <moon9/render/render.hpp>
#include <moon9/render/window.hpp>
#include <moon9/render/shader.hpp>

#include <moon9/debug/die.hpp>

#include <moon9/io/file.hpp>

#include "randworld.hpp"

#include <iostream>

#define _stringify_(a) #a
#define stringify(...) _stringify_(__VA_ARGS__)

// VBO

struct float3
{
    float x,y,z;

    float3( float _x, float _y, float _z ) : x(_x),y(_y),z(_z)
    {}
};

struct VertexXYZColor
{
    float3 m_Pos;
    float3 m_Color;
};

// Define the 8 vertices of a unit cube
VertexXYZColor g_Vertices[8] =
{
    { float3(  1,  1,  1 ), float3( 1, 1, 1 ) }, // 0
    { float3( -1,  1,  1 ), float3( 0, 1, 1 ) }, // 1
    { float3( -1, -1,  1 ), float3( 0, 0, 1 ) }, // 2
    { float3(  1, -1,  1 ), float3( 1, 0, 1 ) }, // 3
    { float3(  1, -1, -1 ), float3( 1, 0, 0 ) }, // 4
    { float3( -1, -1, -1 ), float3( 0, 0, 0 ) }, // 5
    { float3( -1,  1, -1 ), float3( 0, 1, 0 ) }, // 6
    { float3(  1,  1, -1 ), float3( 1, 1, 0 ) }, // 7
};

// Define the vertex indices for the cube.
GLuint g_Indices[24] = {
    0, 1, 2, 3,                 // Front face
    7, 4, 5, 6,                 // Back face
    6, 5, 2, 1,                 // Left face
    7, 0, 3, 4,                 // Right face
    7, 6, 1, 0,                 // Top face
    3, 2, 5, 4,                 // Bottom face
};

#define MEMBER_OFFSET(s,m) ((char *)NULL + (offsetof(s,m)))
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#define SINGLE(...) do { static struct once { once() { __VA_ARGS__; } } _; } while( 0 )

namespace moon9
{
    class vbo2
    {
        GLuint g_uiVerticesVBO;
        GLuint g_uiIndicesVBO;

        public:

        vbo2() : g_uiVerticesVBO(0), g_uiIndicesVBO(0)
        {}

        ~vbo2()
        {
            clear();
        }

        void init()
        {
            clear();

            // Create VBO's
            glGenBuffersARB( 1, &g_uiVerticesVBO );
            glGenBuffersARB( 1, &g_uiIndicesVBO );
        }

        void clear()
        {
            if( g_uiIndicesVBO != 0 )
            {
                glDeleteBuffersARB( 1, &g_uiIndicesVBO );
                g_uiIndicesVBO = 0;
            }
            if( g_uiVerticesVBO != 0 )
            {
                glDeleteBuffersARB( 1, &g_uiVerticesVBO );
                g_uiVerticesVBO = 0;
            }
        }

        void set()
        {
            SINGLE(
                moon9::init_glut();
                moon9::init_glew();
            );

            clear();
            init();

            // Copy the vertex data to the VBO
            glBindBufferARB( GL_ARRAY_BUFFER_ARB, g_uiVerticesVBO );
            glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(g_Vertices), g_Vertices, GL_STATIC_DRAW_ARB );
            glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );

            // Copy the index data to the VBO
            glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, g_uiIndicesVBO );
            glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(g_Indices), g_Indices, GL_STATIC_DRAW_ARB );
            glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0 );
        }

        void bind()
        {
            // We need to enable the client stats for the vertex attributes we want
            // to render even if we are not using client-side vertex arrays.
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);

            // Bind the vertices's VBO
            glBindBufferARB( GL_ARRAY_BUFFER_ARB, g_uiVerticesVBO );
            glVertexPointer( 3, GL_FLOAT, sizeof(VertexXYZColor), MEMBER_OFFSET(VertexXYZColor,m_Pos) );
            glColorPointer( 3, GL_FLOAT, sizeof(VertexXYZColor), MEMBER_OFFSET(VertexXYZColor,m_Color) );

            // Bind the indices's VBO
            glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, g_uiIndicesVBO );
        }

        void submit()
        {
            glDrawElements( GL_QUADS, 24, GL_UNSIGNED_INT, BUFFER_OFFSET( 0 ) );
        }

        void unbind()
        {
            // Unbind buffers so client-side vertex arrays still work.
            glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0 );
            glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );

            // Disable the client side arrays again.
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_COLOR_ARRAY);
        }
    };

}

#include <moon9/render/texture.hpp>
#include <moon9/render/md2.hpp>

#include <moon9/time/fps.hpp>
#include <moon9/time/tweener.hpp>

int main( int argc, char **argv )
{
    moon9::window2 app;
    moon9::camera camera;
    moon9::vbo2 vbo;
    moon9::shader shader;
    moon9::tweener tweener;
    moon9::fps fps;

    vbo.set();

    moon9::md2 md2;
    md2.load( moon9::file("alita/tris.md2").read() );
    moon9::texture texture;
    texture.load( "alita/tris.png" );
    texture.submit();

    camera.position = moon9::vec3(10,10,10);

    while( app.is_open() )
    {
        if( 0 )
        {
            moon9::file file("template.shader.glsl");

            if( file.has_changed() )
            {
                std::string shader_error = shader.setup( file.read() );
                if( shader_error.size() ) die( 1, shader_error + shader.get_log() );

                shader.uniform( "size", 0.5f );
                shader.uniform( "amount", 0.5f );
            }
        }

        glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        camera.wasdec( 0, 0.2f, 0, 0, 0, 0 );
        camera.lookat( moon9::vec3(0,0,0) );
        camera.resize( app.w, app.h );
        camera.update();
        camera.apply();

        {
            moon9::matrix::scale scl(2.f);
            moon9::geometry::axes axes;
        }

        vbo.bind();
        vbo.submit();
        vbo.unbind();

        app.flush();

        fps.tick();
        fps.wait(60.f);
    }

    return 0;
}
