#ifndef __BACKEND_FREEGLUT_INSTALL_ME__
#define __BACKEND_FREEGLUT_INSTALL_ME__

#include "freeglut.h"

#pragma warning(push, 0)
#pragma comment(lib, "opengl32")
#pragma comment(lib, "glu32")
#pragma comment(lib, "winmm")

#if defined(__cplusplus)
extern "C" {
#endif

#include <GL/freeglut_init.c>

#include <GL/freeglut_callbacks.c>
#include <GL/freeglut_cursor.c>
#include <GL/freeglut_display.c>
#include <GL/freeglut_ext.c>
#include <GL/freeglut_font.c>
#include <GL/freeglut_font_data.c>
#include <GL/freeglut_gamemode.c>
#include <GL/freeglut_geometry.c>
#include <GL/freeglut_glutfont_definitions.c>
#include <GL/freeglut_input_devices.c>
#include <GL/freeglut_internal.h>
#include <GL/freeglut_joystick.c>
#include <GL/freeglut_main.c>
#include <GL/freeglut_menu.c>
#include <GL/freeglut_misc.c>
#include <GL/freeglut_overlay.c>
#include <GL/freeglut_spaceball.c>
#include <GL/freeglut_state.c>
#include <GL/freeglut_stroke_mono_roman.c>
#include <GL/freeglut_stroke_roman.c>
#include <GL/freeglut_structure.c>
#include <GL/freeglut_teapot.c>
#include <GL/freeglut_teapot_data.h>
#include <GL/freeglut_videoresize.c>
#include <GL/freeglut_window.c>

#if defined(__cplusplus)
}
#endif

#pragma warning(pop)

#endif //__BACKEND_FREEGLUT_INSTALL_ME__

