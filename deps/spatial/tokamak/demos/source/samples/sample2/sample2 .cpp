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

#include "sample2.h"
#include "windows.h"

extern char * PerformanceReportString[];

CSample2 gw;

#define MAX_OVERLAPPED_PAIR 100

#define TIME_STEP (1.0f / 60.0f)

void CSample2::Initialise() 
{
	CLuaValue &titleLUA = CLuaValue("Tokamak demo - 40 rigid bodies connected by hinge joint - (c) 2003 Tokamak Ltd");
		
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

void CSample2::Process(OSTime time)
{
	CSampleBase::Process(time);

	if (GETOS->IsKeyPress('P'))
	{
		paused = !paused;
	}
	s32 actOnBody = N_TOTAL_BODIES - 1;

	if (GETOS->IsKeyPress('Q'))
	{
		neV3 vel;
		
		vel.Set (0.0f, 30.0f, 0.0f);

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

void CSample2::DisplayText()
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

	GETDISPLAY->PrintText(4, 5, "'Q' -> lift the end of the chain");	

	GETDISPLAY->PrintText(0, 8, "This sample demonstrate the hinge joints and joint limits.");

	GETDISPLAY->PrintText(0, 9, "By setting the limit on the joint, each box is constraint to");

	GETDISPLAY->PrintText(0, 10, "rotate within a specific range. In this case the range is");
	
	GETDISPLAY->PrintText(0, 11, "+/- 90 degrees.");
	
	if (counter == 100)
	{
		perfReport.Reset();

		counter = 0;
	}
}

void CSample2::Shutdown() 
{
	for (s32 i = 0; i < N_TOTAL_BODIES; i++)
	{
		if (tokens[i])
			tokens[i]->Free();
	}
	neSimulator::DestroySimulator(sim);
	
	CSampleBase::Shutdown();
}

void CSample2::InititialisePhysics()
{
	neV3 gravity; gravity.Set(0.0f, -9.0f, 0.0f);
	
	f32 linkLength = 0.0f;

	neBool fixedHead = false; // make this true to make chain fix to world
	neBool fixedTail = false;

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidBodiesCount = N_TOTAL_BODIES;
	sizeInfo.animatedBodiesCount = WALL_NUMBER;
	sizeInfo.geometriesCount = N_TOTAL_BODIES + WALL_NUMBER;

	s32 totalBody = N_TOTAL_BODIES + WALL_NUMBER;
	sizeInfo.overlappedPairsCount = totalBody * (totalBody - 1) / 2;

	{ //dont need any of these
		sizeInfo.terrainNodesStartCount = 0;
		sizeInfo.rigidParticleCount = 0;
	}

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

	neRigidBody * lastbox = NULL;

	lastJoint = NULL;

	f32 verticalPos = -5.0f;

	for (s32 j = 0; j < N_BODY; j++)
	{
		neRigidBody * rigidBody = sim->CreateRigidBody();

		rigidBody->CollideConnected(true);

		neGeometry * geom = rigidBody->AddGeometry();

		neV3 boxSize; 

		if (j % 2)
			boxSize.Set(1.0f, 0.5f, 2.0f);
		else
			boxSize.Set(1.0f, 0.5f, 2.2f);

		f32 mass = 0.01f;

		neV3 tensorSize; tensorSize = boxSize;//.Set(1.0f, 0.5f, 1.0f);
		
		geom->SetBoxSize(boxSize[0],boxSize[1],boxSize[2]);

		rigidBody->UpdateBoundingInfo();

		rigidBody->SetInertiaTensor(neBoxInertiaTensor(tensorSize[0], tensorSize[0], tensorSize[0], mass));

		rigidBody->SetMass(mass);

		neV3 pos;

		pos.Set(-0.5f * boxSize[0] - (boxSize[0] + linkLength) * j, verticalPos, 0.0f);

		rigidBody->SetPos(pos);

		rigidBody->SetLinearDamping(0.005f);

		neJoint * joint = NULL;
		
		neT3 jointFrame;

		jointFrame.rot[0].Set(1.0f, 0.0f, 0.0f);
		jointFrame.rot[1].Set(0.0f, 0.0f, -1.0f);
		jointFrame.rot[2].Set(0.0f, 1.0f, 0.0f);

		if (j != 0)
		{
			joint = sim->CreateJoint(rigidBody, lastbox);

			jointFrame.pos.Set(-(boxSize[0] + linkLength * 0.5f) -
								(boxSize[0] + linkLength) * (j - 1), verticalPos, 0.0f);
				
			joint->SetJointFrameWorld(jointFrame);
		}
		else if (fixedHead)
		{
			joint = sim->CreateJoint(rigidBody);

			jointFrame.pos.Set(-(boxSize[0] + linkLength * 0.5f) -
								(boxSize[0] + linkLength) * (j - 1), verticalPos, 0.0f);
				
			joint->SetJointFrameWorld(jointFrame);
		}
		if (j == N_BODY - 1)
		{
			lastJoint = joint;
		}
		if (joint)
		{
			joint->SetType(neJoint::NE_JOINT_HINGE);

			joint->SetJointLength(2.0f);

			joint->Enable(true);

			{
				joint->SetLowerLimit(-NE_PI * 0.5f);

				joint->SetUpperLimit(NE_PI * 0.5f);

				joint->EnableLimit(true);

				joint->SetMotor(neJoint::NE_MOTOR_SPEED, 0.0f, 1000.0f);

				//joint->EnableMotor(true);
			}
		}
		if (fixedTail)
		{ 
			if (j == N_BODY - 1)
			{
				joint = sim->CreateJoint(rigidBody);

				jointFrame.pos.Set(-0.5f * boxSize[0] - (boxSize[0] + linkLength) * j, verticalPos, 0.0f);
					
				joint->SetJointFrameWorld(jointFrame);

				joint->SetType(neJoint::NE_JOINT_HINGE);

				joint->SetJointLength(2.0f);

				joint->Enable(true);

				{
					joint->SetLowerLimit(-NE_PI * 0.5f);

					joint->SetUpperLimit(NE_PI * 0.5f);

					joint->EnableLimit(true);
				}
			}

		}
		/* set up the graphical models */

		neSimpleToken * token = MakeToken(geom);;

		if (j % 2)
		{
			token->SetDiffuseColour(neV4(0.8f, 0.2f, 0.2f, 1.0f));
	
			token->SetAmbientColour(neV4(0.3f, 0.0f, 0.0f, 1.0f));
		}
		else
		{
			token->SetDiffuseColour(neV4(0.2f, 0.2f, 0.8f, 1.0f));
	
			token->SetAmbientColour(neV4(0.0f, 0.2f, 0.3f, 1.0f));
		}
		token->SetDrawScale(boxSize);

		tokens[j] = token;

		geom->SetUserData((u32)token);

		rigidBodies[j] = rigidBody;

		lastbox = rigidBody;
	}
	if (lastJoint)
	{
		lastJoint->SetIteration(4);
	}
	SetUpRoom(sim);
}
