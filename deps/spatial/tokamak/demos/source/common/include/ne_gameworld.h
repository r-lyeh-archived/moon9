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

#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include "ne_os.h"
#include "ne_uselua.h"
#include <vector>

#define LUA_STR_Type "Type"
#define LUA_STR_Mesh "Mesh"
#define LUA_STR_Pos "Pos"
#define LUA_STR_Name "Name"
#define LUA_STR_RotX "RotX"
#define LUA_STR_RotY "RotY"
#define LUA_STR_RotZ "RotZ"
#define LUA_STR_OnClick "OnClick"


#define LUA_STR_Simple "Simple"

///////////////////////////////////////////////////////////////////
//
//	Some simple for passing vector in Lua function
//
///////////////////////////////////////////////////////////////////

//typedef float LuaV3[3];
//typedef float LuaV4[4];


///////////////////////////////////////////////////////////////////
//
//	Event 
//	
//	Our game world is driven by events
//
///////////////////////////////////////////////////////////////////
class CEvent
{
public:
	ulong id;
	neByte * param;
};

class neInitialiseParam
{
};

///////////////////////////////////////////////////////////////////
//
//	The GameWorld object is responsible for:
//	
//	  1) maintaining an list of active tokens in the game world
//	  2) passing events between the tokens.
//	  3) sending the tokens to the display list
//
///////////////////////////////////////////////////////////////////

class neToken;

class neGameWorld : public CObjectFactory
{
public:
	neGameWorld() 
	{ 
		if (GETOS->GetGameWorld())
		{
			//there is already a gameworld assigned!!
			ASSERT(0);
		}
		GETOS->SetGameWorld(this); 
	}

	virtual void Initialise();		//called once initialisation
	
	virtual void Shutdown();		//called once shutdown
	
	virtual void Process(OSTime time){}; //advance the game world by time, in milli-second
	
	virtual void ProcessEvent(CEvent& event){}; //process various os event
	
	virtual void Display(){};

	virtual void DisplayText(){};

	virtual int CreateToken(lua_State * lua);

	neToken * GetToken(const char * name);

protected:
	vector <neToken *> allToken;

	virtual CMemObject * _MakeInstance(int objType, bool unique = true);
};

neToken * neGetToken(const char * name);

///////////////////////////////////////////////////////////////////
//
//	A Token represent a entity in the game world
//	It is aware of the game world which it is in
//	It also can be aware of other game tokens
//	It has the following aspects:
//
//		1) Advancing in time
//		2) Game related behaviour information 
//		3) Contain display and audio data for itself
//		4) Recieve and process events
//		5) Send message to the game world and other token
//		6) Load data
//		7) Load/Save it's state
//
///////////////////////////////////////////////////////////////////
class neToken : public CMemObject
{
public:
	neToken(CObjectFactory * factory): CMemObject(factory, true){visible = true;};
	
	//functions
	virtual void Initialise(neInitialiseParam * ){};		//called once initialisation
	
	virtual void Process(OSTime time){}; //advance the token, in milli-second
	
	virtual void ProcessEvent(CEvent& event){}; //process game events event

	neGameWorld * GetGameWorld(){return (neGameWorld*)this->factory;}

	virtual void AddToDisplay() = 0;
	
	// position and orientation
	virtual void SetPos(const neV3 & pos)=0;
	virtual void SetPos(f32 pos[3])=0;

	virtual neV3 GetPos()=0;
	virtual void GetPos(f32 pos[3])=0;

	virtual void SetMatrix(const neM4 & mat) = 0;
	virtual void SetMatrix(f32 col1[3], f32 col2[3], f32 col3[3], f32 col4[3]) = 0;
	virtual void SetMatrix(const neT3 & tran) = 0;

	virtual void LookAt(const neV3 & from, const neV3 & at, const neV3 & worldUp)=0;
	virtual void LookAt(f32 from[3], f32 at[3], f32 worldUp[3])=0;

	virtual neM4 GetMatrix()=0;
	virtual void GetMatrix(f32 col1[3], f32 col2[3], f32 col3[3], f32 col4[3]) = 0;
	
	//info
	string GetName() {return name;}
	char * GetNamePtr() {return (char *)name.c_str();}

	//visible
	void SetVisible(bool vis){visible = vis;}
	bool GetVisible(){return visible;}

	virtual void RotateYDegree(f32 degree){};

protected:
	string name;
	bool visible;
	virtual void SendEvent() {};
};


class neSimpleTokenParam : public CObjectSetupParam
{
public:
	string name;
	string mesh;
	neM4 trans;
	CLuaFunction onClick;
};

class neSimpleToken: public neToken
{
public:
	neSimpleToken(CObjectFactory * factory): neToken(factory)
	{
		frame = NULL;
	}

	virtual ~neSimpleToken();

	bool Setup(CObjectSetupParam * param);

	void SetPos(const neV3 & pos);
	void SetPos(f32 pos[3]);
	
	neV3 GetPos();
	void GetPos(f32 pos[3]);

	void SetMatrix(const neM4 & mat);
	void SetMatrix(f32 col1[3], f32 col2[3], f32 col3[3], f32 col4[3]);
	void SetMatrix(const neT3 & tran);

	void LookAt(const neV3 & from, const neV3 & at, const neV3 & worldUp);
	void LookAt(f32 from[3], f32 at[3], f32 worldUp[3]);

	neM4 GetMatrix();
	void GetMatrix(f32 col1[3], f32 col2[3], f32 col3[3], f32 col4[3]);

	void AddToDisplay();

	neFrame * GetFrame() {return frame;}

	void SetDiffuseColour(const neV4 & c);

	neV4 GetDiffuseColour();

	void SetAmbientColour(const neV4 & c);

	neV4 GetAmbientColour();

	void SetDrawScale(const neV3 & scale);

	void CallLuaOnClick();

	void RotateYDegree(f32 degree);

private:
	neFrame * frame;
	neV3 scale;
	CLuaFunction onClick;
	neV4 dcolour;
	neV4 acolour;
};

neToken * neConvertToToken(void * ptr);

#define NE_SIMPLE_TOKEN 0

#endif