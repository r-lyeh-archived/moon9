#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "device.hpp"
#include "blank.ogg.hpp"

//#include <AL/alut.h>
#include <stb_vorbis.h>
//#include <opus.h>


namespace
{
    enum { INVALID_ID = -1 };

    bool check_oal( const char *file, int line )
    {
        ALenum error = alGetError();

        if( error == AL_NO_ERROR )
            return true;

        std::stringstream ss;
        ss << "<moon9/audio/device.cpp> says: OpenAL error! ";

        switch ( error )
        {
            case AL_INVALID_NAME:
                ss << "invalid name";
                break;
            case AL_INVALID_ENUM:
                ss << "invalid enum";
                break;
            case AL_INVALID_VALUE:
                ss << "invalid value";
                break;
            case AL_INVALID_OPERATION:
                ss << "invalid operation";
                break;
            case AL_OUT_OF_MEMORY:
                ss << "out of memory";
                break;
            default:
                ss << "unknown error";
                break;
        };

        ss << " at " << file << ':' << line;

        std::cerr << ss.str() << std::endl;

        return false;
    }

    template<typename T>
    T check_oal( const char *file, int line, const T &t )
    {
        check_oal( file, line );
        return t;
    }
}

#if defined(NDEBUG) || defined(_NDEBUG)
#define MOON9_AL_0(...) ( (__VA_ARGS__), true )
#define MOON9_AL_N(...) (__VA_ARGS__)
#else
#define MOON9_AL_0(...) ::check_oal( __FILE__, __LINE__, ((__VA_ARGS__), true) )
#define MOON9_AL_N(...) ::check_oal( __FILE__, __LINE__, (__VA_ARGS__) )
#endif

#define $alBufferData(...)          MOON9_AL_0( alBufferData(__VA_ARGS__) )
#define $alcCloseDevice(...)        MOON9_AL_0( alcCloseDevice(__VA_ARGS__) )
#define $alcDestroyContext(...)     MOON9_AL_0( alcDestroyContext(__VA_ARGS__) )
#define $alcMakeContextCurrent(...) MOON9_AL_0( alcMakeContextCurrent(__VA_ARGS__) )
#define $alDeleteBuffers(...)       MOON9_AL_0( alDeleteBuffers(__VA_ARGS__) )
#define $alDeleteSources(...)       MOON9_AL_0( alDeleteSources(__VA_ARGS__) )
#define $alDistanceModel(...)       MOON9_AL_0( alDistanceModel(__VA_ARGS__) )
#define $alGenBuffers(...)          MOON9_AL_0( alGenBuffers(__VA_ARGS__) )
#define $alGenSources(...)          MOON9_AL_0( alGenSources(__VA_ARGS__) )
#define $alGetSourcei(...)          MOON9_AL_0( alGetSourcei(__VA_ARGS__) )
#define $alListenerf(...)           MOON9_AL_0( alListenerf(__VA_ARGS__) )
#define $alListenerfv(...)          MOON9_AL_0( alListenerfv(__VA_ARGS__) )
#define $alSourcef(...)             MOON9_AL_0( alSourcef(__VA_ARGS__) )
#define $alSourcefv(...)            MOON9_AL_0( alSourcefv(__VA_ARGS__) )
#define $alSourcei(...)             MOON9_AL_0( alSourcei(__VA_ARGS__) )
#define $alSourcePause(...)         MOON9_AL_0( alSourcePause(__VA_ARGS__) )
#define $alSourcePlay(...)          MOON9_AL_0( alSourcePlay(__VA_ARGS__) )
#define $alSourceStop(...)          MOON9_AL_0( alSourceStop(__VA_ARGS__) )

#if 0
// why are these generating errors?
#define $alcCreateContext(...)      MOON9_AL_N( alcCreateContext(__VA_ARGS__) )
#define $alcGetString(...)          MOON9_AL_N( alcGetString(__VA_ARGS__) )
#define $alcIsExtensionPresent(...) MOON9_AL_N( alcIsExtensionPresent(__VA_ARGS__) )
#define $alcOpenDevice(...)         MOON9_AL_N( alcOpenDevice(__VA_ARGS__) )

#else

#define $alcCreateContext(...)      ( alcCreateContext(__VA_ARGS__) )
#define $alcGetString(...)          ( alcGetString(__VA_ARGS__) )
#define $alcIsExtensionPresent(...) ( alcIsExtensionPresent(__VA_ARGS__) )
#define $alcOpenDevice(...)         ( alcOpenDevice(__VA_ARGS__) )

#endif

std::string device::name( int devnum ) const
{
//  std::stringstream ss;
//  if ($alGetString(AL_VERSION))
//      ss << "OpenAL version: " << $alGetString(AL_VERSION) << std::endl;
//  if ($alGetString(AL_RENDERER))
//      ss << "OpenAL renderer: " << $alGetString(AL_RENDERER) << std::endl;
//  if ($alGetString(AL_VENDOR))
//      ss << "OpenAL vendor: " << $alGetString(AL_VENDOR) << std::endl;
//  if ($alGetString(AL_EXTENSIONS))
//      ss << "OpenAL extensions: " << $alGetString(AL_EXTENSIONS) << std::endl;

    std::vector<std::string> vs;

    if( $alcIsExtensionPresent( NULL, (const ALCchar *)"ALC_ENUMERATION_EXT" ) == AL_TRUE )
    {
        const char *s = (const char *)$alcGetString( NULL, ALC_DEVICE_SPECIFIER );
        while( *s != '\0' )
        {
            vs.push_back( s );
            while( *s++ != '\0' );
        }
    }

    if( devnum < 0 || devnum >= int(vs.size()) )
        return std::string();

    return vs[ devnum ];
}

bool device::init( int devnum )
{
    alcInit();

    // select device
    std::string devname = name( devnum );
    dev = $alcOpenDevice( devname.empty() ? 0 : devname.c_str() );

    if( !dev )
        return false;

    ctx = $alcCreateContext(dev, NULL);

    if( !ctx )
    {
        $alcCloseDevice( dev );
        dev = 0;
        return false;
    }

    $alcMakeContextCurrent(ctx);

    //$alDistanceModel(AL_INVERSE_DISTANCE);
    $alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
    $alListenerf(AL_GAIN, 1.0f);

    reset();

    return true;
}

void device::quit()
{
    clear();

    if( ctx )
    {
        $alcMakeContextCurrent(NULL);
        $alcDestroyContext(ctx);
        ctx = 0;
    }

    if( dev )
    {
        $alcCloseDevice(dev);
        dev = 0;
    }

    alcDeinit();
}

void device::clear()
{
    int index = int(sounds.size());
    while( index-- )
    {
        soundDelete(index);
    }

    index = int(sources.size());
    while( index-- )
    {
        sourceDelete(index);
    }
}

void device::reset()
{
    clear();

    // reserve id (0) ; blank
    soundAdd( "ogg", blank_ogg::data(), blank_ogg::size() );
}

int device::soundAdd( const std::string &type, const void *data, size_t size )
{
    if( type.empty() )
        return INVALID_ID;

    if( !data )
        return INVALID_ID;

    if( !size )
        return INVALID_ID;

    sound_t sound;

    // Clear error flag
    alGetError();

    if( type == "ogg" )
    {
        auto *oss = stb_vorbis_open_memory( (unsigned char *)data, size, NULL, NULL);;

        if( !oss )
            return INVALID_ID;

        stb_vorbis_info info = stb_vorbis_get_info(oss);

        sound.format = info.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        sound.sampleRate = info.sample_rate;

        sound.size = (stb_vorbis_stream_length_in_samples(oss) * info.channels);

        sound.samples = new short[sound.size];

        stb_vorbis_get_samples_short_interleaved(oss, info.channels, sound.samples, sound.size );

        sound.size *= sizeof(short);

        stb_vorbis_close(oss);
    }
#if 0
    else
    if( type == "opus" )
    {
        int error;

        OpusDecoder *dec;
        dec = opus_decoder_create(48000, 2, &error);

        int samples = 2000;
        opus_int16 *pcm = new opus_int16[ samples * 2 /*stereo*/ ];
        int len = opus_decode( dec, (const unsigned char *)data, (opus_int32)size, pcm, samples, 0 );

        std::cout << "decoded: " << len << std::endl;
/*
        sound.format = info.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        sound.sampleRate = info.sample_rate;
        sound.size = (stb_vorbis_stream_length_in_samples(oss) * info.channels);
        sound.samples = new short[sound.size];
        stb_vorbis_get_samples_short_interleaved(oss, info.channels, sound.samples, sound.size );
        sound.size *= sizeof(short);
*/

        opus_decoder_destroy(dec);

        delete [] pcm;
    }
    else {
        ALboolean al_bool;
        ALvoid *data;
        alutLoadWAVFile(pathfile, &sound.format, &data, &sound.size, &sound.sampleRate, &al_bool);
        sound.samples = (short *) data;
    }
#endif

    $alGenBuffers( 1, &sound.buffer );

    if( !$alBufferData( sound.buffer, sound.format, sound.samples, sound.size, sound.sampleRate ) )
    {
        $alDeleteBuffers( 1, &sound.buffer );
        return "something went wrong", INVALID_ID;
    }

    return soundInsert( sound );
}

int device::soundAdd( const std::string &pathfile )
{
    const char *type = strrchr(pathfile.c_str(), '.') + 1;

#if 0
    FILE *fp = fopen( pathfile, "rb" );
    if( !fp )
        return "cannot open file", INVALID_ID;

    fseek( fp, 0L, SEEK_END );
    size_t size = ftell(fp);
    fseek( fp, 0L, SEEK_SET );

    void *data = malloc( size );
    fread( data, 1, size, fp );
    fclose( fp );

    auto id = addSoundPtr( type, data, size );
    free( data );
#else
    std::ifstream t( pathfile, std::ios::in | std::ios::binary );
    std::stringstream buffer;
    buffer << t.rdbuf();
    const std::string &str = buffer.str();

    if( str.empty() )
        return "cannot open file", INVALID_ID;

    auto id = soundAdd( type, &str[0], str.size() );
#endif

    return id;
}

int device::soundInsert( sound_t &sound )
{
    for( unsigned i = 0; i < sounds.size(); i++ )
    {
        if( sounds[i].samples == NULL )
        {
            sounds[i] = sound;
            return i;
        }
    }

    sounds.push_back(sound);
    return sounds.size() - 1;
}

void device::soundDelete( int sound )
{
    if( sounds[sound].samples )
    {
        $alDeleteBuffers( 1, &sounds[sound].buffer );

#if 0
        alutUnloadWAV( sounds[sound].format, sounds[sound].samples, sounds[sound].size, sounds[sound].sampleRate );
#endif

        delete [] sounds[sound].samples;
        sounds[sound].samples = NULL;
    }
}

int device::sourceAdd( int sound, bool looping, bool relative_pos )
{
    source_t soundSource;

    soundSource.sound = sound;

    $alGenSources( 1, &soundSource.source );
    $alSourcei( soundSource.source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE );
    $alSourcei( soundSource.source, AL_SOURCE_RELATIVE, relative_pos ? AL_TRUE : AL_FALSE );
    $alSourcei( soundSource.source, AL_BUFFER, sounds[sound].buffer );
    $alSourcef( soundSource.source, AL_MIN_GAIN, 0.0f );
    $alSourcef( soundSource.source, AL_MAX_GAIN, 1.0f );

    $alSourcef( soundSource.source, AL_GAIN, 1.0f );
    $alSourcef( soundSource.source, AL_PITCH, 1.0f );

    // seeking : AL_SEC_OFFSET (secs), AL_SAMPLE_OFFSET (samps), AL_BYTE_OFFSET(bytes)
    // must be reset back on every loop
    // $alSourcef( soundSource.source, AL_SEC_OFFSET, 5.0f );

    return sourceInsert( soundSource );
}

int device::sourceInsert( source_t &source )
{
    for( unsigned i = 0; i < sources.size(); i++ )
    {
        if( sources[i].sound == INVALID_ID )
        {
            sources[i] = source;
            return i;
        }
    }

    sources.push_back( source );
    return sources.size() - 1;
}

void device::sourceDelete( int source )
{
    if( sources[source].sound != INVALID_ID )
    {
        $alDeleteSources( 1, &sources[source].source );
        sources[source].sound = INVALID_ID;
    }
}

void device::sourcePlay( int source )
{
    $alSourcePlay( sources[source].source );
}

void device::sourceStop( int source )
{
    $alSourceStop( sources[source].source );
}

void device::sourcePause( int source )
{
    $alSourcePause( sources[source].source );
}

bool device::sourceIsPlaying( int source )
{
    ALint state;
    $alGetSourcei( sources[source].source, AL_SOURCE_STATE, &state );

    return ( state == AL_PLAYING );
}

void device::listenerSetPosition( const float *position3 )
{
    $alListenerfv( AL_POSITION, position3 );
}

void device::listenerSetVelocity( const float *velocity3 )
{
    $alListenerfv( AL_VELOCITY, velocity3 );
}

void device::listenerSetDirection( const float *direction3 )
{
    float orientation6[] = { direction3[0], direction3[1], direction3[2], 0, -1, 0 };
    $alListenerfv( AL_ORIENTATION, orientation6 );
}

void device::sourceSetGain( int source, const float gain )
{
    $alSourcef( sources[source].source, AL_GAIN, gain );
}

void device::sourceSetPosition( int source, const float *position3 )
{
    $alSourcefv( sources[source].source, AL_POSITION, position3 );
}

void device::sourceSetVelocity( int source, const float *velocity3 )
{
    $alSourcefv( sources[source].source, AL_VELOCITY, velocity3 );
}

void device::sourceSetDirection( int source, const float *direction3 )
{
    $alSourcefv( sources[source].source, AL_DIRECTION, direction3 );
}

void device::sourceSetAttenuation( int source, const float rollOff, const float refDistance )
{
    $alSourcef( sources[source].source, AL_REFERENCE_DISTANCE, refDistance );
    $alSourcef( sources[source].source, AL_ROLLOFF_FACTOR, rollOff );
}


// ------------------------------------------------------------------------

#if 0

// proposal

namespace
{
    int makeid()
    {
        static int id = 0;
        return ++id;
    }

    void reset3( float *v )
    {
        v[0] = v[1] = v[2] = 0.f;
    }
}

void init( listener &ltn )
{
    ltn.id = makeid();
    ltn.gain = 0;

    reset3( ltn.position );
    reset3( ltn.velocity );
    reset3( ltn.orientation );
}

void update( listener &ltn )
{}

void init( speaker &spk )
{
    spk.id = makeid();
    spk.source = 0;
    spk.gain = 0;
    spk.reference_distance = 0;
    spk.rolloff = 0;
    spk.playing = 1;
    spk.paused = 0;
    spk.looping = 0;
    spk.relative_position = 0;

    reset3( spk.position );
    reset3( spk.velocity );
    reset3( spk.orientation );
}

int load( const std::string &resource )
{
    return device::defaults()->soundAdd( resource );
}

void unload( int source )
{
    device::defaults()->soundDelete( source );
}

void update( speaker &spk )
{
    if( !spk.source )
        return;

    spk.playing = device::defaults()->sourceIsPlaying( spk.source );

}

context::context() : id( makeid() ), device( 0 )
{
    init( ltn );
}

void update( context &ctx )
{
    update( ctx.ltn );

    for( auto &it : ctx.ui )
        update( it );

    for( auto &it : ctx.sfx )
        update( it );

    for( auto &it : ctx.bgm )
        update( it );

    for( auto &it : ctx.vox )
        update( it );

    for( auto &it : ctx.amb )
        update( it );
}

#endif
