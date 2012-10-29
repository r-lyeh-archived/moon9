#include <map>
#include <string>
#include <iostream>

#include "stats.hpp"

int main( int argc, char **argv )
{
    std::map< std::string, moon9::stats<int> > stats;

    stats["bullets"].push_back( 100 );
    stats["bullets"].push_back( 50 );
    stats["bullets"].push_back( 120 );
    stats["bullets"].push_back( 25 );

    std::cout << stats["bullets"].report("bullets", 80, 20, false);
    std::cout << stats["bullets"].report("bullets", 80, 20,  true);

    return 0;
}

