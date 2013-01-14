#include <GL/glew.h>

#include <assert.h>

#include <string>
#include <vector>

#include "fontstash/fontstash.hpp"
#include "fontstash/fontstash.cpp"

#include "system/system.hpp"
#include "system/system.cpp"

#include "utfcpp/utf8.h"

#include "font.hpp"

#define single(...) do { static struct _do_only_once_ { _do_only_once_() { __VA_ARGS__; } } _do_only_once__; } while(0)

namespace
{
    // cache

    GLubyte rgba[4] = { 127, 127, 127, 255 };

    struct style
    {
        std::string name;
        int height;
        int face;
        float pt;
    };

    std::map< int, style > styles;

    struct text
    {
        float w, h, x, y;
        std::string str;
        style st;
        GLubyte rgba[4];
    };

    std::vector< text > texts;

    // external font engine
    struct sth_stash* stash = 0;

    // returns 0 if not found. facetype id otherwise
    int get_facetype( const std::string &pathfile )
    {
        /* create a font stash with a maximum texture size of 512 x 512 (if needed) */
        /* also deinitializes used memory */
        static struct on
        {
            on()
            {
                stash = sth_create(512,512);
                single( assert( stash ) );
                atexit( off );
            }
            static void off()
            {
                if( stash )
                    sth_delete( stash );
            }
        } _;

        single( assert( stash ) );

        /* load truetype font (if needed) */
        static std::map< std::string, int > map;
        std::map< std::string, int >::iterator find = map.find( pathfile );
        bool found = ( find != map.end() );

        if( found )
            return find->second;

        int fontface = sth_add_font( stash, pathfile.c_str() );
        if( !fontface )
        {
            // error, couldnt load fontface
            // try system font instead (just in case)

            fontface = sth_add_font( stash, moon9::font::system::locate( pathfile ).c_str() );

            if( !fontface )
                return 0; // no luck :(
        }

        return ( map[ pathfile ] = map[ pathfile ] ) = fontface;
    }

    style get_style( int id )
    {
        auto found = styles.find( id );

        if( found == styles.end() )
        {
            style invalid;

            invalid.name = std::string();
            invalid.pt = 0.f;
            invalid.face = 0;
            invalid.height = 0;

            return invalid;
        }

        return found->second;
    }

    // returns {w,h}
    std::pair<float,float> make_rect( int facetype, float pt, const std::string &str )
    {
        single( assert( stash ) );

        float minx = 0, miny = 0, maxx = 0, maxy = 0;
        sth_dim_text( stash, facetype, pt, str.c_str(), &minx, &miny, &maxx, &maxy);

        return std::pair<float,float>( maxx, maxy ); //maxx - minx, maxy - miny );
    }

    bool make_style( int id, const std::string &pathfile, float pt )
    {
        if( pt <= 0.f )
            return false; // invalid font style

        if( id <= 0 )
            return false; // invalid supplied id

        if( styles.find(id) != styles.end() )
            return false; // already defined!

        if( !pathfile.size() )
            return false; // invalid face

        int face = get_facetype( pathfile );

        if( face <= 0 )
            return false; // invalid face

        float minx = 0, miny = 0, maxx = 0, maxy = 0;
        sth_dim_text( stash, face, pt, "|", &minx, &miny, &maxx, &maxy);

        style &s = ( styles[ id ] = styles[ id ] );

        s.name = pathfile;
        s.pt = pt;
        s.face = face;
        s.height = int(maxy);

        return true;
    }

    std::string UTF16toUTF8(const std::wstring& utf16)
    {
        std::string utf8line;
        utf8::utf16to8(utf16.begin(), utf16.end(), std::back_inserter(utf8line));
        return utf8line;
    }

    std::vector<std::string> split( const std::string &text, const std::string &chars )
    {
        std::string map( 256, '\0' );

        for( auto it = chars.begin(), end = chars.end(); it != end; ++it )
            map[ *it ] = '\1';

        std::vector<std::string> tokens;

        tokens.push_back( std::string() );

        for( int i = 0, end = text.size(); i < end; ++i )
        {
            unsigned char c = text.at(i);

            std::string &str = tokens.back();

            if( !map.at(c) )
                str.push_back( c );
            else
            if( str.size() )
                tokens.push_back( std::string() );
        }

        while( tokens.size() && !tokens.back().size() )
            tokens.pop_back();

        return tokens;
    }

    struct ortho
    {
        explicit ortho( const bool &topleft = true ) // false = bottom-left
        {
//                glPushAttrib( GL_TRANSFORM_BIT );

            struct { GLint x, y, width, height; } viewport;
            glGetIntegerv(GL_VIEWPORT, &viewport.x);

            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();

            #if 1
            if( topleft )
            glOrtho(0,viewport.width,viewport.height,0,0,1);
            else
            glOrtho(0,viewport.width,0,viewport.height,0,1);
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();
            //glTranslatef(0,0,0);
            #else
            gluOrtho2D( viewport.x, viewport.width, viewport.y, viewport.height );
            #endif
        }

        ~ortho()
        {
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();

//                glPopAttrib();
        }
    };
}

namespace moon9
{
namespace font
{
    namespace make
    {
        bool style( int id, const std::string &fontfile, float pt ) // bold, color
        {
            return make_style( id, fontfile, pt );
        }

        dims rect( const std::string &utf8, int style )
        {
            auto s = get_style( style );
            auto result = make_rect( s.face, s.pt, utf8 );
            dims d = { result.first, utf8 == " " ? s.height : result.second };
            return d;
        }

        dims rect( const std::wstring &utf16, int style )
        {
            return rect( UTF16toUTF8(utf16), style );
        }
    }

    namespace metrics
    {
        int height( int style_id )
        {
            return get_style( style_id ).height;
        }
    }

    void color( float r, float g, float b, float a )
    {
        /*
        We could fetch color directly from GL instead. Something like:

        float currentColor[4];
        glGetFloatv(GL_CURRENT_COLOR,currentColor);

        However, a major drawback is that wont work unless some tinted geometry has been submitted already.
        */

        rgba[0] = (GLubyte)(r);
        rgba[1] = (GLubyte)(g);
        rgba[2] = (GLubyte)(b);
        rgba[3] = (GLubyte)(a);
    }

    void batch( const std::string &utf8, float x, float y, int style_id )
    {
        std::vector<std::string> lines = split( utf8, "\n" );

        ::style st = get_style( style_id );

        float py = y;
        for( std::vector<std::string>::const_iterator it = lines.begin(), end = lines.end(); it != end; ++it )
        {
            if( *it != "\n" )
            {
                texts.push_back( text() );
                text &t = texts.back();

                t.str = *it;

                t.x = x;
                t.y = py;
                t.st = st;
                t.rgba[0] = rgba[0];
                t.rgba[1] = rgba[1];
                t.rgba[2] = rgba[2];
                t.rgba[3] = rgba[3];
            }

    #if 1
            // truetype
            py += st.height + st.height / 4; // 1.25 interline
    #else
            // freeglut bitmap
            py += 13 / 2;
    #endif
        }
    }

    void batch( const std::wstring &utf16, float x, float y, int style_id )
    {
        batch( UTF16toUTF8(utf16), x, y, style_id );
    }

    void submit()
    {
        single( assert( stash ) );

        float h;

        {
            GLint view[4];
            glGetIntegerv( GL_VIEWPORT, &view[0] );

            h = view[3];
        }

        // truetype

        glPushAttrib( GL_ALL_ATTRIB_BITS );

        {
            ortho flat(false);

            float current_raster_position[4];
            glGetFloatv( GL_CURRENT_RASTER_POSITION, current_raster_position );

            // this is required. why? {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            // }

            glDisable(GL_CULL_FACE);

            /* draw text during your OpenGL render loop */

                glColor4ub( 255, 255, 255, 255 );

                sth_begin_draw(stash);

                for( std::vector<text>::iterator it = texts.begin(), end = texts.end(); it != end; ++it )
                {
                    text &t = *it;

                    float inc_x = 0;

                    glColor4ubv( t.rgba );
                    sth_draw_text(stash, t.st.face, t.st.pt, t.x, h - t.st.height - t.y, t.str.c_str(), &inc_x );
                }

                texts.clear();

                sth_end_draw(stash);

            glEnable(GL_CULL_FACE);

            glRasterPos4fv( current_raster_position );
        }

        glPopAttrib();
    }
}
}

#undef single

