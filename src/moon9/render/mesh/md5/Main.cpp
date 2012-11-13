/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// Main.cpp -- Copyright (c) 2006-2007 David Henry
// last modification: jan. 28, 2007
//
// This code is licenced under the MIT license.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
// Doom 3's MD5Mesh Viewer main source file.
//
/////////////////////////////////////////////////////////////////////////////

#include <GL/glew.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

#include "GlErrors.h"
#include "Mathlib.h"
#include "Font.h"
#include "Md5Model.h"
#include "TextureManager.h"
#include "ArbProgram.h"
#include "Shader.h"
#include "Main.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;


Md5Model *model = NULL;
Md5Object *object = NULL;
TTFont *font = NULL;

// Current shader and vertex/fragment programs
ShaderProgram *shader = NULL;
ArbVertexProgram *vp = NULL;
ArbFragmentProgram *fp = NULL;

// All vertex and fragment programs
ArbVertexProgram *vp_bump = NULL;
ArbVertexProgram *vp_bump_parallax = NULL;

ArbFragmentProgram *fp_diffuse = NULL;
ArbFragmentProgram *fp_diffuse_specular = NULL;
ArbFragmentProgram *fp_ds_parallax = NULL;

// We can use a specific render path, depending on
// which shader/program we want to use...
render_path_e renderPath;

// Tangent uniform's location
GLint tangentLoc = -1;


int renderFlags = Md5Object::kDrawModel;
int fps = 0;

bool bAnimate = true;
bool bTextured = true;
bool bCullFace = true;
bool bBounds = false;
bool bParallax = false;
bool bLight = true;
bool bSmooth = true;
bool bWireframe = false;
bool bDrawNormals = false;

vector<string> animations;

#include <moon9/render/window.hpp>

const char windowTitle[] = "MD5 Model Viewer Demo";
const int windowWidth = 640;
const int windowHeight = 480;
const int windowDepth = 24;

// Camera
Vector3f rot, eye;

// Application's Timer
#include <moon9/time/dt.hpp>
struct Timer
{
  moon9::dt dt;
public:
  Timer ()
    : current_time (0.0), last_time (0.0) { }

public:
  void update () {
    last_time = current_time;
    current_time = dt.s();
  }

  double deltaTime () const {
    return (current_time - last_time);
  }

public:
  double current_time;
  double last_time;

} timer;


// -------------------------------------------------------------------------
// shutdownApp
//
// Application termination.
// -------------------------------------------------------------------------

bool isActive = true;
bool isExiting = false;

static void
shutdownApp (int returnCode)
{
  if( model ) delete model, model = 0;
  if( object ) delete object, object = 0;
  if( font ) delete font, font = 0;
  if( shader ) delete shader, shader = 0;
  if( vp_bump ) delete vp_bump, vp_bump = 0;
  if( vp_bump_parallax ) delete vp_bump_parallax, vp_bump_parallax = 0;
  if( fp_diffuse ) delete fp_diffuse, fp_diffuse = 0;
  if( fp_diffuse_specular ) delete fp_diffuse_specular, fp_diffuse_specular = 0;
  if( fp_ds_parallax ) delete fp_ds_parallax, fp_ds_parallax = 0;

  Texture2DManager::kill ();

  isExiting = true;
}


// -------------------------------------------------------------------------
// begin2D
//
// Enter into 2D mode.
// -------------------------------------------------------------------------

static void
begin2D ()
{
  GLint viewport[4];
  glGetIntegerv (GL_VIEWPORT, viewport);

  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();

  glLoadIdentity ();
  glOrtho (0, viewport[2], 0, viewport[3], -1, 1);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();
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
// extractFromQuotes
//
// Extract a string from quotes.
// -------------------------------------------------------------------------

inline const string
extractFromQuotes (const string &str)
{
  string::size_type start = str.find_first_of ('\"') + 1;
  string::size_type end = str.find_first_of ('\"', start) - 2;
  return str.substr (start, end);
}


// -------------------------------------------------------------------------
// parseLoaderScript
//
// Parse a script file for loading md5mesh and animations.
// -------------------------------------------------------------------------

static void
parseLoaderScript (const string &filename)
{
  // Open the file to parse
  std::ifstream file (filename.c_str(), std::ios::in);

  if (file.fail ())
    {
      cerr << "Couldn't open " << filename << endl;
      shutdownApp (1);
    }

  // Get texture manager
  Texture2DManager *texMgr = Texture2DManager::getInstance ();

  while (!file.eof ())
    {
      string token, buffer;
      string meshFile, animFile, textureFile;
      string meshName, animName;

      // Peek next token
      file >> token;

      if (token == "model")
	{
	  std::getline (file, buffer);
	  meshFile = extractFromQuotes (buffer);

	  // Delete previous model and object if existing
	  delete model;
	  delete object;

	  // Load mesh model
	  model = new Md5Model (meshFile);
	  object = new Md5Object (model);
	}
      else if (token == "anim")
	{
	  std::getline (file, buffer);
	  animFile = extractFromQuotes (buffer);

	  try
	    {
	      // Load animation
	      if (model)
		model->addAnim (animFile);
	    }
	  catch (Md5Exception &err)
	    {
	      cerr << "Failed to load animation "
		   << animFile << endl;
	      cerr << "Reason: " << err.what ()
		   << " (" << err.which () << ")" << endl;
	    }
	}
      else if (token == "hide")
	{
	  std::getline (file, buffer);
	  meshName = extractFromQuotes (buffer);

	  // Set mesh's render state
	  if (model)
	    model->setMeshRenderState (meshName, Md5Mesh::kHide);
	}
      else if ((token == "decalMap") ||
	       (token == "specularMap") ||
	       (token == "normalMap") ||
	       (token == "heightMap"))
	{
	  // Get the next token and extract the mesh name
	  file >> buffer;
	  long start = buffer.find_first_of ('\"') + 1;
	  long end = buffer.find_first_of ('\"', start) - 1;
	  meshName = buffer.substr (start, end);

	  // Get the rest of line and extract texture's filename
	  std::getline (file, buffer);
	  textureFile = extractFromQuotes (buffer);

	  // If the model has been loaded, setup
	  // the texture to the desired mesh
	  if (model)
	    {
	      Texture2D *tex = texMgr->load (textureFile);
	      if (tex->fail ())
		cerr << "failed to load " << textureFile << endl;

	      if (token == "decalMap")
		model->setMeshDecalMap (meshName, tex);
	      else if (token == "specularMap")
		model->setMeshSpecularMap (meshName, tex);
	      else if (token == "normalMap")
		model->setMeshNormalMap (meshName, tex);
	      else if (token == "heightMap")
		model->setMeshHeightMap (meshName, tex);
	    }
	}
      else if (token == "setAnim")
	{
	  std::getline (file, buffer);
	  animName = extractFromQuotes (buffer);

	  // Set object's default animation
	  object->setAnim (animName);
	}
    }

  file.close ();

  if (!model || !object)
    throw Md5Exception ("No mesh found!", filename);
}


// -------------------------------------------------------------------------
// announceRenderPath
//
// Print info about a render path.
// -------------------------------------------------------------------------

static void
announceRenderPath (render_path_e path)
{
  cout << "Render path: ";
  switch (path)
    {
    case R_normal:
      cout << "no bump mapping (fixed pipeline)" << endl;
      break;

    case R_ARBfp_diffuse:
      cout << "bump mapping, diffuse only "
	   << "(ARB vp & fp)" << endl;
      break;

    case R_ARBfp_diffuse_specular:
      cout << "bump mapping, diffuse and specular "
	   << "(ARB vp & fp)" << endl;
      break;

    case R_ARBfp_ds_parallax:
      cout << "bump mapping with parallax "
	   << "(ARB fp & fp)" << endl;
      break;

    case R_shader:
      cout << "bump mapping with parallax "
	   << "(GLSL)" << endl;
      break;
    }
}


// -------------------------------------------------------------------------
// initShader
//
// Shader's uniform variables initialization.
// -------------------------------------------------------------------------

static void
initShader ()
{
  if (NULL == shader)
    return;

  shader->use ();

  if (GLEW_VERSION_2_0)
    {
      GLuint prog = shader->handle ();

      // Set uniform parameters
      glUniform1i (glGetUniformLocation (prog, "decalMap"), 0);
      glUniform1i (glGetUniformLocation (prog, "glossMap"), 1);
      glUniform1i (glGetUniformLocation (prog, "normalMap"), 2);
      glUniform1i (glGetUniformLocation (prog, "heightMap"), 3);
      glUniform1i (glGetUniformLocation (prog, "parallaxMapping"), bParallax);

      // Get attribute location
      tangentLoc = glGetAttribLocation (prog, "tangent");
    }
  else
    {
      GLhandleARB prog = shader->handle ();

      // Set uniform parameters
      glUniform1iARB (glGetUniformLocationARB (prog, "decalMap"), 0);
      glUniform1iARB (glGetUniformLocationARB (prog, "glossMap"), 1);
      glUniform1iARB (glGetUniformLocationARB (prog, "normalMap"), 2);
      glUniform1iARB (glGetUniformLocationARB (prog, "heightMap"), 3);
      glUniform1iARB (glGetUniformLocationARB (prog, "parallaxMapping"), bParallax);

      // Get attribute location
      tangentLoc = glGetAttribLocationARB (prog, "tangent");
    }

  shader->unuse ();

  // Warn ff we fail to get tangent location... We'll can still use
  // the shader, but without tangents
  if (tangentLoc == -1)
    cerr << "Warning! No \"tangent\" uniform found in shader!" << endl;
}


// -------------------------------------------------------------------------
// initOpenGL
//
// OpenGL initialization.
// -------------------------------------------------------------------------

static void
initOpenGL ()
{
  glClearColor (0.5f, 0.5f, 0.5f, 0.0f);
  glShadeModel (GL_SMOOTH);
  glCullFace (GL_BACK);
  glEnable (GL_DEPTH_TEST);

  // Initialize GLEW
  GLenum err = glewInit ();
  if (GLEW_OK != err)
    {
      // Problem: glewInit failed, something is seriously wrong.
      cerr << "Error: " << glewGetErrorString (err) << endl;
      shutdownApp (-1);
    }

  // Print some infos about user's OpenGL implementation
  cout << "OpenGL Version String: " << glGetString (GL_VERSION) << endl;
  cout << "GLU Version String: " << gluGetString (GLU_VERSION) << endl;
  cout << "GLEW Version String: " << glewGetString (GLEW_VERSION) << endl;

  // Initialize ARB vertex/fragment program support
  initArbProgramHandling ();

  // Initialize GLSL shader support
  initShaderHandling ();

  if (hasArbVertexProgramSupport () &&
      hasArbFragmentProgramSupport ())
    {
      // Load ARB programs
      vp_bump = new ArbVertexProgram ("bump.vp");
      vp_bump_parallax = new ArbVertexProgram ("bumpparallax.vp");

      fp_diffuse = new ArbFragmentProgram ("bumpd.fp");
      fp_diffuse_specular = new ArbFragmentProgram ("bumpds.fp");
      fp_ds_parallax = new ArbFragmentProgram ("bumpdsp.fp");

      // Current ARB programs will be bump mapping with diffuse
      // and specular components
      vp = vp_bump;
      fp = fp_diffuse_specular;
    }

  if (hasShaderSupport ())
    {
      // Load shader
      VertexShader vs ("bump.vert");
      FragmentShader fs ("bump.frag");
      shader = new ShaderProgram ("bump mapping", vs, fs);

      // Initialize shader's uniforms
      initShader ();
    }

  // Announce avalaible render paths, select the best
  cout << endl << "Avalaible render paths:" << endl;

  cout << " [F3] - No bump mapping (fixed pipeline)" << endl;
  renderPath = R_normal;

  if (vp_bump && fp_diffuse)
    {
      cout << " [F4] - Bump mapping, diffuse only "
	   << "(ARB vp & fp)" << endl;
      renderPath = R_ARBfp_diffuse;
    }

  if (vp_bump && fp_diffuse_specular)
    {
      cout << " [F5] - Bump mapping, diffuse and specular "
	   << "(ARB vp & fp)" << endl;
      renderPath = R_ARBfp_diffuse_specular;
    }

  if (vp_bump_parallax && fp_ds_parallax)
    {
      cout << " [F6] - Bump mapping with parallax "
	   << "(ARB vp & fp)" << endl;
    }

  if (shader)
    {
      cout << " [F7] - Bump mapping with parallax "
	   << "(GLSL)" << endl;
      renderPath = R_shader;
    }

  // Announce which path has been chosen by default
  cout << endl;
  announceRenderPath (renderPath);

  // Initialize true type font
  try
    {
      font = new TTFont ("Vera.ttf", 12, 1);
    }
  catch (std::runtime_error &err)
    {
      cerr << "Failed to create truetype font" << endl;
      cerr << "Reason: " << err.what () << endl;
    }

  checkOpenGLErrors (__FILE__, __LINE__);
}


// -------------------------------------------------------------------------
// initApplication
//
// Application initialization.
// -------------------------------------------------------------------------

static void
initApplication (const string &filename)
{
  // Load model and animations
  try
    {
      if (filename.find (".md5mesh") == filename.length () - 8)
	{
	  // Load mesh model
	  model = new Md5Model (filename);
	  object = new Md5Object (model);
	}
      else
	parseLoaderScript (filename);
    }
  catch (Md5Exception &err)
    {
      cerr << "Failed to load model from " << filename << endl;
      cerr << "Reason: " << err.what () << " ("
	   << err.which () << ")" << endl;
      shutdownApp (-1);
    }

  // Build animation list
  Md5Model::AnimMap anims = model->anims ();
  animations.reserve (anims.size () + 1);
  animations.push_back (string ()); // bind-pose

  for (Md5Model::AnimMap::iterator itor = anims.begin ();
       itor != anims.end (); ++itor)
    animations.push_back (itor->first);

  // Camera initialization
  rot._x = 0.0;  eye._x = 0.0;
  rot._y = 0.0;  eye._y = 0.0;
  rot._z = 0.0;  eye._z = 200.0;
}


// -------------------------------------------------------------------------
// reshape
//
// Reinit OpenGL viewport when resizing window.
// -------------------------------------------------------------------------

static void
reshape (GLsizei width, GLsizei height)
{
  if (height == 0)
    height = 1;

  glViewport (0, 0, width, height);

  // Reinit projection matrix
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (45.0, width/static_cast<GLfloat>(height), 0.1f, 10000.0f);

  // Reinit model-view matrix
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
}


// -------------------------------------------------------------------------
// gameLogic
//
// Perform application logic.
// -------------------------------------------------------------------------

static void
gameLogic ()
{
  // Calculate frames per seconds
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
}


// -------------------------------------------------------------------------
// setupLight
//
// Setup light position and enable light0.
// -------------------------------------------------------------------------

void
setupLight (GLfloat x, GLfloat y, GLfloat z)
{
  GLfloat lightPos[4];
  lightPos[0] = x;
  lightPos[1] = y;
  lightPos[2] = z;
  lightPos[3] = 1.0f;

  glDisable (GL_LIGHTING);
  glDisable (GL_LIGHT0);

  if (bLight)
    {
      glPushMatrix ();
	glLoadIdentity ();
	glLightfv (GL_LIGHT0, GL_POSITION, lightPos);
      glPopMatrix ();

      glEnable (GL_LIGHTING);
      glEnable (GL_LIGHT0);
    }
}


// -------------------------------------------------------------------------
// drawObb
//
// Draw an Oriented Bouding Box.
// -------------------------------------------------------------------------

static void
drawObb (const OBBox_t &obb)
{
  Vector3f corners[8];

  corners[0] = Vector3f (-obb.extent._x, -obb.extent._y, -obb.extent._z);
  corners[1] = Vector3f ( obb.extent._x, -obb.extent._y, -obb.extent._z);
  corners[2] = Vector3f ( obb.extent._x,  obb.extent._y, -obb.extent._z);
  corners[3] = Vector3f (-obb.extent._x,  obb.extent._y, -obb.extent._z);
  corners[4] = Vector3f (-obb.extent._x, -obb.extent._y,  obb.extent._z);
  corners[5] = Vector3f ( obb.extent._x, -obb.extent._y,  obb.extent._z);
  corners[6] = Vector3f ( obb.extent._x,  obb.extent._y,  obb.extent._z);
  corners[7] = Vector3f (-obb.extent._x,  obb.extent._y,  obb.extent._z);

  glPushAttrib (GL_ENABLE_BIT);
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_LIGHTING);

  for (int i = 0; i < 8; ++i)
    {
      corners[i] += obb.center;
      obb.world.transform (corners[i]);
    }

  GLuint indices[24] =
    {
      0, 1, 1, 2, 2, 3, 3, 0,
      4, 5, 5, 6, 6, 7, 7, 4,
      0, 4, 1, 5, 2, 6, 3, 7
    };

  glColor3f (1.0, 0.0, 0.0);

  glEnableClientState (GL_VERTEX_ARRAY);
  glVertexPointer (3, GL_FLOAT, 0, corners);
  glDrawElements (GL_LINES, 24, GL_UNSIGNED_INT, indices);
  glDisableClientState (GL_VERTEX_ARRAY);

  // GL_ENABLE_BIT
  glPopAttrib();
}


// -------------------------------------------------------------------------
// drawAxes
//
// Draw the X, Y and Z axes at the center of world.
// -------------------------------------------------------------------------

static void
drawAxes (const Matrix4x4f &modelView)
{
  // Setup world model view matrix
  glLoadIdentity ();
  glMultMatrixf (modelView._m);

  // Draw the three axes
  glBegin (GL_LINES);
    // X-axis in red
    glColor3f (1.0f, 0.0f, 0.0f);
    glVertex3fv (kZeroVectorf._v);
    glVertex3fv (kZeroVectorf + Vector3f (10.0f, 0.0f, 0.0));

    // Y-axis in green
    glColor3f (0.0f, 1.0f, 0.0f);
    glVertex3fv (kZeroVectorf._v);
    glVertex3fv (kZeroVectorf + Vector3f (0.0f, 10.0f, 0.0));

    // Z-axis in blue
    glColor3f (0.0f, 0.0f, 1.0f);
    glVertex3fv (kZeroVectorf._v);
    glVertex3fv (kZeroVectorf + Vector3f (0.0f, 0.0f, 10.0));
  glEnd ();
}


// -------------------------------------------------------------------------
// draw3D
//
// Render the 3D part of the scene.
// -------------------------------------------------------------------------

static void
draw3D ()
{
  // Clear the window
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  // Camera rotation
  Matrix4x4f camera;

#if 0
  camera.identity ();

  glTranslated (-eye._x, -eye._y, -eye._z);
  glRotated (rot._x, 1.0f, 0.0f, 0.0f);
  glRotated (rot._y, 0.0f, 1.0f, 0.0f);
  glRotated (rot._z, 0.0f, 0.0f, 1.0f);
#else
  camera.fromEulerAngles (degToRad (rot._x),
			  degToRad (rot._y),
			  degToRad (rot._z));
  camera.setTranslation (-eye);
#endif

  Matrix4x4f axisRotation
    = RotationMatrix (kXaxis, -kPiOver2)
    * RotationMatrix (kZaxis, -kPiOver2);

  Matrix4x4f final = camera * axisRotation;
  //glMultMatrixf (final._m);

  // Setup scene lighting
  setupLight (0.0f, 20.0f, 100.0f);

  // Enable/disable texture mapping (fixed pipeline)
  if (bTextured)
    glEnable (GL_TEXTURE_2D);
  else
    glDisable (GL_TEXTURE_2D);

  // Enable/disable backface culling
  if (bCullFace)
    glEnable (GL_CULL_FACE);
  else
    glDisable (GL_CULL_FACE);

  // Setup polygon mode and shade model
  glPolygonMode (GL_FRONT_AND_BACK, bWireframe ? GL_LINE : GL_FILL);
  glShadeModel (bSmooth ? GL_SMOOTH : GL_FLAT);

  // Draw object
  object->setModelViewMatrix (final);
  object->setRenderFlags (renderFlags);
  object->animate (bAnimate ? timer.deltaTime () : 0.0f);
  object->computeBoundingBox ();
  object->prepare (false);
  object->render ();

  if (bBounds)
    drawObb (object->boundingBox ());

  glDisable (GL_LIGHTING);
  glDisable (GL_TEXTURE_2D);

  drawAxes (final);
}


// -------------------------------------------------------------------------
// draw2D
//
// Render the 2D part of the scene.
// -------------------------------------------------------------------------

static void
draw2D ()
{
  if (!font)
    return;

  begin2D ();

  // Reset texture matrix
  glActiveTexture (GL_TEXTURE0);
  glMatrixMode (GL_TEXTURE);
  glLoadIdentity ();
  glMatrixMode (GL_MODELVIEW);

  glPushAttrib (GL_ENABLE_BIT | GL_POLYGON_BIT);
  glDisable (GL_DEPTH_TEST);
  glDisable (GL_LIGHTING);
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

  // White text
  glColor4f (1.0f, 1.0f, 1.0f, 1.0f);

  // Print frame rate int the bottom-left corner
  font->printText (10, 10, "%i fps", fps);

  // Print current animation
  font->printText (10, 10 + font->getLineSkip (),
		   object->currAnimName ().c_str ());

  glPopAttrib ();

  end2D ();
}


// -------------------------------------------------------------------------
// display
//
// Render the main scene to the screen.
// -------------------------------------------------------------------------

static void
gameCycle ()
{
  gameLogic ();

  draw3D ();

  draw2D ();
}


// -------------------------------------------------------------------------
// handleKeyPress
//
// SDL Keyboard handling.
// -------------------------------------------------------------------------

#include <moon9/hid/windows.hpp>

  static moon9::windows::keyboard keyboard(0);
  static moon9::windows::mouse mouse(0);

static void
handleKeyPress ()
{
  keyboard.update();

  if( keyboard.left.trigger() || keyboard.right.trigger() )
    {
	// Find the current animation in the list
	vector<string>::iterator itor
	  = find (animations.begin (), animations.end (),
		  object->currAnimName ());

	// Peek next or previous animation name
	if ( keyboard.right.trigger() )
	  ++itor;
	else
	  {
	    if (itor == animations.begin ())
	      itor = animations.end ();

	    --itor;
	  }

	if (itor == animations.end ())
	  itor = animations.begin ();

	// Set the new animation to play
	object->setAnim (*itor);
      }

      if( keyboard.space.trigger() )
      bAnimate = !bAnimate;

    if( keyboard.b.trigger() )
      bBounds = !bBounds;

    if( keyboard.c.trigger() )
      bCullFace = !bCullFace;

    if( keyboard.j.trigger() )
      renderFlags ^= Md5Object::kDrawJointLabels;

    if( keyboard.k.trigger() )
      renderFlags ^= Md5Object::kDrawSkeleton;

    if( keyboard.l.trigger() )
      bLight = !bLight;

    if( keyboard.n.trigger() )
      bDrawNormals = !bDrawNormals;

    if( keyboard.p.trigger() )
      {
	bParallax = !bParallax;
	if (bParallax && shader)
	  cout << "Parallax Mapping: on" << endl;
	else
	  cout << "Parallax Mapping: off" << endl;

    	if (shader != NULL)
      {
        shader->use ();

        if (GLEW_VERSION_2_0)
        glUniform1i (glGetUniformLocation (shader->handle (),
        "parallaxMapping"), bParallax);
        else
        glUniform1iARB (glGetUniformLocationARB (shader->handle (),
        "parallaxMapping"), bParallax);

        shader->unuse ();
      }
    }

      if( keyboard.s.trigger() )
      bSmooth = !bSmooth;

      if( keyboard.t.trigger() )
      bTextured = !bTextured;

      if( keyboard.w.trigger() )
      bWireframe = !bWireframe;

      if( keyboard.f1.trigger() )
      ; //SDL_WM_ToggleFullScreen (surface);

    if( keyboard.f3.trigger() )
    {
      renderPath = R_normal;
      announceRenderPath (renderPath);
    }

    if( keyboard.f4.trigger() )
    {
      if (vp_bump && fp_diffuse)
    	{
    	  renderPath = R_ARBfp_diffuse;
    	  vp = vp_bump;
    	  fp = fp_diffuse;
    	}
      announceRenderPath (renderPath);
    }

    if( keyboard.f5.trigger() )
    {
      if (vp_bump && fp_diffuse_specular)
	{
	  renderPath = R_ARBfp_diffuse_specular;
	  vp = vp_bump;
	  fp = fp_diffuse_specular;
	}
      announceRenderPath (renderPath);
}

    if( keyboard.f6.trigger() )
    {
      if (vp_bump_parallax && fp_ds_parallax)
  {
    renderPath = R_ARBfp_ds_parallax;
    vp = vp_bump_parallax;
    fp = fp_ds_parallax;
  }
      announceRenderPath (renderPath);
    }

    if( keyboard.f7.trigger() )
    {
      if (shader)
	renderPath = R_shader;
      announceRenderPath (renderPath);
}

    if( keyboard.escape.trigger() )
    {
      shutdownApp (0);
    }
}


// -------------------------------------------------------------------------
// MouseMove
//
// SDL Mouse Input Control.
// -------------------------------------------------------------------------

static void
mouseMove ()
{
  static int x_pos = 0;
  static int y_pos = 0;

  mouse.update();

  int x = (int)mouse.client.newest().x;
  int y = (int)mouse.client.newest().y;

  // Right button
  if ( mouse.right.hold() )
    {
      // Zoom
      eye._z += (x - x_pos) * 0.1f;
    }
  // Left button
  else if ( mouse.left.hold() )
    {
      if ( keyboard.shift.hold() )
	{
	  // Translation
	  eye._x -= (x - x_pos) * 0.1f;
	  eye._y += (y - y_pos) * 0.1f;
	}
      else
	{
	  // Rotation
	  rot._x += (y - y_pos);
	  rot._y += (x - x_pos);
	}
    }

  x_pos = x;
  y_pos = y;
}


// -------------------------------------------------------------------------
// main
//
// Application main entry point.
// -------------------------------------------------------------------------

int
main (int argc, char *argv[])
{
  cout << "Built release " << __DATE__ << " at " << __TIME__ << endl;

  moon9::window2 surface( "md5", 0.75f ); //windowWidth / windowHeight );

  // Initialize OpenGL
  initOpenGL ();

  // Initialize application
  initApplication ((argc > 1) ? argv[1] : "zfat.loader");

  // Resize OpenGL window
  reshape (windowWidth, windowHeight);

  // Loop until the end
  while ( surface.is_open() && !isExiting )
    {
	      reshape ( surface.w, surface.h );

	      handleKeyPress();
	      mouseMove();

      // Update the timer
      timer.update ();

      // Draw scene if window is active
      if (isActive)
	gameCycle ();

  surface.flush();
    }

    shutdownApp(0);

  // We should never go here
  return 0;
}
