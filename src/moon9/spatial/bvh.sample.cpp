#include <iostream>

#include "sphere.hpp"
#include "bvh.hpp"

int main( int argc, char **argv )
{
    std::cout << "* Creating spheres..." << std::endl;

    moon9::sphere sph1, sph2, sph3;

    sph1.init(  0,2,0,1,"sphere #1");
    sph2.init(  5,6,0,1,"sphere #2");
    sph3.init(2.5,4,10,1,"sphere #3"); // far away!

    std::cout << "* Creating BVH of spheres..." << std::endl;

    bvh<moon9::sphere> b;

    b.add( sph1 );
    b.add( sph2 );

    std::cout << "* Testing collisions..." << std::endl;

    std::cout << sph1 << std::endl;

    std::cout << sph1.id << " collides with " << ( b.find(sph1) ? b.found().id : "nobody" ) << std::endl;
    std::cout << sph2.id << " collides with " << ( b.find(sph2) ? b.found().id : "nobody" ) << std::endl;
    std::cout << sph3.id << " collides with " << ( b.find(sph3) ? b.found().id : "nobody" ) << std::endl;

    return 0;
}
