#include <cassert>
#include <iostream>
#include <vector>

#include <moon9/debug/assert.hpp>
#include <moon9/io/file.hpp>
#include <moon9/render/render.hpp>
#include <moon9/render/mesh.hpp>
#include <moon9/spatial/vec.hpp>
#include <moon9/string/string.hpp>

#include "obj/parser.hpp"

namespace moon9
{
    // @todo: layers: http://www.clockworkcoders.com/oglsl/tutorial8.htm
    // @todo: lights: http://www.clockworkcoders.com/oglsl/tutorial5.htm
}

#include <iostream>

#include <moon9/render/window.hpp>


int main( int argc, char **argv )
{
    std::string filename = ( argc > 1 ? argv[1] : "dungeon.obj" );

    moon9::window2 app;
    moon9::camera camera;
    camera.position = moon9::vec3( 50, 50, 50 );

    moon9::mesh mesh;
    moon9::lightwave::import( mesh, filename );

    // xzy defaults to zxy basis,  CW, apply(),   zxy normals
    // xyz defaults to xyz basis, CCW, apply90(), xyz normals

    bool use_xzy = true;

    if( use_xzy )
    {
        mesh.swap();
    }

    while( app.is_open() )
    {
        camera.wasdec( 0, 0.1f, 0, 0, 0, 0 );
        camera.lookat( moon9::vec3(0,0,0) );
        camera.resize( app.w, app.h );
        camera.update();

        if( use_xzy )
        camera.apply();
        else
        camera.apply90();

        glClearColor( 0.2f, 0.2f, 0.2f, 1.f);
        glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
        glDisable(GL_BLEND);

        moon9::style::red color;
//        moon9::invert::normals inv;
        moon9::style::wireframe wf;

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glFrontFace( use_xzy ? GL_CW : GL_CCW );

        moon9::lightwave::render( mesh );

        app.flush();
    }


    return 0;
}
