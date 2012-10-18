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

#define PYRAMID

class CSample3: public CSampleBase
{
public:
	CSample3() { 
		paused = false;
	}
	
	void Initialise();
	
	void Shutdown();
	
	void Process(OSTime time);
	
	void DisplayText();

	void InititialisePhysics();

	void InititialiseTokens();

	void MakeStack(neV3 position, s32 & index);

	void MakeBullet(int);

	void Reset();

	void Free();

public:
	enum
	{
		STACK_HEIGHT = 10,//4,

		N_STACKS = 5,

		N_DEPTH = 5,

		N_BULLET = 5,

#ifdef PYRAMID
		N_BODY = 55 + N_BULLET,
#else
		N_BODY = STACK_HEIGHT * N_STACKS * N_DEPTH + N_BULLET,
#endif
	};

	neSimulator * sim;
	
	neRigidBody * box[N_BODY];
	
	neSimpleToken * boxToken[N_BODY];

	neAllocatorDefault all;

	nePerformanceReport perfReport;
};
