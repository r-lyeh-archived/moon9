#include <iostream>

#include "confirm.hpp"

int main( int argc, char **argv )
{
    if( confirm( "Are you sure?", "Question") )
        std::cout << "yes!" << std::endl;
    else
        std::cout << "nop!" << std::endl;

    return 0;
}
