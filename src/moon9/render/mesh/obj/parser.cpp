#include <iostream>
#include <vector>

#include <moon9/debug/assert.hpp>
#include <moon9/io/file.hpp>
#include <moon9/render/render.hpp>
#include <moon9/spatial/vec.hpp>
#include <moon9/string/string.hpp>

namespace moon9
{
    struct triangle_strip
    {
        std::vector< float > vertices;
    };

    struct mesh
    {
        std::vector< float > vertices;
        std::vector< unsigned int > indices;
    };

    bool load_wavefront( const std::string &binary, moon9::mesh &mesh )
    {
        mesh = moon9::mesh();

        moon9::strings lines = moon9::string( binary ).tokenize("\r\n");

        std::cout << '.';

        char ch;
        float x,y,z;

        for( auto &it : lines )
        {
            std::stringstream ss;

            if( ss << it )
            if( ss >> ch >> x >> y >> z )
            {
                if( ch == 'v' )
                {
                    mesh.vertices.push_back( x );
                    mesh.vertices.push_back( y );
                    mesh.vertices.push_back( z );
                }
                else
                if( ch == 'f' )
                {
                    int a = int(x) - 1;
                    int b = int(y) - 1;
                    int c = int(z) - 1;
/*
                    size_t vs = mesh.vertices.size();

                    assert3( a, <, vs );
                    assert3( b, <, vs );
                    assert3( c, <, vs );
*/
                    mesh.indices.push_back( a );
                    mesh.indices.push_back( b );
                    mesh.indices.push_back( c );
                }
            }
        }

        return true;
    }

    bool load_wavefront( const std::string &binary, moon9::triangle_strip &strip )
    {
        strip = moon9::triangle_strip();

        moon9::mesh mesh;
        if( !load_wavefront( binary, mesh ) )
            return false;

        for( size_t i = 0; i < mesh.indices.size(); ++i )
            strip.vertices.push_back( mesh.vertices[ mesh.indices[i] ] );

        return true;
    }


    void render( const std::vector< float > &vertices_fff ) // t1{v.a,v.b,v.c}, t2{v.a,v.b,v.c}, ...
    {
#if 0
        const size_t N = vertices_fff.size();

        if( N < 3 )
            return;

        const float *data = vertices_fff.data();

        glBegin(GL_TRIANGLES);  // draw a cube with 12 triangles

        for( size_t i = 0, end = N / 3; i < end; ++i )
            glVertex3fv( &data[i*3] );

        glEnd();
#else
        // activate and specify pointer to vertex array
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vertices_fff.data() );

        // draw a mesh
        glDrawArrays(
            GL_LINE_STRIP, //GL_TRIANGLES,
            0, vertices_fff.size() / 3 );

        // deactivate vertex arrays after drawing
        glDisableClientState(GL_VERTEX_ARRAY);
#endif
    }

    void render( const std::vector< float > &vertices, const std::vector< unsigned int > &indices )
    {
        // activate and specify pointer to vertex array
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vertices.data() );

        // draw a mesh
        glDrawElements(
            GL_LINE_STRIP, //GL_TRIANGLES,
            indices.size(), GL_UNSIGNED_INT, indices.data() );

        // deactivate vertex arrays after drawing
        glDisableClientState(GL_VERTEX_ARRAY);
    }

}

#include <iostream>

#include <moon9/render/window.hpp>

int main( int argc, char **argv )
{
    moon9::window2 app;
    moon9::camera camera;

    moon9::mesh mesh;

    std::cout << "loading...";
    moon9::load_wavefront( moon9::file("dungeon.obj").read(), mesh );
    std::cout << " done." << std::endl;

    while( app.is_open() )
    {
        camera.position = moon9::vec3( 50, 50, 50 );
        camera.lookat( moon9::vec3(0,0,0) );
        camera.resize( app.w, app.h );
        camera.update();
        camera.apply();

        glClearColor( 0.2f, 0.2f, 0.2f, 1.f);
        glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
        glDisable(GL_BLEND);

        moon9::style::red color;
        moon9::sphere sph;
        moon9::render( mesh.vertices, mesh.indices );

        app.flush();
    }

    return 0;
}
