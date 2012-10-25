// taken from vlee
#if 0

#pragma once

#include <render9/render.hpp>

#include <algorithm>

extern typename vec3;

namespace moon9
{
    template <typename T>
    inline float clamp(T x, T minval, T maxval)
    {
        return std::max(std::min(x, maxval), minval);
    }

    template <typename T>
    inline T smoothstep(T edge0, T edge1, float x)
    {
        x = (x - edge0) / (edge1 - edge0);
        float t = clamp(x, 0.0f, 1.0f);
        return t * t * (3 - 2 * t);
    }

    template <typename T>
    inline T lerp(T v0, T v1, float t)
    {
        return v0 + (v1 - v0) * t;
    }

    inline float round(float f)
    {
        return floor(f + 0.5f);
    }

    inline float frac(float f)
    {
        return f - floor(f);
    }

    inline float randf()
    {
        return rand() * (1.f / RAND_MAX);
    }

    // vec3

    inline float length(const vec3 &v)
    {
        return sqrt(
            v.x * v.x +
            v.y * v.y +
            v.z * v.z
            );
    }

    inline vec3 normalize(const vec3 &v)
    {
        float len = length(v);
        float rcp = fabs(len) > 1e-5 ? 1.0f / len : 0.0f;
        return v * rcp;
    }

    inline float dot(const vec3 &v1, const vec3 &v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    inline vec3 cross(const vec3 &v1, const vec3 &v2)
    {
        return vec3(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
    }

    inline float distance(const vec3 &v1, const vec3 &v2)
    {
        return length(v1 - v2);
    }

    static const vec3 UNIT_X(1.0f, 0.0f, 0.0f);
    static const vec3 UNIT_Y(0.0f, 1.0f, 0.0f);
    static const vec3 UNIT_Z(0.0f, 0.0f, 1.0f);
    static const vec3 ZERO(0.0f, 0.0f, 0.0f);

    inline vec3 findOrthogonal(const vec3& v)
    {
        vec3 u = UNIT_X;
        vec3 n = normalize(v);
        if(dot(u, n) >= 0.99f)
            u = UNIT_Y;
        if(dot(u, n) >= 0.99f)
            u = UNIT_Z;
        return u - n * dot(u, n);
    }
}

#endif
