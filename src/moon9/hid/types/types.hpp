#pragma once

#include <cassert>

#include <algorithm>
#include <chrono>
#include <deque>
#include <vector>

// @todo: proposal:
// at(   0) -> current value
// at(-0.5) -> lerp value 0.5 seconds ago (stats)
// at(+0.5) -> prediction value in 0.5 seconds

// @todo: N should be seconds time rather than polling samples

namespace moon9
{
    namespace hid
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
    }

    extern moon9::hid::dt global_timer;

    template< typename SAMPLE_TYPE, const int N = 120 >
    class history : public SAMPLE_TYPE
    {
        // N samples = fixed deque of N samples
        //
        // (t0,sample0) (t1,sample1) ... (tN-1,sampleN-1)
        // newest...oldest
        //
        // now = find_dt( dt = 0 ) -> iter #0
        // Nth = find_dt( dt = 1 ) -> iter #N-1
        //
        // now = find_t( t = 0 secs ago ) -> iter #0
        // then = find_t( t = T secs ago ) -> iter #N-1
        //
        // duration = t0 - tN
        //

        // todo: catmull-rom/math9::stats lerp for previous positions (useful?)
        // todo: burg to predict next positions (useful?)

        std::deque< SAMPLE_TYPE > container;

        public:

        history() : container( N )
        {
			clear();
            std::reverse( container.begin(), container.end() );
        }

        size_t size() const
        {
            return container.size();
        }

		void clear()
		{
            double s = global_timer.s();

            for( size_t i = 0; i < N; ++i )
                container.at(i).t = s;
		}

        typename std::deque< SAMPLE_TYPE >::const_iterator newest_it() const   // ~recent(), ~current()
        {
            assert( container.size() );
            return container.begin();
        }
/* TO_DEPRECATE
        typename std::deque< SAMPLE_TYPE >::const_iterator oldest_it() const
        {
            assert( container.size() );
            return container.end() - 1;
        }
*/
        const SAMPLE_TYPE &newest() const
        {
            return *newest_it();
        }
/* TO_DEPRECATE
        const SAMPLE_TYPE &oldest() const
        {
            return *oldest_it();
        }
*/
        const double duration() const
        {
            return container.at(1).t - container.back().t;
        }

        typename std::deque< SAMPLE_TYPE >::const_iterator find_dt( const double &dt01 ) const
        {
            // 0 = newest ... 1 = oldest

            assert( container.size() );

            size_t dtpos = size_t( dt01 * container.size() );

            if( dtpos >= container.size() )
                dtpos = container.size() - 1;

            return container.begin() + dtpos;
        }
        typename std::deque< SAMPLE_TYPE >::const_iterator find_t( const double &seconds_ago ) const
        {
            // if seconds_ago_lapse <= 0 then return newest() iterator
            // if seconds_ago_lapse = ~0 then return iterator(s) close to newest()
            // ...
            // if seconds_ago_lapse = ~N then return iterator(s) close to oldest()
            // if seconds_ago_lapse >= N then return oldest() iterator

            assert( container.size() );

            if( seconds_ago <= 0 )
                return begin();

            if( seconds_ago >= duration() )
                return end() - 1;

            double current_time = begin()->t;
            double time_target = current_time - seconds_ago;

            const_iterator it;

            for( it = container.begin(); it != container.end(); ++it )
                if( time_target >= it->t )
                    return it;

            return end() - 1;
        }

        const SAMPLE_TYPE &then_dt( const double &dt ) const
        {
            return *find_dt( dt );
        }
        const SAMPLE_TYPE &then_t( const double &seconds_ago_lapse ) const
        {
            return *find_t( seconds_ago_lapse );
        }

        history interval_dt( const double &from_01, const double &to_01 ) const
        {
            const_iterator from = find_dt( from_01 );
            const_iterator to = find_dt( to_01 );

            assert( (to - from) && "invalid interval" );

            size_t N = to - from;

            history h( N );

            std::copy( from, to, h.begin() );

            return h;
        }
        history interval_t( const double &from_t, const double &to_t ) const
        {
            const_iterator from = find_t( from_t );
            const_iterator to = find_t( to_t );

            assert( (to - from) && "invalid interval" );

            size_t N = to - from;

            history h( N );

            std::copy( from, to, h.begin() );

            return h;
        }

        private:

        void update_timestamp( int pos )
        {
            container.at( pos ).t = global_timer.s();
        }

        //public:

        // set: function that push_front's element when relevant

        void set( const SAMPLE_TYPE &new_sample )
        {
            if( new_sample == container.front() )   // update timestamp if value same than previous (~rle), insert if new value is relevant ~treshold
            {
                update_timestamp(0); //y del resto... //useful?
            }
            else
            {
                // @todo: hago el resto... useful?
                //update_timestamp(N-1);
                //...
                //update_timestamp(1);
                update_timestamp(0);

                #if 1
                // @todo: keep treshold and other members from current deque
                container.push_front( container.front() );
                container.front().set( new_sample );
                #else
                container.push_front( new_sample );
                #endif
                container.front().t = global_timer.s();

                if( container.size() > N )
                    container.pop_back();
            }

            import( newest() );
        }

        public:

        // sugars{

        template <typename T>
        void set( const T &t0 )
        {
            set( SAMPLE_TYPE(t0) );
        }

        template <typename T>
        void set( const T &t0, const T &t1 )
        {
            set( SAMPLE_TYPE(t0,t1) );
        }

        template <typename T>
        void set( const T &t0, const T &t1, const T &t2 )
        {
            set( SAMPLE_TYPE(t0,t1,t2) );
        }



        //}

        // pattern matching {

            // @todo
            // los patterns estaria mejor que dieran un gestalt o un valor aproximado segun encuentren
            // un patron o no. tiene mas sentido; mejor que devolver un booleano. ademas enlazo ya
            // con los gestures : )

#if 1
            // idle: low [then] -> low [now]
            bool idle( float interval_t = 0.0125f ) //interval useful here?
            {
                double then_t = container.at(1).t;
                double  now_t = container.at(0).t;

                if( now_t - then_t > interval_t )
                    return false; //time exceeded

                double then = container.at(1).x;
                double  now = container.at(0).x;

                return then < 0.5f && now < 0.5f;
            }
#else
            // original code. conflict with release()
            // idle: low [now]
            bool idle()
            {
                double now = container.at(0).x;

                return now < 0.5f;
            }
#endif

            // trigger: [then] low -> high [now]
            bool trigger( float interval_t = 0.0125f ) //interval useful here?
            {
                double then_t = container.at(1).t;
                double  now_t = container.at(0).t;

                if( now_t - then_t > interval_t )
                    return false; //time exceeded

                double then = container.at(1).x;
                double  now = container.at(0).x;

                return then < 0.5f && now >= 0.5f;
            }

            // hold: high [now]
            bool hold()
            {
                double now = container.at(0).x;

                return now >= 0.5f;
            }

            // release: [then] high -> low [now]
            bool release( float interval_t = 0.0125f ) //interval useful here?
            {
                double then_t = container.at(1).t;
                double  now_t = container.at(0).t;

                if( now_t - then_t > interval_t )
                    return false; //time exceeded

                double then = container.at(1).x;
                double  now = container.at(0).x;

                return then >= 0.5f && now < 0.5f;
            }

            // click: [then] low -> high -> low [now]       // also: peak, tap
            bool click( float interval_t = 0.500f )
            {
                double then_t = container.at(2).t;
                double  now_t = container.at(0).t;

                if( now_t - then_t > interval_t )
                    return false; //time exceeded

                double then = container.at(2).x;
                double  mid = container.at(1).x;
                double  now = container.at(0).x;

                return then < 0.5f && mid >= 0.5f && now < 0.5f;
            }

            // click: [then] low -> high -> low -> high -> low [now]
            bool dclick( float interval_t = 0.500f )
            {
                double then_t = container.at(4).t;
                double  now_t = container.at(0).t;

                if( now_t - then_t > interval_t )
                    return false; //time exceeded

                double then = container.at(4).x;
                double mid1 = container.at(3).x;
                double  mid = container.at(2).x;
                double mid2 = container.at(1).x;
                double  now = container.at(0).x;

                bool b0 = then >= 0.5f;
                bool b1 = mid1 >= 0.5f;
                bool b2 =  mid >= 0.5f;
                bool b3 = mid2 >= 0.5f;
                bool b4 =  now >= 0.5f;

                return then < 0.5f && mid1 >= 0.5f && mid < 0.5f && mid2 >= 0.5f && now < 0.5f;
            }

        // }
    };
}

namespace moon9
{
    namespace types
    {
        namespace hid
        {
            struct string
            {
                double t;
                std::string x;
            };

            template <typename T>
            struct vec1
            {
                double t;
                float treshold;
                T x, xdt;

                vec1( const T &t0 = T() ) : t(0),treshold(0.0f),x(t0),xdt(0) {}
                vec1( const vec1 &v ) { operator=(v); }
                vec1 &operator =( const vec1 &v ) { if( this != &v ) t = v.t, treshold = v.treshold, x = v.x, xdt = v.xdt; return *this; }
                void set( const vec1 &v ) { xdt = v.x - x; x = v.x; }
                const bool operator ==( const vec1 &v ) const { return std::abs( x - v.x ) < treshold; }
                void import( const vec1 &v )
                    { operator=( v ); };
            };

            template <typename T>
            struct vec2
            {
                double t;
                float treshold;
                T x, y, xdt, ydt;

                vec2( const T &t0 = T(), const T &t1 = T() ) : t(0),treshold(0.0f),x(t0),y(t1),xdt(0),ydt(0) {}
                vec2( const vec2 &v ) { operator=(v); }
                vec2 &operator =( const vec2 &v ) { if( this != &v ) t = v.t, treshold = v.treshold, x = v.x, y = v.y, xdt = v.xdt, ydt = v.ydt; return *this; }
                void set( const vec2 &v ) { xdt = v.x - x, ydt = v.y - y; x = v.x, y = v.y; }
                const bool operator ==( const vec2 &v ) const { return std::abs( x - v.x ) < treshold && std::abs( y - v.y ) < treshold; }
                void import( const vec2 &v )
                    { operator=( v ); };
            };

            template <typename T>
            struct vec3
            {
                double t;
                float treshold;
                T x, y, z, xdt, ydt, zdt;

                vec3( const T &t0 = T(), const T &t1 = T(), const T &t2 = T() ) : t(0),treshold(0.0f),x(t0),y(t1),z(t2),xdt(0),ydt(0),zdt(0) {}
                vec3( const vec3 &v ) { operator=(v); }
                vec3 &operator =( const vec3 &v ) { if( this != &v ) t = v.t, treshold = v.treshold, x = v.x, y = v.y, z = v.z, xdt = v.xdt, ydt = v.ydt, zdt = v.zdt; return *this; }
                void set( const vec3 &v ) { xdt = v.x - x, ydt = v.y - y, zdt = v.z - z; x = v.x, y = v.y, z = v.z; }
                const bool operator ==( const vec3 &v ) const { return std::abs( x - v.x ) < treshold && std::abs( y - v.y ) < treshold && std::abs( z - v.z ) < treshold; }
                void import( const vec3 &v )
                    { operator=( v ); };
            };
        }

        namespace wip_hid
        {
            // normalized data [ -1, +1 ] for HIDs (human interface devices)

            template <const int N, typename T = float>
            struct vec
            {
                T elem[ N ];

                vec()
                    { for( size_t i = 0; i < N; ++i ) elem[ i ] = 0; }
                vec( const vec<N,T> &v )
                    { operator=( v.elem ); }
                vec( const T *v )
                    { operator=( v ); }

                void operator =( const T *v )
                    { for( size_t i = 0; i < N; ++i ) elem[ i ] = v[ i ]; }

                operator T*()
                    { return elem; }
                operator const T*() const
                    { return elem; }

                T magnitude() const
                    { T dot = 0; for( size_t i = 0; i < N; ++i ) dot += elem[ i ] * elem[ i ]; return (T)sqrtf( (float)dot ); }
            };

            template <const int N>
            struct vec<N, double>
            {
                double magnitude() const
                    { double dot = 0; for( size_t i = 0; i < N; ++i ) dot += elem[ i ] * elem[ i ]; return sqrt( dot ); }
            };

            struct vec1 : public vec<1,float>
            {
                float &x;

                vec1( float X = 0 ) : x(elem[0])
                    { elem[0] = X; }
                vec1( const vec<1,float> &rhs ) : vec<1,float>(rhs), x(elem[0])
                    {}
                vec1( const vec1 &rhs ) : vec<1,float>(rhs), x(elem[0])
                    {}
                explicit vec1( const float *elements ) : vec<1,float>(elements), x(elem[0])
                    {}

                void operator=( const vec1 &v )
                    { elem[0] = v.elem[0]; }
                void import( const vec1 &v )
                    { operator=( v ); };
            };

            struct vec2 : public vec<2,float>
            {
                float &x, &y;

                vec2( float X = 0, float Y = 0 ) : x(elem[0]), y(elem[1])
                    { elem[0] = X, elem[1] = Y; }
                vec2( const vec<2,float> &rhs ) : vec<2,float>(rhs), x(elem[0]), y(elem[1])
                    {}
                vec2( const vec2 &rhs ) : vec<2,float>(rhs), x(elem[0]), y(elem[1])
                    {}
                explicit vec2( const float *elements ) : vec<2,float>(elements), x(elem[0]), y(elem[1])
                    {}

                void operator=( const vec2 &v )
                    { elem[0] = v.elem[0], elem[1] = v.elem[1]; }
                void import( const vec2 &v )
                    { operator=( v ); };
            };

            struct vec3 : public vec<3,float>
            {
                float &x, &y, &z;

                vec3( float X = 0, float Y = 0, float Z = 0 ) : x(elem[0]), y(elem[1]), z(elem[2])
                    { elem[0] = X, elem[1] = Y, elem[2] = Z; }
                vec3( const vec2 &xy, float z = 0 ) : x(elem[0]), y(elem[1]), z(elem[2])
                    { elem[0] = xy[0], elem[1] = xy[1], elem[2] = z; }
                vec3( const vec<3,float> &rhs ) : vec<3,float>(rhs), x(elem[0]), y(elem[1]), z(elem[2])
                    {}
                vec3( const vec3 &rhs ) : vec<3,float>(rhs), x(elem[0]), y(elem[1]), z(elem[2])
                    {}
                explicit vec3( const float *elements ) : vec<3,float>(elements), x(elem[0]), y(elem[1]), z(elem[2])
                    {}

                void operator=( const vec3 &v )
                    { elem[0] = v.elem[0], elem[1] = v.elem[1], elem[2] = v.elem[2]; }
                void import( const vec3 &v )
                    { operator=( v ); };

                // conversions
                const vec2 &xy() const
                    { return *reinterpret_cast<const vec2 *>(elem); }
                vec2 &xy()
                    { return *reinterpret_cast<vec2 *>(elem); }
            };

            struct vec4 : public vec<4,float>
            {
                float &x, &y, &z, &w;

                vec4( float X = 0, float Y = 0, float Z = 0, float W = 0 ) : x(elem[0]), y(elem[1]), z(elem[2]), w(elem[3])
                    { elem[0] = X, elem[1] = Y, elem[2] = Z, elem[3] = W; }
                vec4( const vec2 &xy, float Z = 0, float W = 0) : x(elem[0]), y(elem[1]), z(elem[2]), w(elem[3])
                    { elem[0] = xy[0], elem[1] = xy[1], elem[2] = Z, elem[3] = W; }
                vec4( const vec3 &xyz, float W = 0) : x(elem[0]), y(elem[1]), z(elem[2]), w(elem[3])
                    { elem[0] = xyz[0], elem[1] = xyz[1], elem[2] = xyz[2], elem[3] = W; }
                vec4( const vec<4,float> &rhs ) : vec<4,float>(rhs), x(elem[0]), y(elem[1]), z(elem[2]), w(elem[3])
                    {}
                vec4( const vec4 &rhs ) : vec<4,float>(rhs), x(elem[0]), y(elem[1]), z(elem[2]), w(elem[3])
                    {}

                explicit vec4( const float *elements ) : vec<4,float>(elements), x(elem[0]), y(elem[1]), z(elem[2]), w(elem[3])
                    {}

                void operator=( const vec4 &v )
                    { elem[0] = v.elem[0], elem[1] = v.elem[1], elem[2] = v.elem[2], elem[3] = v.elem[3]; }
                void import( const vec4 &v )
                    { operator=( v ); };

                // conversions
                const vec2 &xy() const
                    { return *reinterpret_cast<const vec2 *>(elem); }
                vec2 &xy()
                    { return *reinterpret_cast<vec2 *>(elem); }

                const vec3 &xyz() const
                    { return *reinterpret_cast<const vec3 *>(elem); }
                vec3 &xyz()
                    { return *reinterpret_cast<vec3 *>(elem); }
            };
        }
    }

    typedef moon9::history< types::hid::vec1<float> > flag;
    typedef moon9::history< types::hid::vec1<float> > key;
    typedef moon9::history< types::hid::vec1<float> > button;
    typedef moon9::history< types::hid::vec2<float> > coordinate;        //axis2d? xy?
    typedef moon9::history< types::hid::vec3<float> > axis;              //axis3d? xyz?
    typedef moon9::history< types::hid::     string > serializer;

    typedef std::vector<          flag > flags;
    typedef std::vector<           key > keys;
    typedef std::vector<        button > buttons;
    typedef std::vector<    coordinate > coordinates;
    typedef std::vector<          axis > axes;
    typedef std::vector<    serializer > serializers;
}
