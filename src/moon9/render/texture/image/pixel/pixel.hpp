/* Simple pixel/color handling classes
 * - rlyeh
 */

/* @todo {
    
    to_rg() // computer vision: r = r/r+g+b, g = g/r+g+b
    munsell 5 hues: red, yellow, green, blue, purple
    hsl 6 hues: red, yellow, green, cyan, blue, purple
    rgy
    http://en.wikipedia.org/wiki/HSL_color_space#Disadvantages

    std::string str() const { return std::string(); } //#ffffffff

} */

#pragma once

//#include "float16.hpp"

/*
    Hue - Think of a color wheel. Around 0 and 255 are reds 85 are greens, 170 are blues. Use anything in between 0-255. Values above and below will be modulus 255.
    Saturation - 0 is completely denatured (grayscale). 255 is fully saturated (full color).
    Lightness - 0 is completely dark (black). 255 is completely light (white). 127 is average lightness.
    alpha - Opacity/Transparency value. 0 is fully transparent. 255 is fully opaque. 127 is 50% transparent.

    enum hue
    {
        hred1 = 0,
        hgreen = 85,
        hblue = 170,
        hred2 = 255,

        hpurple =  ( ( hblue + hred2 ) / 2 ) % 255,
        hcyan =  ( ( hgreen + hblue ) / 2 ) % 255,
        hyellow = ( ( hred1 + hgreen ) / 2 ) % 255
    };

    unsigned char make_hue( float reds, float greens, float blues )
    {
        return (unsigned char)( int( reds * 255 + greens * 85 + blues * 170 ) % 255 );
    }
*/

// valid color components in range [0..1] 
// components can handle higher values (useful for color algebra), but their values will be clamped when displayed

namespace moon9
{
    union pixel
    {
        public:

        // todo: replace float with float8 (microfloat 0.4.4 ; good idea?)
        // typedef float16 component; // half-float
        // typedef unsigned char component;
        typedef float component;

        enum
        { 
            is_ffff = true,   // float32 type?
            is_hhhh = false,  // half-float16 type?
            is_mmmm = false,  // microfloat8 type?
            is_bbbb = false   // byte8 type?
        };

        struct { component r,g,b,a; };
        struct { component h,s,l,a; };

        pixel() : r(0),g(0),b(0),a(0)
        {}

        pixel operator *( const pixel &other ) const;
        pixel operator +( const pixel &other ) const;

        pixel &operator *=( const pixel &other );
        pixel &operator +=( const pixel &other );

        pixel clamp() const;
        pixel to_rgba() const;
        pixel to_hsla() const;
        pixel to_premultiplied_alpha() const;
        pixel to_straight_alpha() const;

        static pixel hsla();
        static pixel hsla( float h, float s, float l, float a = 1.f );
        static pixel rgba();
        static pixel rgba( float r, float g, float b, float a = 1.f );

        protected:

        pixel( float _r, float _g, float _b, float _a ) : r(_r),g(_g),b(_b),a(_a)
        {}
    };
}

using moon9::pixel;
