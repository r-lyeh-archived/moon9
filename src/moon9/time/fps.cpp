#include <sstream>
#include <string>

#include "fps.hpp"

#include "dt.hpp"
#include "wink.hpp"

namespace moon9
{
    namespace
    {
        template <typename T>
        std::string to_string( const T &t )
        {
            std::stringstream ss;
            ss << t;
            return ss.str();
        }
    }

    fps::fps() : frames(0), frames_per_second(0), format("0 fps")
    {}

    bool fps::tick()
    {
        frames++;

        history.push_back( frame_timer.s() );
        if( history.size() > 60*2 ) history.pop_front();
        frame_timer.reset();

		double sec = dt.s();
        if( sec >= 0.5 )
        {
            frames_per_second = frames,
            frames = 0;

            format = ( frames_per_second >= 1 || frames_per_second == 0 ?
                to_string( int( frames_per_second / sec ) ) + " fps" :
                to_string( int( sec / frames_per_second ) ) + " spf" );

			frames_per_second /= sec;

            dt.reset();

            return true;
        }

        return false;
    }

    void fps::wait( double frames_per_second )
    {
        // @todo: [evaluate] http://gafferongames.com/game-physics/fix-your-timestep/

        if( frames_per_second > 0 )
        {
            double seconds = 1.0/frames_per_second;
            if( seconds > 1 ) seconds = 1;
            do moon9::wink(); while( frame_limiter.s() < seconds ); //yield()?
            frame_limiter.reset();
        }
    }

    std::deque< float > fps::get_history() const
    {
        return history;
    }

    std::string fps::str() const
    {
        return format;
    }

    size_t fps::get() const
    {
        return frames_per_second;
    }
}
