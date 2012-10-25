#include <iostream>
#include <cassert>

#include "rand.hpp"


int main( int argc, char **argv )
{
	for( int i = 0; i < 1000000; ++i )
	{
		float dt01 = moon9::rand01();
		assert( dt01 >= 0 );
		assert( dt01 <= 1 );

		float dt11 = moon9::rand11();
		assert( dt11 >= -1 );
		assert( dt11 <=  1 );

		float nm = moon9::rand( 3, 17 );
		assert( nm >= 3 );
		assert( nm <= 17 );

		float n = moon9::rand( 17 );
		assert( n >= 0 );
		assert( n <= 17 );
	}

	std::cout << "All ok." << std::endl;

	return 0;
}
