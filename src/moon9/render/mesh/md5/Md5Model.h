/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// Md5Model.h -- Copyright (c) 2006 David Henry
// last modification: mar. 2, 2006
//
// This code is licenced under the MIT license.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
// Declarations for MD5 Model Classes (object, mesh, animation and
// skeleton).
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MD5_H__
#define __MD5_H__

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <stdexcept>
#include <memory>
#include <GL/gl.h>

#include "Mathlib.h"
#include "Texture.h"

// Forward declarations
class Md5Skeleton;
class Md5Mesh;
class Md5Model;
class Md5Animation;
class Md5Object;

// MD5 Constants
extern const int kMd5Version;

// OpenGL vector types
typedef GLfloat vec2_t[2];
typedef GLfloat vec3_t[3];
typedef GLfloat vec4_t[4];

struct Md5Joint_t
{
  std::string name;
  int parent;

  Vector3f pos;
  Quaternionf orient;
};


struct Md5Vertex_t
{
  float st[2]; // Texture coordinates

  int startWeight; // Start index weights
  int countWeight; // Number of weights
};


struct Md5Triangle_t
{
  int index[3]; // Vertex indices
};


struct Md5Weight_t
{
  int joint; // Joint index
  float bias;

  Vector3f pos;
  Vector3f norm;
  Vector3f tan;
};


struct BoundingBox_t
{
  Vector3f min;
  Vector3f max;
};


struct OBBox_t
{
  Matrix4x4f world;

  Vector3f center;
  Vector3f extent;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Exception - Exception class for MD5 loader classes.
// This acts like a standard runtime_error exception but
// know which file or mesh has failed to be loaded.
//
/////////////////////////////////////////////////////////////////////////////

class Md5Exception : public std::runtime_error
{
public:
  // Constructors
  Md5Exception (const std::string &error)
    : std::runtime_error (error) { }
  Md5Exception (const std::string &error, const std::string &name)
    : std::runtime_error (error), _which (name) { }
  virtual ~Md5Exception () throw () { }

public:
  // Public interface
  virtual const char *which () const throw () {
    return _which.c_str ();
  }

private:
  // Member variables
  std::string _which;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Skeleton - Skeleton model data class.
//
/////////////////////////////////////////////////////////////////////////////

class Md5Skeleton
{
public:
  // Constructors/Destructor
  Md5Skeleton () { }
  Md5Skeleton (std::ifstream &file, int numJoints)
    throw (Md5Exception);
  ~Md5Skeleton ();

private:
  // Internal types
  typedef std::shared_ptr<Md5Joint_t> Md5JointPtr;

public:
  // Public interface
  void draw (const Matrix4x4f &modelView, bool labelJoints);

  void setNumJoints (int numJoints);
  void addJoint (Md5Joint_t *thisJoint);

  Md5Skeleton *clone () const;

  // Accessors
  int numJoints () const { return _joints.size (); }
  Md5Joint_t *joint (int index) const { return _joints[index].get (); }

private:
  // Member variables
  std::vector<Md5JointPtr> _joints;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Mesh - Mesh data class.
//
/////////////////////////////////////////////////////////////////////////////

class Md5Mesh
{
public:
  // Public internal types
  enum
    {
      kHide,   // Skip mesh
      kNoDraw, // Don't draw but prepare vertices
      kShow,   // Draw mesh
    };

  typedef std::shared_ptr<Md5Vertex_t> Md5VertexPtr;
  typedef std::shared_ptr<Md5Triangle_t> Md5TrianglePtr;
  typedef std::shared_ptr<Md5Weight_t> Md5WeightPtr;

public:
  // Constructor/Destructor
  Md5Mesh (std::ifstream &ifs)
    throw (Md5Exception);
  ~Md5Mesh ();

public:
  // Public interface
  void setupVertexArrays (Md5Skeleton *skel);
  void computeWeightNormals (Md5Skeleton *skel);
  void computeWeightTangents (Md5Skeleton *skel);
  void computeBoundingBox (Md5Skeleton *skel);
  void renderVertexArrays () const;
  void drawNormals () const;

  // Hide/NoDraw/Show state
  void setState (int state) { _renderState = state; }

  // Texture setters
  void setDecalMap (const Texture2D *tex) { _decal = tex; }
  void setSpecularMap (const Texture2D *tex) { _specMap = tex; }
  void setNormalMap (const Texture2D *tex) { _normalMap = tex; }
  void setHeightMap (const Texture2D *tex) { _heightMap = tex; }

  // Accessors
  const std::string &name () const { return _name; }
  const BoundingBox_t &boundingBox () const { return _boundingBox; }

  // Mesh render state
  bool hiden () const { return (_renderState == kHide);  }
  bool noDraw () const { return (_renderState == kNoDraw); }
  bool show () const { return (_renderState == kShow); }

private:
  // Internal functions
  void preRenderVertexArrays () const;
  void postRenderVertexArrays () const;
  void allocVertexArrays ();
  void setupTexCoordArray ();
  void setupTexture (const Texture2D *tex, GLenum texUnit) const;

private:
  // Member variables
  std::string _name;
  std::string _shader;
  int _renderState;

  BoundingBox_t _boundingBox;

  int _numVerts;
  int _numTris;
  int _numWeights;

  // Original mesh data
  std::vector<Md5VertexPtr> _verts;
  std::vector<Md5TrianglePtr> _tris;
  std::vector<Md5WeightPtr> _weights;

  // Final mesh data; vertex arrays for fast rendering
  std::vector<GLfloat> _vertexArray;
  std::vector<GLfloat> _normalArray;
  std::vector<GLfloat> _tangentArray;
  std::vector<GLfloat> _texCoordArray;
  std::vector<GLuint> _vertIndices;

  // Textures
  const Texture2D *_decal;
  const Texture2D *_specMap;
  const Texture2D *_normalMap;
  const Texture2D *_heightMap;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Model - MD5 Mesh Model class.
//
/////////////////////////////////////////////////////////////////////////////

class Md5Model
{
public:
  // Constructor/Destructor
  Md5Model (const std::string &filename)
    throw (Md5Exception);
  ~Md5Model ();

public:
  // Internal type definitions
  typedef std::shared_ptr<Md5Skeleton> Md5SkeletonPtr;
  typedef std::shared_ptr<Md5Mesh> Md5MeshPtr;
  typedef std::shared_ptr<Md5Animation> Md5AnimationPtr;
  typedef std::map<std::string, Md5AnimationPtr> AnimMap;

public:
  // Public interface
  void prepare (Md5Skeleton *skel);
  void drawModel () const;
  bool addAnim (const std::string &filename);

  // Setters
  void setMeshRenderState (const std::string &name, int state);
  void setMeshDecalMap (const std::string &name, const Texture2D *tex);
  void setMeshSpecularMap (const std::string &name, const Texture2D *tex);
  void setMeshNormalMap (const std::string &name, const Texture2D *tex);
  void setMeshHeightMap (const std::string &name, const Texture2D *tex);

  // Accessors
  const Md5Animation *anim (const std::string &name) const;
  int numJoints () const { return _numJoints; }
  const Md5Skeleton *baseSkeleton () const { return _baseSkeleton.get (); }
  const AnimMap &anims () const { return _animList; }
  const BoundingBox_t &bindPoseBoundingBox () const { return _bindPoseBox; }

private:
  // Internal functions
  void computeBindPoseBoundingBox ();

  // Check if an animation is valid for this model,
  // i.e. anim's skeleton matches with model's skeleton
  bool validityCheck (Md5Animation *anim) const;

  // Access to a mesh, given its name
  Md5Mesh *getMeshByName (const std::string &name) const;

private:
  // Member variables
  int _numJoints;
  int _numMeshes;

  Md5SkeletonPtr _baseSkeleton;

  std::vector<Md5MeshPtr> _meshes;
  AnimMap _animList;

  BoundingBox_t _bindPoseBox;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Animation - MD5 model animation class.
//
/////////////////////////////////////////////////////////////////////////////

class Md5Animation
{
public:
  // Constructor/Destructor
  Md5Animation (const std::string &filename)
    throw (Md5Exception);
  ~Md5Animation ();

private:
  // Internal type
  struct JointInfo
  {
    std::string name;
    int parent;

    // NOTE: this structure is stored in
    // little-endian format
    union JointFlags
    {
      short value;

      struct
      {
	bool tx: 1;
	bool ty: 1;
	bool tz: 1;

	bool qx: 1;
	bool qy: 1;
	bool qz: 1;
      };
    } flags;

    int startIndex;
  };

  struct BaseFrameJoint
  {
    Vector3f pos;
    Quaternionf orient;
  };

  typedef std::shared_ptr<Md5Skeleton> Md5SkeletonPtr;
  typedef std::shared_ptr<BoundingBox_t> BoundingBoxPtr;

public:
  // Public interface
  void interpolate (int frameA, int frameB,
	    float interp, Md5Skeleton *out) const;

  // Accessors
  int maxFrame () const { return _numFrames - 1; }
  int frameRate () const { return _frameRate; }
  const std::string &name () const { return _name; }

  Md5Skeleton *frame (int frame) const {
    return _skelframes[frame].get ();
  }

  const BoundingBox_t *frameBounds (int frame) const {
    return _bboxes[frame].get ();
  }

private:
  // Internal functions
  void buildFrameSkeleton (std::vector<JointInfo> &jointInfos,
			   std::vector<BaseFrameJoint> &baseFrame,
			   std::vector<float> &animFrameData);

private:
  // Member variables
  int _numFrames;
  int _frameRate;

  std::string _name;

  // Store each frame as a skeleton
  std::vector<Md5SkeletonPtr> _skelframes;

  // Bounding boxes for each frame
  std::vector<BoundingBoxPtr> _bboxes;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Object - MD5 object class.
//
/////////////////////////////////////////////////////////////////////////////

class Md5Object
{
public:
  // Public internal types/enums
  enum
    {
      kDrawModel = 1,
      kDrawSkeleton = 2,
      kDrawJointLabels = 4,
    };

public:
  // Contructor/Destructor
  Md5Object (Md5Model *model);
  virtual ~Md5Object ();

public:
  // Public interface
  void animate (double dt);
  void computeBoundingBox ();
  void prepare (bool softwareTransformation);
  void render () const;

  // Setters
  void setMd5Model (Md5Model *model);
  void setAnim (const std::string &name);
  void setModelViewMatrix (const Matrix4x4f &mat) { _modelView = mat; }
  void setRenderFlags (int flags) { _renderFlags = flags; }

  // Accessors
  int renderFlags () const { return _renderFlags; }
  const Md5Model *getModelPtr () const { return _model; }
  const std::string currAnimName () const { return _currAnimName; }
  const OBBox_t &boundingBox () const { return _bbox; }

protected:
  // Member variables;
  Md5Model *_model;
  Md5Skeleton *_animatedSkeleton;

  Matrix4x4f _modelView;
  bool _softwareTransformation;

  const Md5Animation *_currAnim;
  std::string _currAnimName;
  unsigned int _currFrame;
  unsigned int _nextFrame;

  double _last_time;
  double _max_time;

  int _renderFlags;

  OBBox_t _bbox;
};

#endif // __MD5_H__
