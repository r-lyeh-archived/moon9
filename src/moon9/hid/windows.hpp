#pragma once

// @todo: Send 'off' to buttons,lists,etc when windows focus is lost

#ifdef _WIN32

#include <limits>
#include <set>

#include <cassert>
#include <iostream>

#include <map>

#include "keycodes.hpp"
#include "types.hpp"

#include <cassert>
#include <winsock2.h>
#include <Windows.h>
#include <mmsystem.h>                   // joystick
#include <XInput.h>                     // gamepad360

#pragma comment(lib, "winmm.lib")       // joystick
#pragma comment(lib, "XInput.lib")      // gamepad360
#pragma comment(lib, "user32.lib")      // GetAsyncKeyState [...]

#include "manymouse.h"



namespace moon9
{
    class sharing_policy
    {
        public:

        template<typename T>
        static T *get_master( T &controller, size_t id , bool is_creating )
        {
            // quick auto-creation map
            struct tid
            {
                T *master;  //shared_ptr
                size_t id, num_instances;
                tid( size_t _id = 0 ) : master(0), id(_id), num_instances(0) {}
                const bool operator <( const tid &t ) const { return id < t.id; }
            };

            static std::map<tid,tid> map;

            tid &ref_id = ( map[tid(id)] = map[tid(id)] );

            if( is_creating )
            {
                if( ref_id.num_instances + 1 <= 1 )
                {
                    ref_id.master = &controller;
                    ++ref_id.num_instances;
                }
                //else //throw moon9::string( "error: device id #\1 already in use! (seen at \2 controller)", id, controller.typeof );
            }
            else
            {
                --ref_id.num_instances;
            }

            return ref_id.master;
        }
    };

    namespace windows_wip
    {

        class joystick
        {
            UINT joy_id;

            public:

            moon9::flag is_ready;
            moon9::coordinate xy;
            moon9::buttons buttons;

            // http://www.gnu-darwin.org/www001/src/ports/graphics/gephex/work/gephex-0.4.3/util/src/libjoystick/win32joystickdriver.cpp
            // http://www.gnu-darwin.org/www001/src/ports/graphics/gephex/work/gephex-0.4.3/util/src/libjoystick/

            joystick( size_t id )
            {
                assert( id <  2 && "invalid joystick id" ); /* // nt [0.. 1]
                assert( id < 16 && "invalid joystick id" ); */ // xp [0..15]

                joy_id = JOYSTICKID1 + id;

                // calibrate
                // WinExec("control joy.cpl", SW_NORMAL);

                /*
                MMRESULT mr;
                JOYCAPS jc;

                mr = joyGetDevCaps(handle, &jc, sizeof(jc));

                if (mr != JOYERR_NOERROR)
                {
                    throw std::runtime_error("Could not get caps for joystick");
                }

                m_num_axes      = jc.wNumAxes;
                m_num_buttons   = jc.wNumButtons;
                m_state.axes    = std::vector<int>(m_num_axes, 0);
                m_state.buttons = std::vector<bool>(m_num_buttons, false);
                */
            }

            void clear()
            {
                is_ready.clear();
                xy.clear();

                for( auto &it : buttons )
                    it.clear();
            }

            void update()
            {
                if( !GetFocus() )
                {
                    if( GetForegroundWindow() != GetConsoleWindow() )
                    {
                        clear();
                        return;
                    }
                }

                //typedef struct joyinfoex_tag {
                //    DWORD dwSize;                /* size of structure */
                //    DWORD dwFlags;               /* flags to indicate what to return */
                //    DWORD dwXpos;                /* x position */
                //    DWORD dwYpos;                /* y position */
                //    DWORD dwZpos;                /* z position */ <-- throttle
                //    DWORD dwRpos;                /* rudder/4th axis position */ <-- pedals
                //    DWORD dwUpos;                /* 5th axis position */
                //    DWORD dwVpos;                /* 6th axis position */
                //    DWORD dwButtons;             /* button states */
                //    DWORD dwButtonNumber;        /* current button number pressed */
                //    DWORD dwPOV;                 /* point of view state */
                //    DWORD dwReserved1;           /* reserved for communication between winmm & driver */
                //    DWORD dwReserved2;           /* reserved for future expansion */
                //} JOYINFOEX, *PJOYINFOEX, NEAR *NPJOYINFOEX, FAR *LPJOYINFOEX;

                JOYCAPS joyCaps;
                joyGetDevCaps(joy_id, &joyCaps, sizeof(joyCaps));
                BOOL has_throttle = (joyCaps.wCaps & JOYCAPS_HASZ);

                JOYINFOEX joyInfoEx;
                ZeroMemory(&joyInfoEx, sizeof(joyInfoEx));

                joyInfoEx.dwSize = sizeof(joyInfoEx);
                joyInfoEx.dwFlags = JOY_RETURNALL | JOY_RETURNCENTERED | JOY_USEDEADZONE | (has_throttle ? JOY_RETURNZ : 0);

                BOOL is_present = (joyGetPosEx(joy_id, &joyInfoEx) == JOYERR_NOERROR);
                is_ready.set( is_present ? 1.f : 0.f );

                if( is_present )
                {
                    // vec6 !
                    /*
                    axes[0] = joyInfoEx.dwXpos;
                    axes[1] = joyInfoEx.dwYpos;
                    if (m_num_axes > 2)
                        m_state.axes[2] = joyInfoEx.dwZpos; // throttle? sure?
                    if (m_num_axes > 3)
                        m_state.axes[3] = joyInfoEx.dwRpos;
                    if (m_num_axes > 4)
                        m_state.axes[4] = joyInfoEx.dwUpos;
                    if (m_num_axes > 5)
                        m_state.axes[5] = joyInfoEx.dwVpos;
                    */

                    size_t num_buttons = buttons.size();
                    for( size_t i = 0; i < num_buttons; ++i )
                        buttons[i].set( (joyInfoEx.dwButtons & (JOY_BUTTON1+i) ) ? 1 : 0 );
                }
            }
        };
    }

    namespace windows
    {
        class gamepad
        {
            private:

                XINPUT_STATE state;
                size_t id;

            public:

                // pad pieces

                // 10x buttons,   1d data input (action)
                //  2x triggers,  1d data input (action)
                moon9::button
                    a, b, x, y,
                    back, start,
                    lb, rb,
                    lthumb, rthumb,
                    ltrigger, rtrigger;

                //  2x axis,      2d data input (spatial)
                //  1x gamepad,   2d data input (spatial)
                moon9::coordinate
                    pad,
                    lpad, rpad;

                //  1x mic,       1d data output (mono audio)
                moon9::button
                    mic;

                //  1x earphones, 2d data output (stereo audio)
                moon9::coordinates
                    earphones;

                // 47x keys,      1d data input (text)
                moon9::keys
                    keymap;

                // 2x rumble,     1d data output (rumble haptic)
                moon9::buttons rumble;

                moon9::flag
                    is_ready;   // ~? is_present, is_plugged, is_connected

                gamepad *master;

            gamepad( const size_t &_id ) :
                id(_id),
                keymap(47),
                rumble(2),
                typeof( "moon9::windows::gamepad" )
            {

                if( id >= max_devices )
                {
                    std::cerr << "error: device id #" << id << " invalid! (seen at " << typeof << " controller)" << std::endl;
                    assert( false );
                    return;
                }

                master = sharing_policy::get_master( *this, id, true );

                if( master == this )
                    set_rumble( 0.f, 0.f );

//                ltrigger.newest().deadzone = 0.1f;
//                rtrigger.newest().deadzone = 0.1f;
            }

            ~gamepad()
            {
                if( master == this )
                    set_rumble( 0.f, 0.f );

                // check & decrement instance counter
                master = sharing_policy::get_master( *this, id, false );
            }

            const char *const typeof;
            static const size_t max_devices = 4;

            protected:

            XINPUT_GAMEPAD get_state()
            {
                ZeroMemory(&state, sizeof(XINPUT_STATE));

                XInputGetState(id, &state);

                return state.Gamepad;
            }

            void set_rumble( const float &left01, const float &right01 )
            {
                assert( left01 >= 0.f && left01 <= 1.f );
                assert( right01 >= 0.f && right01 <= 1.f );

                //if(!is_ready()) return;

                XINPUT_VIBRATION vibration_state;
                ZeroMemory( &vibration_state, sizeof( XINPUT_VIBRATION ) );

                vibration_state.wLeftMotorSpeed  = DWORD(  left01 * 65535.f );
                vibration_state.wRightMotorSpeed = DWORD( right01 * 65535.f );

                XInputSetState( id, &vibration_state );
            }

            public:

            void clear()
            {
                a.clear();
                b.clear();
                x.clear();
                y.clear();
                back.clear();
                start.clear();
                lb.clear();
                rb.clear();
                lthumb.clear();
                rthumb.clear();
                ltrigger.clear();
                rtrigger.clear();
                pad.clear();
                lpad.clear();
                rpad.clear();
                mic.clear();
                is_ready.clear();

                for( auto &it : earphones )
                    it.clear();

                for( auto &it : keymap )
                    it.clear();

                for( auto &it : rumble )
                    it.clear();
            }

            void update()
            {
                if( master != this )
                {
                    a = master->a;
                    b = master->b;
                    x = master->x;
                    y = master->y;
                    back = master->back;
                    start = master->start;
                    lb = master->lb;
                    rb = master->rb;
                    lthumb = master->lthumb;
                    rthumb = master->rthumb;
                    ltrigger = master->ltrigger;
                    rtrigger = master->rtrigger;
                    pad = master->pad;
                    lpad = master->lpad;
                    rpad = master->rpad;
                    mic = master->mic;
                    earphones = master->earphones;
                    keymap = master->keymap;
                    rumble = master->rumble;
                    is_ready = master->is_ready;

                    return;
                }

                if( !GetFocus() )
                {
                    if( GetForegroundWindow() != GetConsoleWindow() )
                    {
                        clear();
                        return;
                    }
                }

                ZeroMemory(&state, sizeof(XINPUT_STATE));
                bool is_present = ( XInputGetState( id, &state ) == ERROR_SUCCESS );

                is_ready.set( is_present );

                if( !is_present )
                    return;

                // read device
                XINPUT_GAMEPAD state = get_state();

                // process inputs : buttons

                     a.set( state.wButtons &              XINPUT_GAMEPAD_A ? 1.0f : 0.f );
                     b.set( state.wButtons &              XINPUT_GAMEPAD_B ? 1.0f : 0.f );
                     x.set( state.wButtons &              XINPUT_GAMEPAD_X ? 1.0f : 0.f );
                     y.set( state.wButtons &              XINPUT_GAMEPAD_Y ? 1.0f : 0.f );
                  back.set( state.wButtons &           XINPUT_GAMEPAD_BACK ? 1.0f : 0.f );
                 start.set( state.wButtons &          XINPUT_GAMEPAD_START ? 1.0f : 0.f );
                    lb.set( state.wButtons &  XINPUT_GAMEPAD_LEFT_SHOULDER ? 1.0f : 0.f );
                    rb.set( state.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? 1.0f : 0.f );
                lthumb.set( state.wButtons &     XINPUT_GAMEPAD_LEFT_THUMB ? 1.0f : 0.f );
                rthumb.set( state.wButtons &    XINPUT_GAMEPAD_RIGHT_THUMB ? 1.0f : 0.f );

                // process inputs : triggers

                ltrigger.set( state.bLeftTrigger / 255.0 );
                rtrigger.set( state.bRightTrigger / 255.0 );

                // process inputs : digital pad

                float padUp    = ( state.wButtons &    XINPUT_GAMEPAD_DPAD_UP ? 1.0f : 0.f );
                float padDown  = ( state.wButtons &  XINPUT_GAMEPAD_DPAD_DOWN ? 1.0f : 0.f );
                float padLeft  = ( state.wButtons &  XINPUT_GAMEPAD_DPAD_LEFT ? 1.0f : 0.f );
                float padRight = ( state.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 1.0f : 0.f );
                pad.set( - padLeft + padRight, + padUp - padDown );

                // process inputs : thumb pads

                float thumbLX = state.sThumbLX / ( state.sThumbLX > 0 ? 32767.0 : 32768.0 );
                float thumbLY = state.sThumbLY / ( state.sThumbLY > 0 ? 32767.0 : 32768.0 );
                float thumbRX = state.sThumbRX / ( state.sThumbRX > 0 ? 32767.0 : 32768.0 );
                float thumbRY = state.sThumbRY / ( state.sThumbRY > 0 ? 32767.0 : 32768.0 );
                lpad.set( thumbLX, thumbLY );
                rpad.set( thumbRX, thumbRY );

                // process outputs : rumble

                set_rumble( rumble[0].newest().x, rumble[1].newest().x );

                /*
                // process virtual buttons

                brake.set( pad.bLeftTrigger / 255.0 );

                thrust.set( pad.bRightTrigger / 255.0 );

                float thumbLX = pad.sThumbLX / ( pad.sThumbLX > 0 ? 32767.0 : 32768.0 );
                turn.set( thumbLX );
                */
            }

            /*
            void test()
            {
                if( earphones.isReady() )
                {
                    std::vector< vec2 > soundSample;
                    earphones << soundSample;

                    vec2 soundSample2[500];
                    earphones << 500 << soundSample2;
                }

                if( rumble.isReady() )
                {
                    rumble << vec2( 1, 1 );
                }

                if( axis.isReady() )
                {
                    vec2 current;
                    //axis[0] >> current;

                    if( current.x > 0.5f ) 0;
                    if( current.y > 0.5f ) 0;
                    if( current.magnitude() > 0.5f ) 0;

                    std::vector<vec2> last_4_samples(4);
                    //axis[0] >> 4 >> last_4_samples;
                }

                if( keys.isReady() )
                {
                    //
                    // ...
                }

                if( mic.isReady() )
                {
                    std::vector<vec1> recording( 300 ); //300 muestras
                    mic >> 300 >> recording;
                }

                // encuentra patterns en el buffer de datas y devuelve probabilidad

                //if peak in last 0.10 seconds in cross is at least 80% found then...
                if( hid::peak( axis[0], 0.10f ) > 0.80f )
                {}

                if( hid::sustain( axis[0], 0.25f ) > 0.50f )
                {}

                vec2 hadoken[3] = { vec2(0,1),vec2(1,1),vec2(1,0) };
                if( hid::stroke( axis[0], 0.25f, 3, hadoken ) > 0.85f )
                {}
            }
            */
        };
    }

    namespace windows
    {

        class keyboard //, public moon9::enums::keyboard
        {
            size_t id;

        public:

            // we are implementing keyboard from two sides:
            // - an input serial data (able to send strings to app)
            // - a 105 input button map (so we can query each key status individually)

            const char *const typeof;

            moon9::buttons keymap;
            moon9::serializers serial;

            /* sorry about the long reference memberlist :) */
            moon9::button &a, &b, &c, &d, &e, &f, &g, &h, &i, &j, &k, &l,
                &m, &n, &o, &p, &q, &r, &s, &t, &u, &v, &w, &x, &y, &z,
                &one, &two, &three, &four, &five, &six, &seven, &eight, &nine, &zero,
                &escape, &backspace, &tab, &enter, &shift, &ctrl, &alt, &space,
                &up, &down, &left, &right, &home, &end, &insert, &del,
                &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &f11, &f12,
                &numpad1, &numpad2, &numpad3, &numpad4, &numpad5,
                &numpad6, &numpad7, &numpad8, &numpad9, &numpad0,
                &add, &subtract, &multiply, &divide, &separator, &decimal;

            moon9::button debug_key;

            moon9::flag is_ready; //is_connected

            keyboard *master;

            keyboard( const size_t &_id )
#if 1
            :
#else
            try :
#endif
                              id(_id),
                  typeof("moon9::windows::keyboard"),
                     keymap( 256 ), serial( 1 ),
                   a( keymap[ moon9::keycode::A ] ),
                   b( keymap[ moon9::keycode::B ] ),
                   c( keymap[ moon9::keycode::C ] ),
                   d( keymap[ moon9::keycode::D ] ),
                   e( keymap[ moon9::keycode::E ] ),
                   f( keymap[ moon9::keycode::F ] ),
                   g( keymap[ moon9::keycode::G ] ),
                   h( keymap[ moon9::keycode::H ] ),
                   i( keymap[ moon9::keycode::I ] ),
                   j( keymap[ moon9::keycode::J ] ),
                   k( keymap[ moon9::keycode::K ] ),
                   l( keymap[ moon9::keycode::L ] ),
                   m( keymap[ moon9::keycode::M ] ),
                   n( keymap[ moon9::keycode::N ] ),
                   o( keymap[ moon9::keycode::O ] ),
                   p( keymap[ moon9::keycode::P ] ),
                   q( keymap[ moon9::keycode::Q ] ),
                   r( keymap[ moon9::keycode::R ] ),
                   s( keymap[ moon9::keycode::S ] ),
                   t( keymap[ moon9::keycode::T ] ),
                   u( keymap[ moon9::keycode::U ] ),
                   v( keymap[ moon9::keycode::V ] ),
                   w( keymap[ moon9::keycode::W ] ),
                   x( keymap[ moon9::keycode::X ] ),
                   y( keymap[ moon9::keycode::Y ] ),
                   z( keymap[ moon9::keycode::Z ] ),
                 one( keymap[ moon9::keycode::ONE ] ),
                 two( keymap[ moon9::keycode::TWO ] ),
               three( keymap[ moon9::keycode::THREE ] ),
                four( keymap[ moon9::keycode::FOUR ] ),
                five( keymap[ moon9::keycode::FIVE ] ),
                 six( keymap[ moon9::keycode::SIX ] ),
               seven( keymap[ moon9::keycode::SEVEN ] ),
               eight( keymap[ moon9::keycode::EIGHT ] ),
                nine( keymap[ moon9::keycode::NINE ] ),
                zero( keymap[ moon9::keycode::ZERO ] ),
              escape( keymap[ moon9::keycode::ESCAPE ] ),
           backspace( keymap[ moon9::keycode::BACKSPACE ] ),
                 tab( keymap[ moon9::keycode::TAB ] ),
               enter( keymap[ moon9::keycode::ENTER ] ),
               shift( keymap[ moon9::keycode::SHIFT ] ),
                ctrl( keymap[ moon9::keycode::CTRL ] ),
                 alt( keymap[ moon9::keycode::ALT ] ),
               space( keymap[ moon9::keycode::SPACE ] ),
                  up( keymap[ moon9::keycode::UP ] ),
                down( keymap[ moon9::keycode::DOWN ] ),
                left( keymap[ moon9::keycode::LEFT ] ),
               right( keymap[ moon9::keycode::RIGHT ] ),
                home( keymap[ moon9::keycode::HOME ] ),
                 end( keymap[ moon9::keycode::END ] ),
              insert( keymap[ moon9::keycode::INSERT ] ),
                 del( keymap[ moon9::keycode::DEL ] ),
                  f1( keymap[ moon9::keycode::F1 ] ),
                  f2( keymap[ moon9::keycode::F2 ] ),
                  f3( keymap[ moon9::keycode::F3 ] ),
                  f4( keymap[ moon9::keycode::F4 ] ),
                  f5( keymap[ moon9::keycode::F5 ] ),
                  f6( keymap[ moon9::keycode::F6 ] ),
                  f7( keymap[ moon9::keycode::F7 ] ),
                  f8( keymap[ moon9::keycode::F8 ] ),
                  f9( keymap[ moon9::keycode::F9 ] ),
                 f10( keymap[ moon9::keycode::F10 ] ),
                 f11( keymap[ moon9::keycode::F11 ] ),
                 f12( keymap[ moon9::keycode::F12 ] ),
             numpad1( keymap[ moon9::keycode::NUMPAD1 ] ),
             numpad2( keymap[ moon9::keycode::NUMPAD2 ] ),
             numpad3( keymap[ moon9::keycode::NUMPAD3 ] ),
             numpad4( keymap[ moon9::keycode::NUMPAD4 ] ),
             numpad5( keymap[ moon9::keycode::NUMPAD5 ] ),
             numpad6( keymap[ moon9::keycode::NUMPAD6 ] ),
             numpad7( keymap[ moon9::keycode::NUMPAD7 ] ),
             numpad8( keymap[ moon9::keycode::NUMPAD8 ] ),
             numpad9( keymap[ moon9::keycode::NUMPAD9 ] ),
             numpad0( keymap[ moon9::keycode::NUMPAD0 ] ),
                 add( keymap[ moon9::keycode::ADD ] ),
            subtract( keymap[ moon9::keycode::SUBTRACT ] ),
            multiply( keymap[ moon9::keycode::MULTIPLY ] ),
              divide( keymap[ moon9::keycode::DIVIDE ] ),
           separator( keymap[ moon9::keycode::SEPARATOR ] ),
             decimal( keymap[ moon9::keycode::DECIMAL ] )
            {
                if( id >= max_devices )
                {
                    std::cerr << "error: device id #" << id << " invalid! (seen at " << typeof << " controller)" << std::endl;
                    assert( false );
                    return;
                }

                // check & increment instance counter
                master = sharing_policy::get_master( *this, id, true );
            }
#if 0
            catch(...)
            {
                throw "";
            }
#endif

            ~keyboard()
            {
                if( master == this )
                {
                    // remove pending keystrokes

                    std::cin.clear();
                    //std::cin.ignore(std::numeric_limits<std::streamsize>::max());
                }

                // check & decrement instance counter
                master = sharing_policy::get_master( *this, id, false );
            }

            static const size_t max_devices = 1;

            void clear()
            {
                for( auto &it : keymap )
                    it.clear();

                for( auto &it : serial )
                    it.clear();

                debug_key.clear();
                is_ready.clear();
            }

            void update()
            {
                if( master != this )
                {
                    this->keymap = master->keymap;
                    this->serial = master->serial;
                    this->debug_key = master->debug_key;
                    this->is_ready = master->is_ready;

                    return;
                    /*
            moon9::button &a, &b, &c, &d, &e, &f, &g, &h, &i, &j, &k, &l,
                &m, &n, &o, &p, &q, &r, &s, &t, &u, &v, &w, &x, &y, &z,
                &one, &two, &three, &four, &five, &six, &seven, &eight, &nine, &zero,
                &escape, &backspace, &tab, &enter, &shift, &ctrl, &space,
                &up, &down, &left, &right, &home, &end, &insert, &del,
                &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &f11, &f12,
                &numpad1, &numpad2, &numpad3, &numpad4, &numpad5,
                &numpad6, &numpad7, &numpad8, &numpad9, &numpad0,
                &add, &subtract, &multiply, &divide, &separator, &decimal;
                */
                }

				if( !GetFocus() )
                {
                    if( GetForegroundWindow() != GetConsoleWindow() )
                    {
                        clear();
                        return;
                    }
                }
                else
                {
                    MSG m;
                    while( PeekMessage( &m, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE ) )
                        ;
                }

                bool is_connected = true;

                // to check: http://molecularmusings.wordpress.com/2011/09/05/properly-handling-keyboard-input/

                /*
                return ( GetFocus() != 0 );

                static HWND myWnd;
                static struct install { install() {
                    HWND prevWnd;
                    myWnd = SetCapture( prevWnd = GetCapture() );
                    //GetWindowThreadProcessId( myWnd, &myProc );
                    ReleaseCapture();
                } } _;
                return GetForegroundWindow() == myWnd;
                */

                is_ready.set( is_connected );

                if( !is_connected )
                    return;

                //GetKeyboardState( (PBYTE)&keymap ) )

                for( size_t i = 0; i < 256; ++i )
                {
                    SHORT key = GetAsyncKeyState( i );
                    keymap[ i ].set( key & 0x8000 ? 0.5f : 0.f );

                    //if( key ) serializer << ch; // y q pasa con el cero?
                }
            }
        };


        class mouse //, public moon9::enums::mouse
        {
            size_t id;

            public:

            enum button_enumeration
            {
                LEFT,
                MIDDLE,
                RIGHT
            };

            enum flag_enumeration
            {
                HOVER,
                CONNECTED,
                HIDDEN,
                CLIPPED,
                CENTERED
            };

            enum axis_enumeration
            {
                WHEEL,
                LOCAL,		// app (dt,dt) center = (0,0)
                GLOBAL,		// desktop (x,y)
                CLIENT,		// app (x,y)
                DESKTOP		// desktop (dt,dt) center = (0,0)
            };

            // como hago la memoization?
            // q es mejor:
            // - leo las entradas, y cada vez que me preguntan un combo compruebo todas las combinaciones?
            //   combo["punch"]? : return (if A[0].hold() && A[1].hold() && A[2].hold() && B[3].tap() ? true : false)
            // - o con cada entrada que leo sigo todas las maquinas de estados vivas, y actualizo valores de retorno
            //   para cuando me consulten?
            //   read pad -> A -> fsm #1 -> step3/13 -> combo["punch"] = false;
            //                    fsm #2 -> step19/19 -> "kick" -> combo["kick"] = true;
            // - hago un map<string,val> cache; y lo limpio/lleno en cada update?

            moon9::flags flags;
            moon9::buttons buttons;
            moon9::coordinates coordinates;

            moon9::button &left, &middle, &right;
            moon9::coordinate &wheel, &local, &global, &client, &desktop;
            moon9::flag &hover, &connected, &hidden, &clipped, &centered;

            mouse *master;

        protected:
            int ix, iy;

        public:
                 mouse( const size_t &_id ) :
                    id(_id), ix(0), iy(0),
              flags( 5 ), buttons( 3 ), coordinates( 5 ),
                    left( buttons[ LEFT ] ),
                  middle( buttons[ MIDDLE ] ),
                   right( buttons[ RIGHT ] ),
                   wheel( coordinates[ WHEEL ] ),
                   local( coordinates[ LOCAL ] ),
                  global( coordinates[ GLOBAL ] ),
                  client( coordinates[ CLIENT ]),
                 desktop( coordinates[ DESKTOP ] ),
                   hover( flags[ HOVER ] ),
               connected( flags[ CONNECTED ] ),
                  hidden( flags[ HIDDEN ] ),
                 clipped( flags[ CLIPPED ] ),
                centered( flags[ CENTERED ]),
               typeof( "moon9::windows::mouse" )
            {
                if( id >= max_devices )
                {
                    std::cerr << "error: device id #" << id << " invalid! (seen at " << typeof << " controller)" << std::endl;
                    assert( false );
                    return;
                }
                //
                // caps = moon9::caps( max_devices, max_instances_per_device );

                // check & decrement instance counter
                master = sharing_policy::get_master( *this, id, true );
            }

            ~mouse()
            {
                // check & decrement instance counter
                master = sharing_policy::get_master( *this, id, false );
            }

            static const size_t max_devices = 1;

            const char *const typeof;

            void clear()
            {
                for( auto &it : buttons )
                    it.clear();
                for( auto &it : coordinates )
                    it.clear();
                for( auto &it : flags )
                    it.clear();
            }

            void update()
            {
                if( master != this )
                {
                    this->flags = master->flags;
                    this->buttons = master->buttons;
                    this->coordinates = master->coordinates;
                    return;
                }

#if 1

                if( !GetFocus() )
                {
                    if( GetForegroundWindow() != GetConsoleWindow() )
                    {
                        clear();
                        return;
                    }
                }
                else
                {
                    MSG m;
                    while( PeekMessage( &m, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE ) )
                        ;
                }

                // @todo: replace is_present with a proper/reliable is_present() function

                static HWND parentWnd = 0;
                static bool is_mouse_present = true;
                static HCURSOR default_cursor;
                static HCURSOR hidden_cursor;
                static HICON hicon;

                static struct detection_setup {
                    detection_setup() {
                        HWND prevWnd; parentWnd = SetCapture( prevWnd = GetCapture() ); //ReleaseCapture();
                        is_mouse_present = ShowCursor( true ) >= 0;
                        default_cursor = LoadCursor(0, IDC_ARROW);

                         // make hidden_cursor transparent

                        int w = GetSystemMetrics( SM_CXCURSOR );
                        int h = GetSystemMetrics( SM_CYCURSOR );

                        std::vector<unsigned char> and_mask( w * h, 0xFF );
                        std::vector<unsigned char> xor_mask( w * h, 0x00 );

                        hidden_cursor = CreateCursor(
                          0,
                          1,
                          1,
                          w,
                          h,
                          &and_mask[0],
                          &xor_mask[0]
                        );

                        BYTE ANDmaskIcon[] = {0xFF, 0xFF, 0xFF, 0xFF,  // line 1
                                              0xFF, 0xFF, 0xC3, 0xFF,  // line 2
                                              0xFF, 0xFF, 0x00, 0xFF,  // line 3
                                              0xFF, 0xFE, 0x00, 0x7F,  // line 4

                                              0xFF, 0xFC, 0x00, 0x1F,  // line 5
                                              0xFF, 0xF8, 0x00, 0x0F,  // line 6
                                              0xFF, 0xF8, 0x00, 0x0F,  // line 7
                                              0xFF, 0xF0, 0x00, 0x07,  // line 8

                                              0xFF, 0xF0, 0x00, 0x03,  // line 9
                                              0xFF, 0xE0, 0x00, 0x03,  // line 10
                                              0xFF, 0xE0, 0x00, 0x01,  // line 11
                                              0xFF, 0xE0, 0x00, 0x01,  // line 12

                                              0xFF, 0xF0, 0x00, 0x01,  // line 13
                                              0xFF, 0xF0, 0x00, 0x00,  // line 14
                                              0xFF, 0xF8, 0x00, 0x00,  // line 15
                                              0xFF, 0xFC, 0x00, 0x00,  // line 16

                                              0xFF, 0xFF, 0x00, 0x00,  // line 17
                                              0xFF, 0xFF, 0x80, 0x00,  // line 18
                                              0xFF, 0xFF, 0xE0, 0x00,  // line 19
                                              0xFF, 0xFF, 0xE0, 0x01,  // line 20

                                              0xFF, 0xFF, 0xF0, 0x01,  // line 21
                                              0xFF, 0xFF, 0xF0, 0x01,  // line 22
                                              0xFF, 0xFF, 0xF0, 0x03,  // line 23
                                              0xFF, 0xFF, 0xE0, 0x03,  // line 24

                                              0xFF, 0xFF, 0xE0, 0x07,  // line 25
                                              0xFF, 0xFF, 0xC0, 0x0F,  // line 26
                                              0xFF, 0xFF, 0xC0, 0x0F,  // line 27
                                              0xFF, 0xFF, 0x80, 0x1F,  // line 28

                                              0xFF, 0xFF, 0x00, 0x7F,  // line 29
                                              0xFF, 0xFC, 0x00, 0xFF,  // line 30
                                              0xFF, 0xF8, 0x03, 0xFF,  // line 31
                                              0xFF, 0xFC, 0x3F, 0xFF}; // line 32

                        // Yang-shaped icon XOR mask

                        BYTE XORmaskIcon[] = {0x00, 0x00, 0x00, 0x00,  // line 1
                                              0x00, 0x00, 0x00, 0x00,  // line 2
                                              0x00, 0x00, 0x00, 0x00,  // line 3
                                              0x00, 0x00, 0x00, 0x00,  // line 4

                                              0x00, 0x00, 0x00, 0x00,  // line 5
                                              0x00, 0x00, 0x00, 0x00,  // line 6
                                              0x00, 0x00, 0x00, 0x00,  // line 7
                                              0x00, 0x00, 0x38, 0x00,  // line 8

                                              0x00, 0x00, 0x7C, 0x00,  // line 9
                                              0x00, 0x00, 0x7C, 0x00,  // line 10
                                              0x00, 0x00, 0x7C, 0x00,  // line 11
                                              0x00, 0x00, 0x38, 0x00,  // line 12

                                              0x00, 0x00, 0x00, 0x00,  // line 13
                                              0x00, 0x00, 0x00, 0x00,  // line 14
                                              0x00, 0x00, 0x00, 0x00,  // line 15
                                              0x00, 0x00, 0x00, 0x00,  // line 16

                                              0x00, 0x00, 0x00, 0x00,  // line 17
                                              0x00, 0x00, 0x00, 0x00,  // line 18
                                              0x00, 0x00, 0x00, 0x00,  // line 19
                                              0x00, 0x00, 0x00, 0x00,  // line 20

                                              0x00, 0x00, 0x00, 0x00,  // line 21
                                              0x00, 0x00, 0x00, 0x00,  // line 22
                                              0x00, 0x00, 0x00, 0x00,  // line 23
                                              0x00, 0x00, 0x00, 0x00,  // line 24

                                              0x00, 0x00, 0x00, 0x00,  // line 25
                                              0x00, 0x00, 0x00, 0x00,  // line 26
                                              0x00, 0x00, 0x00, 0x00,  // line 27
                                              0x00, 0x00, 0x00, 0x00,  // line 28

                                              0x00, 0x00, 0x00, 0x00,  // line 29
                                              0x00, 0x00, 0x00, 0x00,  // line 30
                                              0x00, 0x00, 0x00, 0x00,  // line 31
                                              0x00, 0x00, 0x00, 0x00}; // line 32

                        HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);

                        hicon  = CreateIcon(hInstance, // handle to app. instance
                                     32,               // icon width
                                     32,               // icon height
                                     1,                // number of XOR planes
                                     1,                // number of bits per pixel
                                     ANDmaskIcon,      // AND mask
                                     XORmaskIcon);     // XOR mask
                    }
                    ~detection_setup() {
                        ReleaseCapture();
                        SetCursor(default_cursor);
                        DestroyCursor(hidden_cursor);
                    }}_;

                connected.set( is_mouse_present ? 0.5f : 0.f );

                if( !is_mouse_present )
                    return;

                // process outputs 1/3

                //SetClassLong( hWnd, GCL_HCURSOR, (LONG)( default_cursor ) );

                // process inputs

                SHORT lkey = GetAsyncKeyState( VK_LBUTTON );
                SHORT mkey = GetAsyncKeyState( VK_MBUTTON );
                SHORT rkey = GetAsyncKeyState( VK_RBUTTON );

                  left.set( lkey & 0x8000 ? 0.5f : 0.f );
                middle.set( mkey & 0x8000 ? 0.5f : 0.f );
                 right.set( rkey & 0x8000 ? 0.5f : 0.f );

                bool is_hover = false;

                CURSORINFO ci;
                ci.cbSize = sizeof(CURSORINFO);

                // disable clipping
                ClipCursor( 0 );

                if( GetCursorInfo( &ci ) )
                {
                    POINT pPoint = ci.ptScreenPos;

                    // set virtual screen coordinates in normalised [-1,+1]x[-1,+1] form whose center is (0,0)
                    {
                        float gx, gy;

                        gx = -1.0f + float(pPoint.x+pPoint.x) / float( GetSystemMetrics( SM_CXVIRTUALSCREEN ) );
                        gy =  1.0f - float(pPoint.y+pPoint.y) / float( GetSystemMetrics( SM_CYVIRTUALSCREEN ) );

                        desktop.set( pPoint.x, pPoint.y );
                        global.set( gx, gy );
                    }

                    HWND hWnd = WindowFromPoint( pPoint );

                    // if [ parent window, console window, or kind-of-subchild ] then...
                    if( hWnd == parentWnd || /* hWnd == GetConsoleWindow() || */ GetWindowLongPtr(hWnd, GWLP_WNDPROC) )
                    {
                        // point inside any of our windows
                        is_hover = true;

                        // transform point into window->client rect coordinates
                        WINDOWINFO wi;
                        wi.cbSize=sizeof(WINDOWINFO);
                        GetWindowInfo(hWnd, &wi);

                        if( centered.hold() )
                        {
                            int cx = ( wi.rcClient.right - wi.rcClient.left ) / 2 + wi.rcClient.left;
                            int cy = ( wi.rcClient.bottom - wi.rcClient.top ) / 2 + wi.rcClient.top;

                            int dx = pPoint.x - cx;
                            int dy = pPoint.y - cy;

                            SetCursorPos( cx, cy );

                            client.set( ix += dx, iy += dy );
                        }
                        else
                        {
                            pPoint.x -= wi.rcClient.left;
                            pPoint.y -= wi.rcClient.top;

                            client.set( ix = pPoint.x, iy = pPoint.y );
                        }

                        // set coordinate in normalised [-1,+1]x[-1,+1] form whose center is (0,0)
                        {
                            float lx, ly;

                            lx = -1.0f + float(pPoint.x+pPoint.x) / float( wi.rcClient.right - wi.rcClient.left );
                            ly =  1.0f - float(pPoint.y+pPoint.y) / float( wi.rcClient.bottom - wi.rcClient.top + 1 );

                            local.set( lx, ly );
                        }

                        // process outputs 2/3: enable clipping, if required
                        if( clipped.hold() ) ClipCursor( &wi.rcClient );
                    }

                    // process outputs 3/3: hide cursor, if required

                    {
                        bool hide_request = hidden.hold();
                        bool show_request = hidden.idle();

                        // method #1
						if(0)
                        {
                            ShowCursor( TRUE );
                            int counter = ShowCursor( FALSE ); //<0: no mouse installed, or hidden cursor

                            bool visible = (counter >= 0);

                            if( visible )
                            {
                                if( hide_request ) while(counter>=0) counter=ShowCursor(FALSE);
                            }
                            else
                            {
                                if( show_request ) while(counter <0) counter=ShowCursor(TRUE);
                            }
                        }

                        // method #2
						if(0)
                        SetClassLongPtr( hWnd, GCLP_HCURSOR, (LONG)( show_request ? default_cursor : hidden_cursor ) );

						// method #3
						if( hide_request ) while( ShowCursor(FALSE) >= 0 ) ;
						else
						if( show_request ) while( ShowCursor(TRUE) < 0 ) ;
                    }

                    // stupid feature:

                    SetClassLongPtr( hWnd, GCLP_HICON, (LONG)(hicon) );
                }

                hover.set( is_hover ? 0.5f : 0.f );

#endif




                static struct once {
                    once() {
                        const int num_mice = ManyMouse_Init();
                        driver = ManyMouse_DriverName();
                        for( int i = 0; i < num_mice; ++i )
                        {
                            drivers.push_back( ManyMouse_DeviceName(i) );
                        }
                    }
                    ~once() {
                        ManyMouse_Quit();
                    }
                    const char *driver;
                    std::vector<const char *> drivers;
                } lib;

                    ManyMouseEvent event;

                    wheel.set( 0, 0 );

                    if( ManyMouse_PollEvent(&event) )
                    {
                        if( event.device != 0 )
                            return;

                        if (event.type == MANYMOUSE_EVENT_SCROLL )
                        {
                            float dx = 0, dy = 0;

                            if( event.item == 0 )
                            {
                                dy = event.value;
                            }
                            else
                            {
                                dx = event.value;
                            }

                            wheel.set( dx, dy );
                        }

#if 0
                        else if (event.type == MANYMOUSE_EVENT_RELMOTION )
                        {
                            printf("Mouse #%u relative motion %s %d\n", event.device,
                                    event.item == 0 ? "X" : "Y", event.value);
                        }

                        else if (event.type == MANYMOUSE_EVENT_ABSMOTION )
                        {
                            printf("Mouse #%u absolute motion %s %d\n", event.device,
                                    event.item == 0 ? "X" : "Y", event.value);
                        }

                        else if (event.type == MANYMOUSE_EVENT_BUTTON )
                        {
                            if( event.item == 0 ) buttons[ LEFT ].set( event.value ? 0.5f : 0.f );
                            else
                            if( event.item == 1 ) buttons[ MIDDLE ].set( event.value ? 0.5f : 0.f );
                            else
                            if( event.item == 2 ) buttons[ RIGHT ].set( event.value ? 0.5f : 0.f );
                        }

                        else if (event.type == MANYMOUSE_EVENT_DISCONNECT )
                        {
                            connected.set( 0.f );
                        }

                        else
                        {
                            // unhandled event.type
                        }
#endif

                    }
            }
        };
    }
}


#endif
