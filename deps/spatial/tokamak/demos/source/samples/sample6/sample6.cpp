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

#include "sample6.h"
#include "Windows.h"

CSample6 gw;

#define CAMERA_DIST 70

#define TIME_STEP (1.0f / 60.0f)

void CSample6::Initialise() 
{
	CLuaValue &titleLUA = CLuaValue("Tokamak demo - Rigid Particles and Terrain Demo - (c) 2003 Tokamak Ltd");
		
	titleLUA.SetGlobal(GETLUA->GetLuaState(), "WindowTitle");

	CSampleBase::Initialise();

	targetCam.SetCamera(GETDISPLAY->GetCamera());
	
	neV3 tmp; tmp.Set(0,30,CAMERA_DIST);

	GETDISPLAY->GetCamera()->SetPos(tmp);

	perfReport.Reset();

	perfReport.SetReportType(nePerformanceReport::NE_PERF_RUNNING_AVERAGE);

	InititialisePhysics();

	InititialiseTokens();

	targetCam.xDir = 0.0f;
	
	targetCam.yDir = -0.3f;

	nextBullet = 0;
}

void CSample6::Process(OSTime time)
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

		box[nextBullet]->SetPos(pos);

		box[nextBullet]->SetVelocity(dir * 20.0f);

		nextBullet = (nextBullet + 1) % NUMBER_OF_PARTICLES;
	}

	////////////////////////////////////////////////////////

	for (s32 i = 0; i < NUMBER_OF_PARTICLES; i++)
	{
		if (box[i])
		{
			neT3 trans = box[i]->GetTransform();

			s32 cur = i * GEOMETRY_PER_BODY;

			for (s32 j = 0; j < GEOMETRY_PER_BODY; j++)
			{
				boxToken[cur + j]->SetMatrix(trans);

				boxToken[cur + j]->AddToDisplay();
			}
		}
	}
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
//	paused = true;
}

void CSample6::DisplayText()
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

	GETDISPLAY->PrintText(0, 6, "100 rigid particles interact with arbitrary landscape mesh.");

	GETDISPLAY->PrintText(0, 7, "Rigid particles are light-weight rigid bodies which doesn't");
	
	GETDISPLAY->PrintText(0, 8, "collide with each other, designed for particle effects");

	if (counter == 100)
	{
		perfReport.Reset();
		
		counter = 0;
	}
}

void CSample6::Shutdown() 
{
	for (s32 i = 0; i < NUMBER_OF_PARTICLES * GEOMETRY_PER_BODY; i++)
	{
		if (boxToken[i])
			boxToken[i]->Free();
	}
	
	neSimulator::DestroySimulator(sim);
	
	CSampleBase::Shutdown();
}

void CSample6::InititialisePhysics()
{
	neV3 gravity; gravity.Set(0.0f, -8.0f, 0.0f);

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidParticleCount = NUMBER_OF_PARTICLES;
	sizeInfo.animatedBodiesCount = WALL_NUMBER;
	sizeInfo.geometriesCount = NUMBER_OF_PARTICLES * GEOMETRY_PER_BODY + WALL_NUMBER;
	
	{ //dont need any of these
		sizeInfo.rigidBodiesCount = 0;
		sizeInfo.constraintsCount = 0;
	}

	s32 totalBody = NUMBER_OF_PARTICLES + WALL_NUMBER;
	sizeInfo.overlappedPairsCount = totalBody * (totalBody - 1) / 2;

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

	neV3 position;

	position.SetZero();

	for (s32 j = 0; j < NUMBER_OF_PARTICLES; j++)
	{
		position.Set(0.0f, 2.0f * j + 20.0f, 0.0f);
		//position.Set(13.5f, 20.0f, 1.5f);

		MakeParticle(position, j);	
	}

	SetUpRoom(sim);

	SetupTerrain(sim);
}

void CSample6::MakeParticle(neV3 position, s32 index)
{
	const f32 groundLevel = -10.0f;

	s32 cur;

	cur = index;

	box[cur] = sim->CreateRigidParticle();
	
	neGeometry * geom = box[cur]->AddGeometry();

	neV3 boxSize1; boxSize1.Set(1.f, 1.f, 1.f);
	
	geom->SetBoxSize(boxSize1[0],boxSize1[1],boxSize1[2]);

	box[cur]->UpdateBoundingInfo();
	
	f32 mass = 0.1f;

	box[cur]->SetInertiaTensor(neBoxInertiaTensor(boxSize1[0], boxSize1[1], boxSize1[2], mass));

	box[cur]->SetMass(mass);
	
	box[cur]->SetPos(position);
}

void CSample6::InititialiseTokens()
{
	s32 curToken = 0;

	for (s32 i = 0; i < NUMBER_OF_PARTICLES; i++)
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

			token->SetDrawScale(boxSize);

			boxToken[curToken++] = token;

			geom = box[i]->GetNextGeometry();
		}
	}
}