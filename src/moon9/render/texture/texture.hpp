// simple texture class
// - rlyeh ~~ listening to Twilightning - Painting the blue eyes

#pragma once

#include <map>
#include <string>
#include <vector>

#include "image/pixel/pixel.hpp"

// image = array of pixels[] in RGBA or HSLA format
// texture = strict subset of an image { squared, rgba only }
// so, image > texture

namespace moon9
{
    class image;

    class texture : public std::vector<pixel>
    {
        void create();
        void destroy();

        public:

        size_t w, h;
        float delay; // frame delay, when loading an animation

        size_t id; // GL

        texture();
        texture( size_t _w, size_t _h, const pixel &filler = pixel::rgba() );
        explicit texture( const image &pic, bool mirror_w = false, bool mirror_h = false );
        ~texture();

        bool load( const image &pic, bool mirror_w = false, bool mirror_h = false );
        bool load( const std::string &filename, bool mirror_w = false, bool mirror_h = false );
        void capture();
        void capture( int left, int bottom );
        //void capture( float scale, size_t layer_mask );
        //void capture( float scale, size_t layer_mask, size_t x, size_t y, size_t w, size_t h );
        void submit(); // const;
        size_t delegate(); // useful to delegate texture id to another consumer. this avoids texture destruction when ~texture()

        inline const size_t size() const { return this->std::vector<pixel>::size(); }
        bool loaded() const { return this->std::vector<pixel>::size() != 0; }

        inline pixel &at( size_t offset ) { return this->std::vector<pixel>::at( offset ); }
        inline pixel &at( size_t x, size_t y ) { return this->std::vector<pixel>::at( x + y * w ); }

        inline const pixel &at( size_t offset ) const { return this->std::vector<pixel>::at( offset ); }
        inline const pixel &at( size_t x, size_t y ) const { return this->std::vector<pixel>::at( x + y * w ); }

        void bind() const;      // apply()
        void unbind() const;    // clear()

        void sprite( float scale = 1.f ) const;

        // debug 2d
        std::vector<unsigned char> rgba_data() const;
        void display( const std::string &title = std::string() ) const;
    };

    class texturemap : public std::map< std::string, texture >
    {
        std::map< std::string, texture >::iterator cursor;

        public:

        texturemap()
        {
            cursor = this->end();
        }

        inline bool find( const std::string &pathfile )
        {
            cursor = this->std::map< std::string, texture >::find( pathfile );
            return cursor != this->end();
        }

        inline const texture &found() const
        {
            return cursor->second;
        }

        inline texture &found()
        {
            return cursor->second;
        }
    };
}
