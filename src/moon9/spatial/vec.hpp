#pragma once

#include <cassert>
#include <vector>

#include <glm/glm.hpp>

namespace moon9
{
    // http://bullet.googlecode.com/svn/trunk/Extras/vectormathlibrary/

    struct vec2 : public glm::vec2
    {
#ifdef MSGPACK_DEFINE
        MSGPACK_DEFINE( x, y );
#endif

        vec2() : glm::vec2( 0, 0 )
        {}

        vec2( float x, float y = 0 ) : glm::vec2( x, y )
        {}

        vec2( const float *ptr ) : glm::vec2( ( assert(ptr), ptr[0] ), ptr[1] )
        {}

        vec2( const glm::vec2 &other )
        {
            operator=( other );
        }

        template< typename T >
        explicit vec2( const T &t )
        {
            operator=( t );
        }

        template< typename T >
        vec2 &operator=( const T &t )
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

        vec2 xx() const { return vec2( x, x ); }
        vec2 yy() const { return vec2( y, y ); }

        vec2 xy() const { return vec2( x, y ); }
        vec2 yx() const { return vec2( y, x ); }

        vec2 lx() const { return vec2( 1, x ); }
        vec2 ly() const { return vec2( 1, y ); }
        vec2 xl() const { return vec2( x, 1 ); }
        vec2 yl() const { return vec2( y, 1 ); }

        vec2 ox() const { return vec2( 0, x ); }
        vec2 oy() const { return vec2( 0, y ); }
        vec2 xo() const { return vec2( x, 0 ); }
        vec2 yo() const { return vec2( y, 0 ); }

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

    struct vec3 : public glm::vec3
    {
#ifdef MSGPACK_DEFINE
        MSGPACK_DEFINE( x, y, z );
#endif

        vec3() : glm::vec3( 0, 0, 0 )
        {}

        vec3( float x, float y, float z = 0 ) : glm::vec3( x, y, z )
        {}

        vec3( const float *ptr ) : glm::vec3( ( assert(ptr), ptr[0] ), ptr[1], ptr[2] )
        {}

        vec3( const glm::vec3 &other )
        {
            operator=( other );
        }

        template< typename T >
        explicit vec3( const T &t )
        {
            operator=( t );
        }

        template< typename T >
        vec3 &operator=( const T &t )
        {
            x = t[0]; //t.x;
            y = t[1]; //t.y;
            z = t[2]; //t.z;
            return *this;
        }

        #define with(oper) \
        template< typename T > \
        vec3 &operator oper ( const T &t ) \
        { \
            x oper t.x; \
            y oper t.y; \
            z oper t.z; \
            return *this; \
        } \
        template<> \
        vec3 &operator oper ( const int &f ) \
        { \
            x oper f; \
            y oper f; \
            z oper f; \
            return *this; \
        } \
        template<> \
        vec3 &operator oper ( const float &f ) \
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
        vec3 operator oper ( const T &t ) const \
        { \
            return vec3( x oper t.x, y oper t.y, z oper t.z); \
        } \
        template<> \
        vec3 operator oper ( const int &f ) const \
        { \
            return vec3( x oper f, y oper f, z oper f); \
        } \
        template<> \
        vec3 operator oper ( const float &f ) const \
        { \
            return vec3( x oper f, y oper f, z oper f); \
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
           return T( x, y, z );
        }

        template <typename T> // implicit conversion. handy to convert automatically to glm::vec3, btVector3, vector3, Vector3f, vec3f, etc!
        operator T() const
        {
            return T( x, y, z );
        }

        // swizzles

        vec3 xxx() const { return vec3( x, x, x ); }
        vec3 yyy() const { return vec3( y, y, y ); }
        vec3 zzz() const { return vec3( z, z, z ); }

        vec3 xyz() const { return vec3( x, y, z ); }
        vec3 xzy() const { return vec3( x, z, y ); }
        vec3 yxz() const { return vec3( y, x, z ); }
        vec3 yzx() const { return vec3( y, z, x ); }
        vec3 zxy() const { return vec3( z, x, y ); }
        vec3 zyx() const { return vec3( z, y, x ); }

        vec3 lxy() const { return vec3( 1, x, y ); }
        vec3 lyx() const { return vec3( 1, y, x ); }
        vec3 lxz() const { return vec3( 1, x, z ); }
        vec3 lzx() const { return vec3( 1, z, x ); }
        vec3 lyz() const { return vec3( 1, y, z ); }
        vec3 lzy() const { return vec3( 1, z, y ); }
        vec3 xly() const { return vec3( x, 1, y ); }
        vec3 ylx() const { return vec3( y, 1, x ); }
        vec3 xlz() const { return vec3( x, 1, z ); }
        vec3 zlx() const { return vec3( z, 1, x ); }
        vec3 ylz() const { return vec3( y, 1, z ); }
        vec3 zly() const { return vec3( z, 1, y ); }
        vec3 xyl() const { return vec3( x, y, 1 ); }
        vec3 yxl() const { return vec3( y, x, 1 ); }
        vec3 xzl() const { return vec3( x, z, 1 ); }
        vec3 zxl() const { return vec3( z, x, 1 ); }
        vec3 yzl() const { return vec3( y, z, 1 ); }
        vec3 zyl() const { return vec3( z, y, 1 ); }

        vec3 oxy() const { return vec3( 0, x, y ); }
        vec3 oyx() const { return vec3( 0, y, x ); }
        vec3 oxz() const { return vec3( 0, x, z ); }
        vec3 ozx() const { return vec3( 0, z, x ); }
        vec3 oyz() const { return vec3( 0, y, z ); }
        vec3 ozy() const { return vec3( 0, z, y ); }
        vec3 xoy() const { return vec3( x, 0, y ); }
        vec3 yox() const { return vec3( y, 0, x ); }
        vec3 xoz() const { return vec3( x, 0, z ); }
        vec3 zox() const { return vec3( z, 0, x ); }
        vec3 yoz() const { return vec3( y, 0, z ); }
        vec3 zoy() const { return vec3( z, 0, y ); }
        vec3 xyo() const { return vec3( x, y, 0 ); }
        vec3 yxo() const { return vec3( y, x, 0 ); }
        vec3 xzo() const { return vec3( x, z, 0 ); }
        vec3 zxo() const { return vec3( z, x, 0 ); }
        vec3 yzo() const { return vec3( y, z, 0 ); }
        vec3 zyo() const { return vec3( z, y, 0 ); }

        vec3  xy() const { return vec3( x, y, 0 ); }
        vec3  yx() const { return vec3( y, x, 0 ); }
        vec3  xz() const { return vec3( x, z, 0 ); }
        vec3  zx() const { return vec3( x, z, 0 ); }

        vec3  xx() const { return vec3( x, x, 0 ); }
        vec3  yy() const { return vec3( y, y, 0 ); }
        vec3  zz() const { return vec3( z, z, 0 ); }

        // tools

        const float *data() const
        {
            return &x; // @todo: assert x,y,z are contigous!
        }

        float *data()
        {
            return &x; // @todo: assert x,y,z are contigous!
        }

        vec3 &operator()( const float &_x, const float &_y, const float &_z )
        {
            x = _x;
            y = _y;
            z = _z;
            return *this;
        }
    };

    // used for code clarifications, ie, move_to( point& ), push_from( point &pos, vector &dir ), etc...

    typedef moon9::vec3 ray;
    typedef moon9::vec3 point;
}

// utils

std::ostream &operator<<( std::ostream &os, const moon9::vec3 &v );

// utils

#include "utils.hpp"

