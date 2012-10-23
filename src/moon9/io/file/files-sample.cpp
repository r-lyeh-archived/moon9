#include <cassert>
#include <iostream>

#include "files.hpp"

int main( int argc, char **argv )
{
	moon9::files files;
	files.include("./", "*", false);

	assert( files.size() > 0 );

	std::cout << "All ok." << std::endl;

	return 0;
}
