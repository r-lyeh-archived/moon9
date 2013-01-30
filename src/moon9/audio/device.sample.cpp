#include <iostream>

#include "device.hpp"

int main( int argc, char **argv )
{
    if( argc != 2 )
        return std::cerr << "Usage: " << argv[0] << " file.ogg" << std::endl, -1;

    audio_t audio;

    for( auto &it : audio.devices )
        std::cout << "Audio device: " << it.second.name << std::endl;

    if( !audio.devices[0].init() )
        return std::cerr << "Cant open audio device." << std::endl, -1;

    std::cout << "Using audio device: " << audio.devices[0].name << std::endl;

    auto &channels = audio.devices[0].contexts[0].channels;

    enum { SFX, MUS, AMB, VOX, VID, UI };

    channels[ MUS ].reset();

    int source = channels[ MUS ].playonce( argv[1] );

    while( channels[ MUS ].sources[ source ].is_playing() )
    {}

    return 0;
}
