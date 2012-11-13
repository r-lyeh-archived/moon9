/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// Md3Model.cpp -- Copyright (c) 2006-2007 David Henry
// last modification: may. 7, 2007
//
// This code is licenced under the MIT license.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
// Implementation of MD3 Model Classes.
//
/////////////////////////////////////////////////////////////////////////////

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <cmath>

using std::cout;
using std::cerr;
using std::endl;

#include "Md3Model.h"
#include "TextureManager.h"


/////////////////////////////////////////////////////////////////////////////
//
// struct Md3Model::Md3QuaternionTag implementation.
//
/////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------
// Md3Model::Md3QuaternionTag::Md3QuaternionTag
//
// Constructor.  Initialize the quaternion tag from the original tag
// with 3x3 rotation matrix.
// --------------------------------------------------------------------------

Md3Model::Md3QuaternionTag::Md3QuaternionTag (const Md3Tag_t &tag)
  : name (tag.name)
{
  // Copy the origin vector
  origin._x = tag.origin[0];
  origin._y = tag.origin[1];
  origin._z = tag.origin[2];

  // Create a 4x4 matrix from the 3x3 rotation matrix
  Matrix4x4f m;
  m._m11 = tag.axis[0][0]; m._m12 = tag.axis[0][1]; m._m13 = tag.axis[0][2];
  m._m21 = tag.axis[1][0]; m._m22 = tag.axis[1][1]; m._m23 = tag.axis[1][2];
  m._m31 = tag.axis[2][0]; m._m32 = tag.axis[2][1]; m._m33 = tag.axis[2][2];

  // Convert the matrix to quaternion
  orient.fromMatrix (m);
  orient.normalize ();
}


/////////////////////////////////////////////////////////////////////////////
//
// class NormalLookupTable implementation.
//
/////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------
// NormalLookupTable::NormalLookupTable
//
// Constructor.  Build the normal lookup table.
// --------------------------------------------------------------------------

NormalLookupTable::NormalLookupTable ()
{
  for (int i = 0; i < 256; ++i)
    {
      for (int j = 0; j < 256; ++j)
	{
	  float lng = i * 2.0f * kPi / 255.0f;
	  float lat = j * 2.0f * kPi / 255.0f;

	  _normal[i][j][0] = std::cos (lat) * std::sin (lng);
	  _normal[i][j][1] = std::sin (lat) * std::sin (lng);
	  _normal[i][j][2] = std::cos (lng);
	}
    }
}


/////////////////////////////////////////////////////////////////////////////
//
// class Md3Mesh implementation.
//
/////////////////////////////////////////////////////////////////////////////

// Precomputed normal vector array
const NormalLookupTable Md3Mesh::_kAnorms;

// Magic number (should be 860898377)
const int Md3Mesh::_kMd3Ident = 'I' + ('D'<<8) + ('P'<<16) + ('3'<<24);

// Vertex arrays
vec3_t Md3Mesh::_kVertexArray[kMd3MaxTriangles * 3];
vec3_t Md3Mesh::_kNormalArray[kMd3MaxTriangles * 3];


// --------------------------------------------------------------------------
// Md3Mesh::Md3Mesh
//
// Constructor.  Read a mesh from an ifstream.
// --------------------------------------------------------------------------

Md3Mesh::Md3Mesh (std::ifstream &ifs)
  throw (Md3Exception)
  : _tex (NULL)
{
  // File must be successfully opened
  if (!ifs.is_open ())
    throw Md3Exception ("Stream not opened");

  // Get the current position in the stream
  long pos = ifs.tellg ();

  // Read header
  ifs.read (reinterpret_cast<char *>(&_header),
	    sizeof (Md3MeshHeader_t));

  // Check if ident is valid
  if (_header.ident != _kMd3Ident)
    throw Md3Exception ("Bad mesh ident");

  // Memory allocation for model data
  _shaders.reserve (_header.num_shaders);
  _triangles.reserve (_header.num_triangles);
  _texCoords.reserve (_header.num_verts);
  _vertices.reserve (_header.num_verts * _header.num_frames);
  _indices.reserve (_header.num_triangles * 3);

  // Read shaders
  Md3Shader_t shader;
  ifs.seekg (pos + _header.offset_shaders, std::ios::beg);
  for (int i = 0; i < _header.num_shaders; ++i)
    {
      ifs.read (reinterpret_cast<char *>(&shader), sizeof (Md3Shader_t));
      _shaders.push_back (Md3ShaderPtr (new Md3Shader_t (shader)));
    }

  // Read triangles
  Md3Triangle_t tri;
  ifs.seekg (pos + _header.offset_triangles, std::ios::beg);
  for (int i = 0; i < _header.num_triangles; ++i)
    {
      ifs.read (reinterpret_cast<char *>(&tri), sizeof (Md3Triangle_t));
      _triangles.push_back (Md3TrianglePtr (new Md3Triangle_t (tri)));
    }

  // Read texture coords.
  Md3TexCoord_t st;
  ifs.seekg (pos + _header.offset_st, std::ios::beg);
  for (int i = 0; i < _header.num_verts; ++i)
    {
      ifs.read (reinterpret_cast<char *>(&st), sizeof (Md3TexCoord_t));
      _texCoords.push_back (st);
    }

  // Read vertices
  // Read texture coords.
  Md3Vertex_t vert;
  ifs.seekg (pos + _header.offset_xyznormal, std::ios::beg);
  for (int i = 0; i < _header.num_verts * _header.num_frames; ++i)
    {
      ifs.read (reinterpret_cast<char *>(&vert), sizeof (Md3Vertex_t));
      _vertices.push_back (Md3VertexPtr (new Md3Vertex_t (vert)));
    }

  // Jump to end of mesh into the stream
  ifs.seekg (pos + _header.offset_end, std::ios::beg);

  // Initialize vertex index array
  for (int i = 0; i < _header.num_triangles; ++i)
    {
      // Triangles are stored in clockwise order, reverse them
      // so that front faces are in CCW (OpenGL default)
      for (int j = 2; j >= 0; --j)
	_indices.push_back (_triangles[i]->index[j]);
    }
}


// --------------------------------------------------------------------------
// Md3Mesh::~Md3Mesh
//
// Destructor.  Free allocated memory for the mesh.
// --------------------------------------------------------------------------

Md3Mesh::~Md3Mesh ()
{
}


// --------------------------------------------------------------------------
// Md3Mesh::loadShaders
//
// Load mesh's texture from shader #index.
// --------------------------------------------------------------------------

void
Md3Mesh::loadShader (int index)
{
  if (index < 0 || index >= _header.num_shaders)
    return;

  const string filename (_shaders[index]->name);
  if (!filename.empty ())
    {
      // Get texture manager
      Texture2DManager *texMgr = Texture2DManager::getInstance ();

      // Load the texture
      setTexture (texMgr->load (filename));
    }
}


// --------------------------------------------------------------------------
// Md3Mesh::bindTexture
//
// Bind the active texture unit to mesh's current texture.
// --------------------------------------------------------------------------

void
Md3Mesh::bindTexture () const
{
  if (!_tex)
    {
      // Disable texture and return
      glBindTexture (GL_TEXTURE_2D, 0);
      return;
    }

  _tex->bind ();

  // Quake doesn't use the OpenGL standard coordinate system
  // for images, i.e. image data "starts" at the upper-left
  // corner instead of the lower-left corner.

  // We must reverse the t component if the texture
  // has been built with OpenGL's coord. system
  if (_tex->stdCoordSystem ())
    {
      glMatrixMode (GL_TEXTURE);
      glLoadIdentity ();
      glScalef (1.0f, -1.0f, 1.0f);
      glTranslatef (0.0f, -1.0f, 0.0f);
      glMatrixMode (GL_MODELVIEW);
    }
}


// --------------------------------------------------------------------------
// Md3Mesh::setupVertexArrays
//
// Compute final mesh's vertices from frames frameA and frameB with
// linear interpolation.  The resultant vertices are stored in the
// vertex array.
// NOTE: there is no test if frameA and frameB are valid!
// --------------------------------------------------------------------------

void
Md3Mesh::setupVertexArrays (int frameA, int frameB, float interp, float scale)
{
  int frameOffsetA = frameA * _header.num_verts;
  int frameOffsetB = frameB * _header.num_verts;

  float s = scale * kMd3XYZScale;

  for (int i = 0; i < _header.num_verts; ++i)
    {
      const Md3Vertex_t *pVertA = _vertices[frameOffsetA + i].get ();
      const Md3Vertex_t *pVertB = _vertices[frameOffsetB + i].get ();

      // Compute interpolated normal vector
      const int &uA = pVertA->normal[0];
      const int &vA = pVertA->normal[1];

      const int &uB = pVertB->normal[0];
      const int &vB = pVertB->normal[1];

      const float *normA = _kAnorms[uA][vA];
      const float *normB = _kAnorms[uB][vB];

      _kNormalArray[i][0] = normA[0] + interp * (normB[0] - normA[0]);
      _kNormalArray[i][1] = normA[1] + interp * (normB[1] - normA[1]);
      _kNormalArray[i][2] = normA[2] + interp * (normB[2] - normA[2]);

      // Compute interpolated vertex position
      const short *vertA = pVertA->v;
      const short *vertB = pVertB->v;

      _kVertexArray[i][0] = (vertA[0] + interp * (vertB[0] - vertA[0])) * s;
      _kVertexArray[i][1] = (vertA[1] + interp * (vertB[1] - vertA[1])) * s;
      _kVertexArray[i][2] = (vertA[2] + interp * (vertB[2] - vertA[2])) * s;
    }
}


// --------------------------------------------------------------------------
// Md3Mesh::renderFrameImmediate
//
// Draw the mesh at a specified frame, using immediate mode.
// NOTE: we don't check here if frame is a valid index!
// --------------------------------------------------------------------------

void
Md3Mesh::renderFrameImmediate (int frame, float scale) const
{
  int frameOffset = frame * _header.num_verts;

  // Scale and uncompress vertex in one pass
  float scale_and_uncompress = scale * kMd3XYZScale;

  glPushAttrib (GL_POLYGON_BIT);
  glFrontFace (GL_CW);

  // Bind to mesh's texture
  bindTexture ();

  glBegin (GL_TRIANGLES);
    // Draw each triangle
    for (int i = 0; i < _header.num_triangles; ++i)
      {
	// Draw each vertex of this triangle
	for (int j = 0; j < 3; ++j)
	  {
	    const int &vertIndex = _triangles[i]->index[j];
	    const Md3Vertex_t *pVert = _vertices[frameOffset + vertIndex].get ();
	    const Md3TexCoord_t *pTexCoords = &_texCoords[vertIndex];

	    // Send texture coords. to OpenGL
	    glTexCoord2f (pTexCoords->s, pTexCoords->t);

	    // Send normal vector to OpenGL
	    const unsigned char &un = pVert->normal[0];
	    const unsigned char &vn = pVert->normal[1];

	    glNormal3fv (_kAnorms[un][vn]);

	    // Uncompress vertex position and scale it
	    vec3_t v;

	    v[0] = pVert->v[0] * scale_and_uncompress;
	    v[1] = pVert->v[1] * scale_and_uncompress;
	    v[2] = pVert->v[2] * scale_and_uncompress;

	    glVertex3fv (v);
	  }
      }
  glEnd ();

  // GL_POLYGON_BIT
  glPopAttrib ();
}


// --------------------------------------------------------------------------
// Md3Mesh::renderWithVertexArrays
//
// Draw the mesh using vertex arrays.  Vertex arrays must be previously
// computed by a call to setupVertexArrays().
// --------------------------------------------------------------------------

void
Md3Mesh::renderWithVertexArrays () const
{
  glEnableClientState (GL_VERTEX_ARRAY);
  glEnableClientState (GL_NORMAL_ARRAY);
  glEnableClientState (GL_TEXTURE_COORD_ARRAY);

  // Send arrays to OpenGL
  glVertexPointer (3, GL_FLOAT, 0, _kVertexArray);
  glNormalPointer (GL_FLOAT, 0, _kNormalArray);
  glTexCoordPointer (2, GL_FLOAT, 0, &_texCoords.front ());

  // Bind to mesh's texture
  bindTexture ();

  // Draw triangles
  glDrawElements (GL_TRIANGLES, _header.num_triangles * 3,
		  GL_UNSIGNED_INT, &_indices.front ());

  glDisableClientState (GL_VERTEX_ARRAY);
  glDisableClientState (GL_NORMAL_ARRAY);
  glDisableClientState (GL_TEXTURE_COORD_ARRAY);
}


/////////////////////////////////////////////////////////////////////////////
//
// class Md3Model implementation.
//
/////////////////////////////////////////////////////////////////////////////

// Magic number (should be 860898377)
const int Md3Model::_kMd3Ident = 'I' + ('D'<<8) + ('P'<<16) + ('3'<<24);

// MD3 format version
const int Md3Model::_kMd3Version = 15;


// --------------------------------------------------------------------------
// Md3Model::Md3Model
//
// Constructor.  Load a model from file.
// --------------------------------------------------------------------------

Md3Model::Md3Model (const string &filename)
  throw (Md3Exception)
  : _currFrame (0), _nextFrame (0), _interp (0.0f),
    _scale (1.0f), _name (filename)
{
  // Open the file
  std::ifstream ifs (filename.c_str(), std::ios::in | std::ios::binary);

  if (ifs.fail ())
    throw Md3Exception ("Couldn't open file", filename);

  // Read header
  ifs.read (reinterpret_cast<char *>(&_header),
	    sizeof (Md3Header_t));

  // Check if ident and version are valid
  if (_header.ident != _kMd3Ident)
    throw Md3Exception ("Bad file ident", filename);

  if (_header.version != _kMd3Version)
    throw Md3Exception ("Bad file version", filename);

  // Memory allocation for model data
  _meshes.reserve (_header.num_meshes);
  _qtags.reserve (_header.num_tags * _header.num_frames);
  _links.reserve (_header.num_tags);

  // Read frames.
  Md3Frame_t frame;
  ifs.seekg (_header.offset_frames, std::ios::beg);
  for (int i = 0; i < _header.num_frames; ++i)
    {
      ifs.read (reinterpret_cast<char *>(&frame), sizeof (Md3Frame_t));
      _frames.push_back (Md3FramePtr (new Md3Frame_t (frame)));
    }

  // Read tags
  Md3Tag_t tag;
  ifs.seekg (_header.offset_tag, std::ios::beg);
  for (int i = 0; i < _header.num_tags * _header.num_frames; ++i)
    {
      ifs.read (reinterpret_cast<char *>(&tag), sizeof (Md3Tag_t));
      _qtags.push_back (Md3TagPtr (new Md3QuaternionTag (tag)));
    }

  // Read meshes
  ifs.seekg (_header.offset_meshes, std::ios::beg);
  for (int i = 0; i < _header.num_meshes; ++i)
    _meshes.push_back (Md3MeshPtr (new Md3Mesh (ifs)));

  // We finished reading, close file
  ifs.close();

  // Initialize links to NULL pointers
  for (int i = 0; i < _header.num_tags; ++i)
    _links.push_back (NULL);
}


// --------------------------------------------------------------------------
// Md3Model::~Md3Model
//
// Destructor.  Free allocated memory for the model.  Thanks to
// boost::shared_ptr, we have nothing to do ;)
// --------------------------------------------------------------------------

Md3Model::~Md3Model ()
{
}


// --------------------------------------------------------------------------
// Md3Model::loadShaders
//
// Load textures for each mesh using meshes' shader string number #0.
// --------------------------------------------------------------------------

void
Md3Model::loadShaders () const
{
  for (int i = 0; i < _header.num_meshes; ++i)
    _meshes[i]->loadShader (0);
}


// --------------------------------------------------------------------------
// Md3Model::draw
//
// Draw the model and linked models with frame interpolation between
// current and next frames.
// --------------------------------------------------------------------------

void
Md3Model::draw () const
{
  Matrix4x4f m;

  // Draw the model
  renderFrameItpWithVertexArrays (_currFrame, _nextFrame, _interp);

  // Draw models linked to this one
  for (int i = 0; i < _header.num_tags; ++i)
    {
      if (!_links[i])
	continue;

      const Quaternionf &qA = _qtags[_currFrame * _header.num_tags + i]->orient;
      const Quaternionf &qB = _qtags[_nextFrame * _header.num_tags + i]->orient;

      m.fromQuaternion (Slerp (qA, qB, _interp));

      const Vector3f &currPos = _qtags[_currFrame * _header.num_tags + i]->origin;
      const Vector3f &nextPos = _qtags[_nextFrame * _header.num_tags + i]->origin;

      m.setTranslation ((currPos + _interp * (nextPos - currPos)) * _scale);

      glPushMatrix ();
	glMultMatrixf (m);
	_links[i]->draw ();
      glPopMatrix ();
    }
}


// --------------------------------------------------------------------------
// Md3Model::renderFrameImmediate
//
// Draw each mesh of the model at a specified frame, using immediate mode.
// --------------------------------------------------------------------------

void
Md3Model::renderFrameImmediate (int frame) const
{
  // Check if the frame index is valid
  if ((frame < 0) || (frame >= _header.num_frames))
    return;

  for (int i = 0; i < _header.num_meshes; ++i)
    _meshes[i]->renderFrameImmediate (frame, _scale);
}


// --------------------------------------------------------------------------
// Md3Model::renderFrameImmediate
//
// Draw each mesh of the model with linear interpolation between
// frameA and frameB, using vertex arrays.
// --------------------------------------------------------------------------

void
Md3Model::renderFrameItpWithVertexArrays (int frameA, int frameB,
					  float interp) const
{
  int maxFrame = _header.num_frames - 1;

  // Check if frames are valid
  if ((frameA < 0) || (frameB < 0))
    return;

  if ((frameA > maxFrame) || (frameB > maxFrame))
    return;

  for (int i = 0; i < _header.num_meshes; ++i)
    {
      _meshes[i]->setupVertexArrays (frameA, frameB, interp, _scale);
      _meshes[i]->renderWithVertexArrays ();
    }
}


// --------------------------------------------------------------------------
// Md3Model::link
//
// Link a model to a tag.
// --------------------------------------------------------------------------

bool
Md3Model::link (const string &name, const Md3Model *model)
{
  for (int i = 0; i < _header.num_tags; ++i)
    {
      if (name == _qtags[i]->name)
	{
	  _links[i] = model;
	  return true;
	}
    }

  return false;
}


// --------------------------------------------------------------------------
// Md3Model::unlink
//
// Unlink a model given the tag to which it is associated.
// --------------------------------------------------------------------------

bool
Md3Model::unlink (const string &name)
{
  for (int i = 0; i < _header.num_tags; ++i)
    {
      if (name == _qtags[i]->name)
	{
	  _links[i] = NULL;
	  return true;
	}
    }

  return false;
}

// --------------------------------------------------------------------------
// Md3Model::setTexture
//
// Set the texture to the specified mesh.
// --------------------------------------------------------------------------

void
Md3Model::setTexture (const string &mesh, const Texture2D *tex)
{
  for (int i = 0; i < _header.num_meshes; ++i)
    {
      if (_meshes[i]->name () == mesh)
	_meshes[i]->setTexture (tex);
    }
}


// --------------------------------------------------------------------------
// Md3Model::setupAnimation
//
// Set the current frame, next frame and interpolation percent for
// the next "animated" rendering of the model.
// --------------------------------------------------------------------------

void
Md3Model::setupAnimation (int currFrame, int nextFrame, float interp)
{
  _currFrame = currFrame;
  _nextFrame = nextFrame;
  _interp = interp;

  // Ensure that current frame and next frame aren't out of bounds
  if (_currFrame >= _header.num_frames)
    _currFrame = _header.num_frames - 1;

  if (_nextFrame >= _header.num_frames)
    _nextFrame = _header.num_frames - 1;
}
