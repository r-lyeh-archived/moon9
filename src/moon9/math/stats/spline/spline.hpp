// catmull-rom (spline) class
// -rlyeh

// add me
// Real-time cameras:
// p388 rounded catmull-rom spline
// p389 kochanek-bartels spline

#pragma once

#include <cassert>
#include <cmath>
#include <vector>

namespace moon9
{
    template < typename T, template <class U, class = std::allocator<U> > class container_t = std::vector >
    class spline : public container_t< T >
    {
        public:

        spline() : container_t<T>()
        {}

        spline( size_t size ) : container_t<T>( size )
        {}

        T position( float dt01 ) const
        {
            assert( this->size() >= 4 );
            assert( dt01 >= 0 && dt01 <= 1.0 );

            size_t size = this->size();
            register float t = dt01 * ( ((float)(size - 3)) - 0.0001f );

            // Determine which spline segment we're on
            //if (t < 0) t = 0;
            //if (t > size - 3) t = (float)(size - 3) - 0.0001f;
            int segment = (int)std::floor(t);
            t -= std::floor(t);

            // Determine the four Hermite control values
            const T P0 =  this->operator[](segment+1);
            const T P1 =  this->operator[](segment+2);
            const T T0 = (this->operator[](segment+2) - this->operator[](segment)) * 0.5f;
            const T T1 = (this->operator[](segment+3) - this->operator[](segment+1)) * 0.5f;

            // Evaluate the Hermite basis functions
            const float hP0 = (2*t - 3)*t*t + 1;
            const float hP1 = (-2*t + 3)*t*t;
            const float hT0 = ((t - 2)*t + 1)*t;
            const float hT1 = (t - 1)*t*t;

            // Evaluate the curve
            return P0 * hP0 + P1 * hP1 + T0 * hT0 + T1 * hT1;
        }

        T tangent( float dt01 ) const
        {
            assert( this->size() >= 4 );
            assert( dt01 >= 0 && dt01 <= 1.0 );

            size_t size = this->size();
            float t = dt01 * ( ((float)(size - 3)) - 0.0001f );

            // Determine which spline segment we're on
            //if (t < 0) t = 0;
            //if (t > size - 3) t = (float)(size - 3) - 0.0001f;
            int segment = (int)std::floor(t);
            t -= std::floor(t);

            // Determine the four Hermite control values
            const T P0 =  this->operator[](segment+1);
            const T P1 =  this->operator[](segment+2);
            const T T0 = (this->operator[](segment+2) - this->operator[](segment)) * 0.5f;
            const T T1 = (this->operator[](segment+3) - this->operator[](segment+1)) * 0.5f;

            // Evaluate the Hermite basis functions' derivatives
            const float hP0 = (6*t - 6)*t;
            const float hP1 = (-6*t + 6)*t;
            const float hT0 = (3*t - 4)*t + 1;
            const float hT1 = (3*t - 2)*t;

            // Evaluate the curve
            return P0 * hP0 + P1 * hP1 + T0 * hT0 + T1 * hT1;
        }

        T position_w_edges( float dt01 ) const
        {
            // spline pos (border knots included)

            assert( this->size() >= 1 );

/*
            spline knotList;
            knotList.push_back( this->front() );
            knotList.append( *this );
            knotList.push_back( this->back() );
*/
            spline knotList( this->size() + 2 );
            std::copy( this->begin(), this->end(), knotList.begin() + 1 );
            knotList.front() = *( this->begin() );
            knotList.back() = *( this->end() - 1 );

            return knotList.position( dt01 );
        }

        T tangent_w_edges( float dt01 ) const
        {
            // spline tangent (border knots included)

            assert( this->size() >= 1 );

/*
            spline knotList;

            knotList.push_back( this->front() );
            knotList.append( *this );
            knotList.push_back( this->back() );
*/
            spline knotList( this->size() + 2 );
            std::copy( this->begin(), this->end(), knotList.begin() + 1 );
            knotList.front() = *( this->begin() );
            knotList.back() = *( this->end() - 1 );

            return knotList.tangent( dt01 );
        }
    };
}

