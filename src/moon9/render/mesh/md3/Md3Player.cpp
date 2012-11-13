/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// Md3Player.cpp -- Copyright (c) 2006-2007 David Henry
// last modification: may. 7, 2007
//
// This code is licenced under the MIT license.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
// Implementation of MD3 Player Classes.
//
/////////////////////////////////////////////////////////////////////////////

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <cctype>
#include <sstream>
#include <sys/types.h>

#ifdef _WIN32
#include "dirent.h"
#else
#include <dirent.h>
#endif

using std::cout;
using std::cerr;
using std::endl;

using std::string;

#include "Md3Player.h"
#include "TextureManager.h"

#ifdef _WIN32
const char kSeparator = '\\';
#else
const char kSeparator = '/';
#endif


/////////////////////////////////////////////////////////////////////////////
//
// class Md3PlayerSkin implementation.
//
/////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------
// Md3PlayerSkin::Md3PlayerSkin
//
// Constructor.  Load a skin given a path and its name.
// For each portion of the player, load the adequate .skin file.
// --------------------------------------------------------------------------

Md3PlayerSkin::Md3PlayerSkin (const string &path, const string &name)
  throw (Md3Exception)
  : _path (path), _name (name)
{
  // Load lower part's textures
  loadSkinFile (path + "lower_" + name + ".skin", _lowerTextures);

  // Load upper part's textures
  loadSkinFile (path + "upper_" + name + ".skin", _upperTextures);

  // Load head's textures
  loadSkinFile (path + "head_" + name + ".skin", _headTextures);
}


// --------------------------------------------------------------------------
// Md3PlayerSkin::~Md3PlayerSkin
//
// Destructor.  Free allocated memory for the skin.
// boost:shared_ptr should do it for us.
// --------------------------------------------------------------------------

Md3PlayerSkin::~Md3PlayerSkin ()
{
}


// --------------------------------------------------------------------------
// Md3PlayerSkin::loadSkinFile
//
// Constructor.  Load a skin file and initialize a TexMap object given
// by reference.  The TexMap will contain a texture associated to each
// mesh found in the skin file.
// --------------------------------------------------------------------------

void
Md3PlayerSkin::loadSkinFile (const string &filename, TexMap &tmap)
  throw (Md3Exception)
{
  // Open the skin file
  std::ifstream ifs (filename.c_str(), std::ios::in | std::ios::binary);

  if (ifs.fail ())
    throw Md3Exception ("Couldn't open skin file", filename);

  // Get texture manager
  Texture2DManager *texMgr = Texture2DManager::getInstance ();

  // Read the file
  while (!ifs.eof ())
    {
      string meshname, texname, buffer;

      // Read mesh name and texture name
      std::getline (ifs, meshname, ',');
      ifs >> texname;

      // If this is a valid mesh name, load the texture
      if (!meshname.empty () && !texname.empty ()
	  && meshname.compare (0, 4, "tag_") != 0)
	{
	  // Replace the path from the .skin file with the path
	  // where the player's .skin file is
	  long start = texname.find_last_of ('/') + 1;
	  long end = texname.length () - start;
	  texname.assign (texname, start, end);
	  texname = _path + texname;

	  // Load the texture
	  Texture2D *tex = texMgr->load (texname);

	  tmap.insert (TexMap::value_type (meshname, tex));
	}

      // Eat up rest of the line
      std::getline (ifs, buffer);
    }

  ifs.close ();
}


// --------------------------------------------------------------------------
// Md3PlayerSkin::setModelTextures
//
// Setup textures for the specified model from the given Texture Map.
// --------------------------------------------------------------------------

void
Md3PlayerSkin::setModelTextures (Md3Model *model, const TexMap &tmap) const
{
  for (TexMap::const_iterator itor = tmap.begin ();
       itor != tmap.end (); ++itor)
    model->setTexture (itor->first, itor->second);
}


/////////////////////////////////////////////////////////////////////////////
//
// class Md3Weapon implementation.
//
/////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------
// Md3Weapon::Md3Weapon
//
// Constructor.  Load a weapon given a path and the level of details.
// --------------------------------------------------------------------------

Md3Weapon::Md3Weapon (const string &path, Md3PlayerLOD lod)
  throw (Md3Exception)
  : _path (path), _scale (1.0f), _lod (lod)
{
  string suffix, modelpath;

  // Add trailing slash to path name if not present
  if (_path.find_last_of (kSeparator) < _path.length () - 1)
    _path += kSeparator;

  // Extract name from the path
  _name.assign (_path, 0, _path.find_last_of (kSeparator));
  _name.assign (_name, _name.find_last_of (kSeparator) + 1, _name.length ());

  // Get model file suffix from LOD
  switch (lod)
    {
    case kLodLow:
      suffix = "_2";
      break;

    case kLodMedium:
      suffix = "_1";
      break;

    case kLodHigh:
      break;
    }

  // Load models
  modelpath = _path + _name + suffix + ".md3";
  _weapon = Md3ModelPtr (new Md3Model (modelpath));
  _weapon->loadShaders ();

  modelpath = _path + _name + suffix + "_barrel.md3";
  std::ifstream ftest (modelpath.c_str (), std::ios::in);
  if (!ftest.fail ())
    {
      // The barrel model exists, load it
      ftest.close ();

      _barrel = Md3ModelPtr (new Md3Model (modelpath));
      _barrel->loadShaders ();

      // Create links between weapon and barrel
      _weapon->link ("tag_barrel", _barrel.get ());
    }
}


// --------------------------------------------------------------------------
// Md3Weapon::~Md3Weapon
//
// Destructor.
// --------------------------------------------------------------------------

Md3Weapon::~Md3Weapon ()
{
}


// --------------------------------------------------------------------------
// Md3Weapon::draw
//
// Draw the weapon model.  Don't call this function if you want to
// draw the weapon attached to a player: the player will draw it itself.
// --------------------------------------------------------------------------

void
Md3Weapon::draw () const
{
  // Set scale parameter
  scaleModels ();

  // Draw the models
  glPushMatrix ();
    glRotatef (-90.0, 1.0, 0.0, 0.0);
    glRotatef (-90.0, 0.0, 0.0, 1.0);

    _weapon->draw ();
  glPopMatrix ();
}


// --------------------------------------------------------------------------
// Md3Weapon::scaleModels
//
// Set the current scale factor to weapon's models (call this before
// rendering).
// --------------------------------------------------------------------------

void
Md3Weapon::scaleModels () const
{
  _weapon->setScale (_scale);

  if (_barrel)
    _barrel->setScale (_scale);
}


// --------------------------------------------------------------------------
// Md3Weapon::linkToModel
//
// Link weapon model to the given model at "tag_weapon".  The target
// model should be player's upper model.
// --------------------------------------------------------------------------

void
Md3Weapon::linkToModel (Md3Model *model)
{
  if (model)
    model->link ("tag_weapon", _weapon.get ());
}


/////////////////////////////////////////////////////////////////////////////
//
// struct Md3Player::Md3AnimState implementation.
//
/////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------
// Md3Player::Md3AnimState::Md3AnimState
//
// Constructor.
// --------------------------------------------------------------------------

Md3Player::Md3AnimState::Md3AnimState ()
  : anim (NULL), curr_time (0.0f), old_time (0.0f),
    curr_frame (0), next_frame (0), interp (0.0f)
{
}


// --------------------------------------------------------------------------
// Md3Player::Md3AnimState::setup
//
// Initialize member variables given a player animation.
// --------------------------------------------------------------------------

void
Md3Player::Md3AnimState::setup (Md3PlayerAnim_t *a)
{
  anim = a;
  curr_frame = a->first;
  next_frame = a->first;
  interp = 0.0f;
}


// --------------------------------------------------------------------------
// Md3Player::Md3AnimState::update
//
// Update current frame, next frame and interpolation percent given a
// delta time.
// --------------------------------------------------------------------------

void
Md3Player::Md3AnimState::update (float dt)
{
  curr_time += dt;

  if ((curr_time - old_time) > (1.0 / anim->fps))
    {
      curr_frame = next_frame;
      next_frame++;

      if (next_frame > (anim->first + anim->num - 1))
	next_frame = anim->first;

      old_time = curr_time;
    }

  interp = anim->fps * (curr_time - old_time);
}


/////////////////////////////////////////////////////////////////////////////
//
// class Md3Player implementation.
//
/////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------
// Md3Player::Md3Player
//
// Constructor.  Load a player given a path.  Load player's models,
// animations and skins.
// --------------------------------------------------------------------------

Md3Player::Md3Player (const string &path, Md3PlayerLOD lod)
  throw (Md3Exception)
  : _weapon (NULL), _path (path), _scale (1.0f), _lod (lod)
{
  // Add trailing slash to path name if not present
  if (_path.find_last_of (kSeparator) < _path.length () - 1)
    _path += kSeparator;

  // Extract name from the path
  _name.assign (_path, 0, _path.find_last_of (kSeparator));
  _name.assign (_name, _name.find_last_of (kSeparator) + 1, _name.length ());

  // Load models
  loadModels (_path);

  // Load animations
  loadAnimations (_path);

  // Load skins
  loadSkins (_path);
}


// --------------------------------------------------------------------------
// Md3Player::~Md3Player
//
// Destructor.  Free allocated memory for the MD3 player.
// --------------------------------------------------------------------------

Md3Player::~Md3Player ()
{
}


// --------------------------------------------------------------------------
// Md3Player::draw
//
// Draw player's animated models.
// --------------------------------------------------------------------------

void
Md3Player::draw () const
{
  // Set scale parameter
  _lower->setScale (_scale);
  _upper->setScale (_scale);
  _head->setScale (_scale);

  if (_weapon)
    _weapon->scaleModels ();

  // Set current skin's textures
  if (_currentSkin)
    {
      _currentSkin->setLowerTextures (_lower.get ());
      _currentSkin->setUpperTextures (_upper.get ());
      _currentSkin->setHeadTextures (_head.get ());
    }

  // Set animation parameters
  const Md3AnimState &low = _lowerAnim;
  const Md3AnimState &upp = _upperAnim;

  _lower->setupAnimation (low.curr_frame, low.next_frame, low.interp);
  _upper->setupAnimation (upp.curr_frame, upp.next_frame, upp.interp);

  // Draw the models
  glPushMatrix ();
    glRotatef (-90.0, 1.0, 0.0, 0.0);
    glRotatef (-90.0, 0.0, 0.0, 1.0);

    _lower->draw ();
  glPopMatrix ();
}


// --------------------------------------------------------------------------
// Md3Player::renderFrame
//
// Draw player's models at a given frame.
// --------------------------------------------------------------------------

void
Md3Player::renderFrame (int upperFrame, int lowerFrame) const
{
  // Set scale parameter
  _lower->setScale (_scale);
  _upper->setScale (_scale);
  _head->setScale (_scale);

  if (_weapon)
    _weapon->scaleModels ();

  // Set current skin's textures
  if (_currentSkin)
    {
      _currentSkin->setLowerTextures (_lower.get ());
      _currentSkin->setUpperTextures (_upper.get ());
      _currentSkin->setHeadTextures (_head.get ());
    }

  // Set animation parameters
  _lower->setupAnimation (lowerFrame, lowerFrame, 0.0f);
  _upper->setupAnimation (upperFrame, upperFrame, 0.0f);

  // Draw the models
  glPushMatrix ();
    glRotatef (-90.0, 1.0, 0.0, 0.0);
    glRotatef (-90.0, 0.0, 0.0, 1.0);

    _lower->draw ();
  glPopMatrix ();
}


// --------------------------------------------------------------------------
// Md3Player::animate
//
// Animate player's models.
// --------------------------------------------------------------------------

void
Md3Player::animate (float dt)
{
  _lowerAnim.update (dt);
  _upperAnim.update (dt);
}


// --------------------------------------------------------------------------
// Md3Player::setAnimation
//
// Set player's current animation.
// --------------------------------------------------------------------------

void
Md3Player::setAnimation (Md3PlayerAnimType type)
{
  if ((type >= kBothDeath1) && (type <= kBothDead3))
    {
      _lowerAnim.setup (&_anims[type]);
      _upperAnim.setup (&_anims[type]);
    }
  else if ((type >= kTorsoGesture) && (type <= kTorsoStand2))
    {
      _upperAnim.setup (&_anims[type]);
    }
  else if ((type >= kLegsWalkCr) && (type <= kLegsTurn))
    {
      _lowerAnim.setup (&_anims[type]);
    }
}


// --------------------------------------------------------------------------
// Md3Player::setSkin
//
// Set player's current skin.  If the skin does not exist, current
// skin is set to NULL.
// --------------------------------------------------------------------------

void
Md3Player::setSkin (const string &name)
{
  SkinMap::iterator itor = _skins.find (name);
  if (itor != _skins.end ())
    {
      _currentSkin = itor->second.get ();
      _currentSkinName = itor->first;
    }
  else
    {
      _currentSkin = NULL;
    }
}


// --------------------------------------------------------------------------
// Md3Player::linkWeapon
//
// Set weapon model to the player, and link the weapon to player's models.
// --------------------------------------------------------------------------

void
Md3Player::linkWeapon (Md3Weapon *weapon)
{
  _weapon = weapon;

  if (_weapon)
    _weapon->linkToModel (_upper.get ());
}


// --------------------------------------------------------------------------
// Md3Player::unlinkWeapon
//
// Unset player's weapon.
// --------------------------------------------------------------------------

void
Md3Player::unlinkWeapon ()
{
  if (_weapon)
    {
      _upper->unlink ("tag_weapon");
      _weapon = NULL;
    }
}


// --------------------------------------------------------------------------
// Md3Player::loadModels
//
// Load player's models from the path directory.
// --------------------------------------------------------------------------

void
Md3Player::loadModels (const string &path)
  throw (Md3Exception)
{
  string suffix, modelpath;

  // Get model file suffix from LOD
  switch (_lod)
    {
    case kLodLow:
      suffix = "_2.md3";
      break;

    case kLodMedium:
      suffix = "_1.md3";
      break;

    case kLodHigh:
      suffix = ".md3";
      break;
    }

  // Load models
  modelpath = _path + "lower" + suffix;
  _lower = Md3ModelPtr (new Md3Model (modelpath));

  modelpath = _path + "upper" + suffix;
  _upper = Md3ModelPtr (new Md3Model (modelpath));

  modelpath = _path + "head" + suffix;
  _head = Md3ModelPtr (new Md3Model (modelpath));

  // Create links between the models
  _lower->link ("tag_torso", _upper.get ());
  _upper->link ("tag_head", _head.get ());
}


// --------------------------------------------------------------------------
// Md3Player::loadAnimations
//
// Load animations from the animation.cfg file.
// NOTE: Some animations.cfg files may be broken, i.e. they give wrong
// frame indices or wrong frame number for an animation (which is
// greater or equal than the number of frames of the models).
// We don't fix those files.
// --------------------------------------------------------------------------

void
Md3Player::loadAnimations (const string &path)
  throw (Md3Exception)
{
  string filename (path + "animation.cfg");
  string token, buffer;
  int index = 0;

  // Open the animation file
  std::ifstream ifs (filename.c_str(), std::ios::in | std::ios::binary);

  if (ifs.fail ())
    throw Md3Exception ("Couldn't open animation file", filename);

  // Read animation lines
  while (!ifs.eof ())
    {
      // Parse the file line by line
      std::getline (ifs, buffer);
      std::istringstream line (buffer);

      // Get first char of the line
      char c = line.peek ();

      // If the first token is a numeric, then this line
      // contains animation infos
      if (std::isdigit (c))
	{
	  line >> _anims[index].first;
	  line >> _anims[index].num;
	  line >> _anims[index].looping;
	  line >> _anims[index].fps;
	  index++;
	}
    }

  ifs.close ();

  // Skip "Torso" frames for the "Legs" animations
  int skip = _anims[kLegsWalkCr].first - _anims[kTorsoGesture].first;

  for (int i = kLegsWalkCr; i < kMaxAnimations; ++i)
    _anims[i].first -= skip;
}


// --------------------------------------------------------------------------
// Md3Player::loadSkins
//
// Load player's skins from the path directory and set the default
// skin as player's current skin.
// --------------------------------------------------------------------------

void
Md3Player::loadSkins (const string &path)
  throw (Md3Exception)
{
  // Open the directory
  DIR *dd = opendir (path.c_str ());
  if (!dd)
    throw Md3Exception ("Couldn't open dir", path);

  dirent *dit;

  // Read directory for textures
  while ((dit = readdir (dd)) != NULL)
    {
      const string filename (dit->d_name);
      const string fileext (".skin");
      const string pattern ("head_");

      // Filter ".skin" files
      if (filename.length () - fileext.length ()
	  == filename.find (fileext))
	{
	  // Now we should have 3 skin files for a player skin:
	  // one for upper portion, one for lower and one for head.
	  // We'll just take one of them to extract the skin name.

	  // Filter "head_*.skin" files
	  if (filename.compare (0, pattern.length (), pattern) == 0)
	    {
	      // Extract skin name from filename
	      const string name (filename, pattern.length (),
		   filename.find (fileext) - pattern.length ());

	      // Load new skin
	      Md3PlayerSkinPtr skin (new Md3PlayerSkin (path, name));
	      _skins.insert (SkinMap::value_type (name, skin));
	    }
	}
    }

  // Close directory
  closedir (dd);

  // Set the default skin as current player's skin
  setSkin ("default");

  // If there is no skin named "default", use the first enry in the map
  if (_currentSkin == NULL)
    {
      _currentSkin = _skins.begin ()->second.get ();
      _currentSkinName = _skins.begin ()->first;
    }
}
