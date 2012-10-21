#pragma once

#include <deque>
#include <string>

#include "dt.hpp"

namespace moon9
{
    class fps
    {
        public:

        fps();

        bool tick();
        void wait( double frames_per_second = 60.0 );

        std::deque< float > get_history() const;

        std::string str() const;
        size_t get() const;

        protected:

        moon9::dt dt, frame_limiter, frame_timer;
        size_t frames, frames_per_second;
        std::string format;

        std::deque< float > history;
    };
}
