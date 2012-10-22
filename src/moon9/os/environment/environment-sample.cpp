#include <cassert>

#include <iostream>

#include "environment.hpp"

int main( int argc, char **argv )
{
    std::string temp_folder = moon9::get_environment("TEMP");

    assert( temp_folder.size() > 0 );

    std::cout << "TEMP folder: " << temp_folder << std::endl;
    std::cout << "All ok." << std::endl;

    return 0;
}
