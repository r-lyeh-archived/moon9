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

#include "sample10.h"
#include "windows.h"

extern char * PerformanceReportString[];

CSample10 gw;

#define MAX_OVERLAPPED_PAIR 100

#define TIME_STEP (1.0f / 60.0f)

void CSample10::Initialise() 
{
	CLuaValue &titleLUA = CLuaValue("Tokamak demo - Hinge joint with Motor turned on - (c) 2003 Tokamak Ltd");

	titleLUA.SetGlobal(GETLUA->GetLuaState(), "WindowTitle");

	CSampleBase::Initialise();

	targetCam.SetCamera(GETDISPLAY->GetCamera());
	
	neV3 tmp; tmp.Set(-16.5f, 0.26f, 21.0f);

	GETDISPLAY->GetCamera()->SetPos(tmp);

	GETOS->FrameLock(false);

	perfReport.Reset();

	perfReport.SetReportType(nePerformanceReport::NE_PERF_RUNNING_AVERAGE);

	//perfReport.SetReportType(nePerformanceReport::NE_PERF_SAMPLE);

	lastJoint = NULL;

	for (s32 i = 0; i < N_TOTAL_BODIES; i++)
	{
		rigidBodies[i] = NULL;
		tokens[i] = NULL;
	}

	InititialisePhysics();

	targetCam.xDir = -0.26f;

	targetCam.yDir = -0.316f;
}

void CSample10::Process(OSTime time)
{
	static neBool pressed = false;

	CSampleBase::Process(time);

	if (GETOS->IsKeyPress('P'))
	{
		paused = !paused;
	}
	s32 actOnBody = 0;//N_TOTAL_BODIES - 1;// -20;

	if (GETOS->IsKeyPress('Q'))
	{
		pressed = true;

		neV3 vel;
		
		vel.Set (0.0f, 30.0f, 0.0f);

		//vel.Set (0.0f, 60.0f, 0.0f);

		rigidBodies[actOnBody]->SetVelocity(vel);
	}
	else if (GETOS->IsKeyPress('W'))
	{
		neV3 vel;
		
		vel.Set (0.0f, -20.0f, 0.0f);

		rigidBodies[actOnBody]->SetForce(vel);
	}
	else
	{
		neV3 vel;
		
		vel.Set (0.0f);

		//rigidBodies[actOnBody]->SetForce(vel);
	}
	if (0)
	{
		neV3 vel;
		
		vel.Set (0.0f, 30.0f, 0.0f);

		//vel.Set (0.0f, 60.0f, 0.0f);

		rigidBodies[actOnBody]->SetVelocity(vel);
	}
	////////////////////////////////////////////////////////
	// Display the boxes

	DisplayRigidBodies(rigidBodies, N_TOTAL_BODIES);

	counter++;

	if (!paused)
	{
		sim->Advance(TIME_STEP, 1, &perfReport);
	}
}

void CSample10::DisplayText()
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

	GETDISPLAY->PrintText(0, 2, "Controls:");	

	GETDISPLAY->PrintText(4, 3, "'P' -> pause/unpause the simulation");	

	GETDISPLAY->PrintText(4, 4, "'A' + 'Z' + mouse + mouse button -> control camera");

	GETDISPLAY->PrintText(4, 5, "'Q' -> lift");	

	GETDISPLAY->PrintText(0, 8, "This sample demonstrate the hinge joint motor.");
	
	if (counter == 100)
	{
		perfReport.Reset();

		counter = 0;
	}
}

void CSample10::Shutdown() 
{
	for (s32 i = 0; i < N_TOTAL_BODIES; i++)
	{
		if (tokens[i])
			tokens[i]->Free();
	}
	neSimulator::DestroySimulator(sim);
	
	CSampleBase::Shutdown();
}

void CSample10::InititialisePhysics()
{
	f32 ypos = -7;

	neV3 gravity; gravity.Set(0.0f, -9.0f, 0.0f);
	//neV3 gravity; gravity.Set(-9.0f, 0.0f, 0.0f);

	//f32 linkLength = 2.2f;
	f32 linkLength = -0.0f;

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidBodiesCount = N_TOTAL_BODIES;
	sizeInfo.animatedBodiesCount = WALL_NUMBER;
	sizeInfo.geometriesCount = N_TOTAL_BODIES + WALL_NUMBER;
	sizeInfo.overlappedPairsCount = 2000;//MAX_OVERLAPPED_PAIR;

	{ //dont need any of these
		sizeInfo.terrainNodesStartCount = 0;
		sizeInfo.rigidParticleCount = 0;
	}

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

	//create the kart body

	f32 mass = 1.0f;

	rigidBodies[0] = sim->CreateRigidBody();

	rigidBodies[0]->SetMass(mass);

	neGeometry * geom = rigidBodies[0]->AddGeometry();

	neV3 boxSize; boxSize.Set(3.0f, 3.f, 1.0f);

	geom->SetBoxSize(boxSize);

	rigidBodies[0]->UpdateBoundingInfo();

	rigidBodies[0]->SetInertiaTensor(neBoxInertiaTensor(boxSize, mass));

	neV3 pos; pos.Set(0.0f, ypos, 0.0f);

	rigidBodies[0]->SetPos(pos);

	rigidBodies[0]->SetSleepingParameter(.0f);

	//create the 4 wheels (sphere)

	s32 j = 1;

	ypos -= 2.0f;

	for (int i = 0; i < 4; i++)
	{
		neRigidBody * rb;
		
		rigidBodies[j++] = rb = sim->CreateRigidBody();

		rb->SetMass(mass);

		geom = rb->AddGeometry();

		geom->SetBoxSize(2.0f, .5f, 1.0f);//SetSphereDiameter(1.0f);

		//geom->SetSphereDiameter(1.0f);

		rb->UpdateBoundingInfo();

		//rb->SetInertiaTensor(neSphereInertiaTensor(1.0f, 0.1f));

		rb->SetInertiaTensor(neBoxInertiaTensor(2.0f, .5f, 1.0f, mass));

		neV3 pos;

		switch(i)
		{
		case 0: pos.Set(-1.5f, ypos, 1.0f);
			break;
		case 1: pos.Set(-1.5f, ypos, -1.0f);
			break;
		case 2: pos.Set(1.5f, ypos, 1.0f);
			break;
		case 3: pos.Set(1.5f, ypos, -1.0f);
			break;
		}
		rb->SetPos(pos);

		neJoint * joint = sim->CreateJoint(rigidBodies[0], rb);

		joint->SetIteration(1);

		neT3 jointFrame;

		jointFrame.rot[0].Set(1.0f, 0.0f, 0.0f);
		jointFrame.rot[1].Set(0.0f, 0.0f, -1.0f);
		jointFrame.rot[2].Set(0.0f, 1.0f, 0.0f);
		jointFrame.pos = pos;
		
		joint->SetJointFrameWorld(jointFrame);
		joint->SetType(neJoint::NE_JOINT_HINGE);
		joint->Enable(true);
		joint->EnableMotor(true);
		joint->SetMotor(neJoint::NE_MOTOR_SPEED, 1.f, 50.f);
	}
	for (i = 0; i < N_BODY; i++)
	{
		rigidBodies[i]->BeginIterateGeometry();
		
		neGeometry * geom = rigidBodies[i]->GetNextGeometry();

		geom->SetUserData((u32)MakeToken(geom));
	}
	SetUpRoom(sim);
}
