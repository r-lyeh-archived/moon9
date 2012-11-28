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

        // swizzles

        vec3t xxx() const { return vec3t( x, x, x ); }
        vec3t yyy() const { return vec3t( y, y, y ); }
        vec3t zzz() const { return vec3t( z, z, z ); }

        vec3t xyz() const { return vec3t( x, y, z ); }
        vec3t xzy() const { return vec3t( x, z, y ); }
        vec3t yxz() const { return vec3t( y, x, z ); }
        vec3t yzx() const { return vec3t( y, z, x ); }
        vec3t zxy() const { return vec3t( z, x, y ); }
        vec3t zyx() const { return vec3t( z, y, x ); }

        vec3t lxy() const { return vec3t( 1, x, y ); }
        vec3t lyx() const { return vec3t( 1, y, x ); }
        vec3t lxz() const { return vec3t( 1, x, z ); }
        vec3t lzx() const { return vec3t( 1, z, x ); }
        vec3t lyz() const { return vec3t( 1, y, z ); }
        vec3t lzy() const { return vec3t( 1, z, y ); }
        vec3t xly() const { return vec3t( x, 1, y ); }
        vec3t ylx() const { return vec3t( y, 1, x ); }
        vec3t xlz() const { return vec3t( x, 1, z ); }
        vec3t zlx() const { return vec3t( z, 1, x ); }
        vec3t ylz() const { return vec3t( y, 1, z ); }
        vec3t zly() const { return vec3t( z, 1, y ); }
        vec3t xyl() const { return vec3t( x, y, 1 ); }
        vec3t yxl() const { return vec3t( y, x, 1 ); }
        vec3t xzl() const { return vec3t( x, z, 1 ); }
        vec3t zxl() const { return vec3t( z, x, 1 ); }
        vec3t yzl() const { return vec3t( y, z, 1 ); }
        vec3t zyl() const { return vec3t( z, y, 1 ); }

        vec3t oxy() const { return vec3t( 0, x, y ); }
        vec3t oyx() const { return vec3t( 0, y, x ); }
        vec3t oxz() const { return vec3t( 0, x, z ); }
        vec3t ozx() const { return vec3t( 0, z, x ); }
        vec3t oyz() const { return vec3t( 0, y, z ); }
        vec3t ozy() const { return vec3t( 0, z, y ); }
        vec3t xoy() const { return vec3t( x, 0, y ); }
        vec3t yox() const { return vec3t( y, 0, x ); }
        vec3t xoz() const { return vec3t( x, 0, z ); }
        vec3t zox() const { return vec3t( z, 0, x ); }
        vec3t yoz() const { return vec3t( y, 0, z ); }
        vec3t zoy() const { return vec3t( z, 0, y ); }
        vec3t xyo() const { return vec3t( x, y, 0 ); }
        vec3t yxo() const { return vec3t( y, x, 0 ); }
        vec3t xzo() const { return vec3t( x, z, 0 ); }
        vec3t zxo() const { return vec3t( z, x, 0 ); }
        vec3t yzo() const { return vec3t( y, z, 0 ); }
        vec3t zyo() const { return vec3t( z, y, 0 ); }

        vec3t  xy() const { return vec3t( x, y, 0 ); }
        vec3t  yx() const { return vec3t( y, x, 0 ); }
        vec3t  xz() const { return vec3t( x, z, 0 ); }
        vec3t  zx() const { return vec3t( x, z, 0 ); }

        vec3t  xx() const { return vec3t( x, x, 0 ); }
        vec3t  yy() const { return vec3t( y, y, 0 ); }
        vec3t  zz() const { return vec3t( z, z, 0 ); }

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

    typedef moon9::vec2t<0,0>   vec2;
    typedef moon9::vec3t<0,0,0> vec3;

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

    // used for code clarifications, ie, move_to( point& ), push_from( point &pos, vector &dir ), etc...

    typedef moon9::vec3 ray;
    typedef moon9::vec3 point;
}

// utils

std::ostream &operator<<( std::ostream &os, const moon9::vec2 &v );
std::ostream &operator<<( std::ostream &os, const moon9::vec3 &v );

// utils

#include "utils.hpp"

