/*
   Copyright (c) 2002 Tokamak Limited

   Date: 2002/10/27 12:00:00 

   This software, the Tokamak Game Physics SDK and the documentaion
   has been developed by Tokamak Ltd (Tokamak) and the copyright 
   and all other intellectual property rights in this software 
   belong to Tokamak. 
   
   All rights conferred by law (including rights under international
   copyright conventions) are reserved to Tokamak. This software
   may also incorporate information which is confidential to
   Tokamak.

   Without the permission by Tokamak, this software and documentaion
   must not be copied (in whole or in part), re-arranged, altered or 
   adapted in any way. In addition, the information contained in the 
   software may not be disseminated without the prior written consent
   of Tokamak.
*/

#ifndef DISPLAYSYSTEM_H
#define DISPLAYSYSTEM_H

#pragma warning(disable:4786)

#include "math/ne_math.h"
#include "ne_colour.h"
#include "factory.h" //for using the factory pattern
#include <queue>
#include <map>

class neDisplaySystem;
class neCamera;
class neTexture;
class neMesh;
class neFrame;
typedef struct neMaterial neMaterial;
typedef struct lua_State lua_State;
class CLuaTable;
///////////////////////////////////////////////////////////////////
// 
//	neDeviceDependentObject
//
//	This is the objects which depend on the 3D hardware device
//	Base class for neMesh, neTexture, neSprite, and neEffect
//
///////////////////////////////////////////////////////////////////
/*
class neDeviceDependentObject: public CMemObject
{
public:
	neDeviceDependentObject(CObjectFactory * factory, bool unique): CMemObject(factory, unique) {}
	
	virtual bool RestoreDeviceObjects() = 0;
	
	virtual void InvalidateDeviceObjects() = 0;
};
*/
///////////////////////////////////////////////////////////////////
// 
// neEffectParamInfo - Information about a effect parameter
// neEffectContext - The context in which an effect is being apply
// neEffectParam - The creation parameters of an neEffect object
// neEffect - Rendering state (effect file on dx8)
//
///////////////////////////////////////////////////////////////////

typedef enum _EFFECT_PARAM_TYPE
{
	EFFECT_PARAM_INT,
	EFFECT_PARAM_FLOAT,
	EFFECT_PARAM_VECTOR,
	EFFECT_PARAM_MATRIX,
	EFFECT_PARAM_TEXTURE,
	
	//special parameter type corresponding to 
	//the material of the mesh
	EFFECT_PARAM_MTL_DIFFUSE,
	EFFECT_PARAM_MTL_AMBIENT,
	EFFECT_PARAM_MTL_TEXTURE,
	EFFECT_PARAM_UNDEF = 0x7fffffff,
}EFFECT_PARAM_TYPE;

//typedef enum _EFFECT_UPDATE_MODE
//{
#define	EFFECT_UPDATE_NONE					(0x00000000L)
#define EFFECT_UPDATE_EXPLICIT				(0x00000001L)
#define	EFFECT_UPDATE_BEGIN_LOOP			(0x00000002L)
#define	EFFECT_UPDATE_EACH_ROOT_FRAME		(0x00000004L)
#define	EFFECT_UPDATE_EACH_FRAME			(0x00000008L)
#define	EFFECT_UPDATE_EACH_MESH				(0x00000010L)
#define	EFFECT_UPDATE_ALL					(EFFECT_UPDATE_EXPLICIT | EFFECT_UPDATE_BEGIN_LOOP | EFFECT_UPDATE_EACH_ROOT_FRAME \
											| EFFECT_UPDATE_EACH_FRAME | EFFECT_UPDATE_EACH_MESH)
#define	EFFECT_UPDATE_UNDEF 0x7fffffff

typedef u32 EFFECT_UPDATE_MODE;

//}EFFECT_UPDATE_MODE;

///////////////////////////////////////////////////////////////////
//
// neEffectParamInfo
//
///////////////////////////////////////////////////////////////////
typedef struct neEffectParamInfo neEffectParamInfo;

struct neEffectParamInfo
{
	neEffectParamInfo()
	{
		id = 0;
		name[0] = '\0';
		value = NULL;
		paramType = EFFECT_PARAM_UNDEF;
		updateMode = EFFECT_UPDATE_UNDEF;
		cookie = NULL;
	}

	//u32 id;
	char name[10];
	u32 id;
	u32 value;
	u32 cookie;
	EFFECT_PARAM_TYPE paramType;
	EFFECT_UPDATE_MODE updateMode;
};

///////////////////////////////////////////////////////////////////
//
// neEffectContext
//
///////////////////////////////////////////////////////////////////
typedef struct neEffectContext neEffectContext;

struct neEffectContext
{
	neMaterial * mtl;
	neMesh * mesh;
	neFrame * frame;
	char * cookie;
	neM4 * matrix;
};

///////////////////////////////////////////////////////////////////
//
// neEffectSetupParam
//
///////////////////////////////////////////////////////////////////
class neEffectSetupParam: public CObjectSetupParam{};

///////////////////////////////////////////////////////////////////
//
// neEffect
//
///////////////////////////////////////////////////////////////////
class neEffect: public CMemObject
{
friend class neDisplaySystem;

public:
	neEffect(CObjectFactory * factory, bool unique): CMemObject(factory, unique) {
														passes = 0;
														updateMode = EFFECT_UPDATE_EXPLICIT;
														currentUpdateFlag = EFFECT_UPDATE_NONE;}

	//applying effect
	virtual u32		StartEffect(neEffectContext * effectContext) = 0; //return the index of the first pass	
	
	virtual bool	ApplyPass(u32 pass) = 0;
	
	virtual void	EndEffect(bool restore) = 0;
	
	u32				GetPassCount() {return passes;}

	virtual void	ApplyAll(neEffectContext * effectContext) = 0;

	//effect parameters
	virtual void		UpdateParams(neEffectContext * effectContext) = 0;
	
	u32					GetParamCount(){ 
							return params.size();}
	
	neEffectParamInfo *	GetParamInfo(u32 i){
							ASSERT(i < params.size());
							return params[i];};

protected:
	vector<neEffectParamInfo*>params;
	u32 passes;
	EFFECT_UPDATE_MODE updateMode;
	EFFECT_UPDATE_MODE currentUpdateFlag;
};

///////////////////////////////////////////////////////////////////
//
// neMesh 
// CMeshSetupParam
// neMaterial
//
///////////////////////////////////////////////////////////////////
struct neMaterial
{
	neV4 diffuse;
	neV4 ambient;
	neTexture * texture;
	neEffect * effect;

	neMaterial()
	{
		diffuse.Set(1.0f, 1.0f, 1.0f, 1.0f);
		ambient.Set(0.7f, 0.7f, 0.7f, 1.0f);
		texture = NULL;
		effect = NULL;
	};
};

///////////////////////////////////////////////////////////////////
//
// CMeshSetupParam
//
///////////////////////////////////////////////////////////////////
class CMeshSetupParam: public CObjectSetupParam{};

///////////////////////////////////////////////////////////////////
//
// neMesh
//
///////////////////////////////////////////////////////////////////
class neMesh: public CMemObject 
{
friend class neDisplaySystem;

public:
	neMesh(CObjectFactory * factory, bool unique): CMemObject(factory, unique){}
	
	virtual f32				GetRadius() = 0;
	
	virtual u32				GetMaterialCount() = 0;
	
	virtual neMaterial *	GetMaterial(u32 materialIndex) = 0;

	virtual u32				GetVertexCount() = 0;

	virtual u32				GetTriangleCount() = 0;

	virtual u32 			GetTriangleList(s32 *) = 0;

	virtual u32 			GetVerticesList(neV3 *) = 0;

protected:
	virtual void Display(neM4 * mat, neV3 * scale) = 0;
};

///////////////////////////////////////////////////////////////////
//
// neFrame and neFrameSetupParam
//
///////////////////////////////////////////////////////////////////
class neFrameSetupParam: public CObjectSetupParam{};

class neFrame: public CMemObject
{
public:
	neFrame(CObjectFactory * factory, bool unique): CMemObject(factory, unique)
	{
		mesh = NULL;
		transform.SetIdentity();
		next = NULL;
		child = NULL;
		radius = 0.0f;
	}
	~neFrame();
	
	neMesh *	GetMesh() {
		return mesh;}
	
	void		SetMesh(neMesh * _mesh) {
					mesh = _mesh; 
					if (mesh)
						radius = mesh->GetRadius();}
	
	void		SetNext(neFrame * frame) {next = frame;}
	
	void		SetChild(neFrame * frame) {child = frame;}
	
	neFrame *	GetNext() {return next;}
	
	neFrame *	GetChild() {return child;}
	
	void		SetMatrix(const neM4& mat) {transform = mat;}
	
	neM4 *		GetMatrix() {return &transform;}
	
	bool		Setup(CObjectSetupParam * param);

	f32			IntersectRay(neV3 * ray, neV3 * orig);

protected:
	neMesh * mesh;
	neM4 transform;
	neFrame * next;
	neFrame * child;
	f32 radius;
};

///////////////////////////////////////////////////////////////////
//
// neCamera
//
///////////////////////////////////////////////////////////////////
class neCameraParam: public CObjectSetupParam{};

class neCamera: public CMemObject
{
public:
	neCamera(CObjectFactory * factory);
	
	void	SetMatrix(neM4& mat);
	
	neM4 *	GetMatrix();
	
	neM4 *	GetProjection();
	
	void	SetViewMatrixMethod1(neV3 & from, neV3 & at, neV3 & worldUp);
	
	void	SetViewMatrixMethod2(f32 Dist,
							neRadian Rx,
							neRadian Ry,
							neV3 & vAt,
							neV3 & vWorldUp );
	bool	Setup(CObjectSetupParam * param);

	neV3	GetPos(){return pos;}

	void	SetPos(const neV3& spos){pos = spos;}
protected:
	neM4 transform;
	neM4 projection;
	neV3 pos;
	f32 fov;
	f32 aspect;
	f32 nearZ;
	f32 farZ;
};

///////////////////////////////////////////////////////////////////
//
// neTexture
//
///////////////////////////////////////////////////////////////////
class neTexture: public CMemObject
{
public:
	neTexture(CObjectFactory * factory, bool unique): CMemObject(factory, unique){}
};

///////////////////////////////////////////////////////////////////
//
// neSprite
//
///////////////////////////////////////////////////////////////////
class neSprite: public CMemObject
{
public:
	neSprite(CObjectFactory * factory, bool unique): CMemObject(factory, unique){}
};

///////////////////////////////////////////////////////////////////
//
// neDisplaySystem
//
///////////////////////////////////////////////////////////////////
class neDisplaySystemFactory: public CObjectFactory
{
protected:
	CMemObject * _MakeInstance(int objType, bool unique = true);
};

typedef struct neDrawMeshListEntry neDrawMeshListEntry;

struct neDrawMeshListEntry
{
	neMesh * mesh;
	neM4 * mat;
	char * cookie;
	neV3 * scale;
};

typedef struct neDrawFrameListEntry neDrawFrameListEntry;

struct neDrawFrameListEntry
{
	neFrame * frame;
	char * cookie;
	neV3 * scale;
};

//typedef map<u32, neEffectParamInfo> neEffectParamMap;
typedef map<u32, neEffectParamInfo> neEffectParamMap;

class neEffectManager
{
public:
	neEffectManager();
	
	virtual void		Initialise(neDisplaySystem * ds);

	virtual void		Shutdown();
	
	virtual string		GetEffectFilename(const char * matName);
	
	void				RegisterEffectParam(neEffectParamInfo * effectParamInfo);
	
	neEffectParamInfo * GetEffectParamInfo(u32 paramID);

	neEffectParamInfo * GetEffectParamInfoByName(const char * paramName);

	virtual bool		CalculateParam(neEffectParamInfo * epi, neEffectContext * effectContext) = 0;

	virtual void		InitialiseEffectParams() = 0;

protected:
	bool isInitialise;
	neDisplaySystem * ds;
	neEffectParamMap effectParams;

private:
	void	AllocEffectParam();
	int		vectorParamNext;
	int		matrixParamNext;
	int		textureParamNext;
	
	int		vectorParamCount;
	int		matrixParamCount;
	int		textureParamCount;
	
	neV4 *	vectorParamMem;
	neM4 *	matrixParamMem;
	neTexture ** textureParamMem;

	friend int neAllocEffectParam(lua_State * lua);
	friend int neRegisterEffectParam(lua_State * lua);
	bool CreateTextureParam(CLuaTable * table, neEffectParamInfo * epi);
};

typedef struct neDisplayMode neDisplayMode;

struct neDisplayMode
{
	neV3 dim;
	bool fullscreen;
};

class neDisplaySystem
{
public:
	enum {
		DS_FRAME,
		DS_MESH,
		DS_TEXTURE,
		DS_SPRITE,
		DS_CAMERA,
		DS_EFFECT,
	} DisplayObjectType;

	enum
	{
		LINE_BUFFER_SIZE = 1000,
		LINE_VERTEX_SIZE = LINE_BUFFER_SIZE * 4,
	};

	///////////////////////////
	//virtual functions
	///////////////////////////
	virtual bool Initialise();
	
	virtual void Shutdown();
	
	virtual bool SetDisplayMode(neDisplayMode * mode) = 0;

	neFrame *			CreateFrame(string fileName);
	
	neCamera *			CreateCamera();
	
	virtual neMesh *	CreateMesh(string fileName, bool uniqueMesh) = 0;
	
	virtual neTexture *	CreateTexture(string fileName, bool unique) = 0;
	
	virtual neSprite *	CreateSprite(string fileName, bool unique) = 0;
	
	virtual neEffect *	CreateEffect(string fileNmae, bool unique) = 0;

	virtual bool BeginDisplay();
	
	virtual void AddToDisplayList(neMesh * mesh, neM4 * mat, neV3 * scale = NULL, char * cookies = NULL);
	
	virtual void AddToDisplayList(neFrame * frame, neV3 * scale = NULL, char * cookies = NULL);

	virtual void EndDisplayList();
	
	virtual void EndDisplay() = 0;
	
	virtual void PrintText(s32 posX, s32 posY, const char * str, ...) = 0;
	
	virtual void PrintText(s32 posX, s32 posY, const string& str) = 0;

	virtual void PrintText2(s32 posX, s32 posY, const char * str) = 0;

	virtual void SetAmbientLight(neColour& Color, bool enabled) = 0;
	
	virtual void SetDirLight(s32 LightID, neV3& Dir, neColour& Color, bool enabled) = 0;
	
	virtual CObjectFactory * GetFactory() = 0;

	virtual void GetRay(s32 screenX, s32 screenY, neV3 * dir, neV3 * orig) = 0;

	virtual void DisplayLines() = 0;

	////////////////////////////
	//non-virtual functions
	////////////////////////////
	void		SetCamera(neCamera * camera);

	void		DrawLine(s32 count, neV3 colour, neV3 * startPoint);
	
	neCamera *	GetCamera();

	void		SetEffectManager(neEffectManager * eman);
	
	neEffectManager * GetEffectManager() {
		return effectManager;}
	
	void ComputeEffectUpdateMode(neEffect * effect);

	void SetEffectUpdateFlag(EFFECT_UPDATE_MODE flag);

	neFrame * GetCurrentFrame() {return currentFrame;}

	char * GetCurrentCookie() {return currentCookie;}

	virtual void ClearDisplayList() 
	{
		while (meshDrawList.size())
		{
			meshDrawList.pop();
		}
		while (frameDrawList.size())
		{
			frameDrawList.pop();
		}
	};
	void ClearDisplayLines();

public:
	s32 lineCount;
	s32 vertexCount;
	s32  linePointCount[LINE_BUFFER_SIZE];
	s32	 lineStartOffset[LINE_BUFFER_SIZE];
	neV3 lineColour[LINE_BUFFER_SIZE];
	neV3 lineVertices[LINE_VERTEX_SIZE];
	
protected:
	neCamera * cam0; //default camera
	neCamera * currentCam;
	neEffectManager * effectManager;
	queue<neDrawMeshListEntry> meshDrawList;
	queue<neDrawFrameListEntry> frameDrawList;
	vector<neEffect *> effectList;
	char * currentCookie;
	neFrame * currentFrame;
};

neDisplaySystem * CreateDisplaySystem();

#endif //DISPLAYSYSTEM_H