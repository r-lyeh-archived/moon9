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

#include "../samplebase.h"

class CSample9: public CSampleBase
{
public:
	CSample9() { 
		paused = false;
	}
	
	void Initialise();
	
	void Shutdown();

	void Reset();

	void Free();
	
	void Process(OSTime time);
	
	void DisplayText();

	void InititialisePhysics();

	void InititialiseTokens();

	void MakeRadDude(neV3 position, s32 & index);

	void MakeStair();

	void MakeStack(neV3 position, s32 & index);

public:
	enum
	{
		N_DUDE = 4,

		BONES_PER_DUDE = 10,

		JOINTS_PER_DUDE = 9,

		STACK_HEIGHT = 3,

		N_STACKS = 7,

		N_DEPTH = 2,

		N_BODY = BONES_PER_DUDE * N_DUDE + STACK_HEIGHT * N_STACKS * N_DEPTH,

		N_STEP = 7,//20,
	};

	neSimulator * sim;
	
	neRigidBody * rigidBodies[N_BODY];

	neAnimatedBody * steps[N_STEP];
	
	neSimpleToken * tokens[N_BODY + N_STEP];

	neAllocatorDefault all;

	nePerformanceReport perfReport;
};

