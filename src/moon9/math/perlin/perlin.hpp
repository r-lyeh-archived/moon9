/*

This source provides a C++ wrapper around Ken Perlin's noise function.You
construct an instance of Perlin as follows: Perlin *perlin = new
Perlin(4,4,1,94);  The first parameter is the number of octaves, this is how
noisy or smooth the function is.  This is valid between 1 and 16. A value of 4
to 8 octaves produces fairly conventional noise results.  The second parameter
is the noise frequency. Values betwen 1 and 8 are reasonable here.  You can try
sampling the data and plotting it to the screen to see what numbers you like.
The last parameter is the amplitude.  Setting this to a value of 1 will return
randomized samples between -1 and +1. The last parameter is the random number
seed.  This number is what causes this instance of the Perlin to be
deterministic and distanct from any other instance.  The perlin noise function
creates some random number tables that are sampled during queries.  This random
number seed determines the contents of those tables so that you will get the
same exact results every time you use it.  To retrieve a sample you simply call
the method get() and pass it the X and Y sample point to query.  X and Y should
be in the ranges of 0 to 1. Each instance of Perlin contains it's own random 
number tables and sampling values. John Ratcliff

*/

#pragma once

class Perlin
{
    static const size_t SAMPLE_SIZE = 1024;

    public:

        Perlin(int octaves /*1..16*/,float freq /*1..8*/,float amp,int seed);

        float get(float x /*0..1*/,float y /*0..1*/);
        float get(float x /*0..1*/,float y /*0..1*/, float z /*0..1*/);

    private:

        void init_perlin(int n,float p);
        float perlin_noise_2D(float vec[2]);
        float perlin_noise_3D(float vec[2]);

        float noise1(float arg);
        float noise2(float vec[2]);
        float noise3(float vec[3]);
        void normalize2(float v[2]);
        void normalize3(float v[3]);
        void init(void);

        int   mOctaves;
        float mFrequency;
        float mAmplitude;
        int   mSeed;

        int p[SAMPLE_SIZE + SAMPLE_SIZE + 2];
        float g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];
        float g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
        float g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];
        bool  mStart;

};
