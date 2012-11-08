#include <moon9/render/os.hpp>
#include <moon9/render/render.hpp>

#include <moon9/play/component.hpp>
#include <moon9/io/get.hpp>

#include <moon9/string/string.hpp>

#include <moon9/os/alert.hpp>

#include "fontstash.hpp"
#include "font.hpp"
#include "system_fonts.hpp"

#define single(...) do { static struct _do_only_once_ { _do_only_once_() { __VA_ARGS__; } } _do_only_once__; } while(0)

namespace
{
    // cache

    GLubyte rgba[4] = { 127, 127, 127, 255 };

    struct style
    {
        std::string name;
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

            fontface = sth_add_font( stash, find_system_font(pathfile).c_str() );

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

        style &s = ( styles[ id ] = styles[ id ] );

        s.name = pathfile;
        s.pt = pt;
        s.face = face;

        return true;
    }
}


namespace font
{
    namespace make
    {
        bool style( int id, const std::string &fontfile, float pt ) // bold, color
        {
            return make_style( id, fontfile, pt );
        }

        dims rect( const std::string &str, int style )
        {
            auto s = get_style( style );
            auto result = make_rect( s.face, s.pt, str );
            dims d = { result.first, result.second };
            return d;
        }
    }

    void color( float r, float g, float b, float a )
    {
        rgba[0] = (GLubyte)(r);
        rgba[1] = (GLubyte)(g);
        rgba[2] = (GLubyte)(b);
        rgba[3] = (GLubyte)(a);
    }

    void batch( const std::string &str, float x, float y, int style_id )
    {
#if 0
        texts.push_back( text() );
        text &t = texts.back();

        t.str = str;
        t.x = x;
        t.y = y;
        t.face = get_style( style ).face;
        t.pt = get_style( style ).pt;
#else
        moon9::strings lines = moon9::string( str ).split("\n");

        style st = get_style( style_id );

        int py = y;
        for( auto it = lines.begin(), end = lines.end(); it != end; ++it )
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
            py += st.pt / 2;
    #else
            // freeglut bitmap
            py += 13 / 2;
    #endif
        }
#endif
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

        //moon9::disable::lighting no_lights;
        //moon9::disable::depth on_top;

        glPushAttrib( GL_ALL_ATTRIB_BITS );

        moon9::matrix::ortho flat(false);


        //glViewport(0, 0, width, height);
        //glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glDisable(GL_TEXTURE_2D);
        //glMatrixMode(GL_PROJECTION);
        //glLoadIdentity();
        //glOrtho(0,width,0,height,-1,1);
        //glMatrixMode(GL_MODELVIEW);
        //glLoadIdentity();
        //glDisable(GL_DEPTH_TEST);
        //glColor4ub(255,255,255,255);
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

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
                    sth_draw_text(stash, t.st.face, t.st.pt, t.x, h - t.st.pt - t.y, t.str.c_str(), &inc_x );
                }

                texts.clear();

                sth_end_draw(stash);

            glEnable(GL_CULL_FACE);

            glRasterPos4fv( current_raster_position );

        glPopAttrib();
    }
}

#undef single
