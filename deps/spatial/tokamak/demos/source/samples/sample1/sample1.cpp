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

#include "sample1.h"
#include "windows.h"

CSample1 gw;

#define MAX_OVERLAPPED_PAIR 300

#define CAMERA_DIST 40

#define EPSILON 0.1f

void CSample1::Initialise() 
{
	CLuaValue &titleLUA = CLuaValue("Tokamak demo - chain of 30 boxes connected by ball joints - (c) 2003 Tokamak Ltd");
		
	titleLUA.SetGlobal(GETLUA->GetLuaState(), "WindowTitle");

	CSampleBase::Initialise();

	targetCam.SetCamera(GETDISPLAY->GetCamera());
	
	neV3 tmp; tmp.Set(-10,5,CAMERA_DIST);

	GETDISPLAY->GetCamera()->SetPos(tmp);

	perfReport.Reset();

	perfReport.SetReportType(nePerformanceReport::NE_PERF_RUNNING_AVERAGE);

	InititialisePhysics();

	targetCam.xDir = 0.1f;

	targetCam.yDir = -0.2f;
}

void CSample1::Process(OSTime time)
{
	s32 actOnBody = N_BODY - 1;

	CSampleBase::Process(time);

	if (GETOS->IsKeyPress('P'))
	{
		paused = !paused;
	}
	
	neV3 vel;

	if (GETOS->IsKeyPress('Q'))
	{
		vel.Set (0.0f,30.0f,0.0f);

		box[actOnBody]->SetVelocity(vel);
	}
	////////////////////////////////////////////////////////
	// Display the boxes

	for (s32 i = 0; i < N_BODY; i++)
	{
		if (box[i])
		{
			boxToken[i]->SetMatrix(box[i]->GetTransform());

			boxToken[i]->AddToDisplay();
		}
	}
	counter++;

	if (!paused)
	{
		sim->Advance(1.0f / 60.0f, 1, &perfReport);
	}
}

void CSample1::DisplayText()
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

	GETDISPLAY->PrintText(4, 2, "'P' -> pause/unpause the simulation");	

	GETDISPLAY->PrintText(4, 3, "'A' + 'Z' + mouse + mouse button -> control camera");

	GETDISPLAY->PrintText(4, 4, "'Q' -> lift the end of the chain");	

	GETDISPLAY->PrintText(0, 6, "This sample demonstrate the ball joint.");

	GETDISPLAY->PrintText(0, 7, "Note the box can contact each other and the ground is modelled");

	GETDISPLAY->PrintText(0, 8, "as a box, rather than just a plane.");
	
	if (counter == 100)
	{
		perfReport.Reset();

		counter = 0;
	}
}

void CSample1::Shutdown() 
{
	for (s32 i = 0; i < N_BODY; i++)
	{
		if (boxToken[i])
			boxToken[i]->Free();
	}
	neSimulator::DestroySimulator(sim);
	
	CSampleBase::Shutdown();
}

void CSample1::InititialisePhysics()
{
	neV3 gravity; gravity.Set(0.0f, -8.0f, 0.0f);

	f32 linkLength = 1.2f;

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidBodiesCount = N_BODY;
	sizeInfo.animatedBodiesCount = WALL_NUMBER;
	sizeInfo.geometriesCount = N_BODY + WALL_NUMBER;
	sizeInfo.overlappedPairsCount = MAX_OVERLAPPED_PAIR;

	sizeInfo.controllersCount = 1;
	{ //dont need any of these
		sizeInfo.rigidParticleCount = 0;
		sizeInfo.terrainNodesStartCount = 0;
	}

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

	neRigidBody * lastbox = NULL;

	for (s32 j = 0; j < N_BODY; j++)
	{
		f32 mass = 0.1f;

		neRigidBody * rigidBody = sim->CreateRigidBody();

		rigidBody->CollideConnected(true);
	
		neGeometry * geom = rigidBody->AddGeometry();

		neV3 boxSize; 
		
		boxSize.Set(1.2f, 0.5f, 0.5f);

		geom->SetBoxSize(boxSize[0],boxSize[1],boxSize[2]);

		neV3 tensorSize;

		tensorSize = boxSize;;

		rigidBody->UpdateBoundingInfo();
		
		//rigidBody->SetInertiaTensor(neBoxInertiaTensor(tensorSize, mass));

		rigidBody->SetInertiaTensor(neSphereInertiaTensor(tensorSize[0], mass));

		rigidBody->SetMass(mass);

		neV3 pos;

		if (j == 0)
		{
			pos.Set(-linkLength, 0.0f, 0.0f);
		}
		else if (j != 0)
		{
			pos.Set(-linkLength * (j+1), 0.0f, 0.0f);
		}
		
		rigidBody->SetPos(pos);

		neJoint * joint = NULL;
		
		neT3 jointFrame;

		jointFrame.SetIdentity();

		if (j != 0)
		{
			joint = sim->CreateJoint(rigidBody, lastbox);
			//joint = sim->CreateJoint(lastbox, rigidBody);

			jointFrame.pos.Set(-linkLength * (0.5f + j), 0.0f, 0.0f);

			joint->SetJointFrameWorld(jointFrame);
		}
		if (j == N_BODY - 1)
		{
			lastJoint = joint;
		}

		if (joint)
		{
			joint->SetType(neJoint::NE_JOINT_BALLSOCKET);

			joint->Enable(true);
		}

		/* set up the graphical models */
		{
			neSimpleTokenParam param;

			param.mesh = "cube3.x";

			param.resourceID = "cube";

			param.trans.SetIdentity();

			neSimpleToken * token = (neSimpleToken *)MakeInstance(NE_SIMPLE_TOKEN, &param, true);

			token->SetDiffuseColour(neV4(0.8f, 0.2f, 0.2f, 1.0f));
	
			token->SetAmbientColour(neV4(0.3f, 0.0f, 0.0f, 1.0f));

			token->SetDrawScale(boxSize);

			boxToken[j] = token;
		}
		box[j] = rigidBody;

		lastbox = rigidBody;
	}
	if (lastJoint)
	{
		lastJoint->SetEpsilon(EPSILON);

		lastJoint->SetIteration(5);
	}

	SetUpRoom(sim);
}

