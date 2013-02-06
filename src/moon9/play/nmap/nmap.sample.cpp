#include <iostream>
#include <string>
#include "nmap.hpp"

int main( int argc, const char **argv )
{
    moon9::nmap<std::string> nmap;

    nmap["hello"] = "world";
    nmap[123] = "456";
    nmap[true] = "sometimes";
    nmap[3.14159f] = "pi";
    nmap[100.00] = "double";

    for( auto &it : nmap )
        std::cout << it.first << ':' << it.second << std::endl;

    return 0;
}
