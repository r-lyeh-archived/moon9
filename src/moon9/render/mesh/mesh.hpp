#pragma once

#include <algorithm>
#include <vector>

#include <moon9/render/render.hpp>
#include <moon9/render/mesh.hpp>

#include <moon9/debug/assert.hpp>
#include <moon9/io/file.hpp>
#include <moon9/spatial/vec.hpp>
#include <moon9/string/string.hpp>

/* Vectors */
typedef float float2_t[2];
typedef float float3_t[3];
typedef float float4_t[4];

union float3
{
    float3_t elem;

    struct {
        float x,y,z;
    };
    struct {
        float r,g,b;
    };
    struct {
        float i,j,k;
    };
    struct {
        float u,v,w;
    };

    const float &operator[]( int i ) const
    {
        return elem[i];
    }
    float &operator[]( int i )
    {
        return elem[i];
    }
};

union float4
{
    float4_t elem;

    struct {
        float x,y,z,w;
    };
    struct {
        float r,g,b,a;
    };
    struct {
        float i,j,k,l;
    };

    const float &operator[]( int i ) const
    {
        return elem[i];
    }
    float &operator[]( int i )
    {
        return elem[i];
    }
};

namespace moon9
{
    // @todo: layers: http://www.clockworkcoders.com/oglsl/tutorial8.htm
    // @todo: lights: http://www.clockworkcoders.com/oglsl/tutorial5.htm

    struct mesh
    {
        std::vector< float4 > vertices;
        std::vector< float3 > normals;
        std::vector< float3 > texcoords;

        std::vector< unsigned int > points;
        std::vector< unsigned int > lines;
        std::vector< unsigned int > triangles;
        /*
        std::vector< unsigned int > quads;
        std::vector< unsigned int > polygons;
        */

        bool is_hidden;
        bool is_processed;

        mesh() : is_hidden( false ), is_processed( true )
        {}

        // get_sphere / get_aabox / get_oobox
        // anim control -> play tweeners ping pong pingpong loop, speed factor
        // link/unlink -> attach/detach (+= transform?)
        // setscale ?
        // settexture( unit, id ) ?
        // dir/orient/facing
        // render IMM / render VAO / render VBO
        // shadow matrix
        // world matrix
        // local matrix

        void swap() // swaps vertex and normals from xyz to xzy basis
        {
            for( auto it = vertices.begin(), end = vertices.end(); it != end; ++it )
            {
                float4 &vert = *it;
                std::swap( vert[1], vert[2] );
                vert[1] = -vert[1];
            }

            for( auto it = normals.begin(), end = normals.end(); it != end; ++it )
            {
                float3 &vert = *it;
                std::swap( vert[1], vert[2] );
                vert[1] = -vert[1];
            }
        }

        void offset( float x, float y, float z, float w = 0 )
        {
            for( auto &it = vertices.begin(), end = vertices.end(); it != end; ++it )
            {
                float4 &vert = *it;
                vert[0] += x;
                vert[1] += y;
                vert[2] += z;
                vert[3] += w;
            }
        }

        void scale( float x, float y, float z, float w = 1 )
        {
            for( auto &it = vertices.begin(), end = vertices.end(); it != end; ++it )
            {
                float4 &vert = *it;
                vert[0] *= x;
                vert[1] *= y;
                vert[2] *= z;
                vert[3] *= w;
            }
        }

        void invert_normals()
        {
            for( auto &it = normals.begin(), end = normals.end(); it != end; ++it )
            {
                float3 &vert = *it;
                vert[0] = -vert[0];
                vert[1] = -vert[1];
                vert[2] = -vert[2];
            }
        }
    };
}
