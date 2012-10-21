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
#include <moon9/math/tween.hpp>

namespace moon9
{
    class tweener
    {
        moon9::timer ch;

        public:

        tweener()
        {}

        float get( tween_enum type, float interval, bool repeat )
        {
            float dt = moon9::tween( type, float( ch.update() ), interval, repeat );

            return dt;
        }

        double get_elapsed()
        {
            return ch.get();
        }
    };    
}

