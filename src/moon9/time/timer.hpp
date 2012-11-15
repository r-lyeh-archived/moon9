/*
 * Simple classes for time measuring

 * Copyright (c) 2010 Mario 'rlyeh' Rodriguez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.

 * issues:
 * - if year < 1900, moon9::rtc() behavior is undefined

 * to do:
 * - chrono/rtc -> serialize factor as well (and held?)
 * - move factor/shift and pause/resume to dt
 * - kiloseconds, ks | ref: http://bavardage.github.com/Kiloseconds/

 * - template <const float resolution>
 * - improve serialization (missing some info right now; check out all vars)
 * - add accelerate() (till factor to 0, or 0 to factor)
 * - add pauseAll()/resumeAll() (layered, static)
 * - add hold(seconds)
 * - add operator=()
 * - rtc(__DATE__) // rtc(__TIME__) -> get compiled date / time

 * - rlyeh ~~ listening to The Mission / Butterfly on a wheel
 */

#pragma once

#include <cassert>

#include <sstream>
#include <string>

//#include <moon9/os/directives.hpp>
//#include <moon9/os/os.hpp>
//#include <moon9/string/format.hpp>
//#include <moon9/string/string.hpp>

#include "dt.hpp"

namespace moon9
{
    class timer
    {
        public:

            timer();
            void reset();

            void set( const double &t );
            void shift( double f ); // factor(), phase(), speed() instead?
            void pause();
            double resume();
            double update();

            double get() const;
            operator double() const;

            std::string str() const;
            void str( const std::string& import );

        protected:

            moon9::dt dt;
            bool held;
            double factor, creation;

            // object time (in seconds.microseconds)
            double time_obj();
    };
}
