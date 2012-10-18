#pragma once

#include <cassert>
#include <vector>

#include <glm/glm.hpp>

namespace moon9
{
    // http://bullet.googlecode.com/svn/trunk/Extras/vectormathlibrary/

    struct vec3 : public glm::vec3
    {
#ifdef MSGPACK_DEFINE
        MSGPACK_DEFINE( x, y, z );
#endif

        vec3() : glm::vec3(0,0,0)
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
            x = t.x;
            y = t.y;
            z = t.z;
            return *this;
        }

        // conversion

        template <typename T>
        T as()
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
    };

    // used for code clarifications, ie, move_to( point& ), push_from( point &pos, vector &dir ), etc...

    typedef moon9::vec3 ray;
    typedef moon9::vec3 point;
}

// utils

std::ostream &operator<<( std::ostream &os, const moon9::vec3 &v );

// utils

#include "utils.hpp"

