#include <iostream>

#include "string.hpp"
#include "trie.hpp"

int main( int argc, char **argv )
{
    trie t;

    std::cout << "Type any 'word' to insert it. Type any '*', 'w*', 'wo*', 'wor*', 'word*' combination to autocomplete it. Type 'exit' to exit." << std::endl;

    for( ;; )
    {
        std::cout << '>';

        moon9::string line;
        std::getline( std::cin, line );

        if( line.size() )
        {
            line = line.lowercase();

            if( line == "exit" )
                return 0;

            if( line.at(-1) == '*' )
            {
                line.pop_back();
                std::cout << moon9::string("Words found: \1", moon9::strings( t.autocomplete( line ) ).str( "\1," ) ) << std::endl;
            }
            else
            {
                if( !t.find( line ) )
                {
                    t.insert( line );
                    std::cout << moon9::string("'\1' inserted", line ) << std::endl;
                }
                else
                {
                    std::cout << moon9::string("'\1' already exists", line ) << std::endl;
                }
            }
        }
    }

    return 0;
}
