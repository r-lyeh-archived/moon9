#include <iostream>

#include "device.hpp"



int main( int argc, char **argv )
{
    if( argc != 2 )
    {
        std::cout << "Usage: " << argv[0] << " file.ogg" << std::endl;
        return -1;
    }

#if 1

    device audio;

    if( !audio.init(0) )
    {
        std::cerr << "Cant open audio device: " << audio.name(0) << std::endl;
        return -1;
    }

    std::cout << "Using audio device: " << audio.name(0) << std::endl;

    int sndid = audio.soundAdd( argv[1] );

    if( sndid > 0 )
    {
        int srcid = audio.sourceAdd( sndid );

        audio.sourcePlay( srcid );
        while( audio.sourceIsPlaying( srcid ) )
        {}
    }

#else

    context ctx;

    int id = load( argv[1] );

    speaker spk;
    init( spk );
    spk.source = load( argv[1] );

    ctx.ui.push_back( spk );

    update( ctx );

    while( ctx.ui.front().playing )
        ;

#endif

    return 0;
}
