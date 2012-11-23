// simple texture class
// - rlyeh ~~ listening to Twilightning - Painting the blue eyes

#pragma once

#include <map>
#include <string>
#include <vector>

#include "image/pixel/pixel.hpp"
#include "image/image.hpp"

// image = array of pixels[] in RGBA or HSLA format
// texture = strict subset of an image { squared, rgba only }
// so, image > texture

// style: blurry,blocky
// Min is the filtering to use when the texture is smaller than its size on screen
// Mag is the filter to use when it’s larger (magnified)

// check: texture.h (rectangle, cube) from md3/md5/md5b

namespace moon9
{
    class texture : public std::vector<pixel>
    {
        bool delegated;

        void create();
        void destroy();

        public:

        size_t w, h;   // width, height
        size_t iw, ih; // image width, height
        float u0, v0, u1, v1;

        float delay;   // frame delay, when loading an animation

        size_t id; // GL

        texture();
        texture( size_t _w, size_t _h, const pixel &filler = pixel::rgba() );
        explicit texture( const image &pic, bool mirror_w = false, bool mirror_h = false );
        explicit texture( size_t id ); // import id from another texture handler
        ~texture();

        bool load( const image &pic, bool mirror_w = false, bool mirror_h = false );
        bool load( const std::string &filename, bool mirror_w = false, bool mirror_h = false );
        void capture();
        void capture( int left, int bottom );
        //void capture( float scale, size_t layer_mask );
        //void capture( float scale, size_t layer_mask, size_t x, size_t y, size_t w, size_t h );

        void clone( const moon9::texture &that );
        void copy( const moon9::texture &that );
        size_t delegate(); // useful to delegate texture id to another consumer. this avoids texture destruction when ~texture() is called

        void submit(); // const;

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

    template< typename TYPE >
    class texturemap : public std::map< TYPE, texture >
    {
        typename std::map< TYPE, texture >::iterator cursor;

        public:

        texturemap()
        {
            cursor = this->end();
        }

        inline bool find( const TYPE &refkey )
        {
            cursor = this->std::map< TYPE, texture >::find( refkey );
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

        inline texture &insert( const TYPE &refkey )
        {
            return ( this->operator[]( refkey ) = this->operator[]( refkey ) );
        }

        inline texture &insert( const TYPE &refkey, const texture &t )
        {
            return ( ( this->operator[]( refkey ) = this->operator[]( refkey ) ) = t );
        }

        void delegate()
        {
            for( auto &it : *this )
                it.delegate();
        }
    };
}
