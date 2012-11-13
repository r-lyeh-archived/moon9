/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// ArbProgram.h  -- Copyright (c) 2007 David Henry
// last modification: jan. 24, 2007
//
// This code is licenced under the MIT license.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
// Definitions of ARB program related classes.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __ARB_PROGRAM_H__
#define __ARB_PROGRAM_H__

#include <GL/glew.h>
#include <stdexcept>
#include <string>

/////////////////////////////////////////////////////////////////////////////
// ARB Program class diagram:
//
//                    +---------- (abs)
//                    |  ArbProgram  |
//                    +--------------+
//                           ^
//                           |
//             +-------------+-------------+
//             |                           |
//   +------------------+        +--------------------+
//   | ArbVertexProgram |        | ArbFragmentProgram |
//   +------------------+        +--------------------+
//
/////////////////////////////////////////////////////////////////////////////

// Global functions for initializing ARB program extensions and query for
// vertex and fragment program support on the host.
GLboolean hasArbVertexProgramSupport ();
GLboolean hasArbFragmentProgramSupport ();

void initArbProgramHandling ();


/////////////////////////////////////////////////////////////////////////////
//
// class ArbProgram -- ARB Program abstract object.  Can be a vertex program
// or a fragment program.
//
/////////////////////////////////////////////////////////////////////////////

class ArbProgram
{
protected:
  // Constructor
  ArbProgram (const std::string &filename);

public:
  // Destructor
  virtual ~ArbProgram ();

public:
  // Accessors
  const std::string &name () const { return _name; }
  const std::string &code () const { return _code; }
  GLuint handle () const { return _handle; }
  bool fail () const { return _fail; }

  virtual GLenum programType () const = 0;

public:
  // Public interface
  void use () const;
  void unuse () const;

protected:
  // Internal functions
  void printProgramString (int errPos);
  void load ()
    throw (std::runtime_error);
  void loadProgramFile (const std::string &filename)
    throw (std::runtime_error);

protected:
  // Member variables
  std::string _name;
  std::string _code;
  GLuint _handle;
  GLboolean _fail;
};


/////////////////////////////////////////////////////////////////////////////
//
// class ArbVertexProgram -- ARB vertex program object.
//
/////////////////////////////////////////////////////////////////////////////

class ArbVertexProgram : public ArbProgram
{
public:
  // Constructor
  ArbVertexProgram (const std::string &filename);

public:
  // Return the program enum type
  virtual GLenum programType () const {
    return GL_VERTEX_PROGRAM_ARB;
  }
};


/////////////////////////////////////////////////////////////////////////////
//
// class ArbFragmentProgram -- ARB fragment program object.
//
/////////////////////////////////////////////////////////////////////////////

class ArbFragmentProgram : public ArbProgram
{
public:
  // Constructor
  ArbFragmentProgram (const std::string &filename);

public:
  // Return the program enum type
  virtual GLenum programType () const {
    return GL_FRAGMENT_PROGRAM_ARB;
  }
};

#endif // __ARB_PROGRAM_H__
