#include <iostream>
#include <string>
#include <vector>

#include "netstring.hpp"

int main( int argc, char **argv )
{
    std::cout << to_netstring( "hello world" ) << std::endl;
    std::cout << from_netstring( to_netstring( "hello world" ) ) << std::endl;

    std::vector<std::string> tests;
    tests.push_back( "hello world" );
    tests.push_back( "123456" );
    std::string net = to_netstrings( tests );
    std::cout << net << std::endl;

    tests.clear();
    tests = from_netstrings( net );

    for( auto &it : tests )
        std::cout << it << std::endl;

    return 0;
}

