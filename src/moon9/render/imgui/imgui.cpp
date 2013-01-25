/*
 * Simple IMGLUI widgets/classes
 * IMGLUI Immediate Mode OpenGL User Interface

 * Copyright (c) 2011 Mario 'rlyeh' Rodriguez
 * Copyright (c) 2009-2010 Mikko Mononen memon@inside.org

 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.

 * Features:
 * - safe raii based declarations
 * - easy chaining composition
 * - progress bar, pair, list, radio, menu, canvas, glass/lens, dockbar
 * - wip colors
 * - wip boxes
 *   - box persistance; @todo: extend to widgets/user-defined widgets
 *   - box layout: cascade, mosaic, tile
 *   - box physics
 *   - box stickies

 * References:
 * - http://sol.gfxile.net/imgui/
 * - http://mollyrocket.com/forums/viewforum.php?f=10

 * To do:
 * - http://mollyrocket.com/forums/viewtopic.php?t=159 (tree)
 * - moon9::locate(x,y). this is a start:
            int scroll = 0;
            moon9::BeginScrollArea( "", 100, 100, 200, 25, &scroll );
            moon9::ProgressBar( "lalala", 100, false );
            moon9::EndScrollArea();
 * - a few moon9::renderer's per instance (ie, a few hids per game are possible)

tomorrow plan:
- overlapped z-fighting -> collide w/ physics
- overlapped no z-fighting -> transparency
- dragging from borders -> splines
- box framerate slider
- mosaic/tile/minimize all -> y wont decrement
- minimize_all -> minimize then mosaic
- mosaic_all -> -collide then mosaic
- mouse cursor flickering on hot spots

bugs:
- negative canvas -> when collapsing
- canvas #2 & wireframe glass @ collapsed box w/ scroll -> visible scissor (creo q es dpw,dph)
- collide w/ all boxes
- hot under active box
- collide off when window->layout
- collide off @ all boxes when moving/resizing any box
- auto-show -> affects client area
- initial window size -> on_client_change too (normalized window coords+zoom)
- setcursorpos @ blocking boxes
- keyboard && mouse -> !asyncstate

Edit Box. Not all numeric value entries can be converted to sliders. However, change events can be used to make updates effective as the user types in the new value. This way the return key press or update button press is not needed as an additional step.
Pull-Down List Box. An update can occur as a selection highlights in the dropdown list. The user will easily preview content with the different selections and be able to quickly pick the desired one.
Drag & Drop. This is another method that makes property assignment very simple. An example is to drag a property object and drop it on the asset being edited. The property object would instantly associate itself with the content and show the update instantly.

http://www.johno.se/book/imgui.html
http://www.johno.se/sword/september2009.php
http://redprodukt.com/?page_id=5

PRIO #0
accordion
tilepile / dockbar : +physics
box: auto-layout/exposed
box: auto-hide/auto-show property (like menu)
box: sticky & magnets (~links)
box: select multiple boxes with ctrl
vbar
vline
COORDS: 0,0 CENTER

PRIO #1
frame/area < tab < window/box
resize @ 9 hotspots
tree : list of collapse[]
stats: functions(t), pie charts, bars
menus : tree[ items, separators, etc ]
3d manipulators: http://www.gnu.org/software/gleem/doc/manips.jpg
sphere+azis -> rotation + translation | ctrl to multiple select | yellow = selected ; scale how?

PRIO #2
texture -> ~canvas with plane
splines
graphs layout / fsm
hscroll -> (~slider), vslider -> (~scroll)
textfield, password
text

PRIO #3
debug lenses: wireframe, physics/collisions/navmesh/forces, ids/serialization, texture/vertex granularity, ai, shadow mapping, frustum, cameras

PRIO #3
grid
dial
undo/redo

PRIO #4
curves editor
Color Picker/tone cycler -> tinted boxes[] + sliders + manipulators
The standard color picker can be modified to update on mouse roll over or mouse button down. This way color update can occur while the user moves the cursor over the color swatch. Mouse roll over in the color swatch window will display the color on the content being modified or edited. The user will then click once to confirm the selection.

PRIO #5
themes: user defined as well -> callbacks
themes: texture -> seam carving <-> http://seamonster.codeplex.com/ | http://eigenclass.org/hiki/seam-carving-in-ocaml
themes: vectors -> zoom
logger: capture std::cout/err/clog & redirect to box/logger. then, examine callstack to do identation/colorization


IDC_NO between two close boxes T,B,L,R -> sticky on
IDC_SE between two sticked frames -> sticky off


                // send frame delta time
                guiInstance->importUpdate(deltaTime);

                // send mouse x and y coordinates
                guiInstance->importMouseMotion(x,y);

                // send mouse button released events
                guiInstance->importMouseReleased(button);

                // send mouse button pressed events
                guiInstance->importMousePressed(button);

                //send key release event
                guiInstance->importKeyReleased(key,modifier);

                // send key pressed event
                guiInstance->importKeyPressed(key,modifier);

                guiInstance->paint();


 * - rlyeh
 */

#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <map>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#if defined(WIN32) || defined(_WIN32)
#       define snprintf _snprintf
#endif

/*
void free(void* ptr, void* userptr);
void* malloc(size_t size, void* userptr);

#define STBTT_malloc(x,y)    malloc(x,y)
#define STBTT_free(x,y)      free(x,y)
*/
#ifndef STB_TRUETYPE_IMPLEMENTATION
#   ifndef STBTT_ifloor
#       include <cmath>
#       define STBTT_ifloor(x)   ((int)std::floorf(x))
#       define STBTT_iceil(x)    ((int)std::ceilf(x))
#   endif
#   include <stb/stb_truetype.h>
#endif

#include <moon9/io/file.hpp>
#include <moon9/render/imgui.hpp>

#include <moon9/render/render.hpp>
#include <moon9/render/camera.hpp> // this should not be here

#include <moon9/render/ubuntu-b.hpp>


namespace
{
    std::string CRC32( const std::string &str, size_t my_hash = 0 )
    {
        const void *pMem = str.c_str();
        size_t iLen = str.size();

        if( pMem == 0 || iLen == 0 )
        {
            // ...
        }
        else
        {
            static const size_t crcTable[16] =
            {
                0x00000000, 0x1DB71064, 0x3B6E20C8, 0x26D930AC,
                0x76DC4190, 0x6B6B51F4, 0x4DB26158, 0x5005713C,
                0xEDB88320, 0xF00F9344, 0xD6D6A3E8, 0xCB61B38C,
                0x9B64C2B0, 0x86D3D2D4, 0xA00AE278, 0xBDBDF21C
            };

            const unsigned char *pPtr = (const unsigned char *)pMem;
            unsigned h = ~my_hash;

            while( iLen-- )
            {
                h ^= (*pPtr++);
                h = crcTable[h & 0x0f] ^ (h >> 4);
                h = crcTable[h & 0x0f] ^ (h >> 4);
            }

            my_hash = ~h;
        }

        std::stringstream ss;
        ss << std::setfill('0') << std::hex << my_hash;

        return ss.str();
    }
}


namespace moon9
{
    unsigned RGBAf( float r, float g, float b, float a )
    {
        return RGBA( (unsigned char)(r*255), (unsigned char)(g*255), (unsigned char)(b*255), (unsigned char)(a*255) );
    }

    RGBA::RGBA() : color(0) {}
    RGBA::RGBA(unsigned rgba) : color(rgba) {}
    RGBA::RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a ) : color( (r) | (g << 8) | (b << 16) | (a << 24) ) {}

    RGBA::operator const unsigned&() const { return color; }
    RGBA::operator       unsigned&()       { return color; }

    RGBA RGBA::alpha( float a ) const
    {
        if( a < 0.f ) a = 0; else if( a > 1.f ) a = 1.f;
        return (color & RGBA( 255, 255, 255, 0 )) | RGBA( 0, 0, 0, a * 255 );
    }

    RGBA RGBA::saturation( float factor ) const
    {
        int r = ( color >>  0 ) & 0xff; r *= factor; if( r < 0 ) r = 0; else if( r > 255 ) r = 255;
        int g = ( color >>  8 ) & 0xff; g *= factor; if( g < 0 ) g = 0; else if( g > 255 ) g = 255;
        int b = ( color >> 16 ) & 0xff; b *= factor; if( b < 0 ) b = 0; else if( b > 255 ) b = 255;
        int a = ( color >> 24 ) & 0xff;

        return RGBA( r, g, b, a );
    }

    const RGBA black(   0,  0,  0 );
    const RGBA white( 255,255,255 );

    // goo colour scheme, all components make 255+192 (~) to keep
    // tone balance. red, purple and yellow tweak a little bit to
    // fit better with gray colours.

    const RGBA   default_colour_scheme::gray( 149,149,149 ); // gray
    const RGBA   default_colour_scheme::blue(   0,192,255 ); // blue
    const RGBA   default_colour_scheme::cyan(  48,255,144 ); // cyan/turquoise
    const RGBA   default_colour_scheme::pink(  255,48,144 ); // pink
    const RGBA default_colour_scheme::orange(  255,144,48 ); // orange
    const RGBA  default_colour_scheme::green(  144,255,48 ); // green/lime
    const RGBA default_colour_scheme::purple( 178,128,255 ); // purple
    const RGBA default_colour_scheme::yellow(   255,224,0 ); // yellow
    const RGBA    default_colour_scheme::red(   255,48,48 ); // red
}

namespace imgui
{
    moon9::RGBA theme( 255,255,255 );    // ui theme defaults to white
    moon9::RGBA tone(192,255,0,255);     // foreground items
    moon9::RGBA background(0,0,0,128);   // background items
}

// C++ interface

#include <vector>

namespace moon9
{
    using namespace default_colour_scheme;
    using namespace imgui;

    namespace
    {
            float client_w = 0;
            float client_h = 0;

            float mousex = 0, mouseprevx = 0;
            float mousey = 0, mouseprevy = 0;
            float xdt = 0;
            float ydt = 0;
            bool  mouselh = false, mouseprevbl = false;
            bool  mouserh = false, mouseprevbr = false;
            bool  mouselt = false;
            bool  mousert = false;
            bool  mouse_is_hover = false;
            bool  mouse_is_resizing = false;
            bool  mouse_is_moving = false;
    }

    color::color( float r, float g, float b, float a )
    {
        theme = RGBA( RGBAf(r,g,b,a) );
    }

    color::~color()
    {
        theme = RGBA(255,255,255,255);
    }

    ui::operator const bool() const
    {
        return true;
    }

    ui::ui( int width, int height, int mouse_x, int mouse_y, bool mouse_button_left, bool mouse_button_right, const void *ttf_font_data, const size_t ttf_font_size )
    {
        static struct install {
                install( const void *ttf_font_data, size_t ttf_font_size ) {
                size_t errcode = RenderGLInit(ttf_font_data, ttf_font_size);
                if( errcode )
                        std::cout << "moon9::imgui : cannot read supplied font (" << errcode << ")" << std::endl,
                        throw "cannot read ttf font";
        }
                ~install() {
                RenderGLDestroy();
        }} _( ttf_font_data && ttf_font_size ? ttf_font_data : ubuntu_b_ttf::data(), ttf_font_data && ttf_font_size ? ttf_font_size : ubuntu_b_ttf::size() );

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_TEXTURE_2D);

        // Render GUI
        glViewport(0, 0, width, height);
        glDisable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, width, 0, height );
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        client_w = width;
        client_h = height;

        mouse_is_moving = false;
        mouse_is_resizing = false;
        mouse_is_hover = false;

        mousex = mouse_x;                                  //mouse.client.newest().x;
        mousey = height - mouse_y;                         //height - mouse.client.newest().y;

        xdt = (mousex - mouseprevx);
        ydt = (mousey - mouseprevy);

        mouseprevx = mousex;
        mouseprevy = mousey;

        if( (!mouseprevbl) && mouse_button_left ) // triggered?
            mouselt = true,
            mouselh = false;
        else
            mouselt = false,
            mouselh = mouse_button_left;

        if( (!mouseprevbr) && mouse_button_right ) // triggered?
            mousert = true,
            mouserh = false;
        else
            mousert = false,
            mouserh = mouse_button_right;

        mouseprevbl = mouse_button_left;
        mouseprevbr = mouse_button_right;

        BeginFrame( mousex, mousey, mouselh, mouserh, /*int scroll =*/ 0 );

        bool b = true;
        Check( "oh lala", b);
    }

    ui::~ui()
    {
        EndFrame();
        RenderGLDraw();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);

        #if defined(WIN32) || defined(_WIN32)

        // sorted in priority

        if( mouse_is_hover )
            SetCursor(LoadCursor(NULL, IDC_HAND));
        else
        if( mouse_is_moving )
            SetCursor(LoadCursor(NULL, IDC_SIZEALL));
        else
        if( mouse_is_resizing )
            SetCursor(LoadCursor(NULL, IDC_SIZENWSE));   // SIZENESW /, NS |, NWSE \, WE -

        #endif
    }


void box::self_data::defaults( std::string name, int x, int y, int w, int h, int scroll )
{
    init = false;
    hover = false;
    enabled = false;
#if defined(NDEBUG) || defined(_NDEBUG)
    reflexion = false;
#else
    reflexion = true;
#endif

    this->phx = x;
    this->phy = y;
    this->phw = w;
    this->phh = h;
    this->vx = 0;
    this->vy = 0;
    this->vw = 0;
    this->vh = 0;

    this->name = name;
    this->title = "";
    this->ox = x;
    this->oy = y;
    this->ow = w;
    this->oh = h;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->scroll = scroll;

    resizable = true;
    moveable = true;
    constrained = true;
    physics = true;
    minimizable = true;
    scrollable = true;

    is_moving = 0;
    is_resizing = 0;
    is_attached = false;
    is_collapsing = false;

    //detach();
    dx = 0, dy = 0;
    dw = this->w, dh = AREA_HEADER; //this->h;

    opacity = 0.5;

    color_fg =
    color_fg_backup = tone.alpha(1.f);

    color_bg =
    color_bg_backup = moon9::RGBA(0,0,0,opacity * 255.f);

    debug = false;
    debug_verbose = false;
}

void box::self_data::erase( std::string ext )
{
    std::string name = CRC32( this->name );

    std::cout << "removing " << ( name + ext ) << std::endl;

    std::remove( std::string(name + ext).c_str() );
}

void box::self_data::load( std::string ext )
{
    return; // working, but disabled

    std::string name = CRC32( this->name );

    if( !moon9::file(name + ext).size() )
    {
        std::cout << "cant find " << (name + ext) << std::endl;
        return;
    }

    if( moon9::file(name + ext).size() != sizeof(*this) )
    {
        std::cout << "incompatible version found " << (name + ext) << std::endl;
        erase( ext );
        return;
    }

    imgui::string ptr( moon9::file( name + ext ).read() );

    std::cout << (name + ext) << " found!" << std::endl;
    *this = *((self_data *)ptr.c_str()); // @fixme: risky and non portable
}

void box::self_data::save( std::string ext )
{
    return; // working, but disabled

    std::string name = CRC32( this->name );

    std::cout << "saving " << ( name + ext ) << std::endl;

    imgui::string data;
    data.resize( sizeof(*this) );
    memcpy( &data.at(0), this, sizeof(*this) ); // @fixme: risky

    moon9::file( name + ext ).overwrite( data );
}

std::map< imgui::string, box::self_data > &box::self_data::persist( action to_do )
{
    static std::map< imgui::string, box::self_data > map;

    if( to_do == LOAD )
    {
        if( map.find( name ) == map.end() )
        {
            map[ name ] = map[ name ];

            save(".def");
            load(".opt");

            map[ name ] = *this;
        }
        else
            *this = map[ name ];

        tone = color_fg;
        background = color_bg;

        hover = BeginScrollArea( title.c_str(), phx, phy, phw, phh, scrollable ? &scroll : 0 );

#if defined(NDEBUG) || defined(_NDEBUG)
        if( const bool reflexion_debug = false )
#else
        if( const bool reflexion_debug = true )
#endif
        if( reflexion )
        {
            HSeparatorLine();

            if( Collapse( "Debug info", "moon9::box", debug ) )
                debug ^= true;

            if( debug )
            {
                Indent();

                if( Collapse("Expand all fields", "", debug_verbose ) )
                {
                    debug_verbose ^= true;
                }

                if( debug_verbose )
                {
                    Indent();
                    Indent();
                    Pair(        ".name", imgui::string(name).c_str() );
                    Pair(       ".title", imgui::string(title).c_str() );
                    Pair(         ".phx", imgui::string(phx).c_str() );
                    Pair(         ".phy", imgui::string(phy).c_str() );
                    Pair(         ".phw", imgui::string(phw).c_str() );
                    Pair(         ".phh", imgui::string(phh).c_str() );
                    Pair(           ".x", imgui::string(x).c_str() );
                    Pair(           ".y", imgui::string(y).c_str() );
                    Pair(           ".w", imgui::string(w).c_str() );
                    Pair(           ".h", imgui::string(h).c_str() );
                    Pair( ".attached_to", imgui::string(attached_to).c_str() );
                    Pair(          ".dx", imgui::string(dx).c_str() );
                    Pair(          ".dy", imgui::string(dy).c_str() );
                    Pair(          ".dw", imgui::string(dw).c_str() );
                    Pair(          ".dh", imgui::string(dh).c_str() );
                    Pair(          ".vx", imgui::string(vx).c_str() );
                    Pair(          ".vy", imgui::string(vy).c_str() );
                    Pair(          ".vw", imgui::string(vw).c_str() );
                    Pair(          ".vh", imgui::string(vh).c_str() );
                    Pair(          ".ox", imgui::string(ox).c_str() );
                    Pair(          ".oy", imgui::string(oy).c_str() );
                    Pair(          ".ow", imgui::string(ow).c_str() );
                    Pair(          ".oh", imgui::string(oh).c_str() );
                    Pair(      ".scroll", imgui::string(scroll).c_str() );
                    Pair(        ".init", imgui::string(init).c_str() );
                    Pair(       ".hover", imgui::string(hover).c_str() );
                    Pair(     ".enabled", imgui::string(enabled).c_str() );
                    Pair(    ".moveable", imgui::string(moveable).c_str() );
                    Pair(   ".resizable", imgui::string(resizable).c_str() );
                    Pair( ".constrained", imgui::string(constrained).c_str() );
                    Pair(  ".scrollable", imgui::string(scrollable).c_str() );
                    Pair(     ".physics", imgui::string(physics).c_str() );
                    Pair( ".minimizable", imgui::string(minimizable).c_str() );
                    Pair(   ".reflexion", imgui::string(reflexion).c_str() );
                    Pair(   ".is_moving", imgui::string(is_moving).c_str() );
                    Pair( ".is_resizing", imgui::string(is_resizing).c_str() );
                    Pair(     ".opacity", imgui::string(opacity).c_str() );
                    //Pair( ".debug", imgui::string(debug).c_str() );
                    //Pair( ".debug_verbose", imgui::string(debug_verbose).c_str() );
                    Unindent();
                    Unindent();
                }

                if( Check( "Physics", physics ) )
                    physics ^= true;

                if( Check( "Constrained", constrained ) )
                    constrained ^= true;

                if( Check( "Enabled", enabled ) )
                    enabled ^= true;

                if( Check( "Moveable", moveable ) )
                    moveable ^= true;

                if( Check( "Resizable", resizable ) )
                    resizable ^= true;

                if( Check( "Scrollable", scrollable ) )
                    scrollable ^= true;

                {
                    Label("Foreground");

                    unsigned r,g,b;

                    b = ( color_fg >> 16 ) & 0xff;
                    g = ( color_fg >>  8 ) & 0xff;
                    r = ( color_fg >>  0 ) & 0xff;

                    float fr = r / 255.0, fg = g / 255.0, fb = b / 255.0;

                    Slider("R", &fr, 0.f, 1.f, 0.01f );
                    Slider("G", &fg, 0.f, 1.f, 0.01f );
                    Slider("B", &fb, 0.f, 1.f, 0.01f );
                    Slider("A", &opacity, 0.f, 1.f, 0.01f );

                    r = fr * 255.f, g = fg * 255.f, b = fb * 255.f;

                    color_fg = moon9::RGBA(r,g,b);
                }

                {
                    Label("Background");

                    unsigned r,g,b;

                    b = ( color_bg >> 16 ) & 0xff;
                    g = ( color_bg >>  8 ) & 0xff;
                    r = ( color_bg >>  0 ) & 0xff;

                    float fr = r / 255.0, fg = g / 255.0, fb = b / 255.0;

                    Slider("R", &fr, 0.f, 1.f, 0.01f );
                    Slider("G", &fg, 0.f, 1.f, 0.01f );
                    Slider("B", &fb, 0.f, 1.f, 0.01f );
                    Slider("A", &opacity, 0.f, 1.f, 0.01f );

                    r = fr * 255.f, g = fg * 255.f, b = fb * 255.f;

                    color_bg = moon9::RGBA(r,g,b);
                }

                if( Button( "Save options") )
                {
                    debug = false;
                    save(".opt");
                    //debug = true;
                }

                if( Button( "Reset options") )
                {
                    erase(".opt");
                    load(".def");
                    save(".opt");
                }

                Unindent();
            }

            HSeparatorLine();
        }

        init = true;
    }
    else
    if( to_do == SAVE )
    {
        EndScrollArea();

        tone = color_fg_backup;
        background = color_bg_backup;

        map[ name ] = *this;
    }
    else
    {
//                if( to_do == RETRIEVE )
    }

    return map;
}

void box::cascade_all( std::vector<std::string> boxnames )
{
    // @todo: increase z-depth in each step
    // @todo: resolve collisions as well, since this algorithm overlaps boxes (ie, fix collisions)

    std::map< imgui::string, self_data > &map( self.persist( self_data::RETRIEVE ) );

    // cascade from here in alphabetical order
    std::sort( boxnames.begin(), boxnames.end() );

    // top-left to bottom-right, 48 px spaced
    int x = 150, y = client_h - 300 - 48;

    for( size_t i = 0; i < boxnames.size(); ++i )
    {
        // relocate, & resize (useful?)
        map[boxnames[i]].x = x;
        map[boxnames[i]].y = y;
        map[boxnames[i]].w = 200;
        map[boxnames[i]].h = 300;

        // bottom-right diagonal
        x += 48;
        y -= 48;
    }
}

void box::mosaic_all( std::vector<std::string> boxnames )
{
    // @todo: use texture atlas algorithm in here

    std::map< imgui::string, self_data > &map( self.persist( self_data::RETRIEVE ) );

    // mosaic from here in alphabetical order
    std::sort( boxnames.begin(), boxnames.end() );

    // top-left to bottom-right, w spaced
    const int w = 150, h = 150;
    const int ox = 150;

    int x = ox, y = client_h, maxh = 0;

    for( size_t i = 0; i < boxnames.size(); ++i )
    {
        // relocate
        map[boxnames[i]].x = x;
        map[boxnames[i]].y = y - map[boxnames[i]].h;

        maxh = std::max( maxh, map[boxnames[i]].h );

        // bottom-right diagonal
        x += map[boxnames[i]].w;

        if( x > client_w - ox )
            x = ox, y -= maxh, maxh = 0;
    }
}

void box::tile_all( std::vector<std::string> boxnames )
{
    std::map< imgui::string, self_data > &map( self.persist( self_data::RETRIEVE ) );

    // tile from here in alphabetical order
    std::sort( boxnames.begin(), boxnames.end() );

    // top-left to bottom-right, w spaced
    const int w = 150, h = 150;
    const int ox = 150, oy = client_h - h;

    int x = ox, y = oy;

    for( size_t i = 0; i < boxnames.size(); ++i )
    {
        // relocate & resize
        map[boxnames[i]].x = x;
        map[boxnames[i]].y = y;
        map[boxnames[i]].w = w;
        map[boxnames[i]].h = h;

        // bottom-right diagonal
        x += w;

        if( x > client_w )
            x = ox, y -= h;
    }
}

void box::expose_all( std::vector<std::string> boxnames )
{
    // @todo: like mosaic+resizing across whole screen. do not start from top-left corner. distribute instead ; ~= atlas w/ zoom
}

void box::normalize_all( std::vector<std::string> boxnames )
{
    // @todo: make them scrollbar=0 & same size
}

void box::arrange_all( std::vector<std::string> boxnames )
{
    // @todo: fix collisions + minimal adjusts to align user-defined layout (align to grid?)
}

void box::minimize_all( std::vector<std::string> boxnames )
{
    /*
    no es tan facil

    std::map< imgui::string, self_data > &map( self.persist( self_data::RETRIEVE ) );

    // tile from here in alphabetical order
    std::sort( boxnames.begin(), boxnames.end() );

    for( size_t i = 0; i < boxnames.size(); ++i )
    {
        map[boxnames[i]]
    }
    */
}

box::box( const imgui::string &name, int x, int y, int w, int h, int scroll )
{
    //extern RGBA tone.alpha(1.0);
    //pcolor = tone.alpha(1.0);

    self.defaults( name, x, y, w, h, scroll );
}

box::box( const box &t )
{
    self = t.self;

    self.persist( self_data::LOAD );
}

box::~box()
{
    if( self.minimizable && is_hover() && mousert )
    {
        if( !self.is_attached )
            attach( self.name, 0, 0 ).minimize( self.w, AREA_HEADER );
        else
            detach().maximize();
    }

    if( self.is_attached || self.is_collapsing )
    {
        std::map< imgui::string, self_data > &map( self.persist( self_data::RETRIEVE ) );

        if( map.find(self.attached_to) != map.end() )
        {
            self.x = map[self.attached_to].x + self.dx;
            self.y = map[self.attached_to].y + self.dy;
        }
        else
        {
            self.x = self.dx;
            self.y = self.dy;

            self.is_collapsing = false;
        }

        self.w = self.dw;
        self.h = self.dh;
    }

    bool autonomous = !(self.is_attached && self.attached_to != self.name); // parents handle children behaviour

/* box resize { */

    if( self.resizable )
    {
        if( !self.is_resizing )
        {
            if( autonomous && is_hover() && mousey < self.y + 30 && mousex > self.x + self.w - 30 )
            {
                mouse_is_resizing |= true;

                if( mouselt )
                    self.is_resizing = true;
            }
        }
        else
        {
            bool hold = mouselh;

            if( self.is_resizing && hold ) {
                resize_3( xdt, ydt ),
                reset_forces();
            }
            else
            self.is_resizing = 0;
        }

        mouse_is_resizing |= self.is_resizing;
    }

/* box resize } */


/* box movement { */

    if( self.moveable )
    {
        if( !self.is_moving )
        {
            if( autonomous && is_hover() && mousey > self.y + self.h - 30 /* && mousex <= self.x + self.w - 30 */ )
            {
                mouse_is_moving |= true;

                if( mouselt )
                    self.is_moving = true;
            }
        }
        else
        {
            bool hold = mouselh;

            if( self.is_moving && hold ) {
                relocate( xdt, ydt ),
                reset_forces();
            }
            else
            self.is_moving = 0;
        }

        mouse_is_moving |= self.is_moving;
    }

/* box movement } */

/* box rect constrainment { */

    if( autonomous )
    if( self.constrained && !self.is_moving )
    {
        if( self.x + self.w > client_w )
            self.x = self.x * 0.0 + (client_w - self.w) * 1.0;
        else
        if( self.x < 0 )
            self.x = self.x * 0.0;

        if( self.y < 0 )
            self.y = self.y * 0.0;
        else
        if( self.y + self.h > client_h )
            self.y = self.y * 0.0 + (client_h - self.h) * 1.0;
    }

/* box rect constrainment } */

/* box physics { */

    if( self.physics )
    {
        const float mass = 29.9f; // mass: the lighter, the bouncier (must be > 0)
        const float k = 0.2f;     // spring constant
        const float damp = 0.9f;  // damping/friction: the higher, the frictionless (must be <1.0)

        float force, accel;

        force = -k * (self.phx - self.x);       // f = -k*y (spring force)
        accel = force / mass;                   // f = m*a
        self.vx = damp * (self.vx + accel);     // velocity
        self.phx += self.vx;                    // update
        if( abs(self.vx) < 0.00001 )            // avoid minimal jittering when pixel displacement < 1
            {}//self.vx = 0;

        force = -k * (self.phy - self.y);       // f = -k*y (spring force)
        accel = force / mass;                   // f = m*a
        self.vy = damp * (self.vy + accel);     // velocity
        self.phy += self.vy;                    // update
        if( abs(self.vy) < 0.00001 )            // avoid minimal jittering when pixel displacement < 1
            {}//self.vy = 0;

        force = -k * (self.phw - self.w);       // f = -k*y (spring force)
        accel = force / mass;                   // f = m*a
        self.vw = damp * (self.vw + accel);     // velocity
        self.phw += self.vw;                    // update
        if( abs(self.vw) < 0.00001 )            // avoid minimal jittering when pixel displacement < 1
            {}//self.vw = 0;

        // avoid negative height ; parece que no va muy bien con ventanas q tienen scroll grande
        float sh = ( self.h < AREA_HEADER ? abs(self.h) * 0.25 + AREA_HEADER * 0.75 : self.h );
        force = -k * (self.phh - sh);       // f = -k*y (spring force)
        accel = force / mass;                   // f = m*a
        self.vh = damp * (self.vh + accel);     // velocity
        self.phh += self.vh;                    // update
        if( abs(self.vh) < 0.00001 )            // avoid minimal jittering when pixel displacement < 1
            {}//self.vh = 0;
    }
    else reset_forces();

/* box physics } */

    if( self.init )
        self.persist( self_data::SAVE );
}

box &box::locate( float x, float y )
{
    self.x = x;
    self.y = y;
    return *this;
}

box &box::size( float w, float h )
{
    self.w = w;
    self.h = h;
    return *this;
}

box &box::relocate( float offx, float offy )
{
    self.x = self.x + offx;
    self.y = self.y + offy;
    return *this;
}

// 1,7

box &box::resize_9( float offw, float offh ) //rescale = * ? /
{
    if( self.w + offw < 50 || self.h + offh < 50 )
    return *this;
    self.w = self.w + offw;
    self.h = self.h + offh;
    return *this;
}

box &box::resize_3( float offw, float offh ) /* \ rescale = * ?  */
{
    if( self.w + offw < 150 || self.h - offh < 150 )
    return *this;
    self.w = self.w + offw;
    self.y += offh;
    self.h = self.h - offh;
    return *this;
}

box &box::reset_forces()
{
    self.phx = self.x;   // make'em to rest target & position
    self.phy = self.y;
    self.phw = self.w;
    self.phh = self.h;
    return *this;
}

box &box::reset_location()
{
    self.x = self.ox;
    self.y = self.oy;
    return *this;
}

box &box::reset_size()
{
    self.w = self.ow;
    self.h = self.oh;
    return *this;
}

box &box::enabled( bool on )
{
    self.enabled = on;
    return *this;
}
box &box::resizable( bool on )
{
    self.resizable = on;
    return *this;
}
box &box::moveable( bool on )
{
    self.moveable = on;
    return *this;
}
box &box::constrained( bool on )
{
    self.constrained = on;
    return *this;
}
box &box::physics( bool on )
{
    self.physics = on;
    return *this;
}
box &box::minimizable( bool on )
{
    self.minimizable = on;
    return *this;
}
box &box::reflexion( bool on )
{
    self.reflexion = on;
    return *this;
}
box &box::title( const std::string &titlebar )
{
    self.title = titlebar;
    return *this;
}
// dock

box &box::attach( std::string name, int dx, int dy )
{
    self.is_attached = true;
    self.attached_to = name;

    self.ox = self.x;
    self.oy = self.y;

    self.dx = dx;
    self.dy = dy;

    return *this;
}
box &box::detach( bool from_current_position )
{
    self.is_attached = false;
    self.attached_to = std::string();

    self.dx = from_current_position ? self.phx : self.ox;
    self.dy = from_current_position ? self.phy : self.oy;

    return *this;
}
box &box::minimize( int dw, int dh )
{
    self.is_collapsing = true;

    self.ow = self.w;
    self.oh = self.h;

    self.dw = dw;
    self.dh = dh;

    return *this;
}
box &box::maximize()
{
    self.is_collapsing = true;

    self.dw = self.ow;
    self.dh = self.oh;

    return *this;
}

box &box::color_fg( RGBA color )
{
    self.color_fg = color;
    return *this;
}

box &box::color_bg( RGBA color )
{
    self.color_bg = color;
    return *this;
}

bool box::is_hover() const
{
    return self.hover;
}

bool box::is_attached() const
{
    return self.is_attached;
}

box::operator const bool() const
{
   return self.init;
}

// Layout, identation

tab::operator const bool()  const { return true; }
tab::tab()    {         Indent(); }
tab::~tab()    {       Unindent(); }

hbar::operator const bool()  const { return true; }
hbar::hbar()   { HSeparatorLine(); }

hbreak::operator const bool()  const { return true; }
hbreak::hbreak() {     HSeparator(); }

vbar::operator const bool()  const { return true; }
vbar::vbar()   { VSeparatorLine(); }

vbreak::operator const bool()  const { return true; }
vbreak::vbreak() {     VSeparator(); }

// Basic widgets

label::operator const bool() const { return true; }
label::label( const imgui::string &text ) { Label( text.c_str() ); }

value::operator const bool() const { return true; }
value::value( const imgui::string &text ) { Value( text.c_str() ); }


push::operator const bool() const { return result; }
push::push( const imgui::string &text, const bool &enabled ) { result = Button( text.c_str(), enabled ); }

toggle::operator const bool() const { return result; }
toggle::toggle( const imgui::string &text, bool &checked, const bool &enabled ) { result = Check( text.c_str(), checked, enabled ) ? checked ^= true, true : false; }

item::operator const bool() const { return result; }
item::item( const imgui::string &text, const bool &enabled ) { result = Item( text.c_str(), enabled ); }

slider::operator const bool() const { return result; }
slider::slider( const imgui::string &text, float min, float &value, float max, float inc, const bool &enabled ) { if(value>max) value=max; else if(value<min) value=min; result = Slider( text.c_str(), &value, min, max, inc, enabled ); }
slider::slider( const imgui::string &text, float min, float &value, float max, const bool &enabled ) { if(value>max) value=max; else if(value<min) value=min; result = Slider( text.c_str(), &value, min, max, 1.f, enabled ); }

delta::operator const bool() const { return result; }
delta::delta( const imgui::string &text, float &value, float inc, const bool &enabled ) { result = Slider( text.c_str(), &value, 0.0, 1.f, inc, enabled ); }
delta::delta( const imgui::string &text, float &value, const bool &enabled ) { result = Slider( text.c_str(), &value, 0.0, 1.f, 0.01f, enabled ); }

collapse::operator const bool() const { return result; }
collapse::collapse( const imgui::string &text, const imgui::string &subtext, const bool &open_as_default, const bool &enabled )
{
  static std::map< std::string, bool > map;
  std::string idx = text + subtext;
  std::map<std::string, bool>::iterator it = map.find( idx );

  if( it == map.end() )
      it = map.insert( std::pair< std::string, bool >(idx, open_as_default) ).first;

  if( Collapse( text.c_str(), subtext.c_str(), it->second, enabled ) )
      it->second ^= true;

  result = it->second;
}

progressbar::operator const bool() const { return true; }
progressbar::progressbar( const imgui::string &text, const float &val, const bool &show_decimals ) { ProgressBar( text, val, show_decimals ); }

int list::result() const { return _clicked; }
list::operator const bool() const { return _result != 0; }
list::list(const imgui::string &text, size_t n_options, const char **options, const bool &enabled ) {

    //@todo: scroll area, list.onblur(selection.blur)
    //@todo: improve hashing (text only is too weak)

    struct data {
      int choosing, clicked;
      data() : choosing(0), clicked(-1) {}
    };

    static std::map< imgui::string, data > map;
    std::string idx = text;
    std::map<imgui::string, data>::iterator it = map.find( idx );

    if( it == map.end() )
      it = map.insert( std::pair< imgui::string, data >(idx, data() ) ).first;

    int &choosing( it->second.choosing );
    int &clicked( it->second.clicked );

    _result = List( text.c_str(), n_options, options, choosing, clicked, enabled );
    _clicked = clicked;
}

int menu::result() const { return _clicked; }
menu::operator const bool() const { return _result != 0; }
menu::menu(const imgui::string &name, const imgui::string &caption, size_t n_options, const char **options, const bool &enabled ) {

    //@todo: scroll area, list.onblur(selection.blur)
    //@todo: improve hashing (text only is too weak)

    struct data {
      int choosing, clicked;
      data() : choosing(0), clicked(-1) {}
    };

    static std::map< imgui::string, data > map;
    std::string idx = name + "|" + caption;
    std::map<imgui::string, data>::iterator it = map.find( idx );

    if( it == map.end() )
      it = map.insert( std::pair< imgui::string, data >(idx, data() ) ).first;

    int &choosing( it->second.choosing );
    int &clicked( it->second.clicked );

    _result = List( caption.c_str(), n_options, options, choosing, clicked, enabled );
    _clicked = clicked, clicked = -1;

    // close all other submenus, in case there is any open
    if( 0 )
    if( _result || choosing )
    {
        for( it = map.begin(); it != map.end(); ++it )
        {
            std::string submenu = it->first.substr(0, name.size() + 1);
            std::string family = name + "|";
            std::string self = family + caption;

            if( submenu == family && it->first != self )
                it->second.choosing = 0, it->second.clicked = -1;
        }
    }
}

pair::operator const bool() const { return true; }
pair::pair( const imgui::string &text, const imgui::string &value ) { Pair( text.c_str(), value.c_str() ); }

int radio::result() const { return _clicked; }
radio::operator const bool() const { return _result != 0; }
radio::radio(const imgui::string &text, size_t n_options, const char **options, const bool &enabled ) {

    //@todo: scroll area, list.onblur(selection.blur)
    //@todo: improve hashing (text only is too weak)

    struct data {
      int clicked;
      data() : clicked(0) {}
    };

    static std::map< std::string, data > map;
    std::string idx = text;
    std::map<std::string, data>::iterator it = map.find( idx );

    if( it == map.end() )
      it = map.insert( std::pair< imgui::string, data >(idx, data() ) ).first;

    int &clicked( it->second.clicked );

    _result = Radio( text.c_str(), n_options, options, clicked, enabled );
    _clicked = clicked;
}
radio::radio(const imgui::string &text, const imgui::strings &options, const bool &enabled ) {

    //@todo: scroll area, list.onblur(selection.blur)
    //@todo: improve hashing (text only is too weak)

    struct data {
      int clicked;
      data() : clicked(0) {}
    };

    static std::map< std::string, data > map;
    std::string idx = text;
    std::map<std::string, data>::iterator it = map.find( idx );

    if( it == map.end() )
      it = map.insert( std::pair< imgui::string, data >(idx, data() ) ).first;

    int &clicked( it->second.clicked );

    std::vector< const char * > copts( options.size() );
    for( size_t i = 0; i < options.size(); ++i )
        copts[i] = options[i].c_str();

    _result = Radio( text.c_str(), options.size(), &copts[0], clicked, enabled );
    _clicked = clicked;
}


canvas::operator const bool() const { return true; }

canvas::~canvas() {
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);
           glMatrixMode(GL_PROJECTION); glPopMatrix();
           glMatrixMode(GL_MODELVIEW);  glPopMatrix();
           glPopAttrib();
}

canvas::canvas(float x, float y, float w, float h) {
           glMatrixMode(GL_MODELVIEW); glPushMatrix();
           glMatrixMode(GL_PROJECTION); glPushMatrix();
           glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT   | GL_ENABLE_BIT  |
                        GL_TEXTURE_BIT      | GL_TRANSFORM_BIT | GL_VIEWPORT_BIT);

    float FOV = 75.0 * 0.5f;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
        float aspect = ( h == 0 ? 0 : w / h );
        float halfy = std::tan( FOV * 3.14159 / 360.0 );
        float halfx = halfy * aspect;
    //glFrustum(-halfx, halfx, -halfy, halfy, 1.0, 1000.0);

    gluPerspective(FOV, aspect, 0.0001/*std::numeric_limits<double>::epsilon()*/, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float pos[3] = { 0, 0, 0 };
    float eye[3] = { 3, 3, 3 };
    float upv[3] = { 0, 1, 0 };

    gluLookAt( eye[0], eye[1], eye[2],
               pos[0], pos[1], pos[2],
               upv[0], upv[1], upv[2] );


    glViewport(x,y,w,h);
    glScissor(x,y,w,h);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
}

glass::operator const bool() const { return true; }

glass::~glass() {
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);
           glMatrixMode(GL_PROJECTION); glPopMatrix();
           glMatrixMode(GL_MODELVIEW);  glPopMatrix();
           glPopAttrib();
}

#if 1
glass::glass(float desktopw, float desktoph, float x, float y, float w, float h, const moon9::vec3 &pos, const moon9::vec3 &eye, const moon9::vec3 &upv) {
#else
glass::glass(float desktopw, float desktoph, float x, float y, float w, float h, moon9::camera1 &cam) {
#endif
           glMatrixMode(GL_MODELVIEW); glPushMatrix();
           glMatrixMode(GL_PROJECTION); glPushMatrix();
           glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT   | GL_ENABLE_BIT  |
                        GL_TEXTURE_BIT      | GL_TRANSFORM_BIT | GL_VIEWPORT_BIT);

    float FOV = 75.0 * 0.5f;

//        GLdouble modelview[16];
//        glGetDoublev( GL_MODELVIEW_MATRIX, modelview );   //no funciona pq ahora mismo estamos en ortho

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

        GLint viewport[4];
        glGetIntegerv( GL_VIEWPORT, viewport );
        gluPickMatrix(x+w/2, y+h/2, w, h, viewport);

        float aspect = ( desktoph == 0 ? 0 : desktopw / desktoph );
        float halfy = std::tan( FOV * 3.14159 / 360.0 );
        float halfx = halfy * aspect;
    //glFrustum(-halfx, halfx, -halfy, halfy, 1.0, 1000.0);

    gluPerspective(FOV, aspect, 0.0001, 1000);

    glMatrixMode(GL_MODELVIEW);
#if 1
    glLoadIdentity();

    gluLookAt( eye.x, eye.y, eye.z,
               pos.x, pos.y, pos.z,
               upv.x, upv.y, upv.z );
#else
    using namespace nathan;
    glLoadMatrixf(mat4(cam.cam_rotation).transpose().data());
    glTranslatef(-cam.cam_pos[0], -cam.cam_pos[1], -cam.cam_pos[2]);
    //glRotatef( -90, -1, 0, 0 );
#endif

    glViewport(x,y,w,h);
    glScissor(x,y,w,h);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
}

} // ns moon9


namespace
{
    // Some math headers don't have PI defined.
    const float PI = 3.14159265f;
}


/// INTERNALS

// Pull render interface.
enum GfxCmdType
{
        GFXCMD_RECT,
        GFXCMD_TRIANGLE,
        GFXCMD_LINE,
        GFXCMD_TEXT,
        GFXCMD_SCISSOR,
};

struct GfxRect
{
        short x,y,w,h,r;
};

struct GfxText
{
        short x,y;
        imgui::TextAlign align;
        const char* text;
};

struct GfxLine
{
        short x0,y0,x1,y1,r;
};

struct GfxCmd
{
        char type;
        char flags;
        char pad[2];
        moon9::RGBA col;
        union
        {
                GfxLine line;
                GfxRect rect;
                GfxText text;
        };
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    //impl
    const unsigned TEXT_POOL_SIZE = 8000;
    char g_textPool[TEXT_POOL_SIZE];
    unsigned g_textPoolSize = 0;
    const char* allocText(const char* text)
    {
        unsigned len = strlen(text)+1;
        if (g_textPoolSize + len >= TEXT_POOL_SIZE)
                return 0;
        char* dst = &g_textPool[g_textPoolSize];
        memcpy(dst, text, len);
        g_textPoolSize += len;
        return dst;
    }

    const unsigned GFXCMD_QUEUE_SIZE = 5000;
    GfxCmd g_gfxCmdQueue[GFXCMD_QUEUE_SIZE];
    unsigned g_gfxCmdQueueSize = 0;

    void resetGfxCmdQueue()
    {
        g_gfxCmdQueueSize = 0;
        g_textPoolSize = 0;
    }

    void addGfxCmdScissor(int x, int y, int w, int h)
    {
        if (g_gfxCmdQueueSize >= GFXCMD_QUEUE_SIZE)
                return;
        GfxCmd& cmd = g_gfxCmdQueue[g_gfxCmdQueueSize++];
        cmd.type = GFXCMD_SCISSOR;
        cmd.flags = x < 0 ? 0 : 1;      // on/off flag.
        cmd.col = moon9::RGBA(0,0,0,0);
        cmd.rect.x = (short)x;
        cmd.rect.y = (short)y;
        cmd.rect.w = (short)w;
        cmd.rect.h = (short)h;
    }

    void addGfxCmdRect(float x, float y, float w, float h, const moon9::RGBA &color)
    {
        if (g_gfxCmdQueueSize >= GFXCMD_QUEUE_SIZE)
                return;
        GfxCmd& cmd = g_gfxCmdQueue[g_gfxCmdQueueSize++];
        cmd.type = GFXCMD_RECT;
        cmd.flags = 0;
        cmd.col = color;
        cmd.rect.x = (short)(x*8.0f);
        cmd.rect.y = (short)(y*8.0f);
        cmd.rect.w = (short)(w*8.0f);
        cmd.rect.h = (short)(h*8.0f);
        cmd.rect.r = 0;
    }

    void addGfxCmdLine(float x0, float y0, float x1, float y1, float r, const moon9::RGBA &color)
    {
        if (g_gfxCmdQueueSize >= GFXCMD_QUEUE_SIZE)
                return;
        GfxCmd& cmd = g_gfxCmdQueue[g_gfxCmdQueueSize++];
        cmd.type = GFXCMD_LINE;
        cmd.flags = 0;
        cmd.col = color;
        cmd.line.x0 = (short)(x0*8.0f);
        cmd.line.y0 = (short)(y0*8.0f);
        cmd.line.x1 = (short)(x1*8.0f);
        cmd.line.y1 = (short)(y1*8.0f);
        cmd.line.r = (short)(r*8.0f);
    }

    void addGfxCmdRoundedRect(float x, float y, float w, float h, float r, const moon9::RGBA &color)
    {
        if (g_gfxCmdQueueSize >= GFXCMD_QUEUE_SIZE)
                return;
        GfxCmd& cmd = g_gfxCmdQueue[g_gfxCmdQueueSize++];
        cmd.type = GFXCMD_RECT;
        cmd.flags = 0;
        cmd.col = color;
        cmd.rect.x = (short)(x*8.0f);
        cmd.rect.y = (short)(y*8.0f);
        cmd.rect.w = (short)(w*8.0f);
        cmd.rect.h = (short)(h*8.0f);
        cmd.rect.r = (short)(r*8.0f);
    }

    void addGfxCmdTriangle(int x, int y, int w, int h, int flags, const moon9::RGBA &color)
    {
        if (g_gfxCmdQueueSize >= GFXCMD_QUEUE_SIZE)
                return;
        GfxCmd& cmd = g_gfxCmdQueue[g_gfxCmdQueueSize++];
        cmd.type = GFXCMD_TRIANGLE;
        cmd.flags = (char)flags;
        cmd.col = color;
        cmd.rect.x = (short)(x*8.0f);
        cmd.rect.y = (short)(y*8.0f);
        cmd.rect.w = (short)(w*8.0f);
        cmd.rect.h = (short)(h*8.0f);
    }

    void addGfxCmdText(int x, int y, imgui::TextAlign align, const char* text, const moon9::RGBA &color)
    {
        if (g_gfxCmdQueueSize >= GFXCMD_QUEUE_SIZE)
                return;
        GfxCmd& cmd = g_gfxCmdQueue[g_gfxCmdQueueSize++];
        cmd.type = GFXCMD_TEXT;
        cmd.flags = 0;
        cmd.col = color;
        cmd.text.x = (short)x;
        cmd.text.y = (short)y;
        cmd.text.align = align;
        cmd.text.text = allocText(text);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct GuiState
    {
        GuiState() :
                left(false), leftPressed(false), leftReleased(false),
                mx(-1), my(-1), scroll(0),
                active(0), hot(0), hotToBe(0), isHot(false), isActive(false), wentActive(false),
                dragX(0), dragY(0), dragOrig(0), widgetX(0), widgetY(0), widgetW(100),
                insideCurrentScroll(false),  areaId(0), widgetId(0)
        {
        }

        bool left;
        bool leftPressed, leftReleased;
        int mx,my;
        int scroll;
        unsigned int active;
        unsigned int hot;
        unsigned int hotToBe;
        bool isHot;
        bool isActive;
        bool wentActive;
        int dragX, dragY;
        float dragOrig;
        int widgetX, widgetY, widgetW;
        bool insideCurrentScroll;

        unsigned int areaId;
        unsigned int widgetId;
    } g_state;

    inline bool anyActive()
    {
        return g_state.active != 0;
    }

    inline bool anyHot() // @rlyeh: new method
    {
        return g_state.hot != 0;
    }

    inline bool isActive(unsigned int id)
    {
        return g_state.active == id;
    }

    inline bool isHot(unsigned int id)
    {
        bool hover = (g_state.hot == id);
        moon9::mouse_is_hover |= hover;
        return hover;
    }

    inline bool inRect(int x, int y, int w, int h, bool checkScroll )
    {
        bool over = (!checkScroll || g_state.insideCurrentScroll) && g_state.mx >= x && g_state.mx <= x+w && g_state.my >= y && g_state.my <= y+h;
        return over;
    }

    inline void clearInput()
    {
        g_state.leftPressed = false;
        g_state.leftReleased = false;
        g_state.scroll = 0;
    }

    inline void clearActive()
    {
        g_state.active = 0;
        // mark all UI for this frame as processed
        clearInput();
    }

    inline void setActive(unsigned int id)
    {
        g_state.active = id;
        g_state.wentActive = true;
    }

    inline void setHot(unsigned int id)
    {
        g_state.hotToBe = id;
    }


    bool buttonLogic(unsigned int id, bool over)
    {
        bool res = false;
        // process down
        if (!anyActive())
        {
                if (over)
                        setHot(id);
                if (isHot(id) && g_state.leftPressed)
                        setActive(id);
        }

        // if button is active, then react on left up
        if (isActive(id))
        {
                g_state.isActive = true;
                if (over)
                        setHot(id);
                if (g_state.leftReleased)
                {
                        if (isHot(id))
                                res = true;
                        clearActive();
                }
        }

        if (isHot(id))
                g_state.isHot = true;

        return res;
    }

    void updateInput(int mx, int my, bool mbleft, bool mbright, int scroll)
    {
        g_state.mx = mx;
        g_state.my = my;
        g_state.leftPressed = !g_state.left &&  mbleft;
        g_state.leftReleased = g_state.left && !mbleft;
        g_state.left = mbleft;

        g_state.scroll = scroll;
    }

    const GfxCmd* GetRenderQueue()
    {
        return g_gfxCmdQueue;
    }

    int GetRenderQueueSize()
    {
        return g_gfxCmdQueueSize;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    int g_scrollTop = 0;
    int g_scrollBottom = 0;
    int g_scrollRight = 0;
    int g_scrollAreaTop = 0;
    int* g_scrollVal = 0;
    int g_focusTop = 0;
    int g_focusBottom = 0;
    unsigned int g_scrollId = 0;
    bool g_insideScrollArea = false;
}

//////////////////////////////////////
////////////////////////////////RENDER
//////////////////////////////////////

namespace
{
    const unsigned TEMP_COORD_COUNT = 100;
    float g_tempCoords[TEMP_COORD_COUNT*2];
    float g_tempNormals[TEMP_COORD_COUNT*2];

    const int CIRCLE_VERTS = 8*4;
    float g_circleVerts[CIRCLE_VERTS*2];

    stbtt_bakedchar g_cdata[96]; // ASCII 32..126 is 95 glyphs
    GLuint g_ftex = 0;

    void drawPolygon(const float* coords, unsigned numCoords, float r, const moon9::RGBA &col)
    {
        if (numCoords > TEMP_COORD_COUNT) numCoords = TEMP_COORD_COUNT;

        for (unsigned i = 0, j = numCoords-1; i < numCoords; j=i++)
        {
                const float* v0 = &coords[j*2];
                const float* v1 = &coords[i*2];
                float dx = v1[0] - v0[0];
                float dy = v1[1] - v0[1];
                float d = sqrtf(dx*dx+dy*dy);
                if (d > 0)
                {
                        d = 1.0f/d;
                        dx *= d;
                        dy *= d;
                }
                g_tempNormals[j*2+0] = dy;
                g_tempNormals[j*2+1] = -dx;
        }

        for (unsigned i = 0, j = numCoords-1; i < numCoords; j=i++)
        {
                float dlx0 = g_tempNormals[j*2+0];
                float dly0 = g_tempNormals[j*2+1];
                float dlx1 = g_tempNormals[i*2+0];
                float dly1 = g_tempNormals[i*2+1];
                float dmx = (dlx0 + dlx1) * 0.5f;
                float dmy = (dly0 + dly1) * 0.5f;
                float   dmr2 = dmx*dmx + dmy*dmy;
                if (dmr2 > 0.000001f)
                {
                        float   scale = 1.0f / dmr2;
                        if (scale > 10.0f) scale = 10.0f;
                        dmx *= scale;
                        dmy *= scale;
                }
                g_tempCoords[i*2+0] = coords[i*2+0]+dmx*r;
                g_tempCoords[i*2+1] = coords[i*2+1]+dmy*r;
        }

        moon9::RGBA colTrans = col.alpha(0); //moon9::RGBA(col&0xff, (col>>8)&0xff, (col>>16)&0xff, 0);

        glBegin(GL_TRIANGLES);

        glColor4ubv((GLubyte*)&col);

        for (unsigned i = 0, j = numCoords-1; i < numCoords; j=i++)
        {
                glVertex2fv(&coords[i*2]);
                glVertex2fv(&coords[j*2]);
                glColor4ubv((GLubyte*)&colTrans);
                glVertex2fv(&g_tempCoords[j*2]);

                glVertex2fv(&g_tempCoords[j*2]);
                glVertex2fv(&g_tempCoords[i*2]);

                glColor4ubv((GLubyte*)&col);
                glVertex2fv(&coords[i*2]);
        }

        glColor4ubv((GLubyte*)&col);
        for (unsigned i = 2; i < numCoords; ++i)
        {
                glVertex2fv(&coords[0]);
                glVertex2fv(&coords[(i-1)*2]);
                glVertex2fv(&coords[i*2]);
        }

        glEnd();
    }

    void drawRect(float x, float y, float w, float h, float fth, const moon9::RGBA &col)
    {
        float verts[4*2] =
        {
                x+0.5f, y+0.5f,
                x+w-0.5f, y+0.5f,
                x+w-0.5f, y+h-0.5f,
                x+0.5f, y+h-0.5f,
        };
        drawPolygon(verts, 4, fth, col);
    }

    /*
    void drawEllipse(float x, float y, float w, float h, float fth, unsigned int col)
    {
        float verts[CIRCLE_VERTS*2];
        const float* cverts = g_circleVerts;
        float* v = verts;

        for (int i = 0; i < CIRCLE_VERTS; ++i)
        {
                *v++ = x + cverts[i*2]*w;
                *v++ = y + cverts[i*2+1]*h;
        }

        drawPolygon(verts, CIRCLE_VERTS, fth, col);
    }
    */

    void drawRoundedRect(float x, float y, float w, float h, float r, float fth, const moon9::RGBA &col)
    {
        const unsigned n = CIRCLE_VERTS/4;
        float verts[(n+1)*4*2];
        const float* cverts = g_circleVerts;
        float* v = verts;

        for (unsigned i = 0; i <= n; ++i)
        {
                *v++ = x+w-r + cverts[i*2]*r;
                *v++ = y+h-r + cverts[i*2+1]*r;
        }

        for (unsigned i = n; i <= n*2; ++i)
        {
                *v++ = x+r + cverts[i*2]*r;
                *v++ = y+h-r + cverts[i*2+1]*r;
        }

        for (unsigned i = n*2; i <= n*3; ++i)
        {
                *v++ = x+r + cverts[i*2]*r;
                *v++ = y+r + cverts[i*2+1]*r;
        }

        for (unsigned i = n*3; i < n*4; ++i)
        {
                *v++ = x+w-r + cverts[i*2]*r;
                *v++ = y+r + cverts[i*2+1]*r;
        }
        *v++ = x+w-r + cverts[0]*r;
        *v++ = y+r + cverts[1]*r;

        drawPolygon(verts, (n+1)*4, fth, col);
    }


    void drawLine(float x0, float y0, float x1, float y1, float r, float fth, const moon9::RGBA &col)
    {
        float dx = x1-x0;
        float dy = y1-y0;
        float d = sqrtf(dx*dx+dy*dy);
        if (d > 0.0001f)
        {
                d = 1.0f/d;
                dx *= d;
                dy *= d;
        }
        float nx = dy;
        float ny = -dx;
        float verts[4*2];
        r -= fth;
        r *= 0.5f;
        if (r < 0.01f) r = 0.01f;
        dx *= r;
        dy *= r;
        nx *= r;
        ny *= r;

        verts[0] = x0-dx-nx;
        verts[1] = y0-dy-ny;

        verts[2] = x0-dx+nx;
        verts[3] = y0-dy+ny;

        verts[4] = x1+dx+nx;
        verts[5] = y1+dy+ny;

        verts[6] = x1+dx-nx;
        verts[7] = y1+dy-ny;

        drawPolygon(verts, 4, fth, col);
    }

    void getBakedQuad(stbtt_bakedchar *chardata, int pw, int ph, int char_index,
                                                     float *xpos, float *ypos, stbtt_aligned_quad *q)
    {
        stbtt_bakedchar *b = chardata + char_index;
        int round_x = STBTT_ifloor(*xpos + b->xoff);
        int round_y = STBTT_ifloor(*ypos - b->yoff);

        q->x0 = (float)round_x;
        q->y0 = (float)round_y;
        q->x1 = (float)round_x + b->x1 - b->x0;
        q->y1 = (float)round_y - b->y1 + b->y0;

        q->s0 = b->x0 / (float)pw;
        q->t0 = b->y0 / (float)pw;
        q->s1 = b->x1 / (float)ph;
        q->t1 = b->y1 / (float)ph;

        *xpos += b->xadvance;
    }

    const float g_tabStops[4] = {150, 210, 270, 330};

    float getTextLength(stbtt_bakedchar *chardata, const char* text)
    {
        float xpos = 0;
        float len = 0;
        while (*text)
        {
                int c = (unsigned char)*text;
                if (c == '\t')
                {
                        for (int i = 0; i < 4; ++i)
                        {
                                if (xpos < g_tabStops[i])
                                {
                                        xpos = g_tabStops[i];
                                        break;
                                }
                        }
                }
                else if (c >= 32 && c < 128)
                {
                        stbtt_bakedchar *b = chardata + c-32;
                        int round_x = STBTT_ifloor((xpos + b->xoff) + 0.5);
                        len = round_x + b->x1 - b->x0 + 0.5f;
                        xpos += b->xadvance;
                }
                ++text;
        }
        return len;
    }

    void drawText(float x, float y, const char *text, int align, const moon9::RGBA &col)
    {
        if (!g_ftex) return;
        if (!text) return;

        if (align == imgui::ALIGN_CENTER)
                x -= getTextLength(g_cdata, text)/2;
        else if (align == imgui::ALIGN_RIGHT)
                x -= getTextLength(g_cdata, text);

        glColor4ub(col&0xff, (col>>8)&0xff, (col>>16)&0xff, (col>>24)&0xff);

        glEnable(GL_TEXTURE_2D);

        // assume orthographic projection with units = screen pixels, origin at top left
        glBindTexture(GL_TEXTURE_2D, g_ftex);

        glBegin(GL_TRIANGLES);

        const float ox = x;

        while (*text)
        {
                int c = (unsigned char)*text;
                if (c == '\t')
                {
                        for (int i = 0; i < 4; ++i)
                        {
                                if (x < g_tabStops[i]+ox)
                                {
                                        x = g_tabStops[i]+ox;
                                        break;
                                }
                        }
                }
                else if (c >= 32 && c < 128)
                {
                        stbtt_aligned_quad q;
                        getBakedQuad(g_cdata, 512,512, c-32, &x,&y,&q);

                        glTexCoord2f(q.s0, q.t0);
                        glVertex2f(q.x0, q.y0);
                        glTexCoord2f(q.s1, q.t1);
                        glVertex2f(q.x1, q.y1);
                        glTexCoord2f(q.s1, q.t0);
                        glVertex2f(q.x1, q.y0);

                        glTexCoord2f(q.s0, q.t0);
                        glVertex2f(q.x0, q.y0);
                        glTexCoord2f(q.s0, q.t1);
                        glVertex2f(q.x0, q.y1);
                        glTexCoord2f(q.s1, q.t1);
                        glVertex2f(q.x1, q.y1);
                }
                ++text;
        }

        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
}

// UI
/////////////////////

namespace imgui
{
    using namespace moon9;

    bool BeginScrollArea(const char* name, int x, int y, int w, int h, int* scroll)
    {
        g_state.areaId++;
        g_state.widgetId = 0;
        g_scrollId = (g_state.areaId<<16) | g_state.widgetId;

        g_state.widgetX = x + SCROLL_AREA_PADDING;
        g_state.widgetY = y+h-AREA_HEADER + (scroll ? *scroll : 0); // @rlyeh: support for fixed areas
        g_state.widgetW = w - SCROLL_AREA_PADDING*4;
        g_scrollTop = y-AREA_HEADER+h;
        g_scrollBottom = y+SCROLL_AREA_PADDING;
        g_scrollRight = x+w - SCROLL_AREA_PADDING*3;
        g_scrollVal = scroll;

        g_scrollAreaTop = g_state.widgetY;

        g_focusTop = y-AREA_HEADER;
        g_focusBottom = y-AREA_HEADER+h;

        g_insideScrollArea = inRect(x, y, w, h, false);
        g_state.insideCurrentScroll = g_insideScrollArea;

        addGfxCmdRoundedRect((float)x, (float)y, (float)w, (float)h, 6, /*g_insideScrollArea ? tone.saturation(0.05).alpha(0.5) :*/ background );

        addGfxCmdText(x+AREA_HEADER/2, y+h-AREA_HEADER/2-TEXT_HEIGHT/2, ALIGN_LEFT, name, /*!g_insideScrollArea ?  theme.alpha(0.75) :*/ tone.alpha(1.0) );

        if( g_scrollVal ) // @rlyeh: support for fixed areas
              addGfxCmdScissor(
              x < 0 ? 0 : x+SCROLL_AREA_PADDING, //@rlyeh: fix scissor clipping problem when scroll area is on left rect client
              y+SCROLL_AREA_PADDING,
              w-SCROLL_AREA_PADDING*4 + ( x < 0 ? x : 0 ),   // @rlyeh: small optimization; @todo: on the right as well
              h > (AREA_HEADER + SCROLL_AREA_PADDING ) ? h - (AREA_HEADER + SCROLL_AREA_PADDING) : h ); // @rlyeh: fix when collapsing areas ( h <= AREA_HEADER )

        return g_insideScrollArea;
    }

    void EndScrollArea()
    {
        if( g_scrollVal ) // @rlyeh: support for fixed areas
            // Disable scissoring.
            addGfxCmdScissor(-1,-1,-1,-1);

        // Draw scroll bar
        int x = g_scrollRight+SCROLL_AREA_PADDING/2;
        int y = g_scrollBottom;
        int w = SCROLL_AREA_PADDING*2;
        int h = g_scrollTop - g_scrollBottom;

        int stop = g_scrollAreaTop;
        int sbot = g_state.widgetY;
        int sh = stop - sbot; // The scrollable area height.

        float barHeight = (float)h/(float)sh;

        if (h > AREA_HEADER && barHeight < 1) // @rlyeh: fix when area size is too small
        {
                float barY = (float)(y - sbot)/(float)sh;
                if (barY < 0) barY = 0;
                if (barY > 1) barY = 1;

                // Handle scroll bar logic.
                unsigned int hid = g_scrollId;
                int hx = x;
                int hy = y + (int)(barY*h);
                int hw = w;
                int hh = (int)(barHeight*h);

                const int range = h - (hh-1);
                bool over  = inRect(hx, hy, hw, hh, true);
                buttonLogic(hid, over);
                float u = (float)(hy-y) / (float)range;
                if (isActive(hid))
                {
                        if (g_state.wentActive)
                        {
                                g_state.dragY = g_state.my;
                                g_state.dragOrig = u;
                        }
                        if (g_state.dragY != g_state.my)
                        {
                                u = g_state.dragOrig + (g_state.my - g_state.dragY) / (float)range;
                                if (u < 0) u = 0;
                                if (u > 1) u = 1;
                                *g_scrollVal = (int)((1-u) * (sh - h));
                        }
                }
                else if(u<=0||u>1) *g_scrollVal = (int)(sh-h);  //rlyeh: @fix when resizing windows

                // BG
                addGfxCmdRoundedRect((float)x, (float)y, (float)w, (float)h, (float)w/2-1, black.alpha(0.75) );
                // Bar
                if (isActive(hid))
                        addGfxCmdRoundedRect((float)hx, (float)hy, (float)hw, (float)hh, (float)w/2-1, tone.alpha(0.75) );
                else
                        addGfxCmdRoundedRect((float)hx, (float)hy, (float)hw, (float)hh, (float)w/2-1, isHot(hid) ? tone.alpha(0.375) : theme.alpha(0.25) );

                // Handle mouse scrolling.
                if (g_insideScrollArea) // && !anyActive())
                {
                        if (g_state.scroll)
                        {
                                *g_scrollVal += 20*g_state.scroll;
                                if (*g_scrollVal < 0) *g_scrollVal = 0;
                                if (*g_scrollVal > (sh - h)) *g_scrollVal = (sh - h);
                        }
                }
        }
        else *g_scrollVal = 0; // @rlyeh: fix for mismatching scroll when collapsing/uncollapsing content larger than container height
        g_state.insideCurrentScroll = false;
    }

    bool Button(const char* text, bool enabled)
    {
        g_state.widgetId++;
        unsigned int id = (g_state.areaId<<16) | g_state.widgetId;

        int x = g_state.widgetX;
        int y = g_state.widgetY - BUTTON_HEIGHT;
        int w = g_state.widgetW;
        int h = BUTTON_HEIGHT;
        g_state.widgetY -= BUTTON_HEIGHT + DEFAULT_SPACING;

        bool over = enabled && inRect(x, y, w, h, true);
        bool res = buttonLogic(id, over);

        addGfxCmdRoundedRect((float)x, (float)y, (float)w, (float)h, (float)BUTTON_HEIGHT/2-1, isActive(id) ? gray.alpha(0.5) : gray.alpha(0.375) );
        if (enabled)
                addGfxCmdText(x+BUTTON_HEIGHT/2, y+BUTTON_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_LEFT, text, isHot(id) ? tone.alpha(1.0) : theme.alpha(0.75) );
        else
                addGfxCmdText(x+BUTTON_HEIGHT/2, y+BUTTON_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_LEFT, text, gray.alpha(0.75) );

        return res;
    }

    bool Item(const char* text, bool enabled)
    {
        g_state.widgetId++;
        unsigned int id = (g_state.areaId<<16) | g_state.widgetId;

        int x = g_state.widgetX;
        int y = g_state.widgetY - BUTTON_HEIGHT;
        int w = g_state.widgetW;
        int h = BUTTON_HEIGHT;
        g_state.widgetY -= BUTTON_HEIGHT + DEFAULT_SPACING;

        bool over = enabled && inRect(x, y, w, h, true);
        bool res = buttonLogic(id, over);

        if (isHot(id))
                addGfxCmdRoundedRect((float)x, (float)y, (float)w, (float)h, 2.0f, isActive(id) ? tone.alpha(0.75) : tone.alpha(0.375) );

        if (enabled)
                addGfxCmdText(x+BUTTON_HEIGHT/2, y+BUTTON_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_LEFT, text, theme.alpha(0.75) );
        else
                addGfxCmdText(x+BUTTON_HEIGHT/2, y+BUTTON_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_LEFT, text, gray.alpha(0.75) );

        return res;
    }

    bool Check(const char* text, bool checked, bool enabled)
    {
        g_state.widgetId++;
        unsigned int id = (g_state.areaId<<16) | g_state.widgetId;

        int x = g_state.widgetX;
        int y = g_state.widgetY - BUTTON_HEIGHT;
        int w = g_state.widgetW;
        int h = BUTTON_HEIGHT;
        g_state.widgetY -= BUTTON_HEIGHT + DEFAULT_SPACING;

        bool over = enabled && inRect(x, y, w, h, true);
        bool res = buttonLogic(id, over);

        const int cx = x+BUTTON_HEIGHT/2-CHECK_SIZE/2;
        const int cy = y+BUTTON_HEIGHT/2-CHECK_SIZE/2;
        addGfxCmdRoundedRect((float)cx-3, (float)cy-3, (float)CHECK_SIZE+6, (float)CHECK_SIZE+6, 4,  isActive(id) ? gray.alpha(0.5) : gray.alpha(0.375) );
        if (checked)
        {
                if (enabled)
                        addGfxCmdRoundedRect((float)cx, (float)cy, (float)CHECK_SIZE, (float)CHECK_SIZE, (float)CHECK_SIZE/2-1, isActive(id) ? theme.alpha(1.0) : theme.alpha(0.75));
                else
                        addGfxCmdRoundedRect((float)cx, (float)cy, (float)CHECK_SIZE, (float)CHECK_SIZE, (float)CHECK_SIZE/2-1, gray.alpha(0.75) );
        }

        if (enabled)
                addGfxCmdText(x+BUTTON_HEIGHT, y+BUTTON_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_LEFT, text, isHot(id) ? tone.alpha(1.0) : theme.alpha(0.75) );
        else
                addGfxCmdText(x+BUTTON_HEIGHT, y+BUTTON_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_LEFT, text, gray.alpha(0.75) );

        return res;
    }

    bool Collapse(const char* text, const char* subtext, bool checked, bool enabled)
    {
        g_state.widgetId++;
        unsigned int id = (g_state.areaId<<16) | g_state.widgetId;

        int x = g_state.widgetX;
        int y = g_state.widgetY - BUTTON_HEIGHT;
        int w = g_state.widgetW;
        int h = BUTTON_HEIGHT;
        g_state.widgetY -= BUTTON_HEIGHT; // + DEFAULT_SPACING;

        const int cx = x+BUTTON_HEIGHT/2-CHECK_SIZE/2;
        const int cy = y+BUTTON_HEIGHT/2-CHECK_SIZE/2;

        bool over = enabled && inRect(x, y, w, h, true);
        bool res = buttonLogic(id, over);

        if (checked)
                addGfxCmdTriangle(cx, cy, CHECK_SIZE, CHECK_SIZE, 2, isActive(id) ? theme.alpha(1.0) : theme.alpha(0.75) );
        else
                addGfxCmdTriangle(cx, cy, CHECK_SIZE, CHECK_SIZE, 1, isActive(id) ? theme.alpha(1.0) : theme.alpha(0.75) );

        if (enabled)
                addGfxCmdText(x+BUTTON_HEIGHT, y+BUTTON_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_LEFT, text, isHot(id) ? tone.alpha(1.0) : theme.alpha(0.75) );
        else
                addGfxCmdText(x+BUTTON_HEIGHT, y+BUTTON_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_LEFT, text, gray.alpha(0.75) );

        if (subtext)
                addGfxCmdText(x+w-BUTTON_HEIGHT/2, y+BUTTON_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_RIGHT, subtext, theme.alpha(0.5));

        return res;
    }

    void Label(const char* text)
    {
        int x = g_state.widgetX;
        int y = g_state.widgetY - BUTTON_HEIGHT;
        g_state.widgetY -= BUTTON_HEIGHT;
        addGfxCmdText(x, y+BUTTON_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_LEFT, text, theme.alpha(1.0) );
    }

    void Value(const char* text)
    {
        const int x = g_state.widgetX;
        const int y = g_state.widgetY - BUTTON_HEIGHT;
        const int w = g_state.widgetW;
        g_state.widgetY -= BUTTON_HEIGHT;

        addGfxCmdText(x+w-BUTTON_HEIGHT/2, y+BUTTON_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_RIGHT, text, theme.alpha(0.75) );
    }

    void Pair(const char* text, const char *value)  // @rlyeh: new widget
    {
        Label(text);
        g_state.widgetY += BUTTON_HEIGHT;
        Value(value);
    }

    bool List(const char* text, size_t n_options, const char** options, int &choosing, int &clicked, bool enabled) // @rlyeh: new widget
    {
        g_state.widgetId++;
        unsigned int id = (g_state.areaId<<16) | g_state.widgetId;

        int x = g_state.widgetX;
        int y = g_state.widgetY - BUTTON_HEIGHT;
        int w = g_state.widgetW;
        int h = BUTTON_HEIGHT;
        g_state.widgetY -= BUTTON_HEIGHT; // + DEFAULT_SPACING;

        const int cx = x+BUTTON_HEIGHT/2-CHECK_SIZE/2;
        const int cy = y+BUTTON_HEIGHT/2-CHECK_SIZE/2;

        bool over = enabled && inRect(x, y, w, h, true);
        bool res = buttonLogic(id, over);

        if (enabled)
        {
            addGfxCmdRoundedRect((float)x, (float)y, (float)w, (float)h, 2.0f, !isHot(id) ? gray.alpha(0.25) : (isActive(id) ? tone.alpha(0.75) : tone.alpha(choosing ? 0.25 : 0.375)) );

            addGfxCmdTriangle((float)cx, (float)cy, CHECK_SIZE, CHECK_SIZE, choosing ? 2 : 1, isActive(id) ? theme.alpha(1.0) : theme.alpha(0.75) );

            addGfxCmdText(x+BUTTON_HEIGHT, y+BUTTON_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_LEFT, clicked < 0 ? text : options[clicked], theme.alpha(0.75) );

            //addGfxCmdRoundedRect(x+w-BUTTON_HEIGHT/2, y+BUTTON_HEIGHT/2-TEXT_HEIGHT/2, (float)CHECK_SIZE, (float)CHECK_SIZE, (float)CHECK_SIZE/2-1, isActive(id) ? theme.alpha(1.0) : theme.alpha(0.75));

            if( res )
                choosing ^= 1;
        }
        else
        {
            addGfxCmdRoundedRect((float)x, (float)y, (float)w, (float)h, 2.0f, gray.alpha(0.25) );

            addGfxCmdTriangle((float)cx, (float)cy, CHECK_SIZE, CHECK_SIZE, choosing ? 2 : 1, isActive(id) ? theme.alpha(1.0) : theme.alpha(0.75) );

            addGfxCmdText(x+BUTTON_HEIGHT, y+BUTTON_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_LEFT, clicked < 0 ? text : options[clicked], gray.alpha(0.75) );

            //addGfxCmdRoundedRect(x+w-BUTTON_HEIGHT/2, y+BUTTON_HEIGHT/2-TEXT_HEIGHT/2, (float)CHECK_SIZE, (float)CHECK_SIZE, (float)CHECK_SIZE/2-1, isActive(id) ? theme.alpha(1.0) : theme.alpha(0.75));
        }

        bool result = false;

        if( choosing )
        {
            // choice selector
            Indent();
                // hotness = are we on focus?
                bool hotness = isHot(id) | isActive(id);
                // choice selector list
                for( size_t n = 0; !result && n < n_options; ++n )
                {
                    // got answer?
                    if( Item( options[n], enabled ) )
                        clicked = n, choosing = 0, result = true;

                    unsigned int id = (g_state.areaId<<16) | g_state.widgetId;

                    // ensure that widget is still on focus while choosing
                    hotness |= isHot(id) | isActive(id);
                }
                // close on blur
                if( !hotness && anyHot() )
                {}//    choosing = 0;
            Unindent();
        }

        return result;
    }

    bool Radio(const char* text, size_t n_options, const char** options, int &clicked, bool enabled) // @rlyeh: new widget
    {
        g_state.widgetId++;
        unsigned int id = (g_state.areaId<<16) | g_state.widgetId;

        int x = g_state.widgetX;
        int y = g_state.widgetY - BUTTON_HEIGHT;
        int w = g_state.widgetW;
        int h = BUTTON_HEIGHT;
        g_state.widgetY -= BUTTON_HEIGHT; // + DEFAULT_SPACING;

        const int cx = x+BUTTON_HEIGHT/2-CHECK_SIZE/2;
        const int cy = y+BUTTON_HEIGHT/2-CHECK_SIZE/2;

        bool over = enabled && inRect(x, y, w, h, true);
        bool res = buttonLogic(id, over);

        if (enabled)
                addGfxCmdText(cx, cy, ALIGN_LEFT, text, isHot(id) ? tone.alpha(1.0) : theme.alpha(0.75) );
        else
                addGfxCmdText(cx, cy, ALIGN_LEFT, text, gray.alpha(0.75) );

        bool result = false;

        Indent();
            for( size_t i = 0; i < n_options; ++i )
            {
                bool cl = ( clicked == i );
                if( Check( options[i], cl, enabled ) )
                    clicked = i, result = true;
            }
        Unindent();

        return result;
    }

    void ProgressBar(const char* text, float val, bool show_decimals) // @rlyeh: new widget
    {
        const float vmin = 0.00f, vmax = 100.00f;

        if( val < 0.f ) val = 0.f; else if( val > 100.f ) val = 100.f;

        g_state.widgetId++;
        unsigned int id = (g_state.areaId<<16) | g_state.widgetId;

        int x = g_state.widgetX;
        int y = g_state.widgetY - BUTTON_HEIGHT;
        int w = g_state.widgetW;
        int h = SLIDER_HEIGHT;
        g_state.widgetY -= SLIDER_HEIGHT + DEFAULT_SPACING;

        addGfxCmdRoundedRect((float)x, (float)y, (float)w, (float)h, 4.0f, black.alpha(0.375) );

        const int range = w - SLIDER_MARKER_WIDTH;

        float u = (val - vmin) / (vmax-vmin);
        if (u < 0) u = 0;
        if (u > 1) u = 1;
        int m = (int)(u * range);

        addGfxCmdRoundedRect((float)(x+0), (float)y, (float)(SLIDER_MARKER_WIDTH+m), (float)SLIDER_HEIGHT, 4.0f, theme.alpha(0.25) );

        // TODO: fix this, take a look at 'nicenum'.
        int digits = (int)(std::ceilf(std::log10f(0.01f)));
        char msg[128];
        if( show_decimals )
        sprintf(msg, "%.*f%%", digits >= 0 ? 0 : -digits, val);
        else
        sprintf(msg, "%d%%", int(val) );

        addGfxCmdText(x+SLIDER_HEIGHT/2, y+SLIDER_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_LEFT, text, theme.alpha(0.75) );
        addGfxCmdText(x+w-SLIDER_HEIGHT/2, y+SLIDER_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_RIGHT, msg, theme.alpha(0.75) );
    }

    bool Slider(const char* text, float* val, float vmin, float vmax, float vinc, bool enabled)
    {
        g_state.widgetId++;
        unsigned int id = (g_state.areaId<<16) | g_state.widgetId;

        int x = g_state.widgetX;
        int y = g_state.widgetY - BUTTON_HEIGHT;
        int w = g_state.widgetW;
        int h = SLIDER_HEIGHT;
        g_state.widgetY -= SLIDER_HEIGHT + DEFAULT_SPACING;

        addGfxCmdRoundedRect((float)x, (float)y, (float)w, (float)h, 4.0f, black.alpha(0.375) );

        const int range = w - SLIDER_MARKER_WIDTH;

        float u = (*val - vmin) / (vmax-vmin);
        if (u < 0) u = 0;
        if (u > 1) u = 1;
        int m = (int)(u * range);

        bool over = enabled && inRect(x+m, y, SLIDER_MARKER_WIDTH, SLIDER_HEIGHT, true);
        bool res = buttonLogic(id, over);
        bool valChanged = false;

        if (isActive(id))
        {
                if (g_state.wentActive)
                {
                        g_state.dragX = g_state.mx;
                        g_state.dragOrig = u;
                }
                if (g_state.dragX != g_state.mx)
                {
                        u = g_state.dragOrig + (float)(g_state.mx - g_state.dragX) / (float)range;
                        if (u < 0) u = 0;
                        if (u > 1) u = 1;
                        *val = vmin + u*(vmax-vmin);
                        *val = floorf(*val/vinc+0.5f)*vinc; // Snap to vinc
                        m = (int)(u * range);
                        valChanged = true;
                }
        }

        if (isActive(id))
                addGfxCmdRoundedRect((float)(x+m), (float)y, (float)SLIDER_MARKER_WIDTH, (float)SLIDER_HEIGHT, 4.0f, theme.alpha(1.0) );
        else
                addGfxCmdRoundedRect((float)(x+m), (float)y, (float)SLIDER_MARKER_WIDTH, (float)SLIDER_HEIGHT, 4.0f, isHot(id) ? tone.alpha(0.5) : theme.alpha(0.25) );

        // TODO: fix this, take a look at 'nicenum'.
        int digits = (int)(std::ceilf(std::log10f(vinc)));
        char msg[128];
        sprintf(msg, "%.*f", digits >= 0 ? 0 : -digits, *val); // @rlyeh: cleaner impl

        if (enabled)
        {
                addGfxCmdText(x+SLIDER_HEIGHT/2, y+SLIDER_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_LEFT, text, isHot(id) | isActive(id) ? tone.alpha(1.0) : theme.alpha(0.75) ); // @rlyeh: fix blinking colours
                addGfxCmdText(x+w-SLIDER_HEIGHT/2, y+SLIDER_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_RIGHT, msg, isHot(id) | isActive(id) ? tone.alpha(1.0) : theme.alpha(0.75) ); // @rlyeh: fix blinking colours
        }
        else
        {
                addGfxCmdText(x+SLIDER_HEIGHT/2, y+SLIDER_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_LEFT, text, gray.alpha(0.75) );
                addGfxCmdText(x+w-SLIDER_HEIGHT/2, y+SLIDER_HEIGHT/2-TEXT_HEIGHT/2, ALIGN_RIGHT, msg, gray.alpha(0.75) );
        }

        return res || valChanged;
    }


    void Indent()
    {
        g_state.widgetX += INDENT_SIZE;
        g_state.widgetW -= INDENT_SIZE;
    }

    void Unindent()
    {
        g_state.widgetX -= INDENT_SIZE;
        g_state.widgetW += INDENT_SIZE;
    }

    void HSeparator()
    {
        g_state.widgetY -= DEFAULT_SPACING*3;
    }

    void HSeparatorLine()
    {
        int x = g_state.widgetX;
        int y = g_state.widgetY - DEFAULT_SPACING*2;
        int w = g_state.widgetW;
        int h = 1;
        g_state.widgetY -= DEFAULT_SPACING*4;

        addGfxCmdRect((float)x, (float)y, (float)w, (float)h, theme.alpha(0.125) );
    }

    void VSeparator() // @rlyeh: new widget (@fixme)
    {
        const int BUTTON_WIDTH = g_state.widgetW > g_state.widgetX ? g_state.widgetW - g_state.widgetX : g_state.widgetX - g_state.widgetW;

        g_state.widgetX += BUTTON_WIDTH;
        g_state.widgetW += BUTTON_WIDTH;

        g_state.widgetY += BUTTON_HEIGHT;   // en realidad es el tamaño del ultimo widget (sacarlo de un vector)
    }

    void VSeparatorLine() // @rlyeh: new widget (@fixme)
    {
        int x = g_state.widgetX + DEFAULT_SPACING*2;
        int y = g_state.widgetY;
        int w = 1;
        int h = 100; //g_state.widgetH;

        g_state.widgetX += DEFAULT_SPACING*4;
        g_state.widgetW += DEFAULT_SPACING*4;

        addGfxCmdRect((float)x, (float)y, (float)w, (float)h, theme.alpha(0.125) );
    }

    void DrawText(int x, int y, TextAlign align, const char* text, const moon9::RGBA &color)
    {
        addGfxCmdText(x, y, align, text, color);
    }

    void DrawLine(float x0, float y0, float x1, float y1, float r, const moon9::RGBA &color)
    {
        addGfxCmdLine(x0, y0, x1, y1, r, color);
    }

    void DrawRect(float x, float y, float w, float h, const moon9::RGBA &color)
    {
        addGfxCmdRect(x, y, w, h, color);
    }

    void DrawRoundedRect(float x, float y, float w, float h, float r, const moon9::RGBA &color)
    {
        addGfxCmdRoundedRect(x, y, w, h, r, color);
    }

    size_t RenderGLInit( const void* ttf_font_data, size_t ttf_font_size )
    {
        for (int i = 0; i < CIRCLE_VERTS; ++i)
        {
                float a = (float)i/(float)CIRCLE_VERTS * PI*2;
                g_circleVerts[i*2+0] = std::cosf(a);
                g_circleVerts[i*2+1] = std::sinf(a);
        }

        // Load font.

        unsigned char* ttfBuffer = (unsigned char*)( ttf_font_data );
        if (!ttfBuffer)
        {
                return __LINE__;
        }

        if( !ttf_font_size )
        {
            return __LINE__;
        }

        unsigned char* bmap = (unsigned char*)malloc(512*512);
        if (!bmap)
        {
                free(ttfBuffer);
                return __LINE__;
        }

        stbtt_BakeFontBitmap(ttfBuffer,0, 15.0f, bmap,512,512, 32,96, g_cdata);

        glGenTextures(1, &g_ftex);
        glBindTexture(GL_TEXTURE_2D, g_ftex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512,512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        free(bmap);

        return 0;
    }

    void RenderGLDestroy()
    {
        if (g_ftex)
        {
                glDeleteTextures(1, &g_ftex);
                g_ftex = 0;
        }
    }


    void RenderGLDraw()
    {
        const GfxCmd* q = GetRenderQueue();
        int nq = GetRenderQueueSize();

        const float s = 1.0f/8.0f;

        glDisable(GL_SCISSOR_TEST);
        for (int i = 0; i < nq; ++i)
        {
                const GfxCmd& cmd = q[i];
                if (cmd.type == GFXCMD_RECT)
                {
                        if (cmd.rect.r == 0)
                        {
                                drawRect((float)cmd.rect.x*s+0.5f, (float)cmd.rect.y*s+0.5f,
                                                 (float)cmd.rect.w*s-1, (float)cmd.rect.h*s-1,
                                                 1.0f, cmd.col);
                        }
                        else
                        {
                                drawRoundedRect((float)cmd.rect.x*s+0.5f, (float)cmd.rect.y*s+0.5f,
                                                                (float)cmd.rect.w*s-1, (float)cmd.rect.h*s-1,
                                                                (float)cmd.rect.r*s, 1.0f, cmd.col);
                        }
                }
                else if (cmd.type == GFXCMD_LINE)
                {
                        drawLine(cmd.line.x0*s, cmd.line.y0*s, cmd.line.x1*s, cmd.line.y1*s, cmd.line.r*s, 1.0f, cmd.col);
                }
                else if (cmd.type == GFXCMD_TRIANGLE)
                {
                        if (cmd.flags == 1) // right
                        {
                                const float verts[3*2] =
                                {
                                        (float)cmd.rect.x*s+0.5f,                       (float)cmd.rect.y*s+0.5f,
                                        (float)cmd.rect.x*s+0.5f+(float)cmd.rect.w*s-1, (float)cmd.rect.y*s+0.5f+(float)cmd.rect.h*s/2-0.5f,
                                        (float)cmd.rect.x*s+0.5f,                       (float)cmd.rect.y*s+0.5f+(float)cmd.rect.h*s-1,
                                };
                                drawPolygon(verts, 3, 1.0f, cmd.col);
                        }
                        if (cmd.flags == 2) // down
                        {
                                const float verts[3*2] =
                                {
                                        (float)cmd.rect.x*s+0.5f,                            (float)cmd.rect.y*s+0.5f+(float)cmd.rect.h*s-1,
                                        (float)cmd.rect.x*s+0.5f+(float)cmd.rect.w*s/2-0.5f, (float)cmd.rect.y*s+0.5f,
                                        (float)cmd.rect.x*s+0.5f+(float)cmd.rect.w*s-1,      (float)cmd.rect.y*s+0.5f+(float)cmd.rect.h*s-1,
                                };
                                drawPolygon(verts, 3, 1.0f, cmd.col);
                        }
                        if (cmd.flags == 3) // @rlyeh new rotation: left
                        {
                                const float verts[3*2] =
                                {
                                        (float)cmd.rect.x*s+0.5f+(float)cmd.rect.w*s-1, (float)cmd.rect.y*s+0.5f,
                                        (float)cmd.rect.x*s+0.5f+(float)cmd.rect.w*s-1, (float)cmd.rect.y*s+0.5f+(float)cmd.rect.h*s-1,
                                        (float)cmd.rect.x*s+0.5f,                       (float)cmd.rect.y*s+0.5f+(float)cmd.rect.h*s/2-0.5f,
                                };
                                drawPolygon(verts, 3, 1.0f, cmd.col);
                        }
                }
                else if (cmd.type == GFXCMD_TEXT)
                {
                        drawText(cmd.text.x, cmd.text.y, cmd.text.text, cmd.text.align, cmd.col);
                }
                else if (cmd.type == GFXCMD_SCISSOR)
                {
                        if (cmd.flags)
                        {
                                glEnable(GL_SCISSOR_TEST);
                                glScissor(cmd.rect.x, cmd.rect.y, cmd.rect.w, cmd.rect.h);
                        }
                        else
                        {
                                glDisable(GL_SCISSOR_TEST);
                        }
                }
        }
        glDisable(GL_SCISSOR_TEST);
    }

    void BeginFrame(int mx, int my, bool mbleft, bool mbright, int scroll)
    {
        updateInput(mx,my,mbleft,mbright,scroll);

        g_state.hot = g_state.hotToBe;
        g_state.hotToBe = 0;

        g_state.wentActive = false;
        g_state.isActive = false;
        g_state.isHot = false;

        g_state.widgetX = 0;
        g_state.widgetY = 0;
        g_state.widgetW = 0;

        g_state.areaId = 1;
        g_state.widgetId = 1;

        resetGfxCmdQueue();
    }

    void EndFrame()
    {
        clearInput();
    }
}
