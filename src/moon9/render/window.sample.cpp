#include <moon9/render/gl.hpp>
#include <moon9/render/window.hpp>

class window : public moon9::window // moon9::miniwin, moon9::miniwin2
{
    public:

    window( const char *name, float size ) :
        moon9::window( name, size )
    {
        glClearColor( 0.2f, 0.2f, 0.2f, 1.f);
        glDisable(GL_BLEND);
    }

    void update( double t, float dt )
    {
        camera.position = moon9::vec3( 50, 50, 50 );
        camera.lookat( moon9::vec3(0,0,0) );
        camera.resize( w, h );
        camera.update();
        camera.apply();
    }

    void render( double t, float dt ) //const
    {
        glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );

        moon9::matrix::translate tr( moon9::vec3( 0, 0, 0 ) );
        moon9::matrix::scale sc( 1 + abs(sin(t)) * 10 );
        moon9::matrix::rotate_x rx( sin(t) * 90 );
        moon9::style::red red;
        moon9::sphere sph;
    }
};

int main( int argc, char **argv )
{
    window app( "window sample", 0.85f );

    while( app.is_open() )
    {
        app.flush();
    }

    return 0;
}
