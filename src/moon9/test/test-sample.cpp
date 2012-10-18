#include "test.hpp"

int main( int argc, char **argv )
{
	test1( true );
	test1( false );

	test3( 1, ==, 2 );
	test3( 1, !=, 2 );
	test3( 1, <, 2 );
	test3( 1, >, 2 );

	return 0;
}
