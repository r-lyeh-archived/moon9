/*
    parametric curves:

    hermite curves
        -> cardinal curves: hermite curves with no tangent points required (calc from control points)
            T(i) = a * (P(i+1) - P(i-1))
            -> catmull rom curves: cardinal curves with a=0.5
    -> kochanek-bartel splines = hermite + handfull formulas to calc tangents with 3 control values:
        - tension: How sharply does the curve bend?
        - continuity: How rapid is the change in speed and direction?
        - bias: What is the direction of the curve as it passes through the keypoint?



    a ver, aclarando catmull-rom curves
    las curvas me sirven para trazar una ruta, o un tween de un valor a otro a medida.
    añado keypoints y defino la curva que *pasa* por todos los keypoints y puedo acceder a ella
    en un instante t (0 <= t <= 1).

    si le pido la longitud me da la longitud aproximada (mediante segmentos)

    si le pido la tangente en ese punto me da la normal, vector face, o direccion del objeto
    que sigue la curva (pa donde apunta el morro del coche)

    el problema de hacer 0 <= t <= 1 para acceder a la posicion en cualquier instante es que la
    knotlist no tiene los puntos distribuidos a intervalos regulares sobre la curva, y esto provo-
    cara que vayamos accediendo a la curva a velocidad variable. podriamos reparametrizar la curva
    o convertirla en uniforme (arc-length parametrization (1) o (2)), o segmentarla en lineas regulares y
    extrapolar la posicion entre vertices de cada segmento.

    la parametrizacion se puede hacer uniforme (alpha=0), chordal (alpha=1) o centripetal (alpha=0.5)

    (1) Arc Length Parameterization of Spline Curve (catmull->spline)
     http://www.saccade.com/writing/graphics/RE-PARAM.PDF

    (2) On the Parameterization of Catmull-Rom Curves
    http://www.cemyuksel.com/research/catmullrom_param/catmullrom.pdf

    otros tipos de curva


    spline
    bspline

    hermite: p1,p2,p3,p4. curve passes from p2 to p3.
    catrom, catmull-rom


    http://drdobbs.com/article/print?articleId=184410198&siteSectionName=

    BSpline and Catmull-Rom spline.
    The BSpline is an exterpolating cubic spline, which means that the resulting curve
    does not pass through its control points, but stays inside the bounding box generated
    by its control points. The Catmull-Rom spline is an interpolating cubic spline that
    passes through every control point, but is not as smooth as the BSpline.

    The main trade-off between the Catmull-Rom spline and the BSpline involves continuity.
    Wherever two curve segments join, BSplines are continuous in the second derivative
    (curvature). Catmull-Rom splines, on the other hand, are only continuous in the first
    derivative (slope). However, BSplines do not pass through the control points.

    Trigonometric splines (TSplines) are a good tradeoff. They're continuous in all
    derivatives plus you can choose to pass through all points (interpolating tspline) or
    not (exterpolating tspline).

    Parameterization and Applications of Catmull-Rom Curves
    http://faculty.cs.tamu.edu/schaefer/research/cr_cad.pdf
    http://faculty.cs.tamu.edu/schaefer/research/slides/catmullrom.pdf
*/

#pragma once

#include "vec.hpp"

namespace moon9
{
    namespace detail
    {
        template<typename T>
        class vector : public std::vector<T>
        {
            public:

            vector( const size_t N = 0, const T& t = T() ) : std::vector<T>(N, t)
            {}

            template<typename S>
            void append( const S &t )
            {
                /*
                size_t pos = this->size() ? this->size() - 1 : 0;
                this->resize( this->size() + t.size() );
                std::copy( t.begin(), t.end(), this->begin() + pos );
                */
                for( size_t i = 0; i < t.size(); ++i)
                    this->push_back( t[i] );
            }
        };
    }

    class spline : public moon9::detail::vector<vec3>
    {
        public:

        spline( const int &N = 0, const vec3 &V = vec3() ) : moon9::detail::vector<vec3>(N, V)
        {}

        static vec3 SplinePos (const spline & spline, float dt01)
        {
            assert( spline.size() >= 4 );
            assert( dt01 >= 0 && dt01 <= 1.0 );

            size_t size = spline.size();
            register float t = dt01 * ( ((float)(size - 3)) - 0.0001f );

            // Determine which spline segment we're on
            //if (t < 0) t = 0;
            //if (t > size - 3) t = (float)(size - 3) - 0.0001f;
            int segment = (int)floorf(t);
            t -= floorf(t);

            // Determine the four Hermite control values
            const vec3 P0 = spline[segment+1];
            const vec3 P1 = spline[segment+2];
            const vec3 T0 = (spline[segment+2] - spline[segment]) * 0.5f;
            const vec3 T1 = (spline[segment+3] - spline[segment+1]) * 0.5f;

            // Evaluate the Hermite basis functions

            const float hP0 = (2*t - 3)*t*t + 1;
            const float hP1 = (-2*t + 3)*t*t;
            const float hT0 = ((t - 2)*t + 1)*t;
            const float hT1 = (t - 1)*t*t;

            // Evaluate the curve
            return P0 * hP0 + P1 * hP1 + T0 * hT0 + T1 * hT1;
        }

        static vec3 SplineTangent (const spline & spline, float dt01)
        {
            assert( spline.size() >= 4 );
            assert( dt01 >= 0 && dt01 <= 1.0 );

            size_t size = spline.size();
            float t = dt01 * ( ((float)(size - 3)) - 0.0001f );

            // Determine which spline segment we're on
            //if (t < 0) t = 0;
            //if (t > size - 3) t = (float)(size - 3) - 0.0001f;
            int segment = (int)floorf(t);
            t -= floorf(t);

            // Determine the four Hermite control values
            const vec3 P0 = spline[segment+1];
            const vec3 P1 = spline[segment+2];
            const vec3 T0 = (spline[segment+2] - spline[segment]) * 0.5f;
            const vec3 T1 = (spline[segment+3] - spline[segment+1]) * 0.5f;

            // Evaluate the Hermite basis functions' derivatives
            const float hP0 = (6*t - 6)*t;
            const float hP1 = (-6*t + 6)*t;
            const float hT0 = (3*t - 4)*t + 1;
            const float hT1 = (3*t - 2)*t;

            // Evaluate the curve
            return P0 * hP0 + P1 * hP1 + T0 * hT0 + T1 * hT1;
        }

        static vec3 BSplinePos (const spline & spline, float dt01)
        {
            assert( spline.size() >= 4 );
            assert( dt01 >= 0 && dt01 <= 1.0 );

            size_t size = spline.size();
            register float t = dt01 * ( ((float)(size - 3)) - 0.0001f );

            // Determine which spline segment we're on
            //if (t < 0) t = 0;
            //if (t > size - 3) t = (float)(size - 3) - 0.0001f;
            int segment = (int)floorf(t);
            t -= floorf(t);

            //t = dt01;

            float const t2( t * t );
            float const t3( t2 * t );

            // Determine the four Hermite control values
            const vec3 a = spline[segment+0];
            const vec3 b = spline[segment+1];
            const vec3 c = spline[segment+2];
            const vec3 d = spline[segment+3];

            const vec3 result(
                ( a * ((  -t3)+( 3*t2)+(-3*t)+ 1) ) +
                ( b * (( 3*t3)+(-6*t2)       + 4) ) +
                ( c * ((-3*t3)+( 3*t2)+( 3*t)+ 1) ) +
                ( d * (   (t3)                  ) )
                );

            return vec3( result.x / 6.0f, result.y / 6.0f, result.z / 6.0f );
        }

        static vec3 SplinePosIncludingEdges( const spline &pointList, float dt01 )
        {
            // spline pos (border knots included)

            assert( pointList.size() >= 1 );

            spline knotList;

            knotList.push_back( pointList.front() );
            knotList.append( pointList );
            knotList.push_back( pointList.back() );

            return /*B*/SplinePos( knotList, dt01 );
        }

        static vec3 SplineTangentIncludingEdges( const spline &pointList, float dt01 )
        {
            // spline pos (border knots included)

            assert( pointList.size() >= 1 );

            spline knotList;

            knotList.push_back( pointList.front() );
            knotList.append( pointList );
            knotList.push_back( pointList.back() );

            return SplineTangent( knotList, dt01 );
        }

        vec3 tandt( float dt01 ) const
        {
            return SplineTangentIncludingEdges( *this, dt01 );
        }

        vec3 atdt( float dt01 ) const
        {
            return SplinePosIncludingEdges( *this, dt01 );
        }

        float length( size_t segments /* >= 1 */ )
        {
            float overall = 0.f;

            for (size_t i = 0; i < segments; ++i)
            {
                float dt01 = i / (float)segments;

                vec3 A = atdt( dt01 );
                vec3 B = atdt( dt01 + ( 1.f / float(segments) ) );

                overall += len( vec3( B - A ) );
            }

            return overall;
        }
    };
}
