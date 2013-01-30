// Simple OpenAL wrapper. Based on code by Emil Persson
// @todo: doppler: alDopplerFactor, alSpeedOfSound
// @todo: playonce( int src );
// - rlyeh

#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <string>
#include <vector>
#include <map>


struct sound_t
{
    ALenum format;
    ALuint buffer;

    short *samples;
    int sampleRate;
    int size;
//    bool ok;

    bool load( const std::string &pathfile );
    bool load( const std::string &type, const void *data, size_t size );
    void unload();
};

struct source_t
{
    ALuint source;
    ALuint buffer;

    bool create();
    bool bind( int buffer, bool looping = false, bool relative_pos = false );
    void unbind();
    void purge();

    void gain( const float gain );
    void position( const float *position3 );
    void velocity( const float *velocity3 );
    void direction( const float *direction3 );
    void attenuation( const float rollOff, const float refDistance );

    void play();
    void stop();
    void pause();
    bool is_playing() const;
};

struct listener_t
{
    void gain( const float gain );
    void position( const float *position3 );
    void velocity( const float *velocity3 );
    void direction( const float *direction3 );
};

struct channel_t
{
    std::vector<sound_t> sounds;
    std::vector<source_t> sources;

    void clear();
    void reset();

     int soundInsert( sound_t &sound );
    void soundDelete( int sound );

     int sourceInsert( source_t &source );
    void sourceDelete( int source );

    int playonce( const std::string &pathfile )
    {
        sound_t snd;

        if( !snd.load( pathfile ) )
            return -1;

        soundInsert( snd );

        source_t src;

        src.create();
        src.bind( snd.buffer );
        src.play();

        // @todo : implement remove flags @sourcePlay()

        return sourceInsert( src );
    }
};

struct context_t
{
    ALCcontext *ctx;
    int contextnum;

    listener_t ltn;
    std::map< int, channel_t > channels;

    bool init( ALCdevice * );
    void quit();

    void enable();
    void disable();

    void clear();
    void reset();
};

struct device_t
{
    ALCdevice *dev;
    int devnum;
//    bool ok;

    std::string name;
    std::map< int, context_t > contexts;

    bool init();
    void quit();

    void clear();
    void reset();
};

struct audio_t
{
    std::map< int, device_t > devices;

      audio_t();
     ~audio_t();

//     bool init( int devnum, int contextnum );
//     void quit( int devnum, int contextnum );

    void clear();
    void reset();
};


#if 0

// proposal

audio.devices[N].context[M].listener
                            channel[O].sounds[P]
                                      .sources[Q]

// proposal

struct listener
{
    int id;

    float gain;
    float position[3];
    float velocity[3];
    float orientation[3];
};

void init( listener &ltn );
void update( listener &ltn );

struct speaker
{
    int id;
    int source;

    float gain;
    float position[3];
    float velocity[3];
    float orientation[3];

    //cone

    float reference_distance;   // the distance under which the volume for the source would normally drop by half (before being influenced by rolloff factor or AL_MAX_DISTANCE)
    float rolloff;              // the rolloff rate for the source

    bool  playing;
    bool  paused;
    bool  looping;
    bool  relative_position;    // determines if the positions are relative to the listener
};

 int load( const std::string &resource );
void unload( int source );
void init( speaker &spk );
void submit( speaker &spk );

struct context
{
    context();

    int id;
    int device;

    listener ltn;
    std::vector< speaker > ui;
    std::vector< speaker > sfx;
    std::vector< speaker > bgm;
    std::vector< speaker > amb;
    std::vector< speaker > vox;
};

void update( context &ctx );

#endif
