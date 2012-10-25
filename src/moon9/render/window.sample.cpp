#include "os.hpp"
#include "window.hpp"

class window : public moon9::window
{
    public:

    window( const char *name, float size ) :
        moon9::window( name, size )
    {}

    void update( double t, float dt )
    {
        camera.position = moon9::vec3(7000 * 1,70,70);
        camera.lookat( norm( moon9::vec3(1 * 1000, 1, 1) ) );
        camera.update();
        camera.apply();
    }

    void render( double t, float dt ) //const
    {
        glClearColor(0.32f,0.32f,0.32f, 1.f);
        glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
        glColor4f(1.f,1.f,1.f,1.f);
        glDisable(GL_BLEND); //ojo!, mover a moon9

        moon9::matrix::translate tr( moon9::vec3(100,0,0) );
        moon9::matrix::scale sc( 999.0 );
        moon9::matrix::rotate_x rx( sin(t) * 90 );
        moon9::style::red red;
        moon9::sphere sph;
    }
};

int main( int argc, char **argv )
{
    window app( "window sample", 0.85f );

    while( app.is_open() )
    {}

    return 0;
}
