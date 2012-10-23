#include <iostream>

#include "error.hpp"

int main( int argc, char **argv )
{
    std::cout << moon9::get_os_error() << std::endl;
    return 0;
}
