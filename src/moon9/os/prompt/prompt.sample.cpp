#include <iostream>
#include <string>

#include "prompt.hpp"

int main( int argc, char **argv )
{
    // prompts user and prints result

    std::string result = prompt("Enter a string");

    std::cout << "prompt was '" << result << "'" << std::endl;

    return 0;
}
