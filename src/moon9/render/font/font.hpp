// Simple OpenGL Truetype fonts
// - rlyeh

#pragma once

#include <string>

namespace moon9
{
    namespace font
    {
        struct style
        {
            int faceid;
            float pt, scale;
        };

        struct rect
        {
            float w, h;
/*
            explicit rect( const std::string  &utf8,  int style_id = 1 );
            explicit rect( const std::wstring &utf16, int style_id = 1 );
*/
        };

        struct dims
        {
            float w, h;
        };

        namespace make
        {
            bool style( int id, const std::string &fontfile, float pt ); // bold, color
            dims rect( const std::string  &utf8,  int style_id = 1 );
            dims rect( const std::wstring &utf16, int style_id = 1 );
        }

        namespace metrics
        {
             int height( int style_id = 1 );
        }

        void clip( size_t x, size_t y, size_t w, size_t h );
        void color( unsigned rgba );
        void color( float r /*= 127*/, float g /*= 127*/, float b /*= 127*/, float a = 255 );
        void batch( const std::string  &utf8,  float x = 0, float y = 0, int style_id = 1 );
        void batch( const std::wstring &utf16, float x = 0, float y = 0, int style_id = 1 );
        void submit();
    }
}
