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

#pragma once

#include <map>
#include <vector>
#include <string>
#include <sstream>

#include <moon9/spatial/vec.hpp> // vec3, for glass class

//#include <moon9/render/camera.hpp> //remove this from here

namespace moon9
{
    struct RGBA
    {
        unsigned color;

        public:

        RGBA();
        RGBA(unsigned rgba);
        RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

        operator const unsigned&() const;
        operator       unsigned&();

        RGBA alpha( float a ) const;
        RGBA saturation( float factor ) const;

        // and, or
        // subtract, multiply
    };

    unsigned RGBAf( float r, float g, float b, float a = 1.f );

    extern RGBA black;
    extern RGBA white;

    namespace default_colour_scheme
    {
        extern RGBA   gray; // gray
        extern RGBA   blue; // blue
        extern RGBA   cyan; // cyan/turquoise
        extern RGBA   pink; // pink
        extern RGBA orange; // orange
        extern RGBA  green; // green/lime
        extern RGBA purple; // purple
        extern RGBA yellow; // yellow
        extern RGBA    red; // red
    }

    using namespace default_colour_scheme;
}

namespace imgui
{
    class string : public std::string
    {
        public:

        string() : std::string()
        {}

        string( const std::string &s ) : std::string(s)
        {}

        template<typename T>
        explicit
        string( const T &t ) : std::string()
        {
            std::stringstream ss;
            if( ss << t )
                this->assign( ss.str() );
        }

        template<>
        explicit
        string( const bool &b ) : std::string( b ? "true" : "false" )
        {}

        operator std::string() const
        {
            return *this;
        }

        operator const char*() const
        {
            return this->c_str();
        }
    };

    enum
    {
        // constants
        BUTTON_HEIGHT = 20,
        SLIDER_HEIGHT = 20,
        SLIDER_MARKER_WIDTH = 10,
        CHECK_SIZE = 8,
        DEFAULT_SPACING = 4,
        TEXT_HEIGHT = 8,
        SCROLL_AREA_PADDING = 6,
        INDENT_SIZE = 16,
        AREA_HEADER = 28
    };

    // C interface

    // library interface

    size_t RenderGLInit(const void* ttf_font_data, size_t ttf_font_size);
    // frame control [...] {
    void BeginFrame(int mx, int my, bool mbleft, bool mbright, int scroll);
    void EndFrame();
    void RenderGLDraw();
    // }
    void RenderGLDestroy();

            // high level, group

            bool BeginScrollArea(const char* name, int x, int y, int w, int h, int* scroll = 0); // @rlyeh, fixed area support
            void EndScrollArea();

                    // layout

                    void Indent();
                    void Unindent();
                    void HSeparator();
                    void HSeparatorLine();
                    void VSeparator();
                    void VSeparatorLine();

                    // widgets

                    bool Button(const char* text, bool enabled = true);
                    bool Check(const char* text, bool checked, bool enabled = true);
                    bool Collapse(const char* text, const char* subtext, bool checked, bool enabled = true);
                    bool Item(const char* text, bool enabled = true);
                    void Label(const char* text);
                    bool List(const char* text, size_t n_options, const char** options, int &choosing, int &clicked, bool enabled);
                    void Pair(const char *text, const char *value);
                    void ProgressBar(const char* text, float val, bool show_decimals = false);
                    bool Radio(const char* text, size_t n_options, const char** options, int &clicked, bool enabled);
                    bool Slider(const char* text, float* val, float vmin, float vmax, float vinc, bool enabled = true);
                    void Value(const char* text);

            // low level, drawing commands

            enum TextAlign
            {
                    ALIGN_LEFT,
                    ALIGN_CENTER,
                    ALIGN_RIGHT,
            };

            extern moon9::RGBA tone;
            extern moon9::RGBA background;

            void DrawLine(float x0, float y0, float x1, float y1, float r, const moon9::RGBA &color);
            void DrawRect(float x, float y, float w, float h, const moon9::RGBA &color);
            void DrawRoundedRect(float x, float y, float w, float h, float r, const moon9::RGBA &color);
            void DrawText(int x, int y, TextAlign align, const char* text, const moon9::RGBA &color);
}

// UI
/////////////////////

namespace imgui
{
    using namespace moon9;
}

// C++ interface

namespace moon9
{
    using namespace default_colour_scheme;
    using namespace imgui;

    class ui
    {
         public:
         ui( int width, int height, int mouse_x, int mouse_y, bool mouse_button_left, bool mouse_button_right, const void *ttf_font_data = 0, const size_t ttf_font_size = 0 );
        ~ui();
         operator const bool() const;
    };

    class color
    {
        public:

         color( float r, float g, float b, float a = 1.f );
        ~color();
    };

    class box
    {
        public:

        struct self_data
        {
            std::string name;

            float opacity;
            bool constrained;
            bool physics;

            float
                phx, phy,   // physical coordinates in screen/area
                phw, phh,
                vx, vy,     // velocity
                vw, vh;

            bool is_attached;
            float
                dx, dy,
                dw, dh;              // width & height when docked
            std::string attached_to; // box to attach to (pointer would be better?)

            int x,y,w,h,ox,oy,ow,oh,scroll;
            moon9::RGBA color_def, color_bkp;

            bool init;
            bool hover, enabled;

            bool moveable, resizable, minimizable, scrollable;
            bool is_moving, is_resizing, is_collapsing;

            bool debug, debug_verbose, reflexion;

            enum action
            {
                LOAD,
                SAVE,
                RETRIEVE
            };

            protected: friend class box;

            void defaults( std::string name = "undefined", int x = 0, int y = 0, int w = 200, int h = 350, int scroll = 0 );
            void erase( std::string ext );
            void load( std::string ext );
            void save( std::string ext );
            std::map< imgui::string, self_data > &persist( action to_do );

        } self;

        void cascade_all( std::vector<std::string> boxnames );
        void mosaic_all( std::vector<std::string> boxnames );
        void tile_all( std::vector<std::string> boxnames );
        void expose_all( std::vector<std::string> boxnames );
        void normalize_all( std::vector<std::string> boxnames );
        void arrange_all( std::vector<std::string> boxnames );
        void minimize_all( std::vector<std::string> boxnames );

        explicit box( const imgui::string &name, int x, int y, int w, int h, int scroll = 0 );

         box( const box &t );
        ~box();

        box &locate( float x, float y );
        box &size( float w, float h );
        box &relocate( float offx, float offy );
        // 1,7
        box &resize_9( float offw, float offh ); //rescale = * ? /
        box &resize_3( float offw, float offh ); /*rescale = * ?  */
        box &reset_forces();
        box &reset_location();
        box &reset_size();
        box &enabled( bool on = true );
        box &resizable( bool on = true );
        box &moveable( bool on = true );
        box &constrained( bool on = true );
        box &physics( bool on = true );
        box &minimizable( bool on = true );
        box &reflexion( bool on = true );

        // dock

        box &attach( std::string name, int dx, int dy );
        box &detach( bool from_current_position = true );
        box &minimize( int dw, int dh );
        box &maximize();


        box &color( RGBA color );
        bool is_hover() const;
        bool is_attached() const;

        operator const bool() const;
    };

    // Layout, identation

    class tab      { public:
          operator const bool()  const;
          tab();
         ~tab();
    };

    class hbar     { public:
          operator const bool() const;
          hbar();
    };

    class hbreak   { public:
          operator const bool() const;
          hbreak();
    };

    class vbar     { public: //@todo: fixme
          operator const bool() const;
          vbar();
    };

    class vbreak   { public: //@todo: fixme
          operator const bool() const;
          vbreak();
    };

    // Basic widgets

    class label { public:
          operator const bool() const;
          label( const imgui::string &text );
    };

    class value { public:
          operator const bool() const;
          value( const imgui::string &text );
    };

    class push {
          bool result; public: operator const bool() const;
          push( const imgui::string &text, const bool &enabled = true );
    };

    class toggle {
          bool result; public: operator const bool() const;
          toggle( const imgui::string &text, bool &checked, const bool &enabled = true );
    };

    class item {
          bool result; public: operator const bool() const;
          item( const imgui::string &text, const bool &enabled = true );
    };

    class slider {
          bool result; public: operator const bool() const;
          slider( const imgui::string &text, float min, float &value, float max, float inc, const bool &enabled = true );
          slider( const imgui::string &text, float min, float &value, float max, const bool &enabled = true );
    };

    class delta {
          bool result; public: operator const bool() const;
          delta( const imgui::string &text, float &value, float inc, const bool &enabled = true );
          delta( const imgui::string &text, float &value, const bool &enabled = true );
    };

    class collapse {
          bool result; public: operator const bool() const;
          collapse( const imgui::string &text, const imgui::string &subtext = imgui::string(), const bool &open_as_default = true, const bool &enabled = true );
    };

    class debug { public:
          template<typename T> debug( const imgui::string &text, const T &t ) { Pair( text, imgui::string(t) ); }
    };

    class progressbar { public:
          operator const bool() const;
          progressbar( const imgui::string &text, const float &val, const bool &show_decimals = false );
    };

    class list {
          int _result, _clicked;
          public:
          int result() const;
          operator const bool() const;
          list(const imgui::string &text, size_t n_options, const char **options, const bool &enabled = true);
    };

    class menu {
          int _result, _clicked;
          public:
          int result() const;
          operator const bool() const;
          menu(const imgui::string &name, const imgui::string &caption, size_t n_options, const char **options, const bool &enabled = true);
    };

    class pair {
          public:
          operator const bool() const;
          pair( const imgui::string &text, const imgui::string &value );
    };

    class radio {
          int _result, _clicked;
          public:
          int result() const;
          operator const bool() const;
          radio(const imgui::string &text, const imgui::strings &options, const bool &enabled = true);
          radio(const imgui::string &text, size_t n_options, const char **options, const bool &enabled = true);
    };

    class canvas { public:
            operator const bool() const;
            ~canvas();
             canvas(float x, float y, float w, float h);
            // @todo: ensure test quality vs this method http://www.bramstein.com/articles/openglcanvas.html (~quite similar)
    };

#if 0
    // not working :P
    class canvas2d { public:

            // @todo: ensure test quality vs this method http://www.bramstein.com/articles/openglcanvas.html (~quite similar)

            operator const bool() const { return true; }

            ~canvas2d() {
                //glDisable(GL_DEPTH_TEST);
                       glMatrixMode(GL_PROJECTION); glPopMatrix();
                       glMatrixMode(GL_MODELVIEW);  glPopMatrix();
                       glPopAttrib();
            }

            canvas2d(float x, float y, float w, float h) {
                       glMatrixMode(GL_MODELVIEW); glPushMatrix();
                       glMatrixMode(GL_PROJECTION); glPushMatrix();
                       glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT   | GL_ENABLE_BIT  |
                                    GL_TEXTURE_BIT      | GL_TRANSFORM_BIT | GL_VIEWPORT_BIT);

//                glViewport(x,y,w,h);

                glMatrixMode ( GL_PROJECTION) ;
                glLoadIdentity ();
                glOrtho ( 0.0, w, 0.0, h, -1.0, 1.0 );
                glMatrixMode(GL_MODELVIEW);

                //glEnable(GL_DEPTH_TEST);
            }
    };
#endif

    class glass { public:
            operator const bool() const;
#if 1
            glass(float desktopw, float desktoph, float x, float y, float w, float h, const moon9::vec3 &pos, const moon9::vec3 &eye, const moon9::vec3 &upv);
#else
            glass(float desktopw, float desktoph, float x, float y, float w, float h, moon9::camera1 &cam); // this should not be here
#endif
            ~glass();
    };
}

