#include <iostream>
#include <string>

#include "arc4.hpp"

int main( int argc, char **argv )
{
    std::string text = "Hello world.";
    std::string passkey = "abc123";

    std::cout << "Passkey: " << passkey << std::endl;
    std::cout << "Original text: " << text << std::endl;

    {
        std::string encrypted = moon9::ARC4( text, passkey );
        std::string decrypted = moon9::ARC4( encrypted, passkey );

        std::cout << "ARC4 Encrypted text: " << encrypted << std::endl;
        std::cout << "ARC4 Decrypted text: " << decrypted << std::endl;
    }

    return 0;
}
