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

#include <moon9/render/stash.hpp>

#include <moon9/io/file.hpp>

#include <deque>
#include <string>

namespace moon9
{
    namespace // taken from <moon9/render/image.cpp>
    {
        template<typename T>
        T as( const std::string &text )
        {
            T t;
            std::stringstream ss( text );
            return ss >> t ? t : (T)( as<bool>(text) );
        }
        template<>
        bool as( const std::string &text )
        {
            return text.size() > 0 && text != "0" && text != "false";
        }
        std::deque< std::string > split( const std::string &text, const std::string& delimiters ) // tokenize_incl_separators
        {
            std::deque< std::string > tokens;
            std::string str;

            for( size_t i = 0; i < text.size(); ++i )
            {
                char c = text.at(i);

                if( delimiters.find_first_of( c ) == std::string::npos )
                {
                    str += c;
                }
                else
                {
                    if( str.size() )
                        tokens.push_back( str );

                    tokens.push_back( std::string() + c );

                    str = "";
                }
            }

            tokens.push_back( str );
            return tokens;
        }
    }


    stash::sth_row::sth_row(const short &_x, const short &_y, const short &_h)
        : x(_x),y(_y),h(_h)
    {}

    // stupid msvc 2008 needs this {
    stash::sth_font &stash::sth_font::operator=( const stash::sth_font &t )
    {
        font = t.font;
        data = t.data;
        rows = t.rows;
        glyphs = t.glyphs;
        tex = t.tex;
        tw = t.tw;
        th = t.th;
        ascender = t.ascender;
        descender = t.descender;
        lineh = t.lineh;
        return *this;
    }
    // }

    stash::sth_font::sth_font( /*const*/ GLuint &_tex, std::vector< sth_row > *_rows, int _tw, int _th )
        : tex(_tex), rows(_rows),tw(_tw),th(_th),ascender(0),descender(0),lineh(0)
    {}

    stash::sth_glyph* stash::sth_font::get_glyph(unsigned int codepoint, short isize)
    {
        // Find code point and size.
        std::pair<unsigned int,int> key(codepoint, isize);

        if (glyphs.find( key ) != glyphs.end() )
            return &glyphs.find( key )->second;

        // Could not find glyph, create it.
        int advance,lsb,x0,y0,x1,y1;
        int g = stbtt_FindGlyphIndex(&font, codepoint);
        float scale = stbtt_ScaleForPixelHeight(&font, isize/10.0f);
        stbtt_GetGlyphHMetrics(&font, g, &advance, &lsb);
        stbtt_GetGlyphBitmapBox(&font, g, scale,scale, &x0,&y0,&x1,&y1);

        int gw = x1-x0;
        int gh = y1-y0;

        // Find row where the glyph can be fit (vertically first, then horizontally)
        sth_row* br = 0;
        int rh = (gh+7) & ~7;
        for (size_t i = 0; i < rows->size() && !br; ++i)
        {
            if (rows->at(i).h == rh && rows->at(i).x+gw+1 <= tw)
                br = &rows->at(i);
        }

        // If no row found, add new.
        if (br == NULL)
        {
            short py = 0;
            // Check that there is enough space.
            if (rows->size())
            {
                py = rows->back().y + rows->back().h+1;
                if (py+rh > th)
                    return 0;
            }
            // Init and add row
            rows->push_back( sth_row(0,py,rh) );
            br = &rows->back();
        }

        // Init glyph.
        sth_glyph glyph;

        glyph.codepoint = codepoint;
        glyph.size = isize;
        glyph.x0 = br->x;
        glyph.y0 = br->y;
        glyph.x1 = glyph.x0+gw;
        glyph.y1 = glyph.y0+gh;
        glyph.xadv = scale * advance;
        glyph.xoff = (float)x0;
        glyph.yoff = (float)y0;

//                glyphs[ key ] = glyphs[ key ];
        glyphs[ key ] = glyph;

        // Advance row location.
        br->x += gw+1;

        // Rasterize
        std::vector< unsigned char > bmp(gw*gh);
        if( bmp.size() )
        {
            stbtt_MakeGlyphBitmap(&font, &bmp[0], gw,gh,gw, scale,scale, g);

            // Update texture
            glBindTexture(GL_TEXTURE_2D, tex);
            glPixelStorei(GL_UNPACK_ALIGNMENT,1);
            glTexSubImage2D(GL_TEXTURE_2D, 0, glyph.x0,glyph.y0, gw,gh, GL_ALPHA,GL_UNSIGNED_BYTE,&bmp[0]);
        }

        return &glyphs[ key ];
    }

    stash::stash( int cachew, int cacheh ) : tex(0),tw(cachew), th(cacheh), itw(1.0f/cachew), ith(1.0f/cacheh), nverts(0), drawing(0)
    {}

    stash::~stash()
    {
        if (tex)
            glDeleteTextures(1, &tex);
    }

    bool stash::add_font( const std::string &path, const std::string &alias )
    {
        std::vector<unsigned char> data;
        std::string content = moon9::file(path).read();
        data.resize( content.size() );
        std::copy( content.begin(), content.end(), data.begin() );
        return add_font( data, alias );
    }

    bool stash::add_font( const void *ptr, size_t size, const std::string &alias )
    {
        std::vector<unsigned char> data;
        data.resize( size );
        memcpy( &data[0], ptr, size );
        return add_font( data, alias );
    }

    bool stash::add_font( const std::vector<unsigned char> &data, const std::string &alias )
    {
        if( !tex )
        {
            // Create texture for the cache.
            glGenTextures(1, &tex);
            if( !tex ) return false;
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, tw,th, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        sth_font fnt(tex, &rows, tw, th);

        // Init stb_truetype
        if( data.size() && stbtt_InitFont(&fnt.font, &data[0], 0) )
        {
            // Read in the font data.
            fnt.data = data;

            // Store normalized line height. The real line height is got
            // by multiplying the lineh by font size.
            int ascent, descent, lineGap;
            stbtt_GetFontVMetrics(&fnt.font, &ascent, &descent, &lineGap);
            int fh = ascent - descent;
            fnt.ascender = (float)ascent / (float)fh;
            fnt.descender = (float)descent / (float)fh;
            fnt.lineh = (float)(fh + lineGap) / (float)fh;

            fonts.push_back( fnt );

            aliases[ alias ] = fonts.size() - 1;

            // refresh data pointers because of std::vector reordering
            for( size_t i = 0; i < fonts.size(); ++i )
                fonts[i].font.data = &fonts[i].data[0];

            return true;
        }

        return false;
    }

    void stash::render( const float &sx, const float &sy, const float &interlining_pt, const std::string &text )
    {
            float dx = sx, dy = sy, lh;

            vmetrics( 0, interlining_pt, NULL,NULL, &lh );
            lh *= 1.2f; // interleave

            glRasterPos2f( sx, sy );

            std::deque<std::string> tokens = split( text, "/*{|}\r\n" );

            begin_draw();

            bool italic = false, bold = false;
            int face = 0;
            float size = 12.f;

            for( size_t i = 0; i < tokens.size(); ++i )
            {
                std::string &token = tokens[i];

                if( token == "/" )
                {
                    if( italic ^= true )
                        face = 1;
                    else
                        face = 0;
                    continue;
                }
                if( token == "*" )
                {
                    if( bold ^= true )
                        face = 2;
                    else
                        face = 0;
                    continue;
                }
                if( token == "\r" || token == "\n" )
                {
                    dx  = sx;
                    dy -= lh;
                    continue;
                }
                if( token == "{" || token == "|" )
                {
                    ++i;

                    if( i >= tokens.size() )
                        continue;

                    aliases_it it = aliases.find( tokens[i] );
                    if( it != aliases.end() )
                    {
                        face = it->second;
                        continue;
                    }

                    if( as<float>(tokens[ i ]) > 0.f )
                        size = as<float>(tokens[ i ]);

                    continue;
                }
                if( token == "}" )
                {
                    continue;
                }

                draw_text( face, size, dx,dy, token.c_str(), &dx );
            }

            end_draw();
    }

    void stash::draw_text( size_t idx, float size, float x, float y, const char* s, float* dx )
    {
        if( tex && idx < fonts.size() && fonts[idx].data.size() )
        {
            short isize = (short)(size*10.0f);
            sth_quad q;

            for ( unsigned int codepoint, state = 0; *s; ++s)
            {
                if (decutf8(&state, &codepoint, *(unsigned char*)s)) continue;

                if (nverts+6 >= VERT_COUNT)
                    flush_draw();

                if (!get_quad(&fonts[idx], codepoint, isize, &x, &y, &q)) continue;

                float *v = &verts[nverts*4];

                v = setv(v, q.x0, q.y0, q.s0, q.t0);
                v = setv(v, q.x1, q.y0, q.s1, q.t0);
                v = setv(v, q.x1, q.y1, q.s1, q.t1);

                v = setv(v, q.x0, q.y0, q.s0, q.t0);
                v = setv(v, q.x1, q.y1, q.s1, q.t1);
                v = setv(v, q.x0, q.y1, q.s0, q.t1);

                nverts += 6;
            }

            if (dx) *dx = x;
        }
    }

    void stash::dim_text( size_t idx, float size, const char* s, float* minx, float* miny, float* maxx, float* maxy )
    {
        if( tex && idx < fonts.size() && fonts[idx].data.size() )
        {
            short isize = (short)(size*10.0f);
            float x = 0, y = 0;
            sth_quad q;

            *minx = *maxx = x;
            *miny = *maxy = y;

            for (unsigned int codepoint, state = 0; *s; ++s)
            {
                if (decutf8(&state, &codepoint, *(unsigned char*)s)) continue;
                if (!get_quad(&fonts[idx], codepoint, isize, &x, &y, &q)) continue;
                if (q.x0 < *minx) *minx = q.x0;
                if (q.x1 > *maxx) *maxx = q.x1;
                if (q.y1 < *miny) *miny = q.y1;
                if (q.y0 > *maxy) *maxy = q.y0;
            }
        }
    }

    void stash::vmetrics( size_t idx, float size, float* ascender, float* descender, float *lineh )
    {
        if( tex && idx < fonts.size() && fonts[idx].data.size() )
        {
            if( descender ) *descender = fonts[idx].descender * size;
            if(  ascender )  *ascender =  fonts[idx].ascender * size;
            if(     lineh )     *lineh =     fonts[idx].lineh * size;
        }
    }

    void stash::begin_draw()
    {
        if( drawing )
            flush_draw();

        drawing = 1;
    }

    void stash::end_draw()
    {
        if( !drawing )
            return;

        // Debug dump.
        if( debug )
        {
            if( nverts+6 < VERT_COUNT )
            {
                float x = 0, y = 0, *v = &verts[ nverts*4 ];

                v = setv(v, x, y, 0, 0);
                v = setv(v, x+tw, y, 1, 0);
                v = setv(v, x+tw, y+th, 1, 1);

                v = setv(v, x, y, 0, 0);
                v = setv(v, x+tw, y+th, 1, 1);
                v = setv(v, x, y+th, 0, 1);

                nverts += 6;
            }
        }

        flush_draw();
        drawing = 0;
    }

    unsigned int stash::decutf8(unsigned int* state, unsigned int* codep, unsigned int byte)
    {
        // Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
        // See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

        const int UTF8_REJECT = 1;
        const int UTF8_ACCEPT = 0;

        static const unsigned char utf8d[] = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
            7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
            8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
            0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
            0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
            0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
            1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
            1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
            1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1  // s7..s8
        };
        unsigned int type = utf8d[byte];
        *codep = (*state != UTF8_ACCEPT) ?
            (byte & 0x3fu) | (*codep << 6) :
        (0xff >> type) & (byte);
        *state = utf8d[256 + *state*16 + type];
        return *state;
    }

    bool stash::get_quad(sth_font* fnt, unsigned int codepoint, short isize, float* x, float* y, sth_quad* q)
    {
        sth_glyph* glyph = fnt->get_glyph(codepoint, isize);
        if (!glyph) return false;

        float rx = /*int*/(floorf(*x + glyph->xoff)),
              ry = /*int*/(floorf(*y - glyph->yoff));

        q->x0 = rx;
        q->y0 = ry;
        q->x1 = rx + glyph->x1 - glyph->x0;
        q->y1 = ry - glyph->y1 + glyph->y0;

        q->s0 = (glyph->x0) * itw;
        q->t0 = (glyph->y0) * ith;
        q->s1 = (glyph->x1) * itw;
        q->t1 = (glyph->y1) * ith;

        *x += glyph->xadv;

        return true;
    }

    int stash::flush_draw()
    {
        if (nverts != 0)
        {
            glBindTexture(GL_TEXTURE_2D, tex);
            glEnable(GL_TEXTURE_2D);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glVertexPointer(2, GL_FLOAT, VERT_STRIDE, verts);
            glTexCoordPointer(2, GL_FLOAT, VERT_STRIDE, verts+2);
            glDrawArrays(GL_TRIANGLES, 0, nverts);
            glDisable(GL_TEXTURE_2D);

            glDisable(GL_BLEND);

            if( debug )
            {
                glColor4ub( 255, 0, 255, 255 );
                glDrawArrays(GL_LINES, 0, nverts);
                glColor4ub( 255, 255, 0, 255 );
            }

            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            nverts = 0;
        }

        return tex;
    }

    float* stash::setv(float* v, float x, float y, float s, float t)
    {
        v[0] = x;
        v[1] = y;
        v[2] = s;
        v[3] = t;
        return v+4;
    }
}
