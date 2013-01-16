#include "gl/gl.hpp"

#include <cassert>

#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>

//#pragma comment( lib, "user32.lib" )
//#pragma comment( lib, "gdi32.lib" )
//#pragma comment( lib, "shell32.lib" )

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <cimg/cimg.h>
#pragma comment(lib,"shell32.lib")

#include "pixel/pixel.hpp"
#include "image.hpp"
#include "image.inl"

namespace
{
    std::string image_load( const std::string &pathFile, bool mirror_w, bool mirror_h, bool as_hsla, bool make_squared, size_t &w, size_t &h, float &delay, std::vector<pixel> &image )
    {
        if( !pathFile.size() )
            return "Error: given filename is empty";

        //std::cout << "Loading texture: " << pathFile << std::endl;

        // decode
        int imageWidth = 0, imageHeight = 0, imageBpp;
        std::vector<stbi_uc> temp;

        {
            std::vector<std::string> pathFileExt = moon9::tokenize( pathFile, "|" );
            int subframe = ( pathFileExt.size() > 1 ? moon9::as<int>(pathFileExt[1]) : 0 );

            stbi_gif_subframe_delay = 0.f;
            stbi_gif_subframe_selector = subframe;
            stbi_uc *imageuc = stbi_load( pathFileExt[0].c_str(), &imageWidth, &imageHeight, &imageBpp, 4 );

            if( !imageuc )
            {
                // assert( false );
                // yellow/black texture instead?
                return moon9::custom( "Error: cant find/decode texture '\1'\n", pathFile );
            }

            if( make_squared )
            {
                bool is_power_of_two_w = imageWidth && !(imageWidth & (imageWidth - 1));
                bool is_power_of_two_h = imageHeight && !(imageHeight & (imageHeight - 1));

                if( is_power_of_two_w && is_power_of_two_h )
                {
                    temp.resize( imageWidth * imageHeight * 4 );
                    memcpy( &temp[0], imageuc, imageWidth * imageHeight * 4 );
                }
                else
                {
                    int nw = 1, nh = 1, atx, aty;
                    while( nw < imageWidth ) nw <<= 1;
                    while( nh < imageHeight ) nh <<= 1;

                    // squared as well, cos we want them pixel perfect
                    if( nh > nw ) nw = nh; else nh = nw;

                    temp.resize( nw * nh * 4, 0 );
                    atx = (nw - imageWidth) / 2;
                    aty = (nh - imageHeight) / 2;

                    //std::cout << custom( "\1x\2 -> \3x\4 @(\5,\6)\n", imageWidth, imageHeight, nw, nh, atx, aty);

                    for( int y = 0; y < imageHeight; ++y )
                        memcpy( &((stbi_uc*)&temp[0])[ ((atx)+(aty+y)*nw)*4 ], &imageuc[ (y*imageWidth) * 4 ], imageWidth * 4 );

                    imageWidth = nw;
                    imageHeight = nh;
                }
            }
            else
            {
                    temp.resize( imageWidth * imageHeight * 4 );
                    memcpy( &temp[0], imageuc, imageWidth * imageHeight * 4 );
            }

            stbi_image_free( imageuc );
        }

        if( mirror_w && !mirror_h )
        {
            std::vector<stbi_uc> mirrored( temp.size() );

            for( int c = 0, y = 0; y < imageHeight; ++y )
            for( int x = imageWidth - 1; x >= 0; --x )
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 0 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 1 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 2 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 3 ];

            temp = mirrored;
        }
        else
        if( mirror_h && !mirror_w )
        {
            std::vector<stbi_uc> mirrored( temp.size() );

            for( int c = 0, y = imageHeight - 1; y >= 0; --y )
            for( int x = 0; x < imageWidth; ++x )
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 0 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 1 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 2 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 3 ];

            temp = mirrored;
        }
        else
        if( mirror_w && mirror_h )
        {
            std::vector<stbi_uc> mirrored( temp.size() );

            for( int c = 0, y = imageHeight - 1; y >= 0; --y )
            for( int x = imageWidth - 1; x >= 0; --x )
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 0 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 1 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 2 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 3 ];

            temp = mirrored;

            std::swap( imageWidth, imageHeight );
        }

        image.resize( imageWidth * imageHeight );

        if( as_hsla )
            for( size_t i = 0, e = image.size(); i < e; ++i )
                image.at(i) = pixel::rgba( temp[ i * 4 + 0 ] / 255.f, temp[ i * 4 + 1 ] / 255.f, temp[ i * 4 + 2 ] / 255.f, temp[ i * 4 + 3 ] / 255.f ).to_hsla();
        else
            for( size_t i = 0, e = image.size(); i < e; ++i )
                image.at(i) = pixel::rgba( temp[ i * 4 + 0 ] / 255.f, temp[ i * 4 + 1 ] / 255.f, temp[ i * 4 + 2 ] / 255.f, temp[ i * 4 + 3 ] / 255.f );

        w = imageWidth;
        h = imageHeight;
        delay = stbi_gif_subframe_delay;

        return std::string();
    }

    void setup_filtering( bool high_quality )
    {
    #if 0

        // Nearest Filtered Texture
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(...)

        // Linear Filtered Texture
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(...)

        // MipMapped Texture
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(...)

    #endif

        /* blurry/blocky

        ref: http://gregs-blog.com/2008/01/17/opengl-texture-filter-parameters-explained/
        MAG_FILTER/MIN_FILTER                 Bilinear Filtering          Mipmapping
                                                Near   Far
        GL_NEAREST / GL_NEAREST_MIPMAP_NEAREST  Off    Off                Standard
        GL_NEAREST / GL_LINEAR_MIPMAP_NEAREST   Off    On                 Standard (* chars)
        GL_NEAREST / GL_NEAREST_MIPMAP_LINEAR   Off    Off                Use trilinear filtering
        GL_NEAREST / GL_LINEAR_MIPMAP_LINEAR    Off    On                 Use trilinear filtering (* chars)
        GL_NEAREST / GL_NEAREST                 Off    Off                None
        GL_NEAREST / GL_LINEAR                  Off    On                 None (* scene)
        GL_LINEAR / GL_NEAREST_MIPMAP_NEAREST   On     Off                Standard
        GL_LINEAR / GL_LINEAR_MIPMAP_NEAREST    On     On                 Standard
        GL_LINEAR / GL_NEAREST_MIPMAP_LINEAR    On     Off                Use trilinear filtering
        GL_LINEAR / GL_LINEAR_MIPMAP_LINEAR     On     On                 Use trilinear filtering
        GL_LINEAR / GL_NEAREST                  On     Off                None
        GL_LINEAR / GL_LINEAR                   On     On                 None

        (*) intended for prjx

        @todo
        enum { near_blurry, near_blocky }
        enum { far_blurry, far_blocky }
        enum { mipmap_none, mipmap_std, mipmap_trilinear };
        mask -> near_blocky | far_blurry | mipmap_std

        */

        if( high_quality )
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
}

namespace moon9
{

image::image() : std::vector<pixel>(), w(0), h(0), delay(0.f)
{}

image::image( size_t _w, size_t _h, const pixel &filler ) : std::vector<pixel>(_w*_h,filler), w(_w), h(_h), delay(0.f)
{}

image::image( const std::string &pathFile, bool mirror_w, bool mirror_h ) : std::vector<pixel>(), w(0), h(0), delay(0.f)
{
    load( pathFile, mirror_w, mirror_h );
}

bool image::load( const std::string &pathFile, bool mirror_w, bool mirror_h )
{
    std::string error = image_load( pathFile, mirror_w, mirror_h, true, false, w, h, delay, *this );

    if( error.size() )
    {
        std::cerr << error << std::endl;
        return false;
    }

    return true;
}

bool image::save_as_dds( const std::string &filename ) const
{
    const image &pic = *this;

    if( !pic.size() )
        return false;

    if( w * h <= 0 )
        return false;

    /*
    if( filename.size() < 5 )
        return false;

    if( filename.lowercase().ends_with(".png") )
        stbi_write_png( filename.c_str(), w, h, 3, &pixels[0], w * 4 );
    else
    if( filename.lowercase().ends_with(".bmp") )
        stbi_write_bmp( filename.c_str(), w, h, 3, &pixels[0] );
    else
    if( filename.lowercase().ends_with(".tga") )
        stbi_write_tga( filename.c_str(), w, h, 3, &pixels[0] );
    else
        return false;

    return true;
    */

    std::vector<unsigned char> pixels = rgba_data();
    return ( stbi_write_dds( filename.c_str(), w, h, 4, &pixels[0] ) != 0 );
}

bool image::save_as_png( const std::string &filename ) const
{
    const image &pic = *this;

    if( !pic.size() )
        return false;

    if( w * h <= 0 )
        return false;

    std::vector<unsigned char> pixels = rgba_data();
    return ( stbi_write_png( filename.c_str(), w, h, 4, &pixels[0], w * 4 ) != 0 );
}

bool image::save_as_tga( const std::string &filename ) const
{
    const image &pic = *this;

    if( !pic.size() )
        return false;

    if( w * h <= 0 )
        return false;

    std::vector<unsigned char> pixels = rgba_data();
    return ( stbi_write_tga( filename.c_str(), w, h, 4, &pixels[0] ) != 0 );
}

image image::flip_w() const
{
    const image &pic = *this;

    image nimg( w, h );
    nimg.resize(0);
    nimg.delay = delay;

    for( size_t y = 0; y < h; ++y )
        for( int x = w - 1 ; x >= 0; --x )
            nimg.push_back( at(x,y) );

    return nimg;
}

image image::flip_h() const
{
    const image &pic = *this;

    image nimg( w, h );
    nimg.resize(0);
    nimg.delay = delay;

    for( int y = h-1; y >= 0; --y )
        for( size_t x = 0; x < w; ++x )
            nimg.push_back( at(x,y) );

    return nimg;
}

image image::rotate_left() const
{
    const image &pic = *this;

    image nimg( h, w );
    nimg.resize(0);
    nimg.delay = delay;

    for( int x = w - 1; x >= 0; --x )
        for( size_t y = 0; y < h; ++y )
            nimg.push_back( at(x,y) );

    return nimg;
}

image image::rotate_right() const
{
    const image &pic = *this;

    image nimg( h, w );
    nimg.resize(0);
    nimg.delay = delay;

    for( size_t x = 0; x < w; ++x )
        for( int y = h - 1; y >= 0; --y )
            nimg.push_back( at(x,y) );

    return nimg;
}

image image::operator *( const pixel &other ) const
{
    image that = *this;
    that.delay = delay;

    for( image::iterator it = that.begin(), end = that.end(); it != end; ++it )
        (*it) *= other;

    return that;
}

image image::operator +( const pixel &other ) const
{
    image that = *this;
    that.delay = delay;

    for( image::iterator it = that.begin(), end = that.end(); it != end; ++it )
        (*it) += other;

    return that;
}

image &image::operator *=( const pixel &other )
{
    for( image::iterator it = this->begin(), end = this->end(); it != end; ++it )
        (*it) *= other;

    return *this;
}

image &image::operator +=( const pixel &other )
{
    for( image::iterator it = this->begin(), end = this->end(); it != end; ++it )
        (*it) += other;

    return *this;
}

image image::to_hsla() const
{
    image pic( w, h );
    pic.resize( 0 );
    pic.delay = delay;

    for( image::const_iterator it = this->begin(), end = this->end(); it != end; ++it )
        pic.push_back( it->to_hsla() );

    return pic;
}

image image::to_rgba() const
{
    image pic( w, h );
    pic.resize( 0 );
    pic.delay = delay;

    for( image::const_iterator it = this->begin(), end = this->end(); it != end; ++it )
        pic.push_back( it->to_rgba() );

    return pic;
}

image image::to_premultiplied_alpha() const
{
    image pic( w, h );
    pic.resize( 0 );
    pic.delay = delay;

    for( image::const_iterator it = this->begin(), end = this->end(); it != end; ++it )
        pic.push_back( it->to_premultiplied_alpha() );

    return pic;
}

image image::to_straight_alpha() const
{
    image pic( w, h );
    pic.resize( 0 );
    pic.delay = delay;

    for( image::const_iterator it = this->begin(), end = this->end(); it != end; ++it )
        pic.push_back( it->to_straight_alpha() );

    return pic;
}


std::vector<unsigned char> image::rgba_data() const
{
    std::vector<unsigned char> pixels( w * h * 4 );
    pixels.resize(0);
    for( image::const_iterator it = this->begin(), end = this->end(); it != end; ++it )
    {
        pixel p = it->clamp().to_rgba();

        pixels.push_back( (unsigned char)(p.r * 255.f) );
        pixels.push_back( (unsigned char)(p.g * 255.f) );
        pixels.push_back( (unsigned char)(p.b * 255.f) );
        pixels.push_back( (unsigned char)(p.a * 255.f) );
    }
    return pixels;
}

void image::display( const std::string &title ) const
{
    const image &pic = *this;

    cimg_library::CImg<unsigned char> ctexture( pic.w, pic.h, 1, 4, 0 );

    for( size_t y = 0; y < pic.h; ++y )
        for( size_t x = 0; x < pic.w; ++x )
        {
            pixel pix = pic.at( x, y ).clamp().to_rgba();

            ctexture( x, y, 0 ) = (unsigned char)(pix.r * 255.f);
            ctexture( x, y, 1 ) = (unsigned char)(pix.g * 255.f);
            ctexture( x, y, 2 ) = (unsigned char)(pix.b * 255.f);
            ctexture( x, y, 3 ) = (unsigned char)(pix.a * 255.f);
        }

#if 1
    ctexture
//        .deriche(0.5)
        .display( title.size() ? title.c_str() : custom("[\1] moon9::image", this).c_str() );
#else
    cimg_library::CImgDisplay disp;
    disp.resize( pic.w, pic.h );

    ctexture.display( disp );
    while(!disp.is_closed());
#endif
}

} // namespace moon9
