/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// Main.h  -- Copyright (c) 2007 David Henry
// last modification: jan. 27, 2007
//
// This code is licenced under the MIT license.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MAIN_H__
#define __MAIN_H__

// We can use a specific render path, depending on
// which shader/program we want to use...
enum render_path_e
  {
    R_fixed,
    R_shader
  };

extern render_path_e renderPath;

// Current shader and vertex/fragment programs
extern ShaderProgram *shader;

// OpenGL Font
extern TTFont *font;

// Tangent uniform's location
extern GLint tangentLoc;

// ARB program's tangent location
#define TANGENT_LOC 6

// Application parameters
extern bool bParallax;
extern bool bDrawNormals;

#endif // __MAIN_H__
