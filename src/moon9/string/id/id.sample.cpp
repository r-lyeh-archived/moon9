#include <iostream>

#include "id.hpp"

int main( int argc, char **argv )
{
    size_t id = moon9::id("hello world");

    std::cout << id << std::endl;

    return 0;
}
