
#include <iostream>

#include "debugger.hpp"

int main( int argc, char **argv )
{
    if( !debugger("We are about to launch debugger...") )
        std::cerr << "Something went wrong: debugger() didnt work" << std::endl;
    else
        std::cout << "All ok." << std::endl;

	return 0;
}
