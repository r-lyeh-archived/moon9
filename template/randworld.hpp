
#include <moon9/math/rand.hpp>


void render_random_world( int geometries, int dimension )
{
    struct render_random
    {
        static void cube( const moon9::vec3 &pos, int min_dim, int max_dim )
        {
            float dim = moon9::rand( min_dim, max_dim );

            moon9::matrix::translate tr( pos - moon9::vec3(0,0,dim/2) ); // + !!!
            moon9::matrix::scale scl( dim, dim, dim );
            moon9::geometry::cube cb;
        }
        static void pilar( const moon9::vec3 &pos, int min_dim, int max_dim )
        {
            float dim = moon9::rand( min_dim, max_dim / 2 );
            float height = moon9::rand( dim, max_dim );

            moon9::matrix::translate tr( pos - moon9::vec3(0,0,height/2) ); // + !!!
            moon9::matrix::scale scl( dim, dim, height );
            moon9::geometry::cube cb;
        }
    };

    {
        // "floor"
        moon9::matrix::translate tr( moon9::vec3(0,0,0) );
        moon9::matrix::scale scl( dimension, dimension, 0.1f );
        moon9::invert::normals inv;
        moon9::geometry::cube cb;
    }

    for( int i = 0; i < geometries; ++i )
    {
        moon9::vec3 pos( moon9::rand11() * dimension * 0.5f, moon9::rand11() * dimension * 0.5f, i * 0.001f );
        if( i % 2 )
        render_random::cube( pos.xyz(), 1, dimension / 10 );
        else
        render_random::pilar( pos.yxz(), 1, dimension / 10 );
    }
}
