// A few safe zero-init pod types. MIT licensed
// - rlyeh

#pragma once

#include <limits.h>
#include <float.h>

#include <limits>

namespace moon9
{
    template<typename sign, typename T, sign num = 0, sign div = 1 >
    struct pod
    {
        pod() : t( T(((double)num)/div) )
        {}

        pod( const T &_t ) : t(_t)
        {}

        operator const T &() const
        {
            return t;
        }

        operator T &()
        {
            return t;
        }

#       define $pod(OP) \
        template<typename F> \
        pod &operator OP( const F &_t ) \
        { \
            t OP (T)_t; \
            return *this; \
        }
        $pod( =)
        $pod(|=)
        $pod(&=)
        $pod(^=)
        $pod(+=)
        $pod(-=)
        $pod(*=)
        $pod(/=)
        $pod(%=)
#       undef $pod

        pod &operator ++()
        {
            return ++t, *this;
        }
        pod operator ++( int )
        {
            pod other = *this;
            return t++, other;
        }
        pod &operator --()
        {
            return --t, *this;
        }
        pod operator --( int )
        {
            pod other = *this;
            return t--, other;
        }

        T max() const
        {
            return std::numeric_limits<T>::max();
        }
        T min() const
        {
            return std::numeric_limits<T>::min();
        }

        protected: T t;
    };

    typedef moon9::pod< signed, bool, 0 > bool0;
    typedef moon9::pod< signed, bool, 1 > bool1;

    typedef moon9::pod< signed, int, INT_MIN > int_N;
    typedef moon9::pod< signed, int,      -1 > int_1;
    typedef moon9::pod< signed, int,       0 > int0;
    typedef moon9::pod< signed, int,       1 > int1;
    typedef moon9::pod< signed, int,       2 > int2;
    typedef moon9::pod< signed, int,       3 > int3;
    typedef moon9::pod< signed, int, INT_MAX > intN;

    typedef moon9::pod< signed, float,  -1 > float_1;
    typedef moon9::pod< signed, float,   0 > float0;
    typedef moon9::pod< signed, float, 1,2 > float1_2;
    typedef moon9::pod< signed, float, 1,3 > float1_3;
    typedef moon9::pod< signed, float, 1,4 > float1_4;
    typedef moon9::pod< signed, float, 1,5 > float1_5;
    typedef moon9::pod< signed, float,   1 > float1;
    typedef moon9::pod< signed, float,   2 > float2;
    typedef moon9::pod< signed, float,   3 > float3;

    typedef moon9::pod< signed, double,  -1 > double_1;
    typedef moon9::pod< signed, double,   0 > double0;
    typedef moon9::pod< signed, double, 1,2 > double1_2;
    typedef moon9::pod< signed, double, 1,3 > double1_3;
    typedef moon9::pod< signed, double, 1,4 > double1_4;
    typedef moon9::pod< signed, double, 1,5 > double1_5;
    typedef moon9::pod< signed, double,   1 > double1;
    typedef moon9::pod< signed, double,   2 > double2;
    typedef moon9::pod< signed, double,   3 > double3;

    typedef moon9::pod< unsigned, size_t,   0 > sizet0;
    typedef moon9::pod< unsigned, size_t,   1 > sizet1;
    typedef moon9::pod< unsigned, size_t,   2 > sizet2;
    typedef moon9::pod< unsigned, size_t,  -1 > sizetN;

    typedef moon9::pod< unsigned, unsigned int,   0 > uint0;
    typedef moon9::pod< unsigned, unsigned int,   1 > uint1;
    typedef moon9::pod< unsigned, unsigned int,   2 > uint2;
    typedef moon9::pod< unsigned, unsigned int,   3 > uint3;
    typedef moon9::pod< unsigned, unsigned int,  -1 > uintN;

//  typedef moon9::pod<        float, (unsigned)(~0) > floatN;
//  typedef moon9::pod<       double, (unsigned)(~0) > doubleN;

//  typedef moon9::pod<        float, (long long int)(FLT_MAX) > floatN;
//  typedef moon9::pod<       double, (long long int)(DBL_MAX) > doubleN;
}
