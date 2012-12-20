// A few safe zero-init pod types. MIT licensed
// - rlyeh

#pragma once

namespace moon9
{
    template<typename T, int num = 0, int div = 1>
    struct pod
    {
        pod() : t(T(float(num)/div))
        {}

        pod( const T &_t ) : t(_t)
        {}

        T &operator =( const T &_t )
        {
            t = _t;
            return *this;
        }

        operator const T &() const
        {
            return t;
        }

        operator T &()
        {
            return t;
        }

        T &operator ++()
        {
            ++t;
            return t;
        }
        T &operator ++( int )
        {
            t++;
            return t;
        }
        T &operator --()
        {
            --t;
            return t;
        }
        T &operator --( int )
        {
            t--;
            return t;
        }

        protected: T t;
    };

    typedef moon9::pod<          int,  -1 > int_1;
    typedef moon9::pod<          int,   0 > int0;
    typedef moon9::pod<          int,   1 > int1;
    typedef moon9::pod<          int,   2 > int2;
    typedef moon9::pod<          int,   3 > int3;

    typedef moon9::pod<         bool,   0 > bool0;
    typedef moon9::pod<         bool,   1 > bool1;

    typedef moon9::pod<        float,  -1 > float_1;
    typedef moon9::pod<        float,   0 > float0;
    typedef moon9::pod<        float, 1,2 > float1_2;
    typedef moon9::pod<        float, 1,3 > float1_3;
    typedef moon9::pod<        float, 1,4 > float1_4;
    typedef moon9::pod<        float, 1,5 > float1_5;
    typedef moon9::pod<        float,   1 > float1;
    typedef moon9::pod<        float,   2 > float2;
    typedef moon9::pod<        float,   2 > float3;

    typedef moon9::pod<       double,  -1 > double_1;
    typedef moon9::pod<       double,   0 > double0;
    typedef moon9::pod<       double, 1,2 > double1_2;
    typedef moon9::pod<       double, 1,3 > double1_3;
    typedef moon9::pod<       double, 1,4 > double1_4;
    typedef moon9::pod<       double, 1,5 > double1_5;
    typedef moon9::pod<       double,   1 > double1;
    typedef moon9::pod<       double,   2 > double2;
    typedef moon9::pod<       double,   3 > double3;

    typedef moon9::pod< unsigned int,   0 > uint0;
    typedef moon9::pod< unsigned int,   1 > uint1;
    typedef moon9::pod< unsigned int,   2 > uint2;
    typedef moon9::pod< unsigned int,   3 > uint3;
    typedef moon9::pod< unsigned int,  ~0 > uintN;

    typedef moon9::pod<       size_t,   0 > sizet0;
    typedef moon9::pod<       size_t,   1 > sizet1;
    typedef moon9::pod<       size_t,   2 > sizet2;
    typedef moon9::pod<       size_t,  ~0 > sizetN;

    typedef moon9::pod<        float, (unsigned)(~0) > floatN;
    typedef moon9::pod<       double, (unsigned)(~0) > doubleN;
}
