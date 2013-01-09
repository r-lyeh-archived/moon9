//fix transparent floor

#pragma once

#include <cmath>
#include <map>
#include <sstream>

#include "os.hpp"
/*
#include <moon9/core/dt.hpp>
#include <moon9/io/file.hpp>
#include <moon9/time/wink.hpp>
*/
#include "render.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "ubuntu-b.hpp"

#include <GL/freeglut.h>
#include <GL/glfw.h>

#include <moon9/time/wink.hpp>

#include "window.inl"

namespace moon9
{
    void init_glut();
    void init_glew();

    class miniwin
    {
        enum /*settings*/ { full_antialias = false };

        moon9::dt timer;

        public:

        size_t frames;
        float w, h, screen_w, screen_h, fps;
        bool is_fullscreen, is_closing;

        std::string title;

        double t;
        float dt;

        miniwin( const std::string _title = "moon9::window", float proportional_size = 0.85f, bool is_fullscreen = false ) : w(640), h(480), is_fullscreen(false), is_closing( false ), title( _title )
        {
            init( proportional_size, is_fullscreen );
        }

protected:

        void init( float normsize, bool fullscreen )
        {
            init_glut();

            screen_w = glutGet( GLUT_SCREEN_WIDTH );
            screen_h = glutGet( GLUT_SCREEN_HEIGHT );

            {
                // reinit :P

                normsize = ( normsize <= 0 ? 1 : normsize );

                w = screen_w * normsize;
                h = screen_h * normsize;
            }

            //wglSwapIntervalEXT(1);
            glfwOpenWindowHint( GLFW_ACCUM_RED_BITS, 16 );
            glfwOpenWindowHint( GLFW_ACCUM_GREEN_BITS, 16 );
            glfwOpenWindowHint( GLFW_ACCUM_BLUE_BITS, 16 );
            glfwOpenWindowHint( GLFW_ACCUM_ALPHA_BITS, 16 );
            if( glfwOpenWindow( w, h, 0,0,0,0,0,0, fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW ) != GL_TRUE )
            {
                std::cerr << "<moon9/render/window.hpp> says: Cant create " << w << 'x' << h << " window!" << std::endl;
                exit(1);
            }

			glfwSetWindowTitle( title.c_str() );

            resize( w, h );

            init_glew();

            glInit();
        }

        bool blur;

public:

        static void glInit()
        {
                // Enable Texture Mapping
                glEnable(GL_TEXTURE_2D);
                // Use trilinear interpolation (GL_LINEAR_MIPMAP_LINEAR)
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

                glClearColor( 1.f, 0.f, 1.f, 1.f ); //0.15f, 0.15f, 0.20f, 1.f);
                glClearDepth(1.0f);                                 // Depth Buffer Setup
                glEnable(GL_DEPTH_TEST);                            // Enables Depth Testing
                glDepthFunc(GL_LEQUAL);                             // The Type Of Depth Testing To Do
                glDepthMask(GL_TRUE);

                if( 1 )
                {
                    /*
                    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
                    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
                    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
                    GLfloat light_position[] = { 1.0, 1.0, 0.0, 0.0 };

                    glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
                    glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
                    glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
                    glLightfv (GL_LIGHT0, GL_POSITION, light_position);
*/
                    GLfloat global_ambient[] = { 0.6f, 0.6f, 1.0, 1.0 };
                    glLightModelfv (GL_LIGHT_MODEL_AMBIENT, global_ambient);

                    // Enable Lighting
                    glDisable(GL_LIGHTING);
                    glDisable(GL_LIGHT0);

                    //glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
                    // Enable Material Coloring (color texture maps)
                    //glMaterial will control the polygon's specular and emission colours and the ambient and diffuse will both be set using glColor
                    glEnable(GL_COLOR_MATERIAL);
                    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
                }


                glShadeModel(GL_SMOOTH);                            // Enable Smooth Shading


                glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice Perspective Correction

                glEnable( GL_LINE_SMOOTH );
                glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
                glLineWidth( 1.0f );
                glEnable( GL_BLEND );
                glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

                /* decal */
                glEnable(GL_LIGHTING);
                glEnable(GL_LIGHT0);
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);

                glDisable( GL_CULL_FACE ); //glDisable
                //glCullFace( GL_FRONT );
                //glCullFace( GL_BACK );

                /* small opt */

                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                glFrontFace(GL_CCW);
        }

        static std::string list_video_modes()
        {
            std::string out;

            DEVMODE DevMode;
            memset( &DevMode, 0, sizeof(DEVMODE) );
            DevMode.dmSize = sizeof(DEVMODE);

            for( int i = 0; EnumDisplaySettingsEx( NULL, i, &DevMode, 0 ) != 0; ++i )
            {
                char buffer[256];
                sprintf( buffer, "%dx%d:%d@%d\n", DevMode.dmPelsWidth, DevMode.dmPelsHeight, DevMode.dmBitsPerPel, DevMode.dmDisplayFrequency );

                out += buffer;
            }

            return out;
        }

        static const char *get_best_video_mode( size_t w = 0, size_t h = 0, size_t bpp = 0, size_t hz = 0 )
        {
            if( !w ) w = glutGet(GLUT_SCREEN_WIDTH);
            if( !h ) h = glutGet(GLUT_SCREEN_HEIGHT);
            if( !bpp ) bpp = 32;
            if( !hz ) hz = 60;

            std::map< double /*err*/, std::string /* mode */ > modes;

            DEVMODE DevMode;
            memset( &DevMode, 0, sizeof(DEVMODE) );
            DevMode.dmSize = sizeof(DEVMODE);

            for( int i = 0; EnumDisplaySettingsEx( NULL, i, &DevMode, 0 ) != 0; ++i )
            {
                double err = 0, acc;
                acc = (  hz - DevMode.dmDisplayFrequency ) * 1; err += acc * acc;
                acc = ( bpp -       DevMode.dmBitsPerPel ) * 10000; err += acc * acc;
                acc = (   w -        DevMode.dmPelsWidth ) * 10000 * 10000; err += acc * acc;
                acc = (   h -       DevMode.dmPelsHeight ) * 10000 * 10000 * 10000; err += acc * acc;

                char buffer[256];
                sprintf( buffer, "%dx%d:%d@%d", DevMode.dmPelsWidth, DevMode.dmPelsHeight, DevMode.dmBitsPerPel, DevMode.dmDisplayFrequency );

                ( modes[ err ] = modes[ err ] ) = buffer;
            }

            static std::string bestmatch;
            bestmatch = modes.begin()->second;

            return bestmatch.c_str();
        }

        void toggle_fullscreen()
        {}

        bool is_blur()
        {
            blur = false;

            if( !GetFocus() )
                //if( GetForegroundWindow() != GetConsoleWindow() )
                    blur = true;

            return blur;
        }

        virtual void update( double t, float dt ) {}
        virtual void render( double t, float dt ) /*const*/ {}

        virtual void update_idle( double t, float dt )       { update(t,dt); }
        virtual void render_idle( double t, float dt ) /*const*/ { render(t,dt); }

        void flush()
        {
            glfwSetWindowTitle( title.c_str() );
            glfwSwapBuffers();

            dt = timer.s();
            timer = moon9::dt();
            t += dt;

            update(t,dt);
            render(t,dt);

            int nw, nh;
            glfwGetWindowSize( &nw, &nh );
            resize( nw, nh );
        }

        bool is_open()
        {
			if( is_closing )
				return false;

			return glfwGetWindowParam( GLFW_OPENED ) == TRUE;

//            frames++;

/*
            if( win->dt.s() > 1.0 )
            {
                win->fps = win->frames / win->dt.s();

                // todo: spf too

                moon9::format title("%s // %3.2f fps", win->title.c_str(), win->fps );

                #if defined( kBooTargetWindows ) && defined( UNICODE )
                glutSetWindowTitle( (const char *)( std::wstring( title.begin(), title.end() ).c_str() ) );
                #else
                glutSetWindowTitle( title.c_str() );
                #endif

                win->frames = 0;
                win->dt.reset();
            }
*/

//            moon9::wink();

            return true;
        }

        void resize( int x, int y )
        {
            w = (x <= 0 ? 0 : x);
            h = (y <= 0 ? 1 : y);

            glfwSetWindowSize( w, h );
            glfwSetWindowPos( ( screen_w - w ) / 2, ( screen_h - h ) / 2 );
        }

        void close()
        {
            //glfwCloseWindow();
			is_closing = true;
        }
    };

    class window
    {
        enum /*settings*/ { full_antialias = false };

        public:

        size_t frames;
        float w, h, fps;
        bool is_fullscreen;

        std::string title;

        moon9::dt dt;

        moon9::camera camera;

        window( const std::string _title = "moon9::window", float normsize = 0.85f ) : id(0), bIsOpen(true), frames(0), w(640), h(480), fps(0), title( _title ), camera(w,h), is_fullscreen(false) //w(1280), h(720) //w(640), h(480)
        {
            init_glut();

            int screen_w = glutGet( GLUT_SCREEN_WIDTH );
            int screen_h = glutGet( GLUT_SCREEN_HEIGHT );

            {
                // reinit :P

                normsize = ( normsize <= 0 ? 1 : normsize );

                w = screen_w * normsize;
                h = screen_h * normsize;

                camera = moon9::camera(w,h);
            }

            /*
            GLUT_RGBA
            Bit mask to select an RGBA mode window. This is the default if neither GLUT_RGBA nor GLUT_INDEX are specified.
            GLUT_RGB
            An alias for GLUT_RGBA.
            GLUT_INDEX
            Bit mask to select a color index mode window. This overrides GLUT_RGBA if it is also specified.

            GLUT_SINGLE
            Bit mask to select a single buffered window. This is the default if neither GLUT_DOUBLE or GLUT_SINGLE are specified.
            GLUT_DOUBLE
            Bit mask to select a double buffered window. This overrides GLUT_SINGLE if it is also specified.

            GLUT_ACCUM
            Bit mask to select a window with an accumulation buffer.
            GLUT_ALPHA
            Bit mask to select a window with an alpha component to the color buffer(s).
            GLUT_DEPTH
            Bit mask to select a window with a depth buffer.
            GLUT_STENCIL
            Bit mask to select a window with a stencil buffer.
            GLUT_MULTISAMPLE
            Bit mask to select a window with multisampling support. If multisampling is not available, a non-multisampling
            window will automatically be chosen. Note: both the OpenGL client-side and server-side implementations must
            support the GLX_SAMPLE_SGIS extension for multisampling to be available.
            GLUT_STEREO
            Bit mask to select a stereo window.
            GLUT_LUMINANCE
            Bit mask to select a window with a ``luminance'' color model. This model provides the functionality of OpenGL's RGBA
            color model, but the green and blue components are not maintained in the frame buffer. Instead each pixel's red
            component is converted to an index between zero and glutGet(GLUT_WINDOW_COLORMAP_SIZE)-1 and looked up in a
            per-window color map to determine the color of pixels within the window. The initial colormap of GLUT_LUMINANCE
            windows is initialized to be a linear gray ramp, but can be modified with GLUT's colormap routines.
            */

            glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, 1 );

            try {
            // does this have currently any other implication but sharing textures?
            glutSetOption( GLUT_RENDERING_CONTEXT, GLUT_USE_CURRENT_CONTEXT );
            //glutSetOption( GLUT_RENDERING_CONTEXT, GLUT_CREATE_NEW_CONTEXT );
            //glutSetOption( GLUT_RENDERING_CONTEXT, GLUT_USE_SHARED_LISTS );
            }
            catch( ... )
            {
            std::cout << "shared rendering context not available" << std::endl;
            }


            if( full_antialias ) {
                glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA /*| GLUT_ALPHA*/ | GLUT_DEPTH | GLUT_STENCIL | GLUT_ACCUM | GLUT_MULTISAMPLE);
            }
            else {
                //glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
                glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA /*| GLUT_ALPHA*/ | GLUT_DEPTH | GLUT_STENCIL | GLUT_ACCUM );
            }


            glutInitWindowSize( w, h );
            glutInitWindowPosition( ( screen_w - w ) / 2, ( screen_h - h ) / 2 );
            id = glutCreateWindow( title.c_str() );

            instance( id, this );

            init ();
            register_callbacks();
            glutReshapeWindow( w, h );
            //}

            /*
            //moon9::shader sh( "simple shader", io9::file("min.vert").readstr(), io9::file("min.frag").readstr() );
            moon9::shader sh( "toon shader", io9::file("toonf2.vert").readstr(), io9::file("toonf2.frag").readstr() );

            if( sh.compile() )
                if( sh.run() )
                    std::cout << "- Running shader " << sh.get_name() << std::endl;
            */

            init_glew();

            //wglSwapIntervalEXT(1);

        }

        ~window()
        {
            glutDestroyWindow(id);
        }

        void toggle_fullscreen()
        {
            // real fullscreen, not working
            // it has to be done after glutinit and before registerng callbacks apparently

            if( const bool fullscreen_real = false )
            {
                if( glutGameModeGet(GLUT_GAME_MODE_ACTIVE) )
                {
                    glutLeaveGameMode();

            id = glutCreateWindow( title.c_str() );
            instance( id, this );
            register_callbacks();

                    glutSetWindow(id); // int window = glutGetWindow()
                    is_fullscreen = false;
                    return;
                }
                else
                {
                    /*
                    int width = glutGameModeGet( GLUT_GAME_MODE_WIDTH ); //useless
                    int height = glutGameModeGet( GLUT_GAME_MODE_HEIGHT ); //useless
                    int pixelDepth = glutGameModeGet( GLUT_GAME_MODE_PIXEL_DEPTH ); //useless
                    int refreshRate = glutGameModeGet( GLUT_GAME_MODE_REFRESH_RATE ); //useless
                    std::cout << moon9::string("\1x\2:\3@\4", width, height, pixelDepth, refreshRate ) << std::endl;

                    glutGameModeString( moon9::string("\1x\2:32@60", w = glutGet(GLUT_SCREEN_WIDTH), h = glutGet(GLUT_SCREEN_HEIGHT) ).c_str() );
                    */

                    //w = glutGet(GLUT_SCREEN_WIDTH);
                    //h = glutGet(GLUT_SCREEN_HEIGHT);

                    std::string mode = miniwin::get_best_video_mode((size_t)w,(size_t)h);

                    glutGameModeString( mode.c_str() );

                    std::cout << "Trying " << mode << "... ";

                    if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
                    {
                        glutDestroyWindow(id);

                        glutEnterGameMode();
            //instance( id, this );
            register_callbacks();

                        w = as<double>( tokenize( mode, "x:" )[0] );
                        h = as<double>( tokenize( mode, "x:" )[1] );
                        //glutReshapeWindow(w, h);
                        //register_callbacks();
                        //int nw = w / 2, nh = h / 2, nx = (w - nw) * 0.5, ny = (h - nh ) * 0.5;
                        //glutReshapeWindow(nw, nh);
                        //glutPositionWindow(nx, ny);
                        //glutReshapeWindow(nw, nh);
                        //glutPositionWindow(nx, ny);
                        std::cout << "OK! (" << w << ',' << h << ")" << std::endl;

                    /*
                int nw = w / 2, nh = h / 2, nx = (w - nw) * 0.5, ny = (h - nh ) * 0.5;
                glutReshapeWindow(nw, nh);
                glutPositionWindow(nx, ny);
                glutReshapeWindow(nw, nh);
                glutPositionWindow(nx, ny);
                */

                if( is_fullscreen ^= true ) {
                    glutEnterGameMode();
                }

                        return;
                    }
                    else
                    {
                        std::cout << "NOPE!" << std::endl;
                        //couldnt set fullscreen
                    }
                }
            }

            // maximized, working

            if( const bool fullscreen_maximized = true )
            {
                glutFullScreen();

                int nw = w / 2, nh = h / 2, nx = (w - nw) * 0.5, ny = (h - nh ) * 0.5;
                glutReshapeWindow(nw, nh);
                glutPositionWindow(nx, ny);
                glutReshapeWindow(nw, nh);
                glutPositionWindow(nx, ny);

                if( is_fullscreen ^= true ) {
                    glutFullScreen();
                }

#if 0 // @todo : fix fullscreen issue (windows bar on top)
                ::SetWindowPos( 0,       // handle to window
                                HWND_TOPMOST,  // placement-order handle
                                0,     // horizontal position
                                0,      // vertical position
                                0,  // width
                                0, // height
                                SWP_NOMOVE|SWP_NOSIZE // window-positioning options
                );
#endif

            }
        }

        static void status_cb(int state)
        {
            int window = glutGetWindow();
            /*
            switch(state)
            GLUT_HIDDEN
            GLUT_FULLY_RETAINED
            GLUT_PARTIALLY_RETAINED
            GLUT_FULLY_COVERED
            */
            glutPostRedisplay();
        }

        bool is_open()
        {
            glutMainLoopEvent();

            moon9::wink();

            return bIsOpen;
        }

        void resize( int w, int h ) // only works when called from render()! @todo: queue event and act on render().
        {
            glutReshapeWindow( this->w = w, this->h = h);
        }

        void close()
        {
            bIsOpen = false;
        }

        void flush()
        {
            glutSwapBuffers();
        }

        static void destroy_cb()
        {
            window *win = instance( glutGetWindow() );
            win->close();
        }

        static void keyboard_cb()
        {
#if 0
            window *win = instance( glutGetWindow() );
#endif
        }

        static void mouse_cb( int button, int state, int x, int y )
        {
#if 0
            window *win = instance( glutGetWindow() );

            if( button == GLUT_LEFT_BUTTON )
            win->mouse.set_button( 0, state == GLUT_DOWN );

            if( button == GLUT_RIGHT_BUTTON )
            win->mouse.set_button( 1, state == GLUT_DOWN );
#endif
        }

        static void init (void)
        {
            window *win = instance( glutGetWindow() );

                miniwin::glInit();

                win->game_t = 0;
                win->camera.update();
        }

        void register_callbacks()
        {
            glutReshapeFunc( resize_cb );
            glutMouseFunc( mouse_cb );
            glutDisplayFunc( display_cb );
            glutWindowStatusFunc( status_cb );
            glutOverlayDisplayFunc( 0 );
            //if(openglut) glutDestroyWindowFunc( destroy_cb ); else freeglut {
            glutCloseFunc( destroy_cb );
            glutWMCloseFunc( destroy_cb );
        }

        static void resize_cb(int x, int y)
        {
            window *win = instance( glutGetWindow() );

            win->w = (x <= 0 ? 0 : x);
            win->h = (y <= 0 ? 1 : y);

            win->camera.resize( x, y );
            //win->camera.update();
        }

        virtual void update( double t, float dt ) = 0;
        virtual void render( double t, float dt ) /*const*/ = 0;

        virtual void update_idle( double t, float dt )       { update(t,dt); }
        virtual void render_idle( double t, float dt ) /*const*/ { render(t,dt); }

        private:
        moon9::dt game_dt;
        double game_t;

        public:
        static void display_cb( void )
        {
            window *win = instance( glutGetWindow() );

            glutReshapeWindow( win->w, win->h );

            /*
            glClear(GL_COLOR_BUFFER_BIT);

            if( true ) // glIsEnabled(GL_CULL_FACE) == GL_TRUE )
            {
                glDisable(GL_CULL_FACE);
                glPolygonMode(GL_FRONT, GL_LINE);
                glPolygonMode(GL_BACK, GL_FILL);
            }
            else
            {
                glEnable(GL_CULL_FACE);
                glPolygonMode(GL_FRONT, GL_FILL);
            }
            */

            win->camera.update();
            float game_dt = win->game_dt.s();
            win->game_t += game_dt;
            win->game_dt = moon9::dt();

#ifdef _WIN32
            if(!GetFocus())
            win->update_idle( win->game_t, game_dt );
            else
#endif
            win->update( win->game_t, game_dt );

            if( full_antialias )
                glEnable(GL_MULTISAMPLE);

#ifdef _WIN32
            if(!GetFocus())
                win->render_idle( win->game_t, game_dt );
            else
#endif
                win->render( win->game_t, game_dt );

            if( full_antialias )
                glDisable(GL_MULTISAMPLE);

            glutPostRedisplay();
            win->frames++;

            if( win->dt.s() > 1.0 )
            {
                win->fps = win->frames / win->dt.s();

                // todo: spf too

                moon9::format title("%s // %3.2f fps", win->title.c_str(), win->fps );

                #if defined( kBooTargetWindows ) && defined( UNICODE )
                glutSetWindowTitle( (const char *)( std::wstring( title.begin(), title.end() ).c_str() ) );
                #else
                glutSetWindowTitle( title.c_str() );
                #endif

                win->frames = 0;
                win->dt.reset();
            }

        }

        protected:

        int id;
        volatile bool bIsOpen;

        static window *instance( const int &id, window *w = 0 )
        {
            static std::map< int /*id*/, window * /*ptr*/ > map;

            if( w )
            {
                map[ id ] = w;

                return w;
            }
            else
            {
                return map[ id ];
            }

            return 0;
        }

    };

    class window2 : public moon9::window
    {
        // void close();

        // bool is_fullscreen();
        // void toggle_fullscreen();

        bool _open;

        static const window2 *active;

        public:

        window2() : moon9::window( "moon9", 0.75f ), _open( true )
        {}

        explicit window2( const std::string &title, float relsize = 0.85f ) : moon9::window( title, relsize ), _open( true )
        {}

        bool is_open()
        {
            return _open;
        }

        void set() const
        {
            active = this;
        }

        bool is_set() const
        {
            return active == this;
        }

        void flush()
        {
            moon9::window::flush();
            _open = moon9::window::is_open();
        }

        void render( double t, float dt ) {}
        void update( double t, float dt ) {}
    };

} // ns moon9

