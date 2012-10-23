#include <algorithm>
#include <cctype>
#include <string>

std::string lowercase( std::string s )
{
    std::transform( s.begin(), s.end(), s.begin(), (int(*)(int)) std::tolower );

    return s;
}

#include <iostream>

#include "trie.hpp"

int main( int argc, char **argv )
{
    trie t;

    std::cout << "Type any 'word' to insert it. Type any '*', 'w*', 'wo*', 'wor*', 'word*' combination to autocomplete it. Type 'exit' to exit." << std::endl;

    for( ;; )
    {
        std::cout << '>';

        std::string line;
        std::getline( std::cin, line );

        if( line.size() )
        {
            line = lowercase(line);

            if( line == "exit" )
                return 0;

            if( line.size() > 0 && line.at( line.size() - 1 ) == '*' )
            {
                line.pop_back();

                std::cout << "Words found: ";

                for( auto &it : t.autocomplete( line ) )
                    std::cout << it << ',';

                std::cout << std::endl;
            }
            else
            {
                if( !t.find( line ) )
                {
                    t.insert( line );
                    std::cout << "'" << line << "' inserted" << std::endl;
                }
                else
                {
                    std::cout << "'" << line << "' already exists" << std::endl;
                }
            }
        }
    }

    return 0;
}
