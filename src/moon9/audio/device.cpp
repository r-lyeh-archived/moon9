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

#if defined(NDEBUG) || defined(_NDEBUG)
#   define MOON9_AL_0(...) ( (__VA_ARGS__), true )
#   define MOON9_AL_N(...) (__VA_ARGS__)
#else
#   define MOON9_AL_0(...) check_oal( __FILE__, __LINE__, ((__VA_ARGS__), true) )
#   define MOON9_AL_N(...) check_oal( __FILE__, __LINE__, (__VA_ARGS__) )
#endif

#   define $alBufferData(...)          MOON9_AL_0( alBufferData(__VA_ARGS__) )
#   define $alDeleteBuffers(...)       MOON9_AL_0( alDeleteBuffers(__VA_ARGS__) )
#   define $alDeleteSources(...)       MOON9_AL_0( alDeleteSources(__VA_ARGS__) )
#   define $alDistanceModel(...)       MOON9_AL_0( alDistanceModel(__VA_ARGS__) )
#   define $alGenBuffers(...)          MOON9_AL_0( alGenBuffers(__VA_ARGS__) )
#   define $alGenSources(...)          MOON9_AL_0( alGenSources(__VA_ARGS__) )
#   define $alGetSourcei(...)          MOON9_AL_0( alGetSourcei(__VA_ARGS__) )
#   define $alListenerf(...)           MOON9_AL_0( alListenerf(__VA_ARGS__) )
#   define $alListenerfv(...)          MOON9_AL_0( alListenerfv(__VA_ARGS__) )
#   define $alSourcef(...)             MOON9_AL_0( alSourcef(__VA_ARGS__) )
#   define $alSourcefv(...)            MOON9_AL_0( alSourcefv(__VA_ARGS__) )
#   define $alSourcei(...)             MOON9_AL_0( alSourcei(__VA_ARGS__) )
#   define $alSourcePause(...)         MOON9_AL_0( alSourcePause(__VA_ARGS__) )
#   define $alSourcePlay(...)          MOON9_AL_0( alSourcePlay(__VA_ARGS__) )
#   define $alSourceStop(...)          MOON9_AL_0( alSourceStop(__VA_ARGS__) )

//  dont check these
#   define $alcCloseDevice(...)        ( alcCloseDevice(__VA_ARGS__) )
#   define $alcCreateContext(...)      ( alcCreateContext(__VA_ARGS__) )
#   define $alcDestroyContext(...)     ( alcDestroyContext(__VA_ARGS__) )
#   define $alcGetString(...)          ( alcGetString(__VA_ARGS__) )
#   define $alcIsExtensionPresent(...) ( alcIsExtensionPresent(__VA_ARGS__) )
#   define $alcMakeContextCurrent(...) ( alcMakeContextCurrent(__VA_ARGS__) )
#   define $alcOpenDevice(...)         ( alcOpenDevice(__VA_ARGS__) )

}

namespace
{
    enum { INVALID_ID = -1 };

}

audio_t::audio_t()
{
    alcInit();

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
            int dev = int( devices.size() );
            devices[ dev ] = device_t();
			devices[ dev ].dev = 0;
            devices[ dev ].name = s;
            devices[ dev ].devnum = dev;
            while( *s++ != '\0' );
        }
    }

    alcDeinit();
}

audio_t::~audio_t()
{
    clear();

    for( auto &it : devices )
        it.second.quit();
}

bool device_t::init()
{
    alcInit();

    // select device
    dev = 0;
    dev = $alcOpenDevice( name.empty() ? 0 : name.c_str() );

    if( !dev )
        return false;

    contexts[3].init( dev );
    contexts[2].init( dev );
    contexts[1].init( dev );
    contexts[0].init( dev );

    contexts[0].enable();

    return true;
}

void device_t::quit()
{
    for( auto &it : contexts )
        it.second.quit();

    if( dev )
    {
        $alcCloseDevice(dev);
        dev = 0;
    }

    alcDeinit();
}

bool context_t::init( ALCdevice *dev )
{
    alcInit();

    // select device
    ctx = 0;
    ctx = $alcCreateContext(dev, NULL);

    if( !ctx )
        return false;

    enable();

    //$alDistanceModel(AL_INVERSE_DISTANCE);
    $alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
    $alListenerf(AL_GAIN, 1.0f);

    return true;
}

void context_t::enable()
{
    $alcMakeContextCurrent(ctx);
}

void context_t::disable()
{
    $alcMakeContextCurrent(0);
}

void context_t::quit()
{
    if( ctx )
    {
        disable();
        $alcDestroyContext(ctx);
        ctx = 0;
    }

    alcDeinit();
}

// --------------------------------------------------------------------------

void listener_t::gain( const float gain )
{
    $alListenerf( AL_GAIN, gain );
}

void listener_t::position( const float *position3 )
{
    $alListenerfv( AL_POSITION, position3 );
}

void listener_t::velocity( const float *velocity3 )
{
    $alListenerfv( AL_VELOCITY, velocity3 );
}

void listener_t::direction( const float *direction3 )
{
    float orientation6[] = { direction3[0], direction3[1], direction3[2], 0, -1, 0 };
    $alListenerfv( AL_ORIENTATION, orientation6 );
}

// --------------------------------------------------------------------------

bool sound_t::load( const std::string &type, const void *data, size_t size )
{
    if( type.empty() )
        return false;

    if( !data )
        return false;

    if( !size )
        return false;

    // Clear error flag
    alGetError();

    if( type == "ogg" )
    {
        auto *oss = stb_vorbis_open_memory( (unsigned char *)data, size, NULL, NULL);;

        if( !oss )
            return false;

        stb_vorbis_info info = stb_vorbis_get_info(oss);

        this->format = info.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        this->sampleRate = info.sample_rate;

        this->size = (stb_vorbis_stream_length_in_samples(oss) * info.channels);

        this->samples = new short[this->size];

        stb_vorbis_get_samples_short_interleaved(oss, info.channels, this->samples, this->size );

        this->size *= sizeof(short);

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
        this->format = info.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        this->sampleRate = info.sample_rate;
        this->size = (stb_vorbis_stream_length_in_samples(oss) * info.channels);
        this->samples = new short[this->size];
        stb_vorbis_get_samples_short_interleaved(oss, info.channels, this->samples, this->size );
        this->size *= sizeof(short);
*/

        opus_decoder_destroy(dec);

        delete [] pcm;
    }
    else {
        ALboolean al_bool;
        ALvoid *data;
        alutLoadWAVFile(pathfile, &this->format, &data, &this->size, &this->sampleRate, &al_bool);
        this->samples = (short *) data;
    }
#endif

    $alGenBuffers( 1, &this->buffer );

    if( !$alBufferData( this->buffer, this->format, this->samples, this->size, this->sampleRate ) )
    {
        $alDeleteBuffers( 1, &this->buffer );
        return "something went wrong", false;
    }

    return true;
}

bool sound_t::load( const std::string &pathfile )
{
    const char *type = strrchr(pathfile.c_str(), '.') + 1;

    std::ifstream t( pathfile, std::ios::in | std::ios::binary );
    std::stringstream buffer;
    buffer << t.rdbuf();
    const std::string &str = buffer.str();

    if( str.empty() )
        return "cannot open file", false;

    return load( type, &str[0], str.size() );
}

void sound_t::unload()
{
    if( buffer )
    {
        $alDeleteBuffers( 1, &buffer );

#if 0
        alutUnloadWAV( sound.format, sound.samples, sound.size, sound.sampleRate );
#endif
    }

    if( samples )
    {
        delete [] samples;
        samples = NULL;
    }
}

// ---------------------------------------------------------------------------

bool source_t::create()
{
    $alGenSources( 1, &source );
    return source > 0;
}

bool source_t::bind( int buffer, bool looping, bool relative_pos )
{
    this->buffer = buffer;

    $alSourcei( source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE );
    $alSourcei( source, AL_SOURCE_RELATIVE, relative_pos ? AL_TRUE : AL_FALSE );

    $alSourcei( source, AL_BUFFER, buffer );
    //$alSourceQueue

    $alSourcef( source, AL_MIN_GAIN, 0.0f );
    $alSourcef( source, AL_MAX_GAIN, 1.0f );

    $alSourcef( source, AL_GAIN, 1.0f );
    $alSourcef( source, AL_PITCH, 1.0f );

    // seeking : AL_SEC_OFFSET (secs), AL_SAMPLE_OFFSET (samps), AL_BYTE_OFFSET(bytes)
    // must be reset back on every loop
    // $alSourcef( soundSource.source, AL_SEC_OFFSET, 5.0f );

    return true;
}

void source_t::unbind()
{
	if( buffer )
	{
	    $alSourcei( source, AL_BUFFER, NULL );
		buffer = 0;
	}
}

void source_t::purge()
{
    unbind();

    if( source )
    {
        $alDeleteSources( 1, &source );
        source = 0;
    }
}

void source_t::play()
{
    $alSourcePlay( source );
}

void source_t::stop()
{
    $alSourceStop( source );
}

void source_t::pause()
{
    $alSourcePause( source );
}

bool source_t::is_playing() const
{
    ALint state;
    $alGetSourcei( source, AL_SOURCE_STATE, &state );

    return ( state == AL_PLAYING );
}

void source_t::gain( const float gain )
{
    $alSourcef( source, AL_GAIN, gain );
}

void source_t::position( const float *position3 )
{
    $alSourcefv( source, AL_POSITION, position3 );
}

void source_t::velocity( const float *velocity3 )
{
    $alSourcefv( source, AL_VELOCITY, velocity3 );
}

void source_t::direction( const float *direction3 )
{
    $alSourcefv( source, AL_DIRECTION, direction3 );
}

void source_t::attenuation( const float rollOff, const float refDistance )
{
    $alSourcef( source, AL_REFERENCE_DISTANCE, refDistance );
    $alSourcef( source, AL_ROLLOFF_FACTOR, rollOff );
}

/*

ALfloat GetBufferLength(ALuint buffer)
{
    ALint size, bits, channels, freq;

    alGetBufferi(buffer, AL_SIZE, &size);
    alGetBufferi(buffer, AL_BITS, &bits);
    alGetBufferi(buffer, AL_CHANNELS, &channels);
    alGetBufferi(buffer, AL_FREQUENCY, &freq);
    if(alGetError() != AL_NO_ERROR)
        return 0f;

    return (ALfloat)((ALuint)size/channels/(bits/8)) / (ALfloat)freq;
}

*/

// ------------------------------------------------------------------------

void channel_t::clear()
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

void channel_t::reset()
{
    clear();

    // reserve id (0) ; blank
    sound_t snd;
    snd.load( "ogg", blank_ogg::data(), blank_ogg::size() );

    soundInsert( snd );
}

int channel_t::soundInsert( sound_t &sound )
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

void channel_t::soundDelete( int _sound )
{
    auto &sound = sounds[_sound];

    if( sound.samples )
    {
        // unbind sound from all sources
        for( auto &it : sources )
        {
            ALint id;
            $alGetSourcei( it.source, AL_BUFFER, &id );

            if( id == sound.buffer )
                $alSourcei( it.source, AL_BUFFER, NULL );
        }

        sound.unload();
    }
}

int channel_t::sourceInsert( source_t &source )
{
    for( unsigned i = 0; i < sources.size(); i++ )
    {
        if( !sources[i].buffer )
        {
            sources[i] = source;
            return i;
        }
    }

    sources.push_back( source );
    return sources.size() - 1;
}

void channel_t::sourceDelete( int source )
{ sources[source].purge(); }

// ----------------------------------------------------------------------------

void audio_t::clear()
{
    for( auto &it : devices )
        it.second.clear();
}
void audio_t::reset()
{
    for( auto &it : devices )
        it.second.reset();
}

void device_t::clear()
{
    for( auto &it : contexts )
        it.second.clear();
}
void device_t::reset()
{
    for( auto &it : contexts )
        it.second.reset();
}

void context_t::clear()
{
    for( auto &it : channels )
        it.second.clear();
}
void context_t::reset()
{
    for( auto &it : channels )
        it.second.reset();
}


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
