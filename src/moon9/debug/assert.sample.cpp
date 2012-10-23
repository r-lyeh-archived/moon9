#include <iostream>

#include "assert.hpp"

int main( int argc, char **argv )
{
#ifdef NDEBUG
    assert3( 2 + 2, ==, 5 );
    std::cout << "All 'ok'. Asserts are disabled in release mode" << std::endl;
#else
    assert3( 2 + 2, ==, 4 );
    std::cout << "First assert passed. Ready to launch debugger" << std::endl;

    assert3( 2 + 2, ==, 5 );
    std::cout << "Something went wrong: assert3() didnt work" << std::endl;
#endif

	return 0;
}
