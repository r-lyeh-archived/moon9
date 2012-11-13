/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// Md3Player.h -- Copyright (c) 2006 David Henry
// last modification: feb. 25, 2006
//
// This code is licenced under the MIT license.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
// Definition of MD3 Player Classes.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MD3PLAYER_H__
#define __MD3PLAYER_H__

#include "Md3Model.h"


// Level of details
enum Md3PlayerLOD
  {
    kLodLow,
    kLodMedium,
    kLodHigh,

    kLodDefault = kLodHigh,
  };


// Animation list
enum Md3PlayerAnimType
  {
    // Animations for both lower and upper
    // parts of the player
    kBothDeath1,
    kBothDead1,
    kBothDeath2,
    kBothDead2,
    kBothDeath3,
    kBothDead3,

    // Animations for the upper part
    kTorsoGesture,

    kTorsoAttack,
    kTorsoAttack2,

    kTorsoDrop,
    kTorsoRaise,

    kTorsoStand,
    kTorsoStand2,

    // Animations for the lower part
    kLegsWalkCr,
    kLegsWalk,
    kLegsRun,
    kLegsBack,
    kLegsSwim,

    kLegsJump,
    kLegsLand,

    kLegsJumpB,
    kLegsLandB,

    kLegsIdle,
    kLegsIdleCr,

    kLegsTurn,

    kMaxAnimations,
  };


// Animation infos
struct Md3PlayerAnim_t
{
  int first;     // First frame
  int num;       // Last frame
  int looping;   // Looping frames
  int fps;       // Frames per second
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md3PlayerSkin -- MD3 Player Skin Class.  A player skin is
// constituted of a list of mesh-texture pairs, one for each portion
// of player's models (lower, upper and head).
//
/////////////////////////////////////////////////////////////////////////////

class Md3PlayerSkin
{
public:
  // Constructor/destructor
  Md3PlayerSkin (const string &path, const string &name)
    throw (Md3Exception);
  ~Md3PlayerSkin ();

public:
  // Public interface
  void setLowerTextures (Md3Model *model) {
    setModelTextures (model, _lowerTextures);
  }

  void setUpperTextures (Md3Model *model) {
    setModelTextures (model, _upperTextures);
  }

  void setHeadTextures (Md3Model *model) {
    setModelTextures (model, _headTextures);
  }

  // Accessors
  const string &path () const { return _path; }
  const string &name () const { return _name; }

private:
  // Internal types
  typedef map<string, const Texture2D *> TexMap;

private:
  // Internal functions
  void loadSkinFile (const string &filename, TexMap &tmap)
    throw (Md3Exception);

  void setModelTextures (Md3Model *model, const TexMap &tmap) const;

private:
  // Member variables
  TexMap _lowerTextures;
  TexMap _upperTextures;
  TexMap _headTextures;

  string _path;
  string _name;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md3Weapon -- MD3 Weapon Class.  A weapon is constituted of
// two models: the weapon itself and the barrel (optional).
//
// Weapon models use their shader strings to load their textures.
//
// Weapons can be linked to player's upper model, or be drawn stand-alone.
//
/////////////////////////////////////////////////////////////////////////////

class Md3Weapon
{
public:
  // Constructor/destructor
  Md3Weapon (const string &path, Md3PlayerLOD lod = kLodDefault)
    throw (Md3Exception);
  ~Md3Weapon ();

private:
  // Internal types
  typedef shared_ptr<Md3Model> Md3ModelPtr;

public:
  // Public interface
  void draw () const;
  void scaleModels () const;
  void linkToModel (Md3Model *model);

  void setScale (float scale) { _scale = scale; }

  // Accessors
  float scale () const { return _scale; }
  const string &path () const { return _path; }
  const string &name () const { return _name; }
  Md3PlayerLOD lod () const { return _lod; }

private:
  // Member variables
  Md3ModelPtr _weapon;
  Md3ModelPtr _barrel;

  string _path;
  string _name;
  float _scale;
  Md3PlayerLOD _lod;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md3Player -- MD3 Player Class.  A player is constituted of
// three MD3 models: the lower portion, the upper portion and the head.
//
// A player can have multiple skins, a weapon and animations.  The
// Md3Player loads the models, skins and animations itself at construction.
//
/////////////////////////////////////////////////////////////////////////////

class Md3Player
{
public:
  // Constructor/destructor
  Md3Player (const string &path, Md3PlayerLOD lod = kLodDefault)
    throw (Md3Exception);
  ~Md3Player ();

private:
  // Internal types

  // Model animation state
  struct Md3AnimState
  {
  public:
    // Constructor
    Md3AnimState ();

    // Member functions
    void setup (Md3PlayerAnim_t *a);
    void update (float dt);

  private:
    // Pointer to current animation infos
    Md3PlayerAnim_t *anim;

    float curr_time;
    float old_time;

  public:
    int curr_frame;
    int next_frame;
    float interp;
  };

  typedef shared_ptr<Md3Model> Md3ModelPtr;
  typedef shared_ptr<Md3PlayerSkin> Md3PlayerSkinPtr;

public:
  typedef map<string, Md3PlayerSkinPtr> SkinMap;

public:
  // Public interface
  void draw () const;
  void renderFrame (int upperFrame, int lowerFrame) const;
  void animate (float dt);

  void setScale (float scale) { _scale = scale; }
  void setAnimation (Md3PlayerAnimType type);
  void setSkin (const string &name);

  void linkWeapon (Md3Weapon *weapon);
  void unlinkWeapon ();

  // Accessors
  float scale () const { return _scale; }
  const string &path () const { return _path; }
  const string &name () const { return _name; }
  Md3PlayerLOD lod () const { return _lod; }
  const SkinMap &skins () const { return _skins; }
  const string &skinName () const { return _currentSkinName; }

private:
  // Internal functions
  void loadModels (const string &path)
    throw (Md3Exception);

  void loadAnimations (const string &path)
    throw (Md3Exception);

  void loadSkins (const string &path)
    throw (Md3Exception);

private:
  // Member variables
  Md3ModelPtr _lower;
  Md3ModelPtr _upper;
  Md3ModelPtr _head;

  Md3AnimState _lowerAnim;
  Md3AnimState _upperAnim;

  // Animation list
  Md3PlayerAnim_t _anims[kMaxAnimations];

  // Skin list
  SkinMap _skins;
  Md3PlayerSkin *_currentSkin;
  string _currentSkinName;

  Md3Weapon *_weapon;

  string _path;
  string _name;
  float _scale;
  Md3PlayerLOD _lod;
};

#endif	// __MD3PLAYER_H__
