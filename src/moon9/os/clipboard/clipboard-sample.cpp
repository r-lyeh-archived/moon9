#include <cassert>

#include <string>
#include <iostream>

#include "clipboard.hpp"

int main( int argc, char **argv )
{
    std::string saved = moon9::get_clipboard();
    std::cout << "Saved clipboard text: '" << moon9::get_clipboard() << "'" << std::endl;

    moon9::set_clipboard("hello world");
    std::cout << "Current clipboard text: '" << moon9::get_clipboard() << "'" << std::endl;

    assert( moon9::get_clipboard() == "hello world" );

    moon9::set_clipboard(saved);
    std::cout << "Restored clipboard text: '" << moon9::get_clipboard() << "'" << std::endl;

    std::cout << "All ok." << std::endl;

    return 0;
}
