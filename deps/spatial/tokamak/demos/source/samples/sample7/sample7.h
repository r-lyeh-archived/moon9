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

void BreakageCallbackFn(neByte * originalBody, neBodyType bodyType, neGeometry * brokenGeometry, neRigidBody * newBody);

class CSample7: public CSampleBase
{
public:
	CSample7() { 
		paused = false;
	}
	
	void Initialise();
	
	void Shutdown();
	
	void Process(OSTime time);
	
	void DisplayText();

	void InititialisePhysics();

	void InititialiseTokens();

	void MakeParticle(neV3 position, s32 index);

	void MakeTree(neV3 position, s32 index);

public:
	enum
	{
		NUMBER_OF_PARTICLES = 100,

		NUMBER_OF_TREES = 5,

		BRANCH_PER_TREE = 4, 

		GEOMETRY_PER_TREE = BRANCH_PER_TREE + 2,

		NUMBER_OF_BODIES = NUMBER_OF_PARTICLES + NUMBER_OF_TREES * BRANCH_PER_TREE,

		NUMBER_OF_TOKENS = NUMBER_OF_PARTICLES + NUMBER_OF_TREES * GEOMETRY_PER_TREE,
	};

	neSimulator * sim;
	
	neRigidBody * particles[NUMBER_OF_PARTICLES];

	neRigidBody * branches[NUMBER_OF_TREES * BRANCH_PER_TREE];

	neAnimatedBody * trees[NUMBER_OF_TREES];
	
	neSimpleToken * tokens[NUMBER_OF_TOKENS];

	neAllocatorDefault all;

	nePerformanceReport perfReport;

	s32 nextBullet;

	s32 nextFreeToken;

	s32 nextFreeBranch;
};
