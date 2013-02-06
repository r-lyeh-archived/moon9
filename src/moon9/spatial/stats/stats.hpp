/*
 * Simple statistics class, including ascii art reports.

 * Copyright (c) 2011 Mario 'rlyeh' Rodriguez
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

 * - rlyeh ~~ listening to My Morning Jacket / One Big Holiday
 */


/*
    stats: almaceno las estadisticas para ese intervalo de tiempo, y la diferencia con el anterior (dt)
    en otra tabla. luego puedo buscar por indice (t=idx), o la evolucion partir de cierta fecha (dt=idx)

    game1 -> final_game_stats -> store into db_stats_table
    game2 -> final_game_stats -> store into db_stats_table && store delta12 into db_dt_stats_table
    game3 -> final_game_stats -> store into db_stats_table && store delta23 into db_dt_stats_table
    ...
    N (36) -> ...

*/

#pragma once

#include <deque>
#include <sstream>
#include <limits>
#include <chrono>

#include "spline/spline.hpp"

namespace moon9
{
    template< typename T >
    class stats
    {
        class dt
        {
            typedef std::chrono::high_resolution_clock clock;
            clock::time_point start;

            public:

            dt()
            {
                start = clock::now();
            }

            void reset()
            {
                *this = dt();
            }

            double s()
            {
                return std::chrono::nanoseconds( clock::now() - start ).count() / 1000000000.0; //::seconds
            }

            double ms()
            {
                return std::chrono::nanoseconds( clock::now() - start ).count() / 1000000.0; //::milliseconds
            }

            double us()
            {
                return std::chrono::nanoseconds( clock::now() - start ).count() / 1000.0; //::microseconds
            }

            double ns()
            {
                return std::chrono::nanoseconds( clock::now() - start ).count() / 1.0;
            }
        };

        std::string metrics( const double &seconds ) const
        {
            std::stringstream ss;

            if( seconds > 3600 )
                ss << ( seconds / 3600 ) << " h";
            else if( seconds > 60 )
                ss << ( seconds / 60 ) << " m";
            else if( seconds < 1/1000.0 )
                ss << ( seconds * 1000 ) << " ms";
            else if( seconds < 1/1000000.0 )
                ss << ( seconds * 1000000 ) << " ns";
            else if( seconds < 1/1000000000.0 )
                ss << ( seconds * 1000000000 ) << " us";

            return ss.str();
        }

        struct pair
        {
            double time;
            T value;

            pair()
            {}

            pair( double _time, T _value ) : time(_time), value(_value)
            {}

            pair( const pair &p )
            {
                operator=( p );
            }

            pair &operator=( const pair &p )
            {
                if( this != &p )
                {
                    time = p.time;
                    value = p.value;
                }

                return *this;
            }

            pair operator+( const pair &p ) const
            {
                return pair( time + p.time, value + p.value);
            }

            pair operator-( const pair &p ) const
            {
                return pair( time - p.time, value - p.value);
            }

            pair operator*( const pair &p ) const
            {
                return pair( time * p.time, value * p.value);
            }

            template <typename F>
            pair operator*( const F &f ) const
            {
                return pair( time * f, value * f);
            }

            template <typename F>
            pair operator+( const F &f ) const
            {
                return pair( time + f, value + f);
            }
        };

        moon9::spline< pair, std::deque > history;

        T min, max, avg, cur;
        size_t hits;

        size_t max_history_size, max_history_samples, max_history_duration;

        dt timer;
        double then, now;

    public:

        stats() :
            min(std::numeric_limits<T>::max()),
            max(std::numeric_limits<T>::min()),
            avg(0), cur(T()), hits(0),
            then(0), now(0)
        {
            // defaults to : 256 kbytes max history size, unlimited history samples, unlimited history duration
            set_history_limits( 256, 0, 0 );
        }

#if 0 // useful?
        stats( const stats &t )
        {
            operator=( t );
        }

        stats &operator=( const stats &t )
        {
            if( &t != this )
            {
                history = t.history;
                min = t.min;
                max = t.max;
                avg = t.avg;
                cur = t.cur;
                hits = t.hits;
                max_history_size = t.max_history_size;
                max_history_samples = t.max_history_samples;
                max_history_duration = t.max_history_duration;
                timer = t.timer;
                then = t.then;
                now = t.now;
            }

            return *this;
        }
#endif

        // You can limit history size by specifing none, one, two or three constraints.
        // - max_history_size : max history size in kilobytes (0 will record for unlimited size)
        // - max_history_samples : number of samples you want to keep in history (ie, 250 samples, or 0 for unlimited samples)
        // - max_history_duration : max history duration in minutes (0 will record for unlimited time)

        void set_history_limits( size_t max_history_size = 0, size_t max_history_samples = 0, size_t max_history_duration = 0 )
        {
            this->max_history_size     = max_history_size;
            this->max_history_samples  = max_history_samples;
            this->max_history_duration = max_history_duration;
        }

        private:

        void update( float _now )
        {
            T t = cur;

            now = _now;

            if( !history.size() )
                then = now;

            if( t < min )
                min = t;

            if( t > max )
                max = t;

            avg += t;

            hits++;

            // insert record at tail

            history.push_back( pair( now, t ) );

            // truncate one record from head when any limit is met ( by number of history samples, history size (kb), history duration (min) )

            bool truncate = false;

            if( max_history_samples )
                if( history.size() > max_history_samples )
                    truncate = true;

            if( max_history_size && !truncate )
                if( history.size() * sizeof( pair ) > max_history_size * 1024 )
                    truncate = true;

            if( max_history_duration && !truncate )
                if( ( now - then ) > max_history_duration * 60 )
                    truncate = true;

            if( truncate )
                history.pop_front();
        }

        void update()
        {
            update( timer.s() );
        }

        public:

        void push_back( const T& t )
        {
            cur = t;

            update();
        }

        void push_back( const T& t, double _now )
        {
            cur = t;

            update( _now );
        }

        void pop_front()
        {
            history.pop_front();
        }

        void pop_back()
        {
            history.pop_back();

            cur = back();
        }

        T get_min() const   // base, valley
        {
            return min;
        }

        T get_max() const   // peak
        {
            return max;
        }

        T get_avg() const
        {
            return avg / ( hits ? hits : 1 );
        }

        T get_hits() const
        {
            return hits;
        }

        T get_relative_lerp( double dt01 ) const // get interpolated value at given percent/delta [0..1]
        {
            if( !history.size() )
                return T();

            if( history.size() < 4 )
                return history.begin()->value;

            T value = history.position_w_edges( dt01<0 ? 0 : (dt01>1 ? 1 : dt01) ).value;

            // clamp ('cos splines do not need to constrain to our boundaries)

            return value < min ? min : ( value > max ? max : value );
        }

        T get_absolute_lerp( double t ) const   // get interpolated value at any given time
        {
            return get_relative_lerp( t / now );
        }

#if 0
        double get_elapsed() //const
        {
            double t = timer.s();
            return t ? t : 1;
        }
#else
        double get_elapsed() const
        {
            if( history.size() < 2 )
                return 0;

            return history.rbegin()->time - history.begin()->time; // return abs()
        }
#endif

        size_t size() const
        {
            return history.size();
        }

        //   0    |    .
        // #   1  |  .   .
        //      2 | .     .
        //       .|.
        // -------+--------
        //        |


        T front( size_t i ) const
        {
            return history.size() ? history.at( i ).value : cur;
        }

		T front() const
		{
			return front( 0 );
		}

        //   .    |    1
        // .   .  |  2   0
        //      . | .     #
        //       .|.
        // -------+--------
        //        |

        T back( size_t i ) const
        {
            return history.size() ? history.at( history.size() - 1 - i ).value : cur;
        }

        T back() const
        {
            return back( 0 );
        }

        std::string report( const char *title = 0, int W = 80, int H = 20, bool use_blur = false ) const
        {
            //assert( W > 1 && H > 1 );

            // init map

            char blank = char( use_blur ? 1 : ' ' ), filler = char( use_blur ? 127 : '\xFE' );

            std::vector<std::string> l( H, std::string( W, blank ) );

            //for( int y = 0; y < H; ++y )
            //   l[y] = std::string( W, blank );

            // draw map (downscaled)

            for( int x = 0; x < W; ++x )
            {
                // get Y
                double y = double( get_relative_lerp( x / double(W - 1) ) );

                // convert y to delta
                if( get_max() - get_min() > 0 )
                y = ( y - get_min() ) / ( get_max() - get_min() );
                else
                y = 0;

                // rescale
                y *= (H - 1);

                l[ int(y) ][ x ] = filler;
            }

            // blur map

            if( use_blur )
            {
                std::vector<std::string> l_blur( H );

                l_blur = l;

                for( int y = 0; y < H; ++y )
                for( int x = 0; x < W; ++x )
                {
                    double value = 0;
                    int count = 0;

                    #define blur( IX, IY ) if( x + (IX) >= 0 && x + (IX) < W && y + (IY) >= 0 && y + (IY) < H ) value += l[ y + (IY) ][ x + (IX) ], ++count;

                    blur( -1, -1 );
                    blur( -1,  0 );
                    blur( -1, +1 );

                    blur(  0, -1 );
                    //blur(  0,  0 );
                    blur(  0, +1 );

                    blur( +1, -1 );
                    blur( +1,  0 );
                    blur( +1, +1 );

                    value = ( value / count ) / ( 127.0 ); /* delta 0..1 */

                    #undef blur

#if 1
                    // map blur into ascii art
                    std::string brightness = " \xFA,+*$##";
                    const double gamma = 2.25;
#else
                    std::string brightness = " \xFA";
                    const double gamma = 2.00;
#endif
                    double bright = pow( value, 1.0/gamma );

                    bright = ( bright < 0 ? 0 : ( bright > 1 ? 1 : bright ));

                    l_blur[ y ][ x ] = brightness.at( bright * brightness.size() );
                }

                l = l_blur;
            }

            // insert guides

            for( int x = 0; x < W; ++x )
            {
                if( l[0][x] == ' ' ) //blank )
                    l[0][x] = '-';

                if( l[H - 1][x] == ' ' ) //blank )
                    l[H - 1][x] = '-';
            }

            // print map (upside down)

            std::stringstream header, body, footer;

            if( title )
            {
                header << title << std::endl;
            }

            for( int y = H - 1; y >= 0; --y )
            {
                body << l[y] << " " << ( y == 0 /*|| y == H - 1*/ ? "(v) " : "    " ) << int( get_min() + ( y / double(H - 1) ) * ( get_max() - get_min() )) << std::endl;
            }

            {
                char fmt[512];
                sprintf(fmt, "%s%*s (t)", metrics(then-then).c_str(), W - metrics(then-then).size(), metrics(now-then).c_str() );
                footer << fmt << std::endl;
            }

            float lapse = get_elapsed();

            footer << "Min: " << get_min()  << "; "
                   << "Max: " << get_max()  << "; "
                   << "Avg: " << get_avg()  << "; "
                   << "Hit: " << get_hits() << "; "
                   << "Lapse: " << lapse << "s"  << "; "
                   << "Avg Hits/sec: " << ( lapse > 0 ? get_hits() / lapse : 0 ) << "; "
                   << "Cur: " << back() << std::endl;

            return header.str() + body.str() + footer.str();
        }

    };
}

