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

#include "tokamak.h"
#include "ne.h"
#include "a_effectmanager.h"

class CSampleBase: public neGameWorld
{
public:
	bool paused;
	
	int counter;
	
	neV3 focusPos;

	AEffectManager effectMan;
	
	CTargetCamera targetCam;
	
	CSampleBase() { 
		paused = true;

		landToken = NULL;
	}
	
	//inherited functions
	virtual void Initialise();
	
	virtual void Shutdown();
	
	virtual void Process(OSTime time);

	virtual void Reset();

	virtual void Free();
	
	neSimpleToken * MakeToken(neGeometry * geom);

	void DisplayRigidBodies(neRigidBody ** rb, s32 count);

	void DisplayAnimatedBodies(neAnimatedBody ** ab, s32 count);

	void ProcessEvent(CEvent& e){}

	void DisplayProfileReport();
	
	enum
	{
		WALL_NUMBER = 6,
	};

public:
	void SetUpRoom(neSimulator * sim);

	void SetupTerrain(neSimulator * sim);

	neSimpleToken * landToken;
	
	neAnimatedBody * walls[WALL_NUMBER];
	
	neSimpleToken * wallTokens[WALL_NUMBER];

	nePerformanceReport perfReport;
};

bool ReadConvexData(char * filename, neByte *& adjacency);