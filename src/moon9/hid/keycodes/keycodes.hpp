#pragma once

// Platform independent key codes

#ifdef _WIN32
#   include <winsock2.h>
#   define kMoon9Keycode( windows, linux, apple ) windows
#endif

#if defined(__linux__) || defined(__unix__)
#   define kMoon9Keycode( windows, linux, apple ) linux
#endif

#ifdef __APPLE__
#   define kMoon9Keycode( windows, linux, apple ) apple
#endif

#ifndef kMoon9Keycode
#   error  define a platform
#endif

namespace moon9
{
    // to do: add credit: original keycode list by ...?
    // found this: http://www.meandmark.com/keycodes.html

    struct keycode //portable_keycodes
    {
        enum enumeration
        {
            A          = kMoon9Keycode( int('A'),      int('a'),        0x00 ),
            B          = kMoon9Keycode( int('B'),      int('b'),        0x0B ),
            C          = kMoon9Keycode( int('C'),      int('c'),        0x08 ),
            D          = kMoon9Keycode( int('D'),      int('d'),        0x02 ),
            E          = kMoon9Keycode( int('E'),      int('e'),        0x0E ),
            F          = kMoon9Keycode( int('F'),      int('f'),        0x03 ),
            G          = kMoon9Keycode( int('G'),      int('g'),        0x05 ),
            H          = kMoon9Keycode( int('H'),      int('h'),        0x04 ),
            I          = kMoon9Keycode( int('I'),      int('i'),        0x22 ),
            J          = kMoon9Keycode( int('J'),      int('j'),        0x26 ),
            K          = kMoon9Keycode( int('K'),      int('k'),        0x28 ),
            L          = kMoon9Keycode( int('L'),      int('l'),        0x25 ),
            M          = kMoon9Keycode( int('M'),      int('m'),        0x2E ),
            N          = kMoon9Keycode( int('N'),      int('n'),        0x2D ),
            O          = kMoon9Keycode( int('O'),      int('o'),        0x1F ),
            P          = kMoon9Keycode( int('P'),      int('p'),        0x23 ),
            Q          = kMoon9Keycode( int('Q'),      int('q'),        0x0C ),
            R          = kMoon9Keycode( int('R'),      int('r'),        0x0F ),
            S          = kMoon9Keycode( int('S'),      int('s'),        0x01 ),
            T          = kMoon9Keycode( int('T'),      int('t'),        0x11 ),
            U          = kMoon9Keycode( int('U'),      int('u'),        0x20 ),
            V          = kMoon9Keycode( int('V'),      int('v'),        0x09 ),
            W          = kMoon9Keycode( int('W'),      int('w'),        0x0D ),
            X          = kMoon9Keycode( int('X'),      int('x'),        0x07 ),
            Y          = kMoon9Keycode( int('Y'),      int('y'),        0x10 ),
            Z          = kMoon9Keycode( int('Z'),      int('z'),        0x06 ),

            ONE        = kMoon9Keycode( int('1'),      int('1'),        int('1') ),
            TWO        = kMoon9Keycode( int('2'),      int('2'),        int('2') ),
            THREE      = kMoon9Keycode( int('3'),      int('3'),        int('3') ),
            FOUR       = kMoon9Keycode( int('4'),      int('4'),        int('4') ),
            FIVE       = kMoon9Keycode( int('5'),      int('5'),        int('5') ),
            SIX        = kMoon9Keycode( int('6'),      int('6'),        int('6') ),
            SEVEN      = kMoon9Keycode( int('7'),      int('7'),        int('7') ),
            EIGHT      = kMoon9Keycode( int('8'),      int('8'),        int('8') ),
            NINE       = kMoon9Keycode( int('9'),      int('9'),        int('9') ),
            ZERO       = kMoon9Keycode( int('0'),      int('0'),        int('0') ),

            ESCAPE     = kMoon9Keycode( VK_ESCAPE,     XK_Escape,       0x35 ),
            BACKSPACE  = kMoon9Keycode( VK_BACK,       XK_BackSpace,    0x33 ),
            TAB        = kMoon9Keycode( VK_TAB,        XK_Tab,          0x30 ),
            ENTER      = kMoon9Keycode( VK_RETURN,     XK_Return,       0x24 ),
            SHIFT      = kMoon9Keycode( VK_SHIFT,      XK_Shift_L,      0x38 ), //R?
            CTRL       = kMoon9Keycode( VK_CONTROL,    XK_Control_L,    0x3B ), //R?
            SPACE      = kMoon9Keycode( VK_SPACE,      XK_space,        0x31 ),
            ALT        = kMoon9Keycode( VK_LMENU,      XK_Alt_L,        0x3A ), //R?

            UP         = kMoon9Keycode( VK_UP,         XK_Up,           0x7E ),
            DOWN       = kMoon9Keycode( VK_DOWN,       XK_Down,         0x7D ),
            LEFT       = kMoon9Keycode( VK_LEFT,       XK_Left,         0x7B ),
            RIGHT      = kMoon9Keycode( VK_RIGHT,      XK_Right,        0x7C ),
            HOME       = kMoon9Keycode( VK_HOME,       XK_Home,         0x73 ),
            END        = kMoon9Keycode( VK_END,        XK_End,          0x77 ),
            INSERT     = kMoon9Keycode( VK_INSERT,     XK_Insert,       0x72 ),
            DEL        = kMoon9Keycode( VK_DELETE,     XK_Delete,       0x33 ),

            F1         = kMoon9Keycode( VK_F1,         XK_F1,           0x7A ),
            F2         = kMoon9Keycode( VK_F2,         XK_F2,           0x78 ),
            F3         = kMoon9Keycode( VK_F3,         XK_F3,           0x63 ),
            F4         = kMoon9Keycode( VK_F4,         XK_F4,           0x76 ),
            F5         = kMoon9Keycode( VK_F5,         XK_F5,           0x60 ),
            F6         = kMoon9Keycode( VK_F6,         XK_F6,           0x61 ),
            F7         = kMoon9Keycode( VK_F7,         XK_F7,           0x62 ),
            F8         = kMoon9Keycode( VK_F8,         XK_F8,           0x64 ),
            F9         = kMoon9Keycode( VK_F9,         XK_F9,           0x65 ),
            F10        = kMoon9Keycode( VK_F10,        XK_F10,          0x6D ),
            F11        = kMoon9Keycode( VK_F11,        XK_F11,          0x67 ),
            F12        = kMoon9Keycode( VK_F12,        XK_F12,          0x6F ),

            NUMPAD1    = kMoon9Keycode( VK_NUMPAD1,    XK_KP_1,         0x53 ),
            NUMPAD2    = kMoon9Keycode( VK_NUMPAD2,    XK_KP_2,         0x54 ),
            NUMPAD3    = kMoon9Keycode( VK_NUMPAD3,    XK_KP_3,         0x55 ),
            NUMPAD4    = kMoon9Keycode( VK_NUMPAD4,    XK_KP_4,         0x56 ),
            NUMPAD5    = kMoon9Keycode( VK_NUMPAD5,    XK_KP_5,         0x57 ),
            NUMPAD6    = kMoon9Keycode( VK_NUMPAD6,    XK_KP_6,         0x58 ),
            NUMPAD7    = kMoon9Keycode( VK_NUMPAD7,    XK_KP_7,         0x59 ),
            NUMPAD8    = kMoon9Keycode( VK_NUMPAD8,    XK_KP_8,         0x5B ),
            NUMPAD9    = kMoon9Keycode( VK_NUMPAD9,    XK_KP_9,         0x5C ),
            NUMPAD0    = kMoon9Keycode( VK_NUMPAD0,    XK_KP_0,         0x52 ),

            ADD        = kMoon9Keycode( VK_ADD,        XK_KP_Add,       0x45 ),
            SUBTRACT   = kMoon9Keycode( VK_SUBTRACT,   XK_KP_Subtract,  0x4E ),
            MULTIPLY   = kMoon9Keycode( VK_MULTIPLY,   XK_KP_Multiply,  0x43 ),
            DIVIDE     = kMoon9Keycode( VK_DIVIDE,     XK_KP_Divide,    0x4B ),
            SEPARATOR  = kMoon9Keycode( VK_SEPARATOR,  XK_KP_Separator, 0x2B ),
            DECIMAL    = kMoon9Keycode( VK_DECIMAL,    XK_KP_Decimal,   0x41 )
            //PAUSE    = kMoon9Keycode( VK_PAUSE,      XK_Pause,        ERROR )
        };
    };

    struct mousecode
    {
        enum enumeration
        {
            LEFT,
            MIDDLE,
            RIGHT
        };
    };

    struct mouseflag
    {
        enum enumeration
        {
            HOVER,
            IS_CONNECTED
        };
    };

}

#undef kMoon9Keycode

