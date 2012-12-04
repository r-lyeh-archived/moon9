/* Simple pixel/color class
 * - rlyeh

 * Based on code from: http://www.geekymonkey.com/Programming/CSharp/RGB2HSL_HSL2RGB.htm
 */

#include "pixel.hpp"

namespace
{
    void hsl2rgb( const float *hsl, float *rgb );
    void rgb2hsl( const float *rgb, float *hsl );
}

namespace moon9
{

pixel pixel::operator *( const pixel &other ) const
{
    return pixel( h * other.h, s * other.s, l * other.l, a * other.a );
}

pixel pixel::operator +( const pixel &other ) const
{
    return pixel( h + other.h, s + other.s, l + other.l, a + other.a );
}

pixel pixel::operator -( const pixel &other ) const
{
    return pixel( h - other.h, s - other.s, l - other.l, a - other.a );
}

pixel &pixel::operator *=( const pixel &other )
{
    *this = operator*( other );
    return *this;
}

pixel &pixel::operator +=( const pixel &other )
{
    *this = operator+( other );
    return *this;
}

pixel &pixel::operator -=( const pixel &other )
{
    *this = operator-( other );
    return *this;
}


pixel pixel::hsla()
{
    return pixel( 0, 0, 0, 0 );
}

pixel pixel::hsla( float _h, float _s, float _l, float _a )
{
    return pixel( _h, _s, _l, _a );
}

pixel pixel::rgba()
{
    return pixel( 0, 0, 0, 0 );
}

pixel pixel::rgba( float _r, float _g, float _b, float _a )
{
    return pixel( _r, _g, _b, _a );
}

pixel pixel::clamp() const
{
    pixel result;
    result.r = ( r >= 1.f ? 1.f : ( r <= 0.f ? 0.f : r ) );
    result.g = ( g >= 1.f ? 1.f : ( g <= 0.f ? 0.f : g ) );
    result.b = ( b >= 1.f ? 1.f : ( b <= 0.f ? 0.f : b ) );
    result.a = ( a >= 1.f ? 1.f : ( a <= 0.f ? 0.f : a ) );
    return result;
}

pixel pixel::to_rgba() const
{
    const float hsl[3] = { h, s, l };
    float rgb[3];
    hsl2rgb( hsl, rgb );
    return pixel( rgb[0], rgb[1], rgb[2], a );
}

pixel pixel::to_hsla() const
{
    float rgb[3] = { r, g, b };
    float hsl[3];
    rgb2hsl( rgb, hsl );
    return pixel( hsl[0], hsl[1], hsl[2], a );
}

pixel pixel::to_premultiplied_alpha() const
{
    return pixel( r * a, g * a, b * a, a );
}

pixel pixel::to_straight_alpha() const
{
    return pixel( r / a, g / a, b / a, a );
}

} // namespace moon9

namespace
{
    // Given H,S,L in range of 0-1
    // Returns a Color (RGB struct) in range of 0-1

    void hsl2rgb( const float *hsl, float *rgb )
    {
        const float &h = hsl[0];
        const float &s = hsl[1];
        const float &l = hsl[2];

        float &r = rgb[0];
        float &g = rgb[1];
        float &b = rgb[2];

        float v;

        r = l;   // default to gray
        g = l;
        b = l;
        v = (l <= 0.5f) ? (l * (1.f + s)) : (l + s - l * s);

        if (v > 0)
        {
              float m = l + l - v;
              float sv = (v - m ) / v;
              float h6 = h * 6.f;
              int sextant = (int)h6;
              float fract = h6 - sextant;
              float vsf = v * sv * fract;
              float mid1 = m + vsf;
              float mid2 = v - vsf;

              switch (sextant)
              {
                    default:
                    case 0:
                          r = v;
                          g = mid1;
                          b = m;
                          break;
                    case 1:
                          r = mid2;
                          g = v;
                          b = m;
                          break;
                    case 2:
                          r = m;
                          g = v;
                          b = mid1;
                          break;
                    case 3:
                          r = m;
                          g = mid2;
                          b = v;
                          break;
                    case 4:
                          r = mid1;
                          g = m;
                          b = v;
                          break;
                    case 5:
                          r = v;
                          g = m;
                          b = mid2;
                          break;
              }
        }
    }

    // Given a Color (RGB Struct) in range of 0-1
    // Return H,S,L in range of 0-1
    void rgb2hsl( const float *rgb, float *hsl )
    {
        const float &r = rgb[0];
        const float &g = rgb[1];
        const float &b = rgb[2];
        float v;
        float m;
        float vm;
        float r2, g2, b2;

        float &h = hsl[0];
        float &s = hsl[1];
        float &l = hsl[2];

        h = 0; // default to black
        s = 0;
        l = 0;
        v = ( r > g ? r : g );
        v = ( v > b ? v : b );
        m = ( r < g ? r : g );
        m = ( m < b ? m : b );
        l = (m + v) / 2.f;
        if (l <= 0.f)
        {
              return;
        }
        vm = v - m;
        s = vm;
        if (s > 0.f)
        {
              s /= (l <= 0.5f) ? (v + m ) : (2.f - v - m) ;
        }
        else
        {
              return;
        }
        r2 = (v - r) / vm;
        g2 = (v - g) / vm;
        b2 = (v - b) / vm;
        if (r == v)
        {
              h = (g == m ? 5.f + b2 : 1.f - g2);
        }
        else if (g == v)
        {
              h = (b == m ? 1.f + r2 : 3.f - b2);
        }
        else
        {
              h = (r == m ? 3.f + g2 : 5.f - r2);
        }
        h /= 6.f;
    }
}
