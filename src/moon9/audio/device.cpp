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


#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>
//#include <AL/efx-creative.h>

// Effect objects
LPALGENEFFECTS alGenEffects = 0;
LPALDELETEEFFECTS alDeleteEffects = 0;
LPALISEFFECT alIsEffect = 0;
LPALEFFECTI alEffecti = 0;
LPALEFFECTIV alEffectiv = 0;
LPALEFFECTF alEffectf = 0;
LPALEFFECTFV alEffectfv = 0;
LPALGETEFFECTI alGetEffecti = 0;
LPALGETEFFECTIV alGetEffectiv = 0;
LPALGETEFFECTF alGetEffectf = 0;
LPALGETEFFECTFV alGetEffectfv = 0;
// Filter objects
LPALGENFILTERS alGenFilters = 0;
LPALDELETEFILTERS alDeleteFilters = 0;
LPALISFILTER alIsFilter = 0;
LPALFILTERI alFilteri = 0;
LPALFILTERIV alFilteriv = 0;
LPALFILTERF alFilterf = 0;
LPALFILTERFV alFilterfv = 0;
LPALGETFILTERI alGetFilteri = 0;
LPALGETFILTERIV alGetFilteriv = 0;
LPALGETFILTERF alGetFilterf = 0;
LPALGETFILTERFV alGetFilterfv = 0;
// Slot objects
LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots = 0;
LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots = 0;
LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot = 0;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti = 0;
LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv = 0;
LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf = 0;
LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv = 0;
LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti = 0;
LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv = 0;
LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf = 0;

bool init_efx()
{
    // Effect objects
    alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
    alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
    alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");
    alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
    alEffectiv = (LPALEFFECTIV)alGetProcAddress("alEffectiv");
    alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
    alEffectfv = (LPALEFFECTFV)alGetProcAddress("alEffectfv");
    alGetEffecti = (LPALGETEFFECTI)alGetProcAddress("alGetEffecti");
    alGetEffectiv = (LPALGETEFFECTIV)alGetProcAddress("alGetEffectiv");
    alGetEffectf = (LPALGETEFFECTF)alGetProcAddress("alGetEffectf");
    alGetEffectfv = (LPALGETEFFECTFV)alGetProcAddress("alGetEffectfv");
    // Filter objects
    alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
    alDeleteFilters = (LPALDELETEFILTERS)alGetProcAddress("alDeleteFilters");
    alIsFilter = (LPALISFILTER)alGetProcAddress("alIsFilter");
    alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri");
    alFilteriv = (LPALFILTERIV)alGetProcAddress("alFilteriv");
    alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf");
    alFilterfv = (LPALFILTERFV)alGetProcAddress("alFilterfv");
    alGetFilteri = (LPALGETFILTERI)alGetProcAddress("alGetFilteri");
    alGetFilteriv = (LPALGETFILTERIV)alGetProcAddress("alGetFilteriv");
    alGetFilterf = (LPALGETFILTERF)alGetProcAddress("alGetFilterf");
    alGetFilterfv = (LPALGETFILTERFV)alGetProcAddress("alGetFilterfv");
    // Slot objects
    alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
    alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
    alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot");
    alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
    alAuxiliaryEffectSlotiv = (LPALAUXILIARYEFFECTSLOTIV)alGetProcAddress("alAuxiliaryEffectSlotiv");
    alAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf");
    alAuxiliaryEffectSlotfv = (LPALAUXILIARYEFFECTSLOTFV)alGetProcAddress("alAuxiliaryEffectSlotfv");
    alGetAuxiliaryEffectSloti = (LPALGETAUXILIARYEFFECTSLOTI)alGetProcAddress("alGetAuxiliaryEffectSloti");
    alGetAuxiliaryEffectSlotiv = (LPALGETAUXILIARYEFFECTSLOTIV)alGetProcAddress("alGetAuxiliaryEffectSlotiv");
    alGetAuxiliaryEffectSlotf = (LPALGETAUXILIARYEFFECTSLOTF)alGetProcAddress("alGetAuxiliaryEffectSlotf");

    return true;
}

bool has_efx( ALCdevice *dev )
{
    return (alcIsExtensionPresent(dev, "ALC_EXT_EFX") == AL_TRUE); // ALC_TRUE
}


namespace
{
    size_t mread( void *t, size_t element_size, size_t num_elements, unsigned char *&data )
    {
        size_t offs = 0;

        if( t && (element_size * num_elements > 0) && data )
        {
            unsigned char *ut = (unsigned char *)t;
            unsigned char *udata = (unsigned char *)data;

            for( size_t i = 0; i < num_elements; ++i )
            {
                for( size_t e = 0; e < element_size; ++e )
                {
                    (*ut++) = (*udata++);
                }
            }

            offs = ( int(udata) - int(data) );
            data = udata;
        }

        return offs;
    }

    const char *error( ALenum error )
    {
        switch ( error )
        {
            case AL_INVALID_NAME:
                return "invalid name";
            case AL_INVALID_ENUM:
                return "invalid enum";
            case AL_INVALID_VALUE:
                return "invalid value";
            case AL_INVALID_OPERATION:
                return "invalid operation";
            case AL_OUT_OF_MEMORY:
                return "out of memory";
            default:
                return "unknown error";
        };
    }

    bool check_oal( const char *file, int line )
    {
        ALenum errorno = alGetError();

        if( errorno == AL_NO_ERROR )
            return true;

        std::stringstream ss;
        ss << "<moon9/audio/device.cpp> says: OpenAL error! ";
        ss << error( errorno );
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

// N
#   define $alGetBoolean(...)               MOON9_AL_N( alGetBoolean(__VA_ARGS__) )
#   define $alIsBuffer(...)                 MOON9_AL_N( alIsBuffer(__VA_ARGS__) )
#   define $alIsEnabled(...)                MOON9_AL_N( alIsEnabled(__VA_ARGS__) )
#   define $alIsExtensionPresent(...)       MOON9_AL_N( alIsExtensionPresent(__VA_ARGS__) )
#   define $alIsSource(...)                 MOON9_AL_N( alIsSource(__VA_ARGS__) )
#   define $alGetDouble(...)                MOON9_AL_N( alGetDouble(__VA_ARGS__) )
#   define $alGetEnumValue(...)             MOON9_AL_N( alGetEnumValue(__VA_ARGS__) )
#   define $alGetError(...)                 MOON9_AL_N( alGetError(__VA_ARGS__) )
#   define $alGetFloat(...)                 MOON9_AL_N( alGetFloat(__VA_ARGS__) )
#   define $alGetInteger(...)               MOON9_AL_N( alGetInteger(__VA_ARGS__) )
#   define $alGetString(...)                MOON9_AL_N( alGetString(__VA_ARGS__) )

// 0
#   define $alBuffer3f(...)                 MOON9_AL_0( alBuffer3f(__VA_ARGS__) )
#   define $alBuffer3i(...)                 MOON9_AL_0( alBuffer3i(__VA_ARGS__) )
#   define $alBufferData(...)               MOON9_AL_0( alBufferData(__VA_ARGS__) )
#   define $alBufferf(...)                  MOON9_AL_0( alBufferf(__VA_ARGS__) )
#   define $alBufferfv(...)                 MOON9_AL_0( alBufferfv(__VA_ARGS__) )
#   define $alBufferi(...)                  MOON9_AL_0( alBufferi(__VA_ARGS__) )
#   define $alBufferiv(...)                 MOON9_AL_0( alBufferiv(__VA_ARGS__) )
#   define $alDeleteBuffers(...)            MOON9_AL_0( alDeleteBuffers(__VA_ARGS__) )
#   define $alDeleteSources(...)            MOON9_AL_0( alDeleteSources(__VA_ARGS__) )
#   define $alDisable(...)                  MOON9_AL_0( alDisable(__VA_ARGS__) )
#   define $alDistanceModel(...)            MOON9_AL_0( alDistanceModel(__VA_ARGS__) )
#   define $alDopplerFactor(...)            MOON9_AL_0( alDopplerFactor(__VA_ARGS__) )
#   define $alDopplerVelocity(...)          MOON9_AL_0( alDopplerVelocity(__VA_ARGS__) )
#   define $alEnable(...)                   MOON9_AL_0( alEnable(__VA_ARGS__) )
#   define $alGenBuffers(...)               MOON9_AL_0( alGenBuffers(__VA_ARGS__) )
#   define $alGenSources(...)               MOON9_AL_0( alGenSources(__VA_ARGS__) )
#   define $alGetBooleanv(...)              MOON9_AL_0( alGetBooleanv(__VA_ARGS__) )
#   define $alGetBuffer3f(...)              MOON9_AL_0( alGetBuffer3f(__VA_ARGS__) )
#   define $alGetBuffer3i(...)              MOON9_AL_0( alGetBuffer3i(__VA_ARGS__) )
#   define $alGetBufferf(...)               MOON9_AL_0( alGetBufferf(__VA_ARGS__) )
#   define $alGetBufferfv(...)              MOON9_AL_0( alGetBufferfv(__VA_ARGS__) )
#   define $alGetBufferi(...)               MOON9_AL_0( alGetBufferi(__VA_ARGS__) )
#   define $alGetBufferiv(...)              MOON9_AL_0( alGetBufferiv(__VA_ARGS__) )
#   define $alGetDoublev(...)               MOON9_AL_0( alGetDoublev(__VA_ARGS__) )
#   define $alGetFloatv(...)                MOON9_AL_0( alGetFloatv(__VA_ARGS__) )
#   define $alGetIntegerv(...)              MOON9_AL_0( alGetIntegerv(__VA_ARGS__) )
#   define $alGetListener3f(...)            MOON9_AL_0( alGetListener3f(__VA_ARGS__) )
#   define $alGetListener3i(...)            MOON9_AL_0( alGetListener3i(__VA_ARGS__) )
#   define $alGetListenerf(...)             MOON9_AL_0( alGetListenerf(__VA_ARGS__) )
#   define $alGetListenerfv(...)            MOON9_AL_0( alGetListenerfv(__VA_ARGS__) )
#   define $alGetListeneri(...)             MOON9_AL_0( alGetListeneri(__VA_ARGS__) )
#   define $alGetListeneriv(...)            MOON9_AL_0( alGetListeneriv(__VA_ARGS__) )
#   define $alGetSource3f(...)              MOON9_AL_0( alGetSource3f(__VA_ARGS__) )
#   define $alGetSource3i(...)              MOON9_AL_0( alGetSource3i(__VA_ARGS__) )
#   define $alGetSourcef(...)               MOON9_AL_0( alGetSourcef(__VA_ARGS__) )
#   define $alGetSourcefv(...)              MOON9_AL_0( alGetSourcefv(__VA_ARGS__) )
#   define $alGetSourcei(...)               MOON9_AL_0( alGetSourcei(__VA_ARGS__) )
#   define $alGetSourceiv(...)              MOON9_AL_0( alGetSourceiv(__VA_ARGS__) )
#   define $alListener3f(...)               MOON9_AL_0( alListener3f(__VA_ARGS__) )
#   define $alListener3i(...)               MOON9_AL_0( alListener3i(__VA_ARGS__) )
#   define $alListenerf(...)                MOON9_AL_0( alListenerf(__VA_ARGS__) )
#   define $alListenerfv(...)               MOON9_AL_0( alListenerfv(__VA_ARGS__) )
#   define $alListeneri(...)                MOON9_AL_0( alListeneri(__VA_ARGS__) )
#   define $alListeneriv(...)               MOON9_AL_0( alListeneriv(__VA_ARGS__) )
#   define $alSource3f(...)                 MOON9_AL_0( alSource3f(__VA_ARGS__) )
#   define $alSource3i(...)                 MOON9_AL_0( alSource3i(__VA_ARGS__) )
#   define $alSourcef(...)                  MOON9_AL_0( alSourcef(__VA_ARGS__) )
#   define $alSourcefv(...)                 MOON9_AL_0( alSourcefv(__VA_ARGS__) )
#   define $alSourcei(...)                  MOON9_AL_0( alSourcei(__VA_ARGS__) )
#   define $alSourceiv(...)                 MOON9_AL_0( alSourceiv(__VA_ARGS__) )
#   define $alSourcePause(...)              MOON9_AL_0( alSourcePause(__VA_ARGS__) )
#   define $alSourcePausev(...)             MOON9_AL_0( alSourcePausev(__VA_ARGS__) )
#   define $alSourcePlay(...)               MOON9_AL_0( alSourcePlay(__VA_ARGS__) )
#   define $alSourcePlayv(...)              MOON9_AL_0( alSourcePlayv(__VA_ARGS__) )
#   define $alSourceQueueBuffers(...)       MOON9_AL_0( alSourceQueueBuffers(__VA_ARGS__) )
#   define $alSourceRewind(...)             MOON9_AL_0( alSourceRewind(__VA_ARGS__) )
#   define $alSourceRewindv(...)            MOON9_AL_0( alSourceRewindv(__VA_ARGS__) )
#   define $alSourceStop(...)               MOON9_AL_0( alSourceStop(__VA_ARGS__) )
#   define $alSourceStopv(...)              MOON9_AL_0( alSourceStopv(__VA_ARGS__) )
#   define $alSourceUnqueueBuffers(...)     MOON9_AL_0( alSourceUnqueueBuffers(__VA_ARGS__) )
#   define $alSpeedOfSound(...)             MOON9_AL_0( alSpeedOfSound(__VA_ARGS__) )

//  dont check these (N)
#   define $alcCaptureCloseDevice(...)      ( alcCaptureCloseDevice(__VA_ARGS__) )
#   define $alcCloseDevice(...)             ( alcCloseDevice(__VA_ARGS__) )
#   define $alcIsExtensionPresent(...)      ( alcIsExtensionPresent(__VA_ARGS__) )
#   define $alcMakeContextCurrent(...)      ( alcMakeContextCurrent(__VA_ARGS__) )
#   define $alcCreateContext(...)           ( alcCreateContext(__VA_ARGS__) )
#   define $alcGetCurrentContext(...)       ( alcGetCurrentContext(__VA_ARGS__) )
#   define $alcCaptureOpenDevice(...)       ( alcCaptureOpenDevice(__VA_ARGS__) )
#   define $alcGetContextsDevice(...)       ( alcGetContextsDevice(__VA_ARGS__) )
#   define $alcOpenDevice(...)              ( alcOpenDevice(__VA_ARGS__) )
#   define $alcGetEnumValue(...)            ( alcGetEnumValue(__VA_ARGS__) )
#   define $alcGetError(...)                ( alcGetError(__VA_ARGS__) )
#   define $alcGetProcAddress(...)          ( alcGetProcAddress(__VA_ARGS__) )
#   define $alcGetString(...)               ( alcGetString(__VA_ARGS__) )

//  dont check these (0)
#   define $alcCaptureSamples(...)          ( alcCaptureSamples(__VA_ARGS__) )
#   define $alcCaptureStart(...)            ( alcCaptureStart(__VA_ARGS__) )
#   define $alcCaptureStop(...)             ( alcCaptureStop(__VA_ARGS__) )
#   define $alcDestroyContext(...)          ( alcDestroyContext(__VA_ARGS__) )
#   define $alcGetIntegerv(...)             ( alcGetIntegerv(__VA_ARGS__) )
#   define $alcProcessContext(...)          ( alcProcessContext(__VA_ARGS__) )
#   define $alcSuspendContext(...)          ( alcSuspendContext(__VA_ARGS__) )

}



std::vector<std::string> enumerate()
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
            vs.push_back( s );
            while( *s++ != '\0' );
        }
    }

    alcDeinit();

    return vs;
}

namespace
{
    int numcontexts = 0;
}

int context_t::playonce( const std::string &pathfile, void (*efxpre)(int), void (*efxpost)(int) )
{
    sound_t snd;

    if( !snd.load( pathfile ) )
        return -1;

    //std::cout << "seconds " << snd.seconds() << std::endl;

    insert_sound( snd );

    source_t src;

    src.create();
    src.bind( snd.buffer );

if( efxpre )
    (*efxpre)( src.source );

    src.play();

if( efxpost )
    (*efxpost)( src.source );

    // @todo : implement remove flags @sourcePlay()

    return insert_source( src );
}


bool context_t::init( int devnum )
{
    alcInit();

    // init
    dev = 0;
    ctx = 0;
    has_efx = 0;
    devname = std::string();

    // device name
    auto list = enumerate();
    if( devnum >= 0 && devnum < int(list.size()) )
        devname = list[ devnum ];

    // select device
    dev = $alcOpenDevice( devname.empty() ? 0 : devname.c_str() );

    //if( !dev ) return false;

    // device capabilities
    has_efx = ::has_efx( dev );
    if( has_efx )
        init_efx();

    ALint attribs[4] = { 0 };
    attribs[0] = ALC_MAX_AUXILIARY_SENDS;
    attribs[1] = 4;

    // select device
    ctx = $alcCreateContext(dev, has_efx ? attribs : NULL );

    if( !ctx )
        return false;

    enable();

    //$alDistanceModel( AL_INVERSE_DISTANCE );
    $alDistanceModel( AL_INVERSE_DISTANCE_CLAMPED );
    listener.gain( 1.f );

    reset();

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

    if( dev )
    {
//        $alcCloseDevice(dev);
        dev = 0;
    }

    alcDeinit();
}

void context_t::clear()
{
    int index = int(sounds.size());
    while( index-- )
    {
        erase_sound(index);
    }

    index = int(sources.size());
    while( index-- )
    {
        erase_source(index);
    }
}

void context_t::reset()
{
    clear();

    // reserve id (0) ; blank
    sound_t snd;
    snd.load( "ogg", blank_ogg::data(), blank_ogg::size() );

    insert_sound( snd );
}

int context_t::insert_sound( sound_t &sound )
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

void context_t::erase_sound( int _sound )
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

int context_t::insert_source( source_t &source )
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

void context_t::erase_source( int source )
{ sources[source].purge(); }


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
    // orientation { norm(at), norm(up) };
    float orientation6[] = { direction3[0], direction3[1], direction3[2], 0, -1, 0 };
    $alListenerfv( AL_ORIENTATION, orientation6 );
}

// --------------------------------------------------------------------------

/*

ALbyte buf[BUF_SIZE];

alcGetIntegerv(mydevice, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &samples);
alcCaptureSamples(mydevice, (ALCvoid *)buf, samples);

*/

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
            return "cant decode .ogg file", false;

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
    if( type == "wav" )
    {
        ALboolean al_bool;
        ALvoid *data;
        alutLoadWAVFile(pathfile, &this->format, &data, &this->size, &this->sampleRate, &al_bool);
        this->samples = (short *) data;
    }
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
#endif
    else
    {
        return "unsupported file format", false;
    }

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

double sound_t::seconds() const
{
    ALint size, bits, channels, freq;

    $alGetBufferi(buffer, AL_SIZE, &size);
    $alGetBufferi(buffer, AL_BITS, &bits);
    $alGetBufferi(buffer, AL_CHANNELS, &channels);
    $alGetBufferi(buffer, AL_FREQUENCY, &freq);

    if( alGetError() != AL_NO_ERROR )
        return 0.f;

    return (ALfloat)((ALuint)size/channels/(bits/8)) / (ALfloat)freq;
}

// ---------------------------------------------------------------------------

bool source_t::create()
{
    $alGenSources( 1, &source );
    return source > 0;
}

bool source_t::bind( int buffer )
{
    this->buffer = buffer;

    loop( false );
    $alSourcei( source, AL_SOURCE_RELATIVE, AL_FALSE );

    $alSourcei( source, AL_BUFFER, buffer );
    //$alSourceQueue

    $alSourcef( source, AL_MIN_GAIN, 0.0f );
    $alSourcef( source, AL_MAX_GAIN, 1.0f );

    gain( 1.f );
    pitch( 1.f );

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

void source_t::loop( const bool on )
{
    $alSourcei( source, AL_LOOPING, on ? AL_TRUE : AL_FALSE );
}

void source_t::gain( const float gain )
{
    $alSourcef( source, AL_GAIN, gain );
}

/* pitch, speed stretching */
void source_t::pitch( const float pitch )
{
    // if pitch == 0.f pitch = 0.0001f;
    $alSourcef( source, AL_PITCH, pitch );
}

/* tempo, time stretching */
/* @todo: check soundtouch library { pitch, tempo, bpm } */
/*

*/

void source_t::position( const float *position3, bool relative )
{
    $alSourcefv( source, AL_POSITION, position3 );
    $alSourcei( source, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE );
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
void source_t::distance( const float mind, const float maxd )
{
    $alSourcef( source, AL_REFERENCE_DISTANCE, mind );
    $alSourcef( source, AL_MAX_DISTANCE, maxd );
}
*/

// ----------------------------------------------------------------------------


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
    device::defaults()->erase_sound( source );
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
