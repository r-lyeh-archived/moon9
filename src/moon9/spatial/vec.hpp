#pragma once

#include <cassert>
#include <vector>

#include <glm/glm.hpp>

namespace moon9
{
    // http://bullet.googlecode.com/svn/trunk/Extras/vectormathlibrary/

    template< int X = 0, int Y = 0 >
    struct vec2t : public glm::vec2
    {
#ifdef MSGPACK_DEFINE
        MSGPACK_DEFINE( x, y );
#endif

        vec2t() : glm::vec2( X, Y )
        {}

        explicit
        vec2t( float v ) : glm::vec2( v * X, v * Y )
        {}

        vec2t( float x, float y ) : glm::vec2( x, y )
        {}

        //explicit
        vec2t( const float *ptr ) : glm::vec2( ( assert(ptr), ptr[0] ), ptr[1] )
        {}

        vec2t( const glm::vec2 &other )
        {
            operator=( other );
        }

        template< typename T >
        explicit vec2t( const T &t )
        {
            operator=( t );
        }

        template< typename T >
        vec2t &operator=( const T &t )
        {
            x = t.x;
            y = t.y;
            return *this;
        }

        // conversion

        template <typename T>
        T as()
        {
           return T( x, y );
        }

        // swizzles

        vec2t xx() const { return vec2t( x, x ); }
        vec2t yy() const { return vec2t( y, y ); }

        vec2t xy() const { return vec2t( x, y ); }
        vec2t yx() const { return vec2t( y, x ); }

        vec2t lx() const { return vec2t( 1, x ); }
        vec2t ly() const { return vec2t( 1, y ); }
        vec2t xl() const { return vec2t( x, 1 ); }
        vec2t yl() const { return vec2t( y, 1 ); }

        vec2t ox() const { return vec2t( 0, x ); }
        vec2t oy() const { return vec2t( 0, y ); }
        vec2t xo() const { return vec2t( x, 0 ); }
        vec2t yo() const { return vec2t( y, 0 ); }

        // tools

        const float *data() const
        {
            return &x; // @todo: assert x,y,z are contigous!
        }

        float *data()
        {
            return &x; // @todo: assert x,y,z are contigous!
        }
    };

    template< int X = 0, int Y = 0, int Z = 0 >
    struct vec3t : public glm::vec3
    {
#ifdef MSGPACK_DEFINE
        MSGPACK_DEFINE( x, y, z );
#endif

        vec3t() : glm::vec3( X, Y, Z )
        {}

        explicit
        vec3t( float v ) : glm::vec3( v * X, v * Y, v * Z )
        {}

        vec3t( float x, float y, float z ) : glm::vec3( x, y, z )
        {}

        //explicit
        vec3t( const float *ptr ) : glm::vec3( ( assert(ptr), ptr[0] ), ptr[1], ptr[2] )
        {}

        vec3t( const glm::vec3 &other )
        {
            operator=( other );
        }

        template< typename T >
        explicit vec3t( const T &t )
        {
            operator=( t );
        }

        template< typename T >
        vec3t &operator=( const T &t )
        {
            x = t[0]; //t.x;
            y = t[1]; //t.y;
            z = t[2]; //t.z;
            return *this;
        }

        #define with(oper) \
        template< typename T > \
        vec3t &operator oper ( const T &t ) \
        { \
            x oper t[0]; \
            y oper t[1]; \
            z oper t[2]; \
            return *this; \
        } \
        template<> \
        vec3t &operator oper ( const int &f ) \
        { \
            x oper f; \
            y oper f; \
            z oper f; \
            return *this; \
        } \
        template<> \
        vec3t &operator oper ( const float &f ) \
        { \
            x oper f; \
            y oper f; \
            z oper f; \
            return *this; \
        }
        with(+=)
        with(-=)
        with(*=)
        with(/=)
        #undef with

        #define with(oper) \
        template< typename T > \
        vec3t operator oper ( const T &t ) const \
        { \
            return vec3t( x oper t[0], y oper t[1], z oper t[2]); \
        } \
        template<> \
        vec3t operator oper ( const int &f ) const \
        { \
            return vec3t( x oper f, y oper f, z oper f); \
        } \
        template<> \
        vec3t operator oper ( const float &f ) const \
        { \
            return vec3t( x oper f, y oper f, z oper f); \
        }
        with(+)
        with(-)
        with(*)
        with(/)
        #undef with

        // conversion

        template <typename T>
        T as() const
        {
            T t;
            t[0] = x, t[1] = y, t[2] = z;
            return t;
        }

        template <typename T> // implicit conversion. handy to convert automatically to glm::vec3, btVector3, vector3, Vector3f, vec3f, etc!
        operator T() const
        {
           return as<T>();
        }

        // basis conversion
        // right,forward,up to right,up,forward
        // right,up,forward to right,forward,up
        vec3t swap0() const
        {
            return vec3t( x, -z, y );
        }
        vec3t swap1() const
        {
            return vec3t( x, z, -y );
        }

        // swizzles, generated code for base-5 { o, l, x, y, z }

        vec3t  oo() const { return vec3t( 0, 0, 0 ); }
        vec3t  ol() const { return vec3t( 0, 1, 0 ); }
        vec3t  ox() const { return vec3t( 0, x, 0 ); }
        vec3t  oy() const { return vec3t( 0, y, 0 ); }
        vec3t  oz() const { return vec3t( 0, z, 0 ); }
        vec3t  lo() const { return vec3t( 1, 0, 0 ); }
        vec3t  ll() const { return vec3t( 1, 1, 0 ); }
        vec3t  lx() const { return vec3t( 1, x, 0 ); }
        vec3t  ly() const { return vec3t( 1, y, 0 ); }
        vec3t  lz() const { return vec3t( 1, z, 0 ); }
        vec3t  xo() const { return vec3t( x, 0, 0 ); }
        vec3t  xl() const { return vec3t( x, 1, 0 ); }
        vec3t  xx() const { return vec3t( x, x, 0 ); }
        vec3t  xy() const { return vec3t( x, y, 0 ); }
        vec3t  xz() const { return vec3t( x, z, 0 ); }
        vec3t  yo() const { return vec3t( y, 0, 0 ); }
        vec3t  yl() const { return vec3t( y, 1, 0 ); }
        vec3t  yx() const { return vec3t( y, x, 0 ); }
        vec3t  yy() const { return vec3t( y, y, 0 ); }
        vec3t  yz() const { return vec3t( y, z, 0 ); }
        vec3t  zo() const { return vec3t( z, 0, 0 ); }
        vec3t  zl() const { return vec3t( z, 1, 0 ); }
        vec3t  zx() const { return vec3t( z, x, 0 ); }
        vec3t  zy() const { return vec3t( z, y, 0 ); }
        vec3t  zz() const { return vec3t( z, z, 0 ); }

        vec3t ooo() const { return vec3t( 0, 0, 0 ); }
        vec3t ool() const { return vec3t( 0, 0, 1 ); }
        vec3t oox() const { return vec3t( 0, 0, x ); }
        vec3t ooy() const { return vec3t( 0, 0, y ); }
        vec3t ooz() const { return vec3t( 0, 0, z ); }
        vec3t olo() const { return vec3t( 0, 1, 0 ); }
        vec3t oll() const { return vec3t( 0, 1, 1 ); }
        vec3t olx() const { return vec3t( 0, 1, x ); }
        vec3t oly() const { return vec3t( 0, 1, y ); }
        vec3t olz() const { return vec3t( 0, 1, z ); }
        vec3t oxo() const { return vec3t( 0, x, 0 ); }
        vec3t oxl() const { return vec3t( 0, x, 1 ); }
        vec3t oxx() const { return vec3t( 0, x, x ); }
        vec3t oxy() const { return vec3t( 0, x, y ); }
        vec3t oxz() const { return vec3t( 0, x, z ); }
        vec3t oyo() const { return vec3t( 0, y, 0 ); }
        vec3t oyl() const { return vec3t( 0, y, 1 ); }
        vec3t oyx() const { return vec3t( 0, y, x ); }
        vec3t oyy() const { return vec3t( 0, y, y ); }
        vec3t oyz() const { return vec3t( 0, y, z ); }
        vec3t ozo() const { return vec3t( 0, z, 0 ); }
        vec3t ozl() const { return vec3t( 0, z, 1 ); }
        vec3t ozx() const { return vec3t( 0, z, x ); }
        vec3t ozy() const { return vec3t( 0, z, y ); }
        vec3t ozz() const { return vec3t( 0, z, z ); }
        vec3t loo() const { return vec3t( 1, 0, 0 ); }
        vec3t lol() const { return vec3t( 1, 0, 1 ); }
        vec3t lox() const { return vec3t( 1, 0, x ); }
        vec3t loy() const { return vec3t( 1, 0, y ); }
        vec3t loz() const { return vec3t( 1, 0, z ); }
        vec3t llo() const { return vec3t( 1, 1, 0 ); }
        vec3t lll() const { return vec3t( 1, 1, 1 ); }
        vec3t llx() const { return vec3t( 1, 1, x ); }
        vec3t lly() const { return vec3t( 1, 1, y ); }
        vec3t llz() const { return vec3t( 1, 1, z ); }
        vec3t lxo() const { return vec3t( 1, x, 0 ); }
        vec3t lxl() const { return vec3t( 1, x, 1 ); }
        vec3t lxx() const { return vec3t( 1, x, x ); }
        vec3t lxy() const { return vec3t( 1, x, y ); }
        vec3t lxz() const { return vec3t( 1, x, z ); }
        vec3t lyo() const { return vec3t( 1, y, 0 ); }
        vec3t lyl() const { return vec3t( 1, y, 1 ); }
        vec3t lyx() const { return vec3t( 1, y, x ); }
        vec3t lyy() const { return vec3t( 1, y, y ); }
        vec3t lyz() const { return vec3t( 1, y, z ); }
        vec3t lzo() const { return vec3t( 1, z, 0 ); }
        vec3t lzl() const { return vec3t( 1, z, 1 ); }
        vec3t lzx() const { return vec3t( 1, z, x ); }
        vec3t lzy() const { return vec3t( 1, z, y ); }
        vec3t lzz() const { return vec3t( 1, z, z ); }
        vec3t xoo() const { return vec3t( x, 0, 0 ); }
        vec3t xol() const { return vec3t( x, 0, 1 ); }
        vec3t xox() const { return vec3t( x, 0, x ); }
        vec3t xoy() const { return vec3t( x, 0, y ); }
        vec3t xoz() const { return vec3t( x, 0, z ); }
        vec3t xlo() const { return vec3t( x, 1, 0 ); }
        vec3t xll() const { return vec3t( x, 1, 1 ); }
        vec3t xlx() const { return vec3t( x, 1, x ); }
        vec3t xly() const { return vec3t( x, 1, y ); }
        vec3t xlz() const { return vec3t( x, 1, z ); }
        vec3t xxo() const { return vec3t( x, x, 0 ); }
        vec3t xxl() const { return vec3t( x, x, 1 ); }
        vec3t xxx() const { return vec3t( x, x, x ); }
        vec3t xxy() const { return vec3t( x, x, y ); }
        vec3t xxz() const { return vec3t( x, x, z ); }
        vec3t xyo() const { return vec3t( x, y, 0 ); }
        vec3t xyl() const { return vec3t( x, y, 1 ); }
        vec3t xyx() const { return vec3t( x, y, x ); }
        vec3t xyy() const { return vec3t( x, y, y ); }
        vec3t xyz() const { return vec3t( x, y, z ); }
        vec3t xzo() const { return vec3t( x, z, 0 ); }
        vec3t xzl() const { return vec3t( x, z, 1 ); }
        vec3t xzx() const { return vec3t( x, z, x ); }
        vec3t xzy() const { return vec3t( x, z, y ); }
        vec3t xzz() const { return vec3t( x, z, z ); }
        vec3t yoo() const { return vec3t( y, 0, 0 ); }
        vec3t yol() const { return vec3t( y, 0, 1 ); }
        vec3t yox() const { return vec3t( y, 0, x ); }
        vec3t yoy() const { return vec3t( y, 0, y ); }
        vec3t yoz() const { return vec3t( y, 0, z ); }
        vec3t ylo() const { return vec3t( y, 1, 0 ); }
        vec3t yll() const { return vec3t( y, 1, 1 ); }
        vec3t ylx() const { return vec3t( y, 1, x ); }
        vec3t yly() const { return vec3t( y, 1, y ); }
        vec3t ylz() const { return vec3t( y, 1, z ); }
        vec3t yxo() const { return vec3t( y, x, 0 ); }
        vec3t yxl() const { return vec3t( y, x, 1 ); }
        vec3t yxx() const { return vec3t( y, x, x ); }
        vec3t yxy() const { return vec3t( y, x, y ); }
        vec3t yxz() const { return vec3t( y, x, z ); }
        vec3t yyo() const { return vec3t( y, y, 0 ); }
        vec3t yyl() const { return vec3t( y, y, 1 ); }
        vec3t yyx() const { return vec3t( y, y, x ); }
        vec3t yyy() const { return vec3t( y, y, y ); }
        vec3t yyz() const { return vec3t( y, y, z ); }
        vec3t yzo() const { return vec3t( y, z, 0 ); }
        vec3t yzl() const { return vec3t( y, z, 1 ); }
        vec3t yzx() const { return vec3t( y, z, x ); }
        vec3t yzy() const { return vec3t( y, z, y ); }
        vec3t yzz() const { return vec3t( y, z, z ); }
        vec3t zoo() const { return vec3t( z, 0, 0 ); }
        vec3t zol() const { return vec3t( z, 0, 1 ); }
        vec3t zox() const { return vec3t( z, 0, x ); }
        vec3t zoy() const { return vec3t( z, 0, y ); }
        vec3t zoz() const { return vec3t( z, 0, z ); }
        vec3t zlo() const { return vec3t( z, 1, 0 ); }
        vec3t zll() const { return vec3t( z, 1, 1 ); }
        vec3t zlx() const { return vec3t( z, 1, x ); }
        vec3t zly() const { return vec3t( z, 1, y ); }
        vec3t zlz() const { return vec3t( z, 1, z ); }
        vec3t zxo() const { return vec3t( z, x, 0 ); }
        vec3t zxl() const { return vec3t( z, x, 1 ); }
        vec3t zxx() const { return vec3t( z, x, x ); }
        vec3t zxy() const { return vec3t( z, x, y ); }
        vec3t zxz() const { return vec3t( z, x, z ); }
        vec3t zyo() const { return vec3t( z, y, 0 ); }
        vec3t zyl() const { return vec3t( z, y, 1 ); }
        vec3t zyx() const { return vec3t( z, y, x ); }
        vec3t zyy() const { return vec3t( z, y, y ); }
        vec3t zyz() const { return vec3t( z, y, z ); }
        vec3t zzo() const { return vec3t( z, z, 0 ); }
        vec3t zzl() const { return vec3t( z, z, 1 ); }
        vec3t zzx() const { return vec3t( z, z, x ); }
        vec3t zzy() const { return vec3t( z, z, y ); }
        vec3t zzz() const { return vec3t( z, z, z ); }


        // tools

        const float *data() const
        {
            return &x; // @todo: assert x,y,z are contigous!
        }

        float *data()
        {
            return &x; // @todo: assert x,y,z are contigous!
        }

        vec3t &operator()( const float &_x, const float &_y, const float &_z )
        {
            x = _x;
            y = _y;
            z = _z;
            return *this;
        }
    };


    template< int X = 0, int Y = 0, int Z = 0, int W = 0 >
    struct vec4t : public glm::vec4
    {
#ifdef MSGPACK_DEFINE
        MSGPACK_DEFINE( x, y, z, w );
#endif

        vec4t() : glm::vec4( X, Y, Z, W )
        {}

        explicit
        vec4t( float v ) : glm::vec4( v * X, v * Y, v * Z, v * W )
        {}

        vec4t( float x, float y, float z ) : glm::vec4( x, y, z, w )
        {}

        //explicit
        vec4t( const float *ptr ) : glm::vec4( ( assert(ptr), ptr[0] ), ptr[1], ptr[2], ptr[3] )
        {}

        vec4t( const glm::vec4 &other )
        {
            operator=( other );
        }

        template< typename T >
        explicit vec4t( const T &t )
        {
            operator=( t );
        }

        template< typename T >
        vec4t &operator=( const T &t )
        {
            x = t[0]; //t.x;
            y = t[1]; //t.y;
            z = t[2]; //t.z;
            return *this;
        }

        #define with(oper) \
        template< typename T > \
        vec4t &operator oper ( const T &t ) \
        { \
            x oper t[0]; \
            y oper t[1]; \
            z oper t[2]; \
            return *this; \
        } \
        template<> \
        vec4t &operator oper ( const int &f ) \
        { \
            x oper f; \
            y oper f; \
            z oper f; \
            return *this; \
        } \
        template<> \
        vec4t &operator oper ( const float &f ) \
        { \
            x oper f; \
            y oper f; \
            z oper f; \
            return *this; \
        }
        with(+=)
        with(-=)
        with(*=)
        with(/=)
        #undef with

        #define with(oper) \
        template< typename T > \
        vec4t operator oper ( const T &t ) const \
        { \
            return vec4t( x oper t[0], y oper t[1], z oper t[2], w oper t[3]); \
        } \
        template<> \
        vec4t operator oper ( const int &f ) const \
        { \
            return vec4t( x oper f, y oper f, z oper f, w oper f); \
        } \
        template<> \
        vec4t operator oper ( const float &f ) const \
        { \
            return vec4t( x oper f, y oper f, z oper f, w oper f); \
        }
        with(+)
        with(-)
        with(*)
        with(/)
        #undef with

        // conversion

        template <typename T>
        T as() const
        {
            T t;
            t[0] = x, t[1] = y, t[2] = z, t[3] = w;
            return t;
        }

        template <typename T> // implicit conversion. handy to convert automatically to glm::vec3, btVector3, vector3, Vector3f, vec3f, etc!
        operator T() const
        {
           return as<T>();
        }

        // basis conversion
        // right,forward,up to right,up,forward
        // right,up,forward to right,forward,up
        vec4t swap0() const
        {
            return vec4t( x, -z, y, w );
        }
        vec4t swap1() const
        {
            return vec4t( x, z, -y, w );
        }

        // swizzles, generated code for base-6 { o, l, x, y, z, w }
        // @todo !

        // tools

        const float *data() const
        {
            return &x; // @todo: assert x,y,z,w are contigous!
        }

        float *data()
        {
            return &x; // @todo: assert x,y,z,w are contigous!
        }

        vec4t &operator()( const float &_x, const float &_y, const float &_z, const float &_w )
        {
            x = _x;
            y = _y;
            z = _z;
            w = _w;
            return *this;
        }
    };

    typedef moon9::vec2t<0,0>     vec2;
    typedef moon9::vec3t<0,0,0>   vec3;
    typedef moon9::vec4t<0,0,0,0> vec4;

    typedef moon9::vec2t<0,0>   float00;
    typedef moon9::vec2t<0,1>   float01;
    typedef moon9::vec2t<1,0>   float10;
    typedef moon9::vec2t<1,1>   float11;
    typedef moon9::vec3t<0,0,0> float000;
    typedef moon9::vec3t<0,0,1> float001;
    typedef moon9::vec3t<0,1,0> float010;
    typedef moon9::vec3t<0,1,1> float011;
    typedef moon9::vec3t<1,0,0> float100;
    typedef moon9::vec3t<1,0,1> float101;
    typedef moon9::vec3t<1,1,0> float110;
    typedef moon9::vec3t<1,1,1> float111;

    typedef moon9::vec4t<0,0,0,0> float0000;
    typedef moon9::vec4t<0,0,0,1> float0001;
    typedef moon9::vec4t<0,0,1,0> float0010;
    typedef moon9::vec4t<0,0,1,1> float0011;
    typedef moon9::vec4t<0,1,0,0> float0100;
    typedef moon9::vec4t<0,1,0,1> float0101;
    typedef moon9::vec4t<0,1,1,0> float0110;
    typedef moon9::vec4t<0,1,1,1> float0111;
    typedef moon9::vec4t<1,0,0,0> float1000;
    typedef moon9::vec4t<1,0,0,1> float1001;
    typedef moon9::vec4t<1,0,1,0> float1010;
    typedef moon9::vec4t<1,0,1,1> float1011;
    typedef moon9::vec4t<1,1,0,0> float1100;
    typedef moon9::vec4t<1,1,0,1> float1101;
    typedef moon9::vec4t<1,1,1,0> float1110;
    typedef moon9::vec4t<1,1,1,1> float1111;

    // used for code clarifications, ie, move_to( point& ), push_from( point &pos, vector &dir ), etc...

    typedef moon9::vec3 ray;
    typedef moon9::vec3 point;
}

// utils

std::ostream &operator<<( std::ostream &os, const moon9::vec2 &v );
std::ostream &operator<<( std::ostream &os, const moon9::vec3 &v );
std::ostream &operator<<( std::ostream &os, const moon9::vec4 &v );

// utils

#include "utils.hpp"

