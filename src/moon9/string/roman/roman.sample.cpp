#include <cassert>
#include <iostream>

#include "roman.hpp"

int main( int argc, char **argv )
{
    using moon9::roman;

    assert( roman(0) == "" );
    assert( roman(10) == "X" );
    assert( roman(1990) == "MCMXC" );
    assert( roman(2008) == "MMVIII" );
    assert( roman(99) == "XCIX" );
    assert( roman(47) == "XLVII" );

    std::cout << "All ok." << std::endl;

    return 0;
}
