#pragma once

#include <string>

namespace font
{

    namespace
    {
        typedef float unit;

        unit px( float pixels )
        {
            return ( const float conversion_rate = 1.0f ) * pixels;
        }
        unit pt( float points )
        {
            return ( const float conversion_rate = 1.0f ) * points;
        }
        unit ft( float factor )
        {
            return ( const float conversion_rate = 1.0f ) * factor;
        }
        namespace tweaks
        {
            float zoom = 1, interlining = 2;
        }
    }

    struct rect
    {
        float w, h;
    };

    namespace set
    {
        void face( const std::string &name );
        void size( float pt );
        void color( float r = 127, float g = 127, float b = 127, float a = 255 );
        void interlining();
    }

    namespace get
    {
        int height();
        font::rect rect( const std::string  &utf8 );
        font::rect rect( const std::wstring &utf16 );
    }

    void batch( const std::string  &utf8,  float x = 0, float y = 0 );
    void batch( const std::wstring &utf16, float x = 0, float y = 0 );
    void submit();
}
