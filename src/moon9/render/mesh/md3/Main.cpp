/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// Main.cpp -- Copyright (c) 2007 David Henry
// last modification: may. 7, 2007
//
// This code is licenced under the MIT license.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
// Main file of the MD3 Player loader.
//
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdarg>
#include <cstdio>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>

#include <algorithm>

#include "Md3Player.h"
#include "TextureManager.h"

using std::vector;
using std::cout;
using std::cerr;
using std::endl;

// Keyboard
struct keyboard_input_t
{
  unsigned char keymap[256];
  int special[256];
  int modifiers;

} keyboard;

// Mouse
struct mouse_input_t
{
  int buttons[3];
  int x, y;

} mouse;

// Timer
struct glut_timer_t
{
  double current_time;
  double last_time;

} timer;

// Camera
struct Vector_3d
{
  float x, y, z;

} rot, eye;

const string animStrings[kMaxAnimations] = {
  "Both death 1", "Both dead 1", "Both death 2", "Both dead 2",
  "Both death 3", "Both dead 3", "Torso gesture", "Torso attack",
  "Torso attack 2", "Torso drop", "Torso raise", "Torso stand",
  "Torso stand 2", "Legs walk crouch", "Legs walk", "Legs run",
  "Legs back", "Legs swim", "Legs jump", "Legs land", "Legs jump B",
  "Legs land B", "Legs idle", "Legs idle crouch", "Legs turn"
};

Md3Player *player = NULL;
Md3Weapon *weapon = NULL;

bool bTextured = true;
bool bLightGL = true;
bool bAnimated = true;
bool bWeapon = false;

int verbose = 2;
int fps = 0;

vector<string> skinList;


// --------------------------------------------------------------------------
// animMenu & skinMenu
//
// GLUT menu callback functions. Handle the menus. Select the skin
// to use and the animation to play.
// --------------------------------------------------------------------------

static void
animMenu (int item)
{
  player->setAnimation (static_cast<Md3PlayerAnimType>(item));

  glutPostRedisplay ();
}


static void
skinMenu (int item)
{
  player->setSkin (skinList[item]);

  glutPostRedisplay ();
}


// -------------------------------------------------------------------------
// buildSkinMenu
//
// Create GLUT menu for skin selection.
// -------------------------------------------------------------------------

int
buildSkinMenu (const Md3Player::SkinMap &skinMap)
{
  string skinName;
  int i = 0;

  Md3Player::SkinMap::const_iterator itor;
  for (itor = skinMap.begin (); itor != skinMap.end (); ++itor)
    skinList.push_back (itor->first);

  std::sort (skinList.begin (), skinList.end ());

  int menuId = glutCreateMenu (skinMenu);

  vector<string>::iterator it;
  for (it = skinList.begin (); it != skinList.end (); ++it)
    {
      skinName.assign (*it, it->find_last_of ('/') + 1, it->length ());
      glutAddMenuEntry (skinName.c_str (), i++);
    }

  return menuId;
}


// -------------------------------------------------------------------------
// buildAnimMenu
//
// Create GLUT menu for animation selection.
// -------------------------------------------------------------------------

static int
buildAnimMenu (int start, int end)
{
  int menuId = glutCreateMenu (animMenu);

  for (int i = start; i < end; ++i)
    glutAddMenuEntry (animStrings[i].c_str (), i);

  return menuId;
}


// -------------------------------------------------------------------------
// shutdownApp
//
// Application termination.
// -------------------------------------------------------------------------

static void
shutdownApp ()
{
  Texture2DManager::kill ();

  delete player;
  delete weapon;
}


// -------------------------------------------------------------------------
// init
//
// Application initialization.  Setup keyboard input, mouse input,
// timer, camera and OpenGL.
// -------------------------------------------------------------------------

static void
init (const string &playerpath, const string &weaponpath)
{
  //
  // GLEW Initialization
  //

  GLenum err = glewInit ();
  if (GLEW_OK != err)
    {
      // Problem: glewInit failed, something is seriously wrong.
      cerr << "Error: " << glewGetErrorString (err) << endl;
      shutdownApp ();
    }

  //
  // Application initialization
  //

  // Initialize keyboard
  memset (keyboard.keymap, 0, 256);
  memset (keyboard.special, 0, 256);

  // Inititialize mouse
  mouse.buttons[GLUT_LEFT_BUTTON] = GLUT_UP;
  mouse.buttons[GLUT_MIDDLE_BUTTON] = GLUT_UP;
  mouse.buttons[GLUT_RIGHT_BUTTON] = GLUT_UP;
  mouse.x = 0;
  mouse.y = 0;

  // Initialize timer
  timer.current_time = 0;
  timer.last_time = 0;

  // Initialize camera input
  rot.x = 0.0f;   eye.x = 0.0f;
  rot.y = 0.0f;   eye.y = 0.0f;
  rot.z = 0.0f;   eye.z = 8.0f;

  // Get base dir for player if a *.md3 file is given
  string playerdir (playerpath);
  if (playerdir.find (".md3") == playerdir.length () - 4)
#ifdef _WIN32
    playerdir.assign (playerdir, 0, playerdir.find_last_of ('\\'));
#else
    playerdir.assign (playerdir, 0, playerdir.find_last_of ('/'));
#endif

  // Initialize MD3 player
  try
    {
      /*
      lower = new Md3Model ("models/players/harley/lower.md3");
      upper = new Md3Model ("models/players/harley/upper.md3");
      head = new Md3Model ("models/players/harley/head.md3");

      lower->link ("tag_torso", upper);
      upper->link ("tag_head", head);

      lower->setScale (0.1f);
      upper->setScale (0.1f);
      head->setScale (0.1f);
      */

      // Load MD3 player
      player = new Md3Player (playerdir);
      player->setScale (0.1f);
      player->setAnimation (kTorsoStand);
      player->setAnimation (kLegsIdle);
    }
  catch (std::runtime_error &err)
    {
      cerr << "Error: failed to load " << playerdir << endl;
      cerr << "Reason: " << err.what () << endl;
      exit (-1);
    }

  try
    {
      // Load weapon if a path is given
      if (!weaponpath.empty ())
	{
	  weapon = new Md3Weapon (weaponpath);
	  weapon->setScale (0.1f);
	  player->linkWeapon (weapon);
	  bWeapon = true;
	}
    }
  catch (std::runtime_error &err)
    {
      cerr << "Error: failed to load " << weaponpath << endl;
      cerr << "Reason: " << err.what () << endl;
    }

  //
  // Create GLUT menus
  //

  int deathAnimMenuId = buildAnimMenu (kBothDeath1, kTorsoGesture);
  int torsoAnimMenuId = buildAnimMenu (kTorsoGesture, kLegsWalkCr);
  int legsAnimMenuId = buildAnimMenu (kLegsWalkCr, kMaxAnimations);
  int skinMenuId = buildSkinMenu (player->skins ());

  glutCreateMenu (NULL);
  glutAddSubMenu ("Death animation", deathAnimMenuId);
  glutAddSubMenu ("Torso animation", torsoAnimMenuId);
  glutAddSubMenu ("Legs animation", legsAnimMenuId);
  glutAddSubMenu ("Skins", skinMenuId);
  glutAttachMenu (GLUT_RIGHT_BUTTON);

  //
  // Initialize OpenGL
  //

  glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
  glShadeModel (GL_SMOOTH);

  glEnable (GL_DEPTH_TEST);
  glEnable (GL_TEXTURE_2D);
  glEnable (GL_CULL_FACE);
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);

  glCullFace (GL_BACK);
}


// -------------------------------------------------------------------------
// reshape
//
// OpenGL window resizing.  Update the viewport and the projection matrix.
// -------------------------------------------------------------------------

static void
reshape (int w, int h)
{
  if (h == 0)
    h = 1;

  glViewport (0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (45.0, w / static_cast<GLfloat>(h), 0.1, 1000.0);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  glutPostRedisplay ();
}


// -------------------------------------------------------------------------
// updateTimer
//
// Update the timer.
// -------------------------------------------------------------------------

static void
updateTimer (struct glut_timer_t *t)
{
  t->last_time = t->current_time;
  t->current_time = glutGet (GLUT_ELAPSED_TIME) * 0.001f;
}


// -------------------------------------------------------------------------
// handleKeyboard
//
// Keyboard input handling.  Handle here continuous actions when a key
// is pressed (like moving the camera).
// -------------------------------------------------------------------------

static void
handleKeyboard (struct keyboard_input_t *k)
{
  /*
  if (k->keymap['yourkey'])
    do_something ();
  */
}


// -------------------------------------------------------------------------
// begin2D
//
// Enter into 2D mode.
// -------------------------------------------------------------------------

static void
begin2D ()
{
  int width = glutGet (GLUT_WINDOW_WIDTH);
  int height = glutGet (GLUT_WINDOW_HEIGHT);

  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();

  glLoadIdentity ();
  glOrtho (0, width, 0, height, -1.0f, 1.0f);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
}


// -------------------------------------------------------------------------
// end2D
//
// Return from 2D mode.
// -------------------------------------------------------------------------

static void
end2D ()
{
  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();
  glMatrixMode (GL_MODELVIEW);
}


// -------------------------------------------------------------------------
// glPrintf
//
// printf-like function for OpenGL.
// -------------------------------------------------------------------------



static int
glPrintf (const char *format, ...)
{
  char buffer[1024];
  std::va_list arg;
  int ret;

#if 0
  // Format the text
  va_start (arg, format);
    ret = std::vsnprintf (buffer, sizeof (buffer), format, arg);
  va_end (arg);
#else
  strcpy( buffer, format );
#endif

  // Print it
  for (unsigned int i = 0; i < strlen (buffer); ++i)
    glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, buffer[i]);

  return ret;
}


// -------------------------------------------------------------------------
// gameLogic
//
// Perform application logic.
// -------------------------------------------------------------------------

static void
gameLogic ()
{
  // Calculate frame per seconds
  static double current_time = 0;
  static double last_time = 0;
  static int n = 0;

  n++;
  current_time = timer.current_time;

  if ((current_time - last_time) >= 1.0)
    {
      fps = n;
      n = 0;
      last_time = current_time;
    }

  // Animate player
  if (bAnimated)
    {
      double dt = timer.current_time - timer.last_time;
      player->animate (dt);
    }
}


// -------------------------------------------------------------------------
// draw3D
//
// Render the 3D part of the scene.
// -------------------------------------------------------------------------

static void
draw3D ()
{
  // Clear window
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();

  // Perform camera transformations
  glTranslated (-eye.x, -eye.y, -eye.z);
  glRotated (rot.x, 1.0f, 0.0f, 0.0f);
  glRotated (rot.y, 0.0f, 1.0f, 0.0f);
  glRotated (rot.z, 0.0f, 0.0f, 1.0f);

  glEnable (GL_DEPTH_TEST);

  if (bLightGL)
    glEnable (GL_LIGHTING);

  if (bTextured)
    glEnable (GL_TEXTURE_2D);

  // Draw objects
  player->draw ();

  glDisable (GL_LIGHTING);
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_DEPTH_TEST);
}


// -------------------------------------------------------------------------
// draw2D
//
// Render the 2D part of the scene.
// -------------------------------------------------------------------------

static void
draw2D ()
{
  begin2D ();

  glColor3f (1.0f, 1.0f, 1.0f);

  if (verbose > 0)
    {
      glRasterPos2i (10, 10);
      glPrintf ("FPS: %i", fps);
    }

  if (verbose > 1)
    {
      glRasterPos2i (10, glutGet (GLUT_WINDOW_HEIGHT) - 20);
      glPrintf ("Rendering player: \"%s\"", player->name ().c_str ());

      glRasterPos2i (10, glutGet (GLUT_WINDOW_HEIGHT) - 35);
      glPrintf ("Current skin: \"%s\"", player->skinName ().c_str ());

      if (weapon)
	{
	  glRasterPos2i (10, glutGet (GLUT_WINDOW_HEIGHT) - 50);
	  glPrintf ("Rendering weapon: \"%s\"", weapon->name ().c_str ());

	  if (!bWeapon)
	    glPrintf (" (hiden)");
	}
    }

  end2D ();
}


// -------------------------------------------------------------------------
// display
//
// Render the main scene to the screen.
// -------------------------------------------------------------------------

static void
display ()
{
  gameLogic ();

  draw3D ();

  draw2D ();

  glutSwapBuffers ();
}


// -------------------------------------------------------------------------
// keyPress
//
// Key press glut callback function.  Called when user press a key.
// -------------------------------------------------------------------------

static void
keyPress (unsigned char key, int x, int y)
{
  // Update key state
  keyboard.keymap[key] = 1;

  //
  // Handle here ponctual actions when
  // a key is pressed (like toggle ligthing)
  //

  // Escape
  if (key == 27)
    exit (0);

  if (key == 'a' || key == 'A')
    bAnimated = !bAnimated;

  if (key == 'l' || key == 'L')
    bLightGL = !bLightGL;

  if (key == 'p' || key == 'P')
    {
      bWeapon = !bWeapon;

      if (bWeapon && weapon)
	player->linkWeapon (weapon);
      else
	player->unlinkWeapon ();
    }

  if (key == 's' || key == 'S')
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

  if (key == 't' || key == 'T')
    bTextured = !bTextured;

  if (key == 'v' || key == 'V')
    verbose = (verbose + 1) % 3;

  if (key == 'w' || key == 'W')
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

  glutPostRedisplay ();
}


// -------------------------------------------------------------------------
// keyUp
//
// Key up glut callback function.  Called when user release a key.
// -------------------------------------------------------------------------

static void
keyUp (unsigned char key, int x, int y)
{
  keyboard.keymap[key] = 0;
}


// -------------------------------------------------------------------------
// specialKeyPress
//
// Key press glut callback function.  Called when user press a special key.
// -------------------------------------------------------------------------

static void
specialKeyPress (int key, int x, int y)
{
  keyboard.special[key] = 1;
}


// -------------------------------------------------------------------------
// specialKeyUp
//
// Key up glut callback function.  Called when user release a special key.
// -------------------------------------------------------------------------

static void
specialKeyUp (int key, int x, int y)
{
  keyboard.special[key] = 0;
}


// -------------------------------------------------------------------------
// mouseMotion
//
// Mouse motion glut callback function.  Called when the user move the
// mouse. Update the camera.
// -------------------------------------------------------------------------

static void
mouseMotion (int x, int y)
{
  if (mouse.buttons[GLUT_MIDDLE_BUTTON] == GLUT_DOWN)
    {
      // Zoom
      eye.z += (x - mouse.x) * 0.1;
    }
  else if (mouse.buttons[GLUT_LEFT_BUTTON] == GLUT_DOWN)
    {
      if (keyboard.modifiers & GLUT_ACTIVE_SHIFT)
	{
	  // Translation
	  eye.x -= (x - mouse.x) * 0.02;
	  eye.y += (y - mouse.y) * 0.02;
	}
      else
	{
	  // Rotation
	  rot.x += (y - mouse.y);
	  rot.y += (x - mouse.x);
	}
    }

  mouse.x = x;
  mouse.y = y;

  glutPostRedisplay ();
}


// -------------------------------------------------------------------------
// mouseButton
//
// Mouse button press glut callback function.  Called when the user
// press a mouse button. Update mouse state and keyboard modifiers.
// -------------------------------------------------------------------------

static void
mouseButton (int button, int state, int x, int y)
{
  // Update key modifiers
  keyboard.modifiers = glutGetModifiers ();

  // Update mouse state
  mouse.buttons[button] = state;
  mouse.x = x;
  mouse.y = y;
}


// -------------------------------------------------------------------------
// idleVisible
//
// Idle glut callback function.  Continuously called. Perform background
// processing tasks.
// -------------------------------------------------------------------------

static void
idleVisible ()
{
  // Update the timer
  updateTimer (&timer);

  // Handle keyboard input
  handleKeyboard (&keyboard);

  if (bAnimated)
    glutPostRedisplay ();
}


// -------------------------------------------------------------------------
// windowStatus
//
// Window status glut callback function.  Called when the status of
// the window changes.
// -------------------------------------------------------------------------

static void
windowStatus (int state)
{
  // Disable rendering and/or animation when the
  // window is not visible
  if ((state != GLUT_HIDDEN) &&
      (state != GLUT_FULLY_COVERED))
    {
      glutIdleFunc (idleVisible);
    }
  else
    {
      glutIdleFunc (NULL);
    }
}


// -------------------------------------------------------------------------
// main
//
// Application main entry point.
// -------------------------------------------------------------------------

int
main (int argc, char *argv[])
{
  // Initialize GLUT
  glutInit (&argc, argv);

  if (argc < 2)
    {
      cerr << "usage: " << argv[0] << " <player path> [<weapon path>]" << endl;
      return -1;
    }

  // create an OpenGL window
  glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize (640, 480);
  glutCreateWindow ("Quake 3's MD3 Model Viewer");

  // Initialize application
  atexit (shutdownApp);
  init (argv[1], (argc > 2) ? argv[2] : string ());

  // Setup glut callback functions
  glutReshapeFunc (reshape);
  glutDisplayFunc (display);
  glutKeyboardFunc (keyPress);
  glutKeyboardUpFunc (keyUp);
  glutSpecialFunc (specialKeyPress);
  glutSpecialUpFunc (specialKeyUp);
  glutMotionFunc (mouseMotion);
  glutMouseFunc (mouseButton);
  glutWindowStatusFunc (windowStatus);
  glutIdleFunc (idleVisible);

  // Enter the main loop
  glutMainLoop ();

  return 0;
}
