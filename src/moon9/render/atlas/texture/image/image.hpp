// simple image class
// - rlyeh ~~ listening to Twilightning - Painting the blue eyes

#pragma once

#include <string>
#include <vector>

#include "pixel/pixel.hpp"

// image = array of pixels[] in RGBA or HSLA format
// texture = strict subset of an image { squared, rgba only }
// so, image > texture

// @todo
// vg {
// image.glow( 1px, pixel::black )
// image.glow( 2px, pixel::red )
// image.mask(white);
// image.mask(white).glow( 2x, pixel::blue ).replace(white,transp) --> L4D2 style
// image.aabb(); -> return aabb for this frame
// }
// composition {
// image.append( img[], align09 = 6 )
// image.crop( columns/rows to remove, align09 = 2, method = fast/carving )
// image.shrink( w, h )
// }
// image.noisex()
// image.perlin()
// image.mipmap()
// image.upload()/unload()
// image.download()/capture()
// image.rotate(deg, bgcolor/transparent?)
// image.to_premultiplied_alpha()/from_mulalpha()
// image.gamma(1.22)
// image.atlas( image[] )/combine(other)
// image.blur()/sharpen()
// image.scale(marquee_scale,content_scale,scale_method)
// @todo: also image1 */*=/+/+= image2

namespace moon9
{
    class image : public std::vector<pixel>
    {
        public:

        size_t w, h;
        float delay; // frame delay, when loading an animation

        image();
        image( size_t _w, size_t _h, const pixel &filler = pixel::hsla() );
        explicit image( const std::string &filename, bool mirror_w = false, bool mirror_h = false );

        bool load( const std::string &filename, bool mirror_w = false, bool mirror_h = false );
        bool save_as_png( const std::string &filename ) const;
        bool save_as_dds( const std::string &filename ) const;
        bool save_as_tga( const std::string &filename ) const;

        inline const size_t size() const { return this->std::vector<pixel>::size(); }
        bool loaded() const { return this->std::vector<pixel>::size() != 0; }

        inline pixel &at( size_t offset ) { return this->std::vector<pixel>::at( offset ); }
        inline pixel &at( size_t x, size_t y ) { return this->std::vector<pixel>::at( x + y * w ); }
        inline pixel &atf( float x01, float y01 ) { return this->at( x01 * (w-1), y01 * (h-1) ); }

        inline const pixel &at( size_t offset ) const { return this->std::vector<pixel>::at( offset ); }
        inline const pixel &at( size_t x, size_t y ) const { return this->std::vector<pixel>::at( x + y * w ); }
        inline const pixel &atf( float x01, float y01 ) const { return this->at( x01 * (w-1), y01 * (h-1) ); }

        image flip_w() const;
        image flip_h() const;
        image rotate_left() const;
        image rotate_right() const;

        image operator *( const pixel &other ) const;
        image operator +( const pixel &other ) const;

        image &operator *=( const pixel &other );
        image &operator +=( const pixel &other );

        image to_rgba() const;
        image to_hsla() const;

        image to_premultiplied_alpha() const;
        image to_straight_alpha() const;

        // debug 2d
        std::vector<unsigned char> rgba_data() const;
        void display( const std::string &title = std::string() ) const;
    };
}
