#include <iostream>

#include "format.hpp"

int main( int argc, char **argv )
{
    std::cout << moon9::format("%d %1.3f %s", 10, 3.14159f, "hello world") << std::endl;
    return 0;
}
