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

#include "sample3.h"
#include "Windows.h"

// undefine this to turn collision callback on
//
// #define COLLISION_CALLBACK_ON

CSample3 gw;

#define CAMERA_DIST 35

#define TIME_STEP 1.0f/60.0f

void CSample3::Initialise() 
{
	CLuaValue &titleLUA = CLuaValue("Tokamak demo - stacking of objects - (c) 2003 Tokamak Ltd");
		
	titleLUA.SetGlobal(GETLUA->GetLuaState(), "WindowTitle");

	CSampleBase::Initialise();

	targetCam.SetCamera(GETDISPLAY->GetCamera());
	
	neV3 tmp; tmp.Set(0,0,CAMERA_DIST);

	GETDISPLAY->GetCamera()->SetPos(tmp);

	perfReport.Reset();

	perfReport.SetReportType(nePerformanceReport::NE_PERF_RUNNING_AVERAGE);

	targetCam.xDir = -0.3f;

	targetCam.yDir = -0.3f;

	Reset();
}

void CSample3::Reset()
{
	CSampleBase::Reset();

	InititialisePhysics();

	InititialiseTokens();

	GETDISPLAY->ClearDisplayList();
}

void CSample3::Free()
{
	CSampleBase::Free();

	for (s32 i = 0; i < N_BODY; i++)
	{
		if (boxToken[i])
		{
			boxToken[i]->Free();
			boxToken[i] = NULL;
		}
	}
	if (sim)
	{
		neSimulator::DestroySimulator(sim);

		sim = NULL;
	}
}

void CSample3::Process(OSTime time)
{
	CSampleBase::Process(time);

	if (GETOS->IsKeyPress('R'))
	{
		Reset();

		return;
	}
	if (GETOS->IsKeyPress('P'))
	{
		paused = !paused;
	}
	
	s32 actOnBody = STACK_HEIGHT - 1;

	neBool dontSet = false;

	f32 v = 5.0f;
	
	static nextBullet = 0;

	if (GETOS->IsKeyPress('Q'))
	{
		neT3 t = targetCam.GetTransform();

		neV3 dir = t.rot[2] * -1.0f;

		neV3 pos = t.pos + dir * 3.0f;

		s32 bullet = N_BODY - N_BULLET + nextBullet;

		box[bullet]->SetPos(pos);

		//box[bullet]->SetVelocity(dir * 20.0f);
		box[bullet]->SetVelocity(dir * 30.0f);

		nextBullet = (nextBullet + 1) % N_BULLET;
	}
	if (GETOS->IsKeyPress('E'))
	{
		neV3 dir; dir.Set(5, 0, 0);
		//box[bullet]->SetVelocity(dir * 20.0f);
		box[1]->SetVelocity(dir);
	}
	if (GETOS->IsKeyPress('W'))
	{
		neV3 dir; dir.Set(-5, 0, 0);
		//box[bullet]->SetVelocity(dir * 20.0f);
		box[1]->SetVelocity(dir);
	}
	////////////////////////////////////////////////////////

	for (s32 i = 0; i < N_BODY; i++)
	{
		if (box[i])
		{
			neT3 trans = box[i]->GetTransform();

			boxToken[i]->SetMatrix(trans);

			boxToken[i]->AddToDisplay();
		}
	}
	counter++;

	if (!paused)
	{
		sim->Advance(TIME_STEP, 1, &perfReport);
	}
	//paused = true;
}

void CSample3::DisplayText()
{
	f32 processtime = perfReport.time[nePerformanceReport::NE_PERF_TOTAL_TIME];
	f32 percentage =  processtime * 60.0f;

	char ss[256];
	
	sprintf(ss, "physics time %.3f ms, %.2f", processtime * 1000.0f, percentage * 100.0f);

	strcat(ss, "%% of a 1/60 second frame");

	GETDISPLAY->PrintText2(0, 0, ss);	

	s32 mem;

	sim->GetMemoryAllocated(mem);

	mem /= 1024;

	GETDISPLAY->PrintText(60, 0, "memory allocated %dk", mem);	

	GETDISPLAY->PrintText(0, 1, "Controls:");

	GETDISPLAY->PrintText(4, 2, "'P' -> pause/unpause");
	GETDISPLAY->PrintText(4, 3, "'A' + 'Z' + mouse + mouse button -> control camera");
	GETDISPLAY->PrintText(4, 4, "'Q' -> Fire block");
	GETDISPLAY->PrintText(4, 5, "'R' -> Reset");

	GETDISPLAY->PrintText(0, 7, "Demonstration of fast and stable stacking.");

	sprintf(ss, "%d of rigid bodies arrange in stacks.", N_BODY - 1);
	GETDISPLAY->PrintText(0, 8, ss);

	if (counter == 100)
	{
		perfReport.Reset();
		
		counter = 0;
	}
}

void CSample3::Shutdown() 
{
	Free();
	
	CSampleBase::Shutdown();
}

void CollisionCallback (neCollisionInfo & collisionInfo)
{
	if (collisionInfo.typeA == NE_RIGID_BODY)
	{
		neRigidBody * rbA = (neRigidBody *)collisionInfo.bodyA;

		rbA->GetUserData();
	}
	if (collisionInfo.typeB == NE_RIGID_BODY)
	{
		neRigidBody * rbB = (neRigidBody *)collisionInfo.bodyB;

		rbB->GetUserData();
	}
}

void CSample3::InititialisePhysics()
{
	neV3 gravity; gravity.Set(0.0f, -9.0f, 0.0f);

	f32 linkLength = 0.9f;

	f32 shift = 0.0f;

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidBodiesCount = N_BODY;
	sizeInfo.animatedBodiesCount = WALL_NUMBER;
	sizeInfo.geometriesCount = N_BODY + WALL_NUMBER;
	s32 totalBody = N_BODY + WALL_NUMBER;
	sizeInfo.overlappedPairsCount = totalBody * (totalBody - 1) / 2;
	{ //dont need any of these
		sizeInfo.rigidParticleCount = 0;
		sizeInfo.constraintsCount = 0;
		sizeInfo.terrainNodesStartCount = 0;
	}

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

#ifdef COLLISION_CALLBACK_ON

	sim->GetCollisionTable()->Set(0, 0, neCollisionTable::RESPONSE_IMPULSE_CALLBACK);

	sim->SetCollisionCallback(CollisionCallback);

#endif

	neV3 position;

	position.SetZero();

	s32 i = 0;

#ifdef PYRAMID

	MakeStack(position, i);	
	
#else
	f32 gap = 2.f;

	for (s32 j = 0; j < N_STACKS; j++)
	{
		//position.Set(3.05f * j, 0.0f, 0.0f);

		for (s32 k = 0; k < N_DEPTH; k++)
		{
			position.Set(gap * j, 0.0f, gap * k);

			MakeStack(position, i);	
		}
	}
#endif

	for (s32 jj = 0; jj < N_BULLET; jj++)
		MakeBullet(jj);

	SetUpRoom(sim);
}

void CSample3::MakeStack(neV3 position, s32 & index)
{
	const f32 groundLevel = -10.0f;

	s32 cur;

#ifndef PYRAMID 

	for (s32 i = 0; i < STACK_HEIGHT; i++)
	{
		cur = index + i;

		box[cur] = sim->CreateRigidBody();

		neGeometry * geom = box[cur]->AddGeometry();

		neV3 boxSize1; boxSize1.Set(0.8f, 0.8f, 0.8f);
		//neV3 boxSize1; boxSize1.Set(0.3, 1.9, 0.3);

		geom->SetBoxSize(boxSize1[0],boxSize1[1],boxSize1[2]);

		box[cur]->UpdateBoundingInfo();
		
		f32 mass = 1.0f;

		box[cur]->SetInertiaTensor(neBoxInertiaTensor(boxSize1[0], boxSize1[1], boxSize1[2], mass));

		box[cur]->SetMass(mass);

		neV3 pos;

		pos.Set(position[0], groundLevel + boxSize1[1] * (i + 0.5f) + 0.0f, position[2]);

		box[cur]->SetPos(pos);
	}
	index = cur+1;

#else
	
	neV3 boxRadii; boxRadii.Set(.5f, .5f, .5f);
	neV3 boxSize; boxSize = boxRadii * 2.f;
	f32 stepX    = boxSize[0] + 0.05f;
	s32 baseSize = 10;

	neV3 startPos; startPos.Set( 0.0f, groundLevel + boxSize[1] * 0.5f, 0.0f);

	int k = 0;

	for(int i=0; i<baseSize; i++)
	{
		int rowSize = baseSize - i;

		neV3 start; start.Set(-rowSize * stepX * 0.5f + stepX * 0.5f, i * boxSize[1], 0);

		for(int j=0; j< rowSize; j++)
		{
			neV3 boxPos; boxPos.Set(start);

			neV3 shift; shift.Set(j * stepX, 0.0f, 0.0f);

			boxPos += shift;
			boxPos += startPos;

			cur = index + k;

			box[cur] = sim->CreateRigidBody();

			neGeometry * geom = box[cur]->AddGeometry();

			geom->SetBoxSize(boxSize[0],boxSize[1],boxSize[2]);

			box[cur]->UpdateBoundingInfo();
			
			f32 mass = 1.f;

			box[cur]->SetInertiaTensor(neBoxInertiaTensor(boxSize[0], boxSize[1], boxSize[2], mass));

			box[cur]->SetMass(mass);
			
			box[cur]->SetPos(boxPos);

			k++;
		}
	}
	index = cur+1;
#endif
}

void CSample3::MakeBullet(s32 index)
{
	neSimpleTokenParam param;

	param.mesh = "rock.x";

	param.resourceID = "rock";

	param.trans.SetIdentity();

	neSimpleToken * token = (neSimpleToken *)MakeInstance(NE_SIMPLE_TOKEN, &param, true);

	neByte * neigbhourData;
	
	ReadConvexData("rock.bin", neigbhourData);

	s32 cur = N_BODY - 1 - index;

	boxToken[cur] = token;

	box[cur] = sim->CreateRigidBody();

	neGeometry * geom = box[cur]->AddGeometry();

	geom->SetConvexMesh(neigbhourData);

	geom->SetUserData((u32)token);

	neV3 boxSize1; boxSize1.Set(1,1,1);

	box[cur]->UpdateBoundingInfo();
	
	f32 mass = .3f;

	box[cur]->SetInertiaTensor(neSphereInertiaTensor(1.0f, mass));

	box[cur]->SetMass(mass);
	
	neV3 pos;

	pos.Set(-10.0f, -8.75f + index * 3.0f+3, 0.0f);

	box[cur]->SetPos(pos);
}

void CSample3::InititialiseTokens()
{
	s32 curToken = 0;

	for (s32 i = 0; i < N_BODY - N_BULLET; i++)
	{
		box[i]->BeginIterateGeometry();

		neGeometry * geom = box[i]->GetNextGeometry();

		while (geom)
		{
			neV3 boxSize;

			geom->GetBoxSize(boxSize);

			neSimpleTokenParam param;

			param.mesh = "cube3.x";

			param.resourceID = "cube";

			param.trans.SetIdentity();

			neSimpleToken * token = (neSimpleToken *)MakeInstance(NE_SIMPLE_TOKEN, &param, true);

			if ((curToken % 4) == 0)
			{
				token->SetDiffuseColour(neV4(0.8f, 0.2f, 0.2f, 1.0f));

				token->SetAmbientColour(neV4(0.3f, 0.0f, 0.0f, 1.0f));
			}
			else if ((curToken % 4) == 1)
			{
				token->SetDiffuseColour(neV4(0.2f, 0.1f, 0.8f, 1.0f));

				token->SetAmbientColour(neV4(0.2f, 0.2f, 0.4f, 1.0f));
			}
			else if ((curToken % 4) == 2)
			{
				token->SetDiffuseColour(neV4(0.0f, 0.8f, 0.0f, 1.0f));

				token->SetAmbientColour(neV4(0.1f, 0.3f, 0.0f, 1.0f));
			}
			else
			{
				token->SetDiffuseColour(neV4(0.8f, 0.8f, 0.2f, 1.0f));

				token->SetAmbientColour(neV4(0.3f, 0.3f, 0.1f, 1.0f));
			}
			token->SetDrawScale(boxSize);

			boxToken[curToken++] = token;

			geom = box[i]->GetNextGeometry();
		}
	}
}