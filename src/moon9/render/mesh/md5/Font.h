#pragma once

#include <moon9/render/font.hpp>
#include <string>
#include <cassert>

class TTFont
{
    public:

    TTFont ( const std::string &fontPath, int size, int border = 0) : style(1)
    {
        bool ok = moon9::font::make::style( style, fontPath, size );
        assert( ok );
    }
    ~TTFont ()
    {}

    public:

    // Public interface
    void printText (const char *format, ...) const
    {
        moon9::font::batch( format, 0, 0, style );
        moon9::font::submit();
    }
    void printText (int x, int y, const char *format, ...) const
    {
        moon9::font::batch( format, x, y, style );
        moon9::font::submit();
    }

    // Accessors
    int getLineSkip () const { return moon9::font::metrics::height( style ); }

    // Test functions
    void test (int x, int y) {}

  private:

    int style;
};

