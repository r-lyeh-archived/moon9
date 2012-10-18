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

#include "sample7.h"
#include "Windows.h"

CSample7 gw;

#define MAX_OVERLAPPED_PAIR 1000

#define CAMERA_DIST 50

#define TIME_STEP (1.0f / 60.0f)

neAnimatedBody * testTree = NULL;

neGeometry * testBranch = NULL;

void CSample7::Initialise() 
{
	CLuaValue &titleLUA = CLuaValue("Tokamak demo - Sphere and Cylinder collision Demo - (c) 2003 Tokamak Ltd");
		
	titleLUA.SetGlobal(GETLUA->GetLuaState(), "WindowTitle");

	CSampleBase::Initialise();

	targetCam.SetCamera(GETDISPLAY->GetCamera());
	
	neV3 tmp; tmp.Set(0,10,CAMERA_DIST);

	GETDISPLAY->GetCamera()->SetPos(tmp);

	perfReport.Reset();

	perfReport.SetReportType(nePerformanceReport::NE_PERF_RUNNING_AVERAGE);

	for (s32 i = 0; i < NUMBER_OF_TOKENS; i++)
		tokens[i] = NULL;

	InititialisePhysics();

	InititialiseTokens();

	targetCam.xDir = 0.0f;
	
	targetCam.yDir = 0.2f;

	nextBullet = 0;

	nextFreeBranch = 0;
}

void CSample7::Process(OSTime time)
{
	CSampleBase::Process(time);

	if (GETOS->IsKeyPress('P'))
	{
		paused = !paused;
	}
	
	if (GETOS->IsKeyPress('Q'))
	{
		neT3 t = targetCam.GetTransform();

		neV3 dir = t.rot[2] * -1.0f;

		neV3 pos = t.pos + dir * 10.0f;

		particles[nextBullet]->SetPos(pos);

		particles[nextBullet]->SetVelocity(dir * 30.0f);

		nextBullet = (nextBullet + 1) % NUMBER_OF_PARTICLES;
	}
	if (GETOS->IsKeyPress('W'))
	{
		gw.branches[gw.nextFreeBranch] = testTree->BreakGeometry(testBranch);
	
		gw.nextFreeBranch++;
	}
	////////////////////////////////////////////////////////

	DisplayRigidBodies(&particles[0], NUMBER_OF_PARTICLES);

	DisplayRigidBodies(&branches[0], NUMBER_OF_TREES * BRANCH_PER_TREE);

	DisplayAnimatedBodies(&trees[0], NUMBER_OF_TREES);

	if (landToken)
	{
		neT3 trans;

		trans.SetIdentity();

		landToken->SetMatrix(trans);

		landToken->AddToDisplay();
	}
	counter++;

	if (!paused)
	{
		sim->Advance(TIME_STEP, 1, &perfReport);
	}
	//paused = true;
}

void CSample7::DisplayText()
{
	f32 processtime = perfReport.time[nePerformanceReport::NE_PERF_TOTAL_TIME];
	f32 percentage =  processtime * 60.0f;

	char ss[256];
	
	sprintf(ss, "physics time %.3f ms, %.0f", processtime * 1000.0f, percentage * 100.0f);

	strcat(ss, "%% of a 1/60 second frame");

	GETDISPLAY->PrintText2(0, 0, ss);	

	s32 mem;

	sim->GetMemoryAllocated(mem);

	mem /= 1024;

	GETDISPLAY->PrintText(60, 0, "memory allocated %dk", mem);	

	GETDISPLAY->PrintText(0, 1, "Controls:");

	GETDISPLAY->PrintText(4, 2, "'P' -> pause/unpause");
	GETDISPLAY->PrintText(4, 3, "'A' + 'Z' + mouse + mouse button -> control camera");
	GETDISPLAY->PrintText(4, 4, "'Q' -> Fire particles");

	GETDISPLAY->PrintText(0, 6, "100 sphere rigid particles interact with arbitrary landscape mesh.");

	GETDISPLAY->PrintText(0, 7, "Trees constructed with cylinders and branches are specified");
	
	GETDISPLAY->PrintText(0, 8, "to break off when hit.");

	if (counter == 100)
	{
		perfReport.Reset();
		
		counter = 0;
	}
}

void CSample7::Shutdown() 
{
	for (s32 i = 0; i < NUMBER_OF_TOKENS; i++)
	{
		if (tokens[i])
			tokens[i]->Free();
	}

	neSimulator::DestroySimulator(sim);
	
	CSampleBase::Shutdown();
}

void BreakageCallbackFn(neByte * originalBody, neBodyType bodyType, neGeometry * brokenGeometry, neRigidBody * newBody)
{
	gw.branches[gw.nextFreeBranch] = (neRigidBody*)newBody;

	gw.nextFreeBranch++;
}

neV3 treePosition[CSample7::NUMBER_OF_TREES] =
{
	{0.0f, 5.0f, 0.0f},
	{25.0f, 5.0f, -20.0f},
	{-20.0f, 0.0f, 30.0f},
	{30.0f, 5.0f, 10.0f},
	{-30.0f, 0.0f, -20.0f},
};

void CSample7::InititialisePhysics()
{
	for (s32 i = 0; i < NUMBER_OF_BODIES; i++)
	{
		branches[i] = NULL;
	}
	neV3 gravity; gravity.Set(0.0f, -10.0f, 0.0f);

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidBodiesCount = NUMBER_OF_TREES * BRANCH_PER_TREE;
	sizeInfo.rigidParticleCount = NUMBER_OF_PARTICLES;
	sizeInfo.animatedBodiesCount = WALL_NUMBER + NUMBER_OF_TREES;
	sizeInfo.geometriesCount = NUMBER_OF_TOKENS + WALL_NUMBER;
	
	{ //dont need any of these
		
		sizeInfo.constraintsCount = 0;
	}

	s32 totalBody = sizeInfo.rigidBodiesCount + sizeInfo.animatedBodiesCount;

	sizeInfo.overlappedPairsCount = totalBody * (totalBody - 1) / 2;

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

	sim->SetBreakageCallback(BreakageCallbackFn);

	neV3 position;
	
	position.SetZero();

	for (s32 j = 0; j < NUMBER_OF_PARTICLES; j++)
	{
		position.Set(2.0f * j, 2.0f, 100.0f);

		MakeParticle(position, j);	
	}
	for (j = 0; j < NUMBER_OF_TREES; j++)
	{
		MakeTree(treePosition[j], j);	
	}

	SetUpRoom(sim);

	SetupTerrain(sim);
}

void CSample7::MakeParticle(neV3 position, s32 index)
{
	const f32 groundLevel = -10.0f;

	s32 cur;

	cur = index;

	particles[cur] = sim->CreateRigidParticle();

	particles[cur]->GravityEnable(false);

	neGeometry * geom = particles[cur]->AddGeometry();

	neV3 boxSize1; boxSize1.Set(0.8f, 0.8f, 0.8f);

	geom->SetSphereDiameter(0.8f);

	particles[cur]->UpdateBoundingInfo();
	
	f32 mass = 1.0f;

	particles[cur]->SetInertiaTensor(neSphereInertiaTensor(0.8f, mass));

	particles[cur]->SetMass(mass);
	
	particles[cur]->SetPos(position);
}

struct TreeData
{
	f32 diameter;
	f32 height;
	neV3 pos;
	neV3 rotation;
	neV3 colour;
	neGeometry::neBreakFlag breakFlag;
	f32 breakageMass;
	f32 breakageMagnitude;
	f32 breakageAbsorption;
};

TreeData treeData[CSample7::GEOMETRY_PER_TREE] = 
{
	{1.5f, 10.0f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.5f, 0.3f, 0.1f}, neGeometry::NE_BREAK_DISABLE, 2.0f, 0.0f, 0.0f},
	{1.2f, 10.0f, {0.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.5f, 0.3f, 0.1f}, neGeometry::NE_BREAK_DISABLE, 2.0f, 0.0f, 0.0f},
	
	{0.8f, 7.0f, {-4.0f, 3.0f, 0.0f}, {0.0f, 0.0f, -NE_PI / 4.0f}, {0.4f, 0.5f, 0.1f}, neGeometry::NE_BREAK_NORMAL, 5.0f, 1.0f, 0.3f},
	{0.8f, 7.0f, {4.0f, 6.0f, 0.0f}, {0.0f, 0.0f, NE_PI / 4.0f}, {0.4f, 0.5f, 0.1f}, neGeometry::NE_BREAK_NORMAL, 5.0f, 1.0f, 0.3f},

	{0.8f, 6.0f, {-3.5f, 10.0f, 0.0f}, {0.0f, 0.0f, -NE_PI / 4.0f}, {0.4f, 0.5f, 0.1f}, neGeometry::NE_BREAK_NORMAL, 5.0f, 2.0f, 0.3f},
	{0.8f, 6.0f, {3.5f, 12.0f, 0.0f}, {0.0f, 0.0f, NE_PI / 4.0f}, {0.4f, 0.5f, 0.1f}, neGeometry::NE_BREAK_NORMAL, 5.0f, 2.0f, 0.3f},

};

void CSample7::MakeTree(neV3 position, s32 index)
{
	const f32 groundLevel = -10.0f;

	s32 cur;

	cur = index;

	trees[cur] = sim->CreateAnimatedBody();

	trees[cur]->SetPos(position);

	testTree = trees[cur];

	for (s32 i = 0; i < GEOMETRY_PER_TREE; i++)
	{
		neGeometry * geom = trees[cur]->AddGeometry();

		geom->SetCylinder(treeData[i].diameter, treeData[i].height);

		neT3 trans;

		trans.SetIdentity();

		trans.pos = treeData[i].pos;

		trans.rot.RotateXYZ(treeData[i].rotation);
		
		geom->SetTransform(trans);

		geom->SetBreakageFlag(treeData[i].breakFlag);

		geom->SetBreakageMass(treeData[i].breakageMass);

		geom->SetBreakageInertiaTensor(neCylinderInertiaTensor(treeData[i].height, treeData[i].height, treeData[i].breakageMass));

		geom->SetBreakageMagnitude(treeData[i].breakageMagnitude);

		geom->SetBreakageAbsorption(treeData[i].breakageAbsorption);

		neV3 plane;

		plane.Set(0.0f, 1.0f, 0.0f);

		geom->SetBreakagePlane(plane);

		if (i == 1)
			testBranch = geom;
	}
	trees[cur]->UpdateBoundingInfo();
}

void CSample7::InititialiseTokens()
{
	s32 curToken = 0;

	for (s32 i = 0; i < NUMBER_OF_PARTICLES; i++)
	{
		particles[i]->BeginIterateGeometry();

		neGeometry * geom = particles[i]->GetNextGeometry();

		while (geom)
		{
			neSimpleToken * token = MakeToken(geom);

			geom->SetUserData((u32)token);

			if ((curToken % 4) == 0)
			{
				token->SetDiffuseColour(neV4(0.8f, 0.2f, 0.2f, 1.0f));

				token->SetAmbientColour(neV4(0.3f, 0.0f, 0.0f, 1.0f));
			}
			else if ((curToken % 4) == 1)
			{
				token->SetDiffuseColour(neV4(0.2f, 0.4f, 0.6f, 1.0f));

				token->SetAmbientColour(neV4(0.1f, 0.2f, 0.3f, 1.0f));
			}
			else if ((curToken % 4) == 2)
			{
				token->SetDiffuseColour(neV4(0.2f, 0.6f, 0.2f, 1.0f));

				token->SetAmbientColour(neV4(0.1f, 0.3f, 0.1f, 1.0f));
			}
			else
			{
				token->SetDiffuseColour(neV4(0.6f, 0.6f, 0.2f, 1.0f));

				token->SetAmbientColour(neV4(0.3f, 0.3f, 0.1f, 1.0f));
			}

			tokens[curToken++] = token;

			geom = particles[i]->GetNextGeometry();
		}
	}

	for (i = 0; i < NUMBER_OF_TREES; i++)
	{
		trees[i]->BeginIterateGeometry();

		neGeometry * geom = trees[i]->GetNextGeometry();

		s32 b = 0;

		while (geom)
		{
			{
			neSimpleToken * token = MakeToken(geom);

			geom->SetUserData((u32)token);

			token->SetDiffuseColour(neV4(treeData[b].colour[0], 
										treeData[b].colour[1], 
										treeData[b].colour[2], 
										1.0f));

			token->SetAmbientColour(neV4(treeData[b].colour[0] * 0.5f , 
										treeData[b].colour[1] * 0.5f, 
										treeData[b].colour[2] * 0.5f, 
										1.0f));

			tokens[curToken++] = token;
			}
			geom = trees[i]->GetNextGeometry();

			b++;
		}
	}
	nextFreeToken = curToken;
}

