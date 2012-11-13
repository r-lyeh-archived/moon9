// EASE/IN/OUT/BUMP/TWEEN
// based on robert penner's work (BSD)
// based on tomas cepeda's work
// todo: tween: sequence (using containers?), yoyo(), rewind()

// complete with realtime cameras book info
// complete with http://code.google.com/p/cpptweener/
// http://www.pixelwit.com/blog/2007/06/actionscript-easing-functions/#more-81
// http://jet.ro/2009/10/11/steps-and-pulses/

// note: current can be bigger than total (so does looping if set)
// to do: <T>

#pragma once

#include <cmath>
#include <algorithm>

#include <moon9/time/timer.hpp>

namespace moon9
{
    class tweener
    {
        moon9::timer ch;

        public:

        enum type
        {
            LINEAR_01,

            QUADIN_01,          // t^2
            QUADOUT_01,
            QUADINOUT_01,
            CUBICIN_01,         // t^3
            CUBICOUT_01,
            CUBICINOUT_01,
            QUARTIN_01,         // t^4
            QUARTOUT_01,
            QUARTINOUT_01,
            QUINTIN_01,         // t^5
            QUINTOUT_01,
            QUINTINOUT_01,
            SINEIN_01,          // sin(t)
            SINEOUT_01,
            SINEINOUT_01,
            EXPOIN_01,          // 2^t
            EXPOOUT_01,
            EXPOINOUT_01,
            CIRCIN_01,          // sqrt(1-t^2)
            CIRCOUT_01,
            CIRCINOUT_01,
            ELASTICIN_01,       // exponentially decaying sine wave
            ELASTICOUT_01,
            ELASTICINOUT_01,
            BACKIN_01,          // overshooting cubic easing: (s+1)*t^3 - s*t^2
            BACKOUT_01,
            BACKINOUT_01,
            BOUNCEIN_01,        // exponentially decaying parabolic bounce
            BOUNCEOUT_01,
            BOUNCEINOUT_01,

            SINESQUARE,         // http://www.gapjumper.com/
            EXPONENTIAL,        // http://www.gapjumper.com/
            SCHUBRING1,         // terry schubring's formula 1
            SCHUBRING2,         // terry schubring's formula 2
            SCHUBRING3,         // terry schubring's formula 3

            SINPI2_01,
            ACELBREAK_01,
            COS2PI_11,

            SIN2PI_00,
            SINPI_00,
            SINPI2PI_10,
            SIN4PI_00,
            SIN3PI4_00
        };

        tweener()
        {}

        float get( type tweener_type, float interval, bool repeat )
        {
            float dt = tween( tweener_type, float( ch.update() ), interval, repeat );

            return dt;
        }

        double get_elapsed()
        {
            return ch.get();
        }


    protected:

        inline float tween( type tweener_type, float current, float total, bool looped = false )
        {
            const float pi = 3.1415926535897932384626433832795f;

            float t = current, d = total;

            /* small optimization { */

            //if( d <= 0.0f )
            //    return 1.0f;

            if( d <= 0.f || t <= 0.f )
            {
                return( tweener_type == COS2PI_11 || tweener_type == SINPI2PI_10 ? 1.f : 0.f );
            }

            if( t >= d )
            {
                if( looped )
                {
                    // todo: optimize me
                    while( t >= d )
                        t -= d;
                }
                else
                {
                    if( tweener_type >= SIN2PI_00 )
                        return 0.f;

                    return 1.f;
                }
            }

            /* } */

            switch( tweener_type )
            {
            case LINEAR_01:
                {
                    return t/d;
                }

            case SIN2PI_00:
                {
                    float fDelta = t/d;
                    return std::sin(fDelta * 2.0f * pi);
                }

            case SINPI_00:
                {
                    float fDelta = t/d;
                    return std::sin(fDelta * pi);
                }

            case SINPI2PI_10:
                {
                    float fDelta = t/d;
                    return std::sin((0.5f * pi) * (fDelta + 1));
                }

            case SIN4PI_00:
                {
                    float fDelta = t/d;
                    return std::sin(fDelta * 4.0f * pi);
                }

            case SIN3PI4_00:
                {
                    float fDelta = t/d;
                    return std::sin(fDelta * 3.0f * pi);
                }

            case SINPI2_01:
                {
                    float fDelta = t/d;
                    return std::sin(fDelta * 0.5f * pi);
                }

            case ACELBREAK_01:
                {
                    float fDelta = t/d;
                    return (std::sin((fDelta * pi) - (pi * 0.5f)) + 1.0f) * 0.5f;
                }

            case COS2PI_11:
                {
                    float fDelta = t/d;
                    return std::cos(fDelta * 2.0f * pi);
                }

            case BACKIN_01:
                {
                    float s = 1.70158f;
                    float postFix = t/=d;
                    return postFix * t * ((s + 1) * t - s);
                }

            case BACKOUT_01:
                {
                    float s = 1.70158f;
                    return 1.f * ((t = t/d-1)*t*((s+1)*t + s) + 1);
                }

            case BACKINOUT_01:
                {
                    float s = 1.70158f;
                    if ((t/=d/2) < 1)
                        return 1.f/2*(t*t*(((s*=(1.525f))+1)*t - s));

                    float postFix = t-=2;
                    return 1.f/2*((postFix)*t*(((s*=(1.525f))+1)*t + s) + 2);
                }

    #       define BOUNCE(v) \
                if ((t/=d) < (1/2.75f)) \
                    { \
                    v = 1.f*(7.5625f*t*t); \
                    } \
                else if (t < (2/2.75f)) \
                    { \
                    float postFix = t-=(1.5f/2.75f); \
                    \
                    v = 1.f*(7.5625f*(postFix)*t + .75f); \
                    } \
                else if (t < (2.5/2.75)) \
                    { \
                    float postFix = t-=(2.25f/2.75f); \
                    \
                    v = 1.f*(7.5625f*(postFix)*t + .9375f); \
                    } \
                else \
                    { \
                    float postFix = t-=(2.625f/2.75f); \
                    \
                    v = 1.f*(7.5625f*(postFix)*t + .984375f); \
                    }

            case BOUNCEIN_01:
                {
                    float v;
                    t = d-t;
                    BOUNCE(v);
                    return 1.f - v;
                }

            case BOUNCEOUT_01:
                {
                    float v;
                    BOUNCE(v);
                    return v;
                }

            case BOUNCEINOUT_01:
                {
                    float v;
                    if (t < d/2) {
                        t = t*2;
                        t = d-t;
                        BOUNCE(v);
                        return (1.f - v) * .5f;
                    } else {
                        t = t*2 -d;
                        BOUNCE(v);
                        return v * .5f + 1.f*.5f;
                    }
                }

    #       undef BOUNCE

            case CIRCIN_01:
                t /= d;
                return 1.f - std::sqrt(1 - t*t);

            case CIRCOUT_01:
                t /= d;
                t--;
                return std::sqrt(1 - t*t);

            case CIRCINOUT_01:
                t /= d/2;
                if(t < 1)
                    return -1.f/2 * (std::sqrt(1 - t*t) - 1);

                t-=2;
                return 1.f/2 * (std::sqrt(1 - t*t) + 1);


            case ELASTICIN_01:
                {
                    t/=d;

                    float p=d*.3f;
                    float a=1.f;
                    float s=p/4;
                    float postFix =a*std::pow(2,10*(t-=1)); // this is a fix, again, with post-increment operators

                    return -(postFix * std::sin((t*d-s)*(2*pi)/p ));
                }

            case ELASTICOUT_01:
                {
                    float p=d*.3f;
                    float a=1.f;
                    float s=p/4;

                    return (a*std::pow(2,-10*t) * std::sin( (t*d-s)*(2*pi)/p ) + 1.f);
                }

            case ELASTICINOUT_01:
                {
                    t/=d/2;

                    float p=d*(.3f*1.5f);
                    float a=1.f;
                    float s=p/4;

                    if (t < 1) {
                        float postFix =a*std::pow(2,10*(t-=1)); // postIncrement is evil
                        return -.5f*(postFix* std::sin( (t*d-s)*(2*pi)/p ));
                    }

                    float postFix =  a*std::pow(2,-10*(t-=1)); // postIncrement is evil
                    return postFix * std::sin( (t*d-s)*(2*pi)/p )*.5f + 1.f;
                }

            case EXPOIN_01:
                return std::pow(2, 10 * (t/d - 1));

            case EXPOOUT_01:
                return 1.f - ( t == d ? 0 : std::pow(2, -10.f * (t/d)));

            case EXPOINOUT_01:
                t /= d/2;
                if (t < 1)
                    return 1.f/2 * std::pow(2, 10 * (t - 1));

                t--;
                return 1.f/2 * (-std::pow(2, -10 * t) + 2);

            case QUADIN_01:
                t /= d;
                return t*t;

            case QUADOUT_01:
                t /= d;
                return (2.f - t) * t;

            case QUADINOUT_01:
                t /= d/2;
                if(t < 1)
                    return (1.f/2*t*t);

                t--;
                return -1.f/2 * (t*(t-2) - 1);

            case CUBICIN_01:
                t /= d;
                return t*t*t;

            case CUBICOUT_01:
                t /= d;
                t--;
                return (1.f + t*t*t);

            case CUBICINOUT_01:
                t /= d/2;
                if (t < 1)
                    return 1.f/2*t*t*t;

                t -= 2;
                return 1.f/2*(t*t*t + 2);

            case QUARTIN_01:
                t /= d;
                return t*t*t*t;

            case QUARTOUT_01:
                t /= d;
                t--;
                return (1.f - t*t*t*t);

            case QUARTINOUT_01:
                t /= d/2;
                if(t < 1)
                    return 1.f/2*t*t*t*t;

                t -= 2;
                return -1.f/2 * (t*t*t*t - 2);

            case QUINTIN_01:
                t /= d;
                return t*t*t*t*t;

            case QUINTOUT_01:
                t /= d;
                t--;
                return (1.f + t*t*t*t*t);

            case QUINTINOUT_01:
                t /= d/2;
                if(t < 1)
                    return 1.f/2*t*t*t*t*t;

                t -= 2;
                return 1.f/2*(t*t*t*t*t + 2);

            case SINEIN_01:
                return 1.f - std::cos(t/d * (pi/2));

            case SINEOUT_01:
                return std::sin(t/d * (pi/2));

            case SINEINOUT_01:
                return -1.f/2 * (std::cos(pi*t/d) - 1);


            case SINESQUARE:
                {
                    float A = std::sin(0.5f*(t/d)*pi);
                    return A*A;
                }

            case EXPONENTIAL:
                {
                    return 1/(1+std::exp(6-12*(t/d)));
                }

            case SCHUBRING1:
                {
                    t /= d;
                    return 2*(t+(0.5f-t)*std::abs(0.5f-t))-0.5f;
                }

            case SCHUBRING2:
                {
                    t /= d;
                    float p1pass= 2*(t+(0.5f-t)*std::abs(0.5f-t))-0.5f;
                    float p2pass= 2*(p1pass+(0.5f-p1pass)*std::abs(0.5f-p1pass))-0.5f;
                    return p2pass;
                }

            case SCHUBRING3:
                {
                    t /= d;
                    float p1pass= 2*(t+(0.5f-t)*std::abs(0.5f-t))-0.5f;
                    float p2pass= 2*(p1pass+(0.5f-p1pass)*std::abs(0.5f-p1pass))-0.5f;
                    float pAvg=(p1pass+p2pass)/2;
                    return pAvg;
                }

            default:
                return 0;
            }
        }
    };
}

