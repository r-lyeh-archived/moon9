#include <iostream>
#include "multinmap.hpp"

int main( int argc, const char **argv )
{
    moon9::multinmap<int> mnm;

    // voxel example
    mnm[10.f][0.f][-10.f].get() = 12312921;

    // agenda example
    mnm["john"]["doe"].get() = -1;

    // direct access
    std::cout << "Result = " << mnm["john"]["doe"].get() << std::endl;

    // quick search
    if( mnm["john"]["doe"].findit() )
        std::cout << "Result = " << mnm.found() << std::endl;

    // std search
    auto iterator = mnm["john"]["doe"].find();
    if( iterator != mnm.end() )
        std::cout << "Result = " << (iterator->second) << std::endl;

    return 0;
}
