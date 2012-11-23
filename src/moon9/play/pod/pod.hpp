// Safe init pod types. MIT licensed
// - rlyeh

#pragma once

namespace moon9
{
    template<typename T, int div = 0, int num = 1>
    struct pod
    {
        pod() : t(T(float(div)/num))
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

        protected: T t;
    };

    typedef moon9::pod<double,-1>  double1n;
    typedef moon9::pod<double,0>   double0;
    typedef moon9::pod<double,1,2> doubleh;
    typedef moon9::pod<double,1>   double1;
    typedef moon9::pod<double,2>   double2;

    typedef moon9::pod<float,-1>  float1n;
    typedef moon9::pod<float,0>   float0;
    typedef moon9::pod<float,1,2> floath;
    typedef moon9::pod<float,1>   float1;
    typedef moon9::pod<float,2>   float2;

    typedef moon9::pod<bool,0> bool0;
    typedef moon9::pod<bool,1> bool1;

    typedef moon9::pod<int,-1> int1n;
    typedef moon9::pod<int, 0> int0;
    typedef moon9::pod<int, 1> int1;
    typedef moon9::pod<int, 2> int2;

    typedef moon9::pod<unsigned int, 0> uint0;
    typedef moon9::pod<unsigned int, 1> uint1;
    typedef moon9::pod<unsigned int, 2> uint2;
    typedef moon9::pod<unsigned int,~0> uintN;
}
