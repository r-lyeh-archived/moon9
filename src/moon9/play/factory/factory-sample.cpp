#include <cassert>
#include <string>
#include <iostream>

#include "factory.hpp"

int main( int argc, char **argv )
{
	moon9::factory factory;

	std::string original("blue"), copy;

	factory.inscribe("blue_string", original);
	factory.clone("blue_string", copy);

	assert( copy == original );

	std::cout << "All ok." << std::endl;

	return 0;
}

