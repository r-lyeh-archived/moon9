#include <iostream>
#include <string>

#include "file.hpp"
#include "pak.hpp"

int main( int argc, char **argv )
{
    if( const bool saving_test = true )
    {
        moon9::pak pak;

        pak.resize(2);

        pak[0]["filename"] = "test.txt";
        pak[0]["content"] = "hello world";

        pak[1]["filename"] = "pak-sample.exe";
        pak[1]["content"] = moon9::file( "pak-sample.exe" ).read();

        std::cout << "zipping files..." << std::endl;

        moon9::file( "test.zip" ).overwrite( pak.bin() );

        std::cout << "saving test:\n" << pak.debug() << std::endl;
    }

    if( const bool loading_test = true )
    {
        std::string binary = moon9::file("test.zip").read();

        std::cout << "unzipping files..." << std::endl;

        moon9::pak pak;
        pak.bin( std::string( binary.begin(), binary.end() ) );

        std::cout << "loading test:\n" << pak.debug() << std::endl;
    }

    return 0;
}
