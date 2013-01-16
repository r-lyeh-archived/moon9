/*
 * Small truetype font class.
 * Based on original code by Mikko Mononen.
 * Using stb_truetype library by Sean Barret.
 * Using utf8 decoding function by Bjoern Hoehrmann.

 * Copyright (c) 2010 Mario 'rlyeh' Rodriguez
 * Copyright (c) 2009 Mikko Mononen <memon@inside.org>
 * Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.

 * Usage:
 * -

 * To do:
 * - remove openGL code
 * - remove hardcoded / * tokens
 * - adjust font texture size dynamically
 * - add rect metrics( sx, sy, interlining, text );
 * - add canvas: rect, anchor, valign, halign, justification, indentation, scroll (move to ui instead?)
 * - add style: underline, strike (by using glLine?)
 * - http://ghostinthecode.posterous.com/better-contour-rendering

 * - rlyeh ~~ listening to Vangelis / Blade Runner Blues
 */

#pragma once

#include <moon9/render/gl.hpp>

#include <cmath>

#include <map>
#include <vector>

#include <stb/stb_truetype.h>


namespace moon9
{
    class stash
    {
    public:

        enum config { debug = true };

    public:

        stash( int cachew, int cacheh );
        ~stash();

        bool add_font( const std::string &path, const std::string &alias );
        bool add_font( const void *ptr, size_t size, const std::string &alias );
        bool add_font( const std::vector<unsigned char> &data, const std::string &alias );

        void render( const float &sx, const float &sy, const float &interlining_pt, const std::string &text );

    private:

        enum { VERT_COUNT = 6 * 128, VERT_STRIDE = sizeof(float) * 4 };

        struct sth_quad
        {
            float x0,y0,s0,t0;
            float x1,y1,s1,t1;
        };

        struct sth_row
        {
            short x,y,h;

            sth_row(const short &_x = 0, const short &_y = 0, const short &_h = 0);
        };

        struct sth_glyph
        {
            unsigned int codepoint;
            short size;
            int x0,y0,x1,y1;
            float xadv,xoff,yoff;
        };

        struct sth_font
        {
            stbtt_fontinfo font;
            std::vector<unsigned char> data;
            std::vector< sth_row > *rows;
            std::map< std::pair<unsigned int /*codepoint*/, short /*size*/>, sth_glyph > glyphs;
            /*const*/ GLuint &tex;
            int tw, th;
            float ascender;
            float descender;
            float lineh;

            sth_font &operator=( const sth_font &t );
            sth_font( /*const*/ GLuint &_tex, std::vector< sth_row > *_rows = 0, int _tw = 0, int _th = 0 );
            sth_glyph* get_glyph(unsigned int codepoint, short isize);
        };

    protected:

        GLuint tex;
        int tw,th;
        float itw,ith;
        std::vector< sth_font > fonts;
        std::vector< sth_row > rows;
        std::map< std::string /*face*/, int > aliases;
        typedef std::map< std::string /*face*/, int >::iterator aliases_it;
        float verts[4*VERT_COUNT];
        int nverts;
        int drawing;

        void draw_text( size_t idx, float size, float x, float y, const char* s, float* dx );
        void dim_text( size_t idx, float size, const char* s, float* minx, float* miny, float* maxx, float* maxy );
        void vmetrics( size_t idx, float size, float* ascender, float* descender, float *lineh );
        void begin_draw();
        void end_draw();
        unsigned int decutf8(unsigned int* state, unsigned int* codep, unsigned int byte);
        bool get_quad(sth_font* fnt, unsigned int codepoint, short isize, float* x, float* y, sth_quad* q);
        int flush_draw();
        float* setv(float* v, float x, float y, float s, float t);

    };
}
