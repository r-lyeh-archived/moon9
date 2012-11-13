#include <moon9/render/render.hpp>
#include <moon9/render/window.hpp>
#include <moon9/render/texture.hpp>
#include <moon9/render/md2.hpp>

#include <moon9/io/file.hpp>

#include <moon9/time/fps.hpp>
#include <moon9/time/tweener.hpp>

#include <moon9/debug/die.hpp>


int main( int argc, char **argv )
{
    moon9::window2 app;
    moon9::camera camera;
    moon9::tweener tweener;
    moon9::fps fps;

    moon9::md2 md2;
    md2.import( moon9::file("alita/tris.md2").read() );

    moon9::texture texture;
    texture.load( "alita/tris.png" );
    texture.submit();

    camera.position = moon9::vec3(10,10,10);

    while( app.is_open() )
    {
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

        {
            moon9::matrix::scale scl(0.25f);
            moon9::disable::lighting nolights;

#if 1
            texture.bind();
                // draw frame
                // md2.render_keyframe(0);
                // animate (using given keyframes)
                // md2.render_keyframe("run", tweener.get( moon9::tweener::LINEAR_01, 2.5f, true ) );
                // animate smoothly
                md2.render_keyframe_lerp("run", tweener.get( moon9::tweener::LINEAR_01, 1.25f, true ) );
            texture.unbind();
#else
            // also
            moon9::style::texture tx1( "alita/tris.png" );
            md2.render_keyframe_lerp("run", tweener.get( moon9::tweener::LINEAR_01, 1.25f, true ) );
#endif
        }

        app.flush();

        fps.tick();
        fps.wait(60.f);
    }

    return 0;
}
