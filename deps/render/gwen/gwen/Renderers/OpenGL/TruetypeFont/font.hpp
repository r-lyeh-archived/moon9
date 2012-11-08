#pragma once

#include <string>

namespace font
{
    struct dims
    {
        float w, h;
    };

    namespace make
    {
        bool style( int id, const std::string &fontfile, float pt ); // bold, color
        dims rect( const std::string &text, int style_id = 1 );
    }

    void color( float r = 127, float g = 127, float b = 127, float a = 255 );
    void batch( const std::string &text, float x = 0, float y = 0, int style_id = 1 );
    void submit();
}
