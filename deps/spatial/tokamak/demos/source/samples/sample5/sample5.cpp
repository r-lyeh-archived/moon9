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

#include "sample5.h"
#include "Windows.h"

CSample5 gw;

#define MAX_OVERLAPPED_PAIR 200

#define CAMERA_DIST 20

#define TIME_STEP (1.0f / 60.0f)

void CSample5::Initialise() 
{
	CLuaValue &titleLUA = CLuaValue("Tokamak demo - build-in of breakage function - (c) 2003 Tokamak Ltd");
		
	titleLUA.SetGlobal(GETLUA->GetLuaState(), "WindowTitle");

	CSampleBase::Initialise();

	targetCam.SetCamera(GETDISPLAY->GetCamera());
	
	neV3 tmp; tmp.Set(0,0,CAMERA_DIST);

	GETDISPLAY->GetCamera()->SetPos(tmp);

	targetCam.xDir = -1.0f;

	targetCam.yDir = -0.144f;

	perfReport.Reset();

	perfReport.SetReportType(nePerformanceReport::NE_PERF_RUNNING_AVERAGE);

	InititialisePhysics();

	InititialiseTokens();

	currentBodyCount = NUMBER_OF_TABLES;
}

void CSample5::Process(OSTime time)
{
	CSampleBase::Process(time);

	if (GETOS->IsKeyPress('P'))
	{
		paused = !paused;
	}
	
	s32 actOnBody = NUMBER_OF_TABLES - 1;

	neV3 force;
	
	neBool dontSet = false;

	f32 v = 5.0f;
	
	if (GETOS->IsKeyPress('Q'))
	{
		force.Set (0.0f,v,0.0f);
	}
	else if (GETOS->IsKeyPress(VK_UP))
	{
		force.Set (0.0f,0.0f,-v);
	}
	else if (GETOS->IsKeyPress(VK_DOWN))
	{
		force.Set (0.0f,0.0f,v);
	}
	else if (GETOS->IsKeyPress(VK_LEFT))
	{
		force.Set (-v,0.0f,0.0f);
	}
	else if (GETOS->IsKeyPress(VK_RIGHT))
	{
		force.Set (v,0.0f,0.0f);
	}
	else
	{
		dontSet = true;
	}
	if (!dontSet)
	{
		for (s32 i = 0; i < NUMBER_OF_TABLES; i++)
			rigidBodies[i]->SetVelocity(force);
	}
	////////////////////////////////////////////////////////

	for (s32 i = 0; i < currentBodyCount; i++)
	{
		neT3 trans = rigidBodies[i]->GetTransform();

		rigidBodies[i]->BeginIterateGeometry();

		neGeometry * geom = rigidBodies[i]->GetNextGeometry();

		while (geom)
		{
			u32 index = geom->GetUserData();
			
			neT3 geomTrans = geom->GetTransform();

			neT3 worldTrans = trans * geomTrans;

			boxToken[index]->SetMatrix(worldTrans);

			boxToken[index]->AddToDisplay();

			geom = rigidBodies[i]->GetNextGeometry();
		}
	}
	counter++;

	if (!paused)
	{
		sim->Advance(TIME_STEP, 1, &perfReport);
	}
	//paused = true;
}

void CSample5::DisplayText()
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
	GETDISPLAY->PrintText(4, 4, "'Q' -> Lift tables");

	GETDISPLAY->PrintText(0, 6, "Demonstrate the build in breaking function of Tokamak.");
	GETDISPLAY->PrintText(0, 7, "A rigid body is assemble with any number of geometries.");
	GETDISPLAY->PrintText(0, 8, "Each geometry can be told to break off at a certain");
	GETDISPLAY->PrintText(0, 9, "collision force. Tokamak will automatically spawn the");
	GETDISPLAY->PrintText(0, 10, "new fragment objects when the breaking occurs.");


	if (counter == 100)
	{
		perfReport.Reset();
		
		counter = 0;
	}
}

void CSample5::Shutdown() 
{
	for (s32 i = 0; i < NUMBER_OF_BODIES; i++)
	{
		if (boxToken[i])
			boxToken[i]->Free();
	}
	neSimulator::DestroySimulator(sim);
	
	CSampleBase::Shutdown();
}

void BreakageCallbackFn(neByte * originalBody, neBodyType bodyType, neGeometry * brokenGeometry, neRigidBody * newBody)
{
	gw.rigidBodies[gw.currentBodyCount] = (neRigidBody*)newBody;

	gw.currentBodyCount++;
}

void CSample5::InititialisePhysics()
{
	neV3 gravity; gravity.Set(0.0f, -9.0f, 0.0f);

	f32 linkLength = 0.9f;

	f32 shift = 0.0f;

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidBodiesCount = NUMBER_OF_BODIES;
	sizeInfo.animatedBodiesCount = WALL_NUMBER;
	sizeInfo.geometriesCount = NUMBER_OF_BODIES + WALL_NUMBER;
	s32 totalBody = NUMBER_OF_BODIES + WALL_NUMBER;
	sizeInfo.overlappedPairsCount = MAX_OVERLAPPED_PAIR;//totalBody * (totalBody - 1) / 2;
	{ //dont need any of these
		sizeInfo.rigidParticleCount = 0;
		sizeInfo.constraintsCount = 0;
		sizeInfo.terrainNodesStartCount = 0;
	}

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

	sim->SetBreakageCallback(BreakageCallbackFn);

	for (s32 i = 0; i < NUMBER_OF_BODIES; i++)
	{
		rigidBodies[i] = NULL;
	}

	neV3 position;

	position.SetZero();

	for (s32 j = 0; j < NUMBER_OF_TABLES; j++)
	{
		position.Set(20.0f + 4.0f * j, 30.0f + 5.0f * j, 0.0f);

		MakeTable(position, j);	
	}

	SetUpRoom(sim);
}

struct TableData
{
	neV3 size;
	neV3 pos;
	neV3 colour;
	neGeometry::neBreakFlag breakFlag;
	f32 breakageMass;
	f32 breakageMagnitude;
	f32 breakageAbsorption;
};

TableData tableData[CSample5::GEOMETRY_PER_TABLE] = 
{
	{{3.0f, 0.3f, 3.0f}, {0.0f, 0.0f, 0.0f}, {0.8f, 0.1f, 0.1f}, neGeometry::NE_BREAK_DISABLE, 2.0f, 0.0f, 0.0f},
	{{0.4f, 1.0f, 0.4f}, {1.3f, -0.65f, 1.3f}, {0.1f, 0.1f, 0.8f}, neGeometry::NE_BREAK_NORMAL, 0.4f, 5.0f, 0.3f},
	{{0.4f, 1.0f, 0.4f}, {-1.3f, -0.65f, 1.3f}, {0.1f, 0.1f, 0.8f}, neGeometry::NE_BREAK_NORMAL, 0.4f, 5.0f, 0.3f},
	{{0.4f, 1.0f, 0.4f}, {1.3f, -0.65f, -1.3f}, {0.1f, 0.1f, 0.8f}, neGeometry::NE_BREAK_NORMAL, 0.4f, 5.0f, 0.3f},
	{{0.4f, 1.0f, 0.4f}, {-1.3f, -0.65f, -1.3f}, {0.1f, 0.1f, 0.8f}, neGeometry::NE_BREAK_NORMAL, 0.4f, 5.0f, 0.3f},

};

void CSample5::MakeTable(neV3 position, s32 index)
{
	const f32 groundLevel = -10.0f;

	neV3 tableSize;

	tableSize.Set(3.0f, 1.0f, 3.0f);

	s32 cur;

	cur = index;

	rigidBodies[cur] = sim->CreateRigidBody();

	rigidBodies[cur]->SetInertiaTensor(neBoxInertiaTensor(tableSize, 2.0f));

	rigidBodies[cur]->SetMass(2.0f);

	//position[1] = groundLevel + 0.95f * (1 + cur) + (0.15f * cur);

	rigidBodies[cur]->SetPos(position);

	neQ rot;

	rot.X = ((f32)rand()) /RAND_MAX;
	rot.Y = ((f32)rand()) /RAND_MAX;
	rot.Z = ((f32)rand()) /RAND_MAX;
	rot.W = ((f32)rand()) /RAND_MAX;

	rot.Normalize();

	rigidBodies[cur]->SetRotation(rot);

	for (s32 i = 0; i < GEOMETRY_PER_TABLE; i++)
	{
		neGeometry * geom = rigidBodies[cur]->AddGeometry();

		geom->SetUserData((u32) (cur * GEOMETRY_PER_TABLE + i));

		geom->SetBoxSize(tableData[i].size);

		neT3 trans;

		trans.SetIdentity();

		trans.pos = tableData[i].pos;
		
		geom->SetTransform(trans);

		geom->SetBreakageFlag(tableData[i].breakFlag);

		geom->SetBreakageMass(tableData[i].breakageMass);

		geom->SetBreakageInertiaTensor(neBoxInertiaTensor(tableData[i].size, tableData[i].breakageMass));

		geom->SetBreakageMagnitude(tableData[i].breakageMagnitude);

		geom->SetBreakageAbsorption(tableData[i].breakageAbsorption);

		neV3 plane;

		plane.Set(0.0f, 1.0f, 0.0f);

		geom->SetBreakagePlane(plane);
	}
	rigidBodies[cur]->UpdateBoundingInfo();
}

void CSample5::InititialiseTokens()
{
	s32 curToken = 0;

	for (s32 i = 0; i < NUMBER_OF_TABLES; i++)
	{
		rigidBodies[i]->BeginIterateGeometry();

		neGeometry * geom = rigidBodies[i]->GetNextGeometry();

		s32 j = 0;

		while (geom)
		{
			neV3 boxSize;

			geom->GetBoxSize(boxSize);

			neSimpleTokenParam param;

			param.mesh = "cube3.x";

			param.resourceID = "cube";

			param.trans.SetIdentity();

			neSimpleToken * token = (neSimpleToken *)MakeInstance(NE_SIMPLE_TOKEN, &param, true);

			neV4 colour;

			if (i % 3 == 0)
			{
				colour = neV4(0.8f, 0.2f, 0.2f, 1.0f);
			}
			else if (i % 3 == 1)
			{
				colour = neV4(0.3f,	0.3f, 1.0f, 1.0f);
			}
			else
			{
				colour = neV4(0.2f,	0.8f, 0.2f, 1.0f);
			}

			token->SetDiffuseColour(colour);

			colour *= 0.3f;

			token->SetAmbientColour(colour);

			token->SetDrawScale(boxSize);

			boxToken[curToken++] = token;

			geom = rigidBodies[i]->GetNextGeometry();

			j++;
		}
	}
}
