#include "image.hpp"

#include <iostream>

int main( int argc, char **argv )
{
    if( argc != 2 )
    {
        std::cout << "Usage: " << argv[0] << " image_file" << std::endl;
        return -1;
    }

    {
        using moon9::image;

#if 0
        image img( 320, 240 );

        for( int y = 0; y < 240; ++y )
            for( int x = 0; x < 320; ++x )
//                img.at(x,y) = pixel::rgba( x / 319.f, y / 239.f, ( x + y ) / 558.f ).to_hsla();
                img.at(x,y) = pixel::hsla( x / 319.f, y / 239.f, ( x + y ) / 558.f );
#else
        image img( argv[1] );
#endif

        image saturated = img * pixel::hsla( 1, 2, 1, 1 );
        image desaturated = img * pixel::hsla( 1, 0.5f, 1, 1 );
        image gray = img * pixel::hsla( 1, 0, 1, 1 );
        image light = img * pixel::hsla( 1, 1, 1.75f, 1 );
        image hue = img + pixel::hsla( 0.5f, 0, 0, 0 );
        image warmer = img + pixel::hsla( -0.08f, 0, 0, 0 );
        image colder = img + pixel::hsla(  0.08f, 0, 0, 0 );

        img.display( "default" );
        saturated.display( "saturated" );
        desaturated.display( "desaturated" );
        gray.display( "gray" );
        hue.display( "hue" );
        light.display( "light" );
        warmer.display( "warmer" );
        colder.display( "colder" );
    }

    //( image( "assets/textures/margo2.png" ) * pixel::hsla( 1, 1, 2, 1 ) ).display();

    return 0;
}
