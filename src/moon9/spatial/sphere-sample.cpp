#include <iostream>

#include "sphere.hpp"

int main( int argc, char **argv )
{
    moon9::sphere s1, s2, s3;

    s1.init(  0,2,0,1,"s1");
    s2.init(  5,6,0,1,"s2");
    s3.init(2.5,4,0,1,"s3");

    std::cout << intersects( s1, s1 ) << std::endl;
    std::cout << intersects( s2, s2 ) << std::endl;
    std::cout << intersects( s3, s3 ) << std::endl;

    std::cout << intersects( s1, s2 ) << std::endl;
    std::cout << intersects( s1, s3 ) << std::endl;
    std::cout << intersects( s2, s3 ) << std::endl;

    return 0;
}

