#include <iostream>

#include "device.hpp"

int main( int argc, char **argv )
{
    if( argc != 2 )
        return std::cerr << "Usage: " << argv[0] << " file.ogg" << std::endl, -1;

    // enumerate audio devices
    for( auto &it : enumerate() )
        std::cout << "Audio device: " << it << std::endl;

    // create as many audio contexts as you want
    context_t sfx, music, voice, ambient, video, ui;

    // sfx uses device #0
    if( !sfx.init(0) )
        return std::cerr << "Cant open audio device." << std::endl, -1;

    // print device name
    std::cout << "Using audio device: " << sfx.devname << std::endl;

    // @todo: purge @playonce
    int source = sfx.playonce( argv[1] );

    // wait for source to finish
    while( sfx.sources[ source ].is_playing() )
    {}

    return 0;
}
