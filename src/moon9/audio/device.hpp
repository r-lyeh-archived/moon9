// Simple OpenAL wrapper. Based on code by Emil Persson
// @todo: doppler: alDopplerFactor, alSpeedOfSound
// - rlyeh

#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <string>
#include <vector>

class device
{
    public:

     explicit
     device( int devnum ) : ctx(0),dev(0) { init(devnum); }

     device() : ctx(0),dev(0) {}
    ~device() { quit(); }

    std::string name( int devnum ) const;

    bool init( int devnum = 0 );
    void quit();
    void clear();
    void reset();

    void listenerSetPosition( const float *position3 );
    void listenerSetVelocity( const float *velocity3 );
    void listenerSetDirection( const float *direction3 );

     int soundAdd( const std::string &pathfile );
     int soundAdd( const std::string &type, const void *data, size_t size );
    void soundDelete( int sound );

     int sourceAdd( int sound, bool looping = false, bool relative_pos = false );
    void sourceDelete( int source );
    void sourceSetGain( int source, const float gain );
    void sourceSetPosition( int source, const float *position3 );
    void sourceSetVelocity( int source, const float *velocity3 );
    void sourceSetDirection( int source, const float *direction3 );
    void sourceSetAttenuation( int source, const float rollOff, const float refDistance );
    void sourcePlay( int source );
    void sourceStop( int source );
    void sourcePause( int source );
    bool sourceIsPlaying( int source );

    static device *defaults() { static device d(0); return &d; }

    protected:

    ALCcontext *ctx;
    ALCdevice *dev;

    struct sound_t
    {
        ALenum format;
        ALuint buffer;

        short *samples;
        int sampleRate;
        int size;
    };

    struct source_t
    {
        ALuint source;
        int sound;
    };

    int soundInsert(sound_t &sound);
    int sourceInsert(source_t &source);

    std::vector<sound_t> sounds;
    std::vector<source_t> sources;
};

#if 0

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
