/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// Md3Model.h -- Copyright (c) 2006-2007 David Henry
// last modification: may. 7, 2007
//
// This code is licenced under the MIT license.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
// Definition of MD3 Model Classes.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MD3MODEL_H__
#define __MD3MODEL_H__

#include <memory>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

#include "Mathlib.h"
#include "Texture.h"

using std::string;
using std::vector;
using std::map;

using std::shared_ptr;


// Constants definitions
const int kMd3MaxFrames = 1024;
const int kMd3MaxTags = 16;
const int kMd3MaxMeshes = 32;
const int kMd3MaxShaders = 256;
const int kMd3MaxTriangles = 8192;
const int kMd3MaxVertices = 4096;
const float kMd3XYZScale = 1.0f / 64.0f;


// OpenGL vector types
typedef GLfloat vec2_t[2];
typedef GLfloat vec3_t[3];


// MDL Header
struct Md3Header_t
{
  int ident;            // Magic number, "IDP3"
  int version;          // Md3 format version, should be 15

  char name[64];        // Path name
  int flags;            // ?

  int num_frames;       // Number of frames
  int num_tags;         // Number of tags
  int num_meshes;       // Number of meshes
  int num_skins;        // Number of skins

  int offset_frames;    // Offset to frame data
  int offset_tag;       // Offset to tag data
  int offset_meshes;    // Offset to meshes
  int offset_eof;       // Offset end of file
};


// Frame data
struct Md3Frame_t
{
  vec3_t min_bounds;    // First corner of the bbox
  vec3_t max_bounds;    // Second corner of the bbox
  vec3_t local_origin;

  float radius;         // Radius of bounding sphere
  char creator[16];
};


// Tag information
struct Md3Tag_t
{
  char name[64];
  vec3_t origin;        // Position vector
  float axis[3][3];     // Orientation matrix
};


// Mesh header
struct Md3MeshHeader_t
{
  int ident;            // Magic number, "IDP3"
  char name[64];        // Mesh's name
  int flags;            // ?

  int num_frames;       // Number of frames
  int num_shaders;      // Number of textures
  int num_verts;        // Number of vertices per frame
  int num_triangles;    // Number of triangles

  int offset_triangles; // Offset to triangle data
  int offset_shaders;   // Offset to skin data
  int offset_st;        // Offset to texture coords.
  int offset_xyznormal; // Offset to vertex data
  int offset_end;       // Offset to the end of the mesh
};


// Mesh texture
struct Md3Shader_t
{
  char name[64];
  int shader_index;
};


// Triangle data
struct Md3Triangle_t
{
  int index[3];         // Vertex indices
};


// Texture coordinates
struct Md3TexCoord_t
{
  float	s;
  float	t;
};


// Compressed vertex data
struct Md3Vertex_t
{
  short v[3];
  unsigned char normal[2];
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md3Exception - Exception class for MD3 loader classes.
// This acts like a standard runtime_error exception but
// know which file or mesh has failed to be loaded.
//
/////////////////////////////////////////////////////////////////////////////

class Md3Exception : public std::runtime_error
{
public:
  // Constructors
  Md3Exception (const string &error)
    : std::runtime_error (error) { }
  Md3Exception (const string &error, const string &name)
    : std::runtime_error (error), _which (name) { }
  virtual ~Md3Exception () throw () { }

public:
  // Public interface
  virtual const char *which () const throw () {
    return _which.c_str ();
  }

private:
  // Member variables
  string _which;
};


/////////////////////////////////////////////////////////////////////////////
//
// class NormalLookupTable -- A Normal Lookup Table Class for MD3 Models.
//
/////////////////////////////////////////////////////////////////////////////

class NormalLookupTable
{
public:
  // Constructor
  NormalLookupTable ();

public:
  // Accessors
  const vec3_t *operator[] (int i) const {
    return _normal[i];
  }

private:
  // Member variables
  vec3_t _normal[256][256];
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md3Mesh -- MD3 Mesh Data Class.
//
/////////////////////////////////////////////////////////////////////////////

class Md3Mesh
{
public:
  // Constructor/destructor
  Md3Mesh (std::ifstream &ifs)
    throw (Md3Exception);
  ~Md3Mesh ();

private:
  // Internal types
  typedef shared_ptr<Md3Shader_t> Md3ShaderPtr;
  typedef shared_ptr<Md3Triangle_t> Md3TrianglePtr;
  typedef shared_ptr<Md3Vertex_t> Md3VertexPtr;

public:
  // Public interface
  void loadShader (int index);
  void bindTexture () const;
  void setupVertexArrays (int frameA, int frameB, float interp, float scale);

  void renderFrameImmediate (int frame, float scale) const;
  void renderWithVertexArrays () const;

  void setTexture (const Texture2D *tex) { _tex = tex; }

  // Accessors
  const char *name () const { return _header.name; }

private:
  // Member variables

  // Constants
  static const NormalLookupTable _kAnorms;
  static const int _kMd3Ident;

  // Vertex arrays (shared by all meshes)
  static vec3_t _kVertexArray[];
  static vec3_t _kNormalArray[];

  // Mesh data
  Md3MeshHeader_t _header;

  vector<Md3ShaderPtr> _shaders;
  vector<Md3TrianglePtr> _triangles;
  vector<Md3TexCoord_t> _texCoords;
  vector<Md3VertexPtr> _vertices;

  // Indices for vertex arrays
  vector<GLuint> _indices;

  // Texture
  const Texture2D *_tex;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md3Model -- MD3 Model Data Class.  A model is contituted of
// multiple meshes.
//
/////////////////////////////////////////////////////////////////////////////

class Md3Model
{
public:
  // Constructor/destructor
  Md3Model (const string &filename)
    throw (Md3Exception);
  ~Md3Model ();

private:
  // Internal types

  // Tag information using Quaternion instead
  // of a 3x3 rotation matrix
  struct Md3QuaternionTag
  {
    // Constructor
    Md3QuaternionTag (const Md3Tag_t &tag);

    string name;
    Vector3f origin;
    Quaternionf orient;
  };

  typedef shared_ptr<Md3Frame_t> Md3FramePtr;
  typedef shared_ptr<Md3Mesh> Md3MeshPtr;
  typedef shared_ptr<Md3QuaternionTag> Md3TagPtr;

public:
  // Public interface
  void loadShaders () const;
  void draw () const;
  void renderFrameImmediate (int frame) const;
  void renderFrameItpWithVertexArrays (int frameA, int frameB,
				       float interp) const;

  bool link (const string &name, const Md3Model *model);
  bool unlink (const string &name);
  void setScale (float scale) { _scale = scale; }
  void setTexture (const string &mesh, const Texture2D *tex);
  void setupAnimation (int currFrame, int nextFrame, float interp);

  // Accessors
  int numMeshes () const { return _header.num_meshes; }
  int numFrames () const { return _header.num_frames; }
  int numTags () const { return _header.num_tags; }
  float scale () const { return _scale; }
  const string &name () const { return _name; }

private:
  // Member variables

  // Constants
  static const int _kMd3Ident;
  static const int _kMd3Version;

  // Model data
  Md3Header_t _header;

  vector<Md3FramePtr> _frames;
  vector<Md3MeshPtr> _meshes;
  vector<Md3TagPtr> _qtags;
  vector<const Md3Model*> _links;

  // Animation data used for the
  // next rendering
  int _currFrame;
  int _nextFrame;
  float _interp;

  GLfloat _scale;
  string _name;
};

#endif	// __MD3MODEL_H__
