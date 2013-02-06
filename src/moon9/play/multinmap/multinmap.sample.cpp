#include <iostream>
#include "multinmap.hpp"

int main( int argc, const char **argv )
{
    moon9::multinmap<int> mnm;

    mnm["helloworld"][true]['a'][-123][3.1459f][100.00].get() = 100;
    mnm["helloworld"][true]['a'][-123][3.1459f][100.00].get() *= 2;
    mnm["helloworld"][true]['a'][-123][3.1459f][100.00].get() -= 200;

    int result = mnm["helloworld"][true]['a'][-123][3.1459f][100.00].get();
    std::cout << "Computing result = " << (result) << " (0 expected)" << std::endl;

    auto iterator = mnm["helloworld"][true]['a'][-123][3.1459f][100.00].find();
    std::cout << "Computing result = " << (iterator->second) << " (0 expected)" << std::endl;

    return 0;
}
