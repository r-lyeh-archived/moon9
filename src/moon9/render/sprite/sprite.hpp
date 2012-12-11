#pragma once

#include <moon9/render/texture.hpp>

namespace moon9
{
    bool is_error_texture( const moon9::texture *t );

    moon9::texture &get_error_texture();

    moon9::texture &get_texture( const std::string &texture_name );

    void texturedquad( bool mirror_w, bool mirror_h, float scale_w = 1.f, float scale_h = 1.f, float texture_tiling = 1.f );

    void sprite( const moon9::texture &texture, bool mirror_w = false, bool mirror_h = false, float texture_scale_w = 1.f, float texture_scale_h = 1.f, float texture_tiling = 1.f );
    void spritebox( const moon9::texture &texture, bool mirror_w = false, bool mirror_h = false, float texture_scale_w = 1.f, float texture_scale_h = 1.f, float texture_tiling = 1.f );
/*
    void sprite( const std::string &texture_name, bool mirror_w = false, bool mirror_h = false, float texture_scale_w = 1.f, float texture_scale_h = 1.f, float texture_tiling = 1.f );
    void spritebox( const std::string &texture_name, bool mirror_w = false, bool mirror_h = false, float texture_scale_w = 1.f, float texture_scale_h = 1.f, float texture_tiling = 1.f );
    */
}

//#include <moon9/play/scene.hpp>
//tree<sprite> spritetree;

#include <moon9/play/pod.hpp>

namespace moon9
{
    struct sprite_ // sprite!
    {
        // material, bucket
        struct texture
        {
            int0 id;
            int0 unit;
            float0 u0,v0;
            float1 u1,v1;
            float1 w,h;
            float1 iw,ih;
            bool0 enabled;
        } diffuse, normal, opacity, specular, glow;

        struct {   float0 x,y; } position;
        struct {   float1 x,y; } face;
        struct {   float1 x,y; } scale;
        struct {   float1 x,y; } tile;
        struct {    bool0 x,y; } mirror;
        struct {   float0 x,y; } anchor; // defaults to center (-1,1: left-top; 0,0 = center-middle; 1,-1 = right-bottom)
        struct {   float0 x,y,angle; } pivot;

        struct {   float1 x,y; } size;

        // transform, transformation
        // link, unlink
        // sphere, aabb, obb
    };

    void render( sprite_ &spr );
    void render2d( sprite_ &spr, float scale = 1 );

    // sprite3d { dims }
}

