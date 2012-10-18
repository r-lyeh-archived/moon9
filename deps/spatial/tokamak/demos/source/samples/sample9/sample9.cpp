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

#include "sample9.h"
#include "Windows.h"

CSample9 gw;

#define TIME_STEP (1.0f / 60.0f)

enum
{
	BONE_BODY,
	BONE_HEAD,
	BONE_RIGHT_ARM,
	BONE_LEFT_ARM,
	BONE_RIGHT_FOREARM,
	BONE_LEFT_FOREARM,
	BONE_RIGHT_THIGH,
	BONE_LEFT_THIGH,
	BONE_RIGHT_LEG,
	BONE_LEFT_LEG,
};

void CSample9::Initialise() 
{
	CLuaValue &titleLUA = CLuaValue("Tokamak demo - Rad Dude hits the wall - (c) 2003 Tokamak Ltd");
		
	titleLUA.SetGlobal(GETLUA->GetLuaState(), "WindowTitle");

	CSampleBase::Initialise();

	perfReport.Reset();

	perfReport.SetReportType(nePerformanceReport::NE_PERF_RUNNING_AVERAGE);

	targetCam.SetCamera(GETDISPLAY->GetCamera());
	
	neV3 tmp; tmp.Set(-7.2f,-3.7f,34.0f);

	targetCam.cam->SetPos(tmp);

	targetCam.xDir = -0.3f;

	targetCam.yDir = -0.12f;

	Reset();
}

void CSample9::Reset()
{
	CSampleBase::Reset();

	InititialisePhysics();

	InititialiseTokens();

	GETDISPLAY->ClearDisplayList();
}

void CSample9::Free()
{
	CSampleBase::Free();

	for (s32 i = 0; i < N_BODY; i++)
	{
		if (tokens[i])
		{
			tokens[i]->Free();

			tokens[i] = NULL;
		}
	}
	if (sim)
	{
		neSimulator::DestroySimulator(sim);

		sim = NULL;
	}
}

f32 fps;

void CSample9::Process(OSTime time)
{
	float fric, resti;

	sim->GetMaterial(0, fric, resti);

	static DWORD lastTick = 0;

	DWORD thisTick = timeGetTime();

	int delta = thisTick - lastTick;

	if (lastTick == 0)
		fps = 0.0f;
	else
		fps = 1000.0 / (f32)delta ;

	lastTick = thisTick;
	
	CSampleBase::Process(time);

	if (GETOS->IsKeyPress('R'))
	{
		Reset();
	}
	if (GETOS->IsKeyPress('P'))
	{
		paused = !paused;
	}
	s32 actOnBody = 0;

	neV3 vel; 

	if (GETOS->IsKeyPress('Q'))
	{
		vel.Set (0.0f,0.0f,3.0f);

		rigidBodies[actOnBody]->SetVelocity(vel);
	}
	else if (GETOS->IsKeyPress('W'))
	{
		vel.Set (0.0f,10.0f,0.0f);

		rigidBodies[actOnBody]->SetVelocity(vel);
	}

	////////////////////////////////////////////////////////

	DisplayRigidBodies(rigidBodies, N_BODY);

	DisplayAnimatedBodies(steps, N_STEP);

	counter++;

	if (!paused)
	{
		sim->Advance(TIME_STEP, 1, &perfReport);
	}
}

void CSample9::DisplayText()
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
	GETDISPLAY->PrintText(4, 3, "'R' -> Reset");
	GETDISPLAY->PrintText(4, 4, "'A' + 'Z' + mouse + mouse button -> control camera");
	GETDISPLAY->PrintText(4, 5, "'Q' -> Push Rad dude");
	GETDISPLAY->PrintText(4, 6, "'W' -> Lift Rad dude");
	GETDISPLAY->PrintText(0, 8, "Rag dolls and Stacks");

	if (counter == 100)
	{
		perfReport.Reset();
		
		counter = 0;
	}
}

void CSample9::Shutdown() 
{
	Free();
	
	CSampleBase::Shutdown();
}

void CSample9::InititialisePhysics()
{
	neV3 gravity; gravity.Set(0.0f, -9.0f, 0.0f);

	f32 linkLength = 0.9f;

	f32 shift = 0.0f;

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidBodiesCount = N_BODY;
	sizeInfo.animatedBodiesCount = WALL_NUMBER + N_STEP;
	sizeInfo.geometriesCount = N_BODY + WALL_NUMBER + N_STEP;
	sizeInfo.constraintsCount = N_BODY * JOINTS_PER_DUDE;
	s32 totalBody = N_BODY + WALL_NUMBER;
	sizeInfo.overlappedPairsCount = totalBody * (totalBody - 1) / 2;

	{ //dont need any of these
		sizeInfo.rigidParticleCount = 0;
		//sizeInfo.terrainNodesStartCount = 0;
	}

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

	neV3 position;

	position.SetZero();

	s32 i = 0;

	for (s32 j = 0; j < N_DUDE; j++)
	{
		position.Set(0.0f, 11.0f + 2.5f * j, 0.0f);
		
		MakeRadDude(position, i);	
	}

	f32 gap = 0.9f;

	for (j = 0; j < N_STACKS; j++)
	{
		for (s32 k = 0; k < N_DEPTH; k++)
		{
			position.Set(gap * j - 3, 0.0f, gap * k  + 17.0f);

			MakeStack(position, i);	
		}
	}

	MakeStair();
	
	SetUpRoom(sim);
}

#define RAD_DUDE_BOX 0
#define RAD_DUDE_SPHERE 1
#define RAD_DUDE_CYLINDER 2

struct BoneData
{
	s32 geometryType;
	f32 zRotation;
	neV3 pos;
	neV3 size;
	neV3 colour;
};

BoneData bones[] = 
{
	{RAD_DUDE_BOX,		0.0f, {0.0f, 0.0f, 0.0f}, {0.55f, 0.7f, 0.3f}, {0.8f, 0.2f, 0.2f}}, //body 		
	{RAD_DUDE_SPHERE,	0.0f, {0.0f, 0.55f, 0.0f}, {0.4f, 0.35f, 0.2f}, {0.8f, 0.8f, 0.2f}}, //head 
	
	{RAD_DUDE_CYLINDER, -NE_PI / 2.0f, {-0.45f, 0.28f, 0.0f}, {0.25f, 0.4f, 0.2f}, {0.2f, 0.2f, 0.8f}}, //right arm 
	{RAD_DUDE_CYLINDER, NE_PI / 2.0f, {0.45f, 0.28f, 0.0f}, {0.25f, 0.4f, 0.2f}, {0.2f, 0.2f, 0.8f}}, //left arm

	{RAD_DUDE_BOX, -NE_PI / 2.0f, {-0.9f, 0.28f, 0.0f}, {0.24f, 0.6f, 0.24f}, {0.2f, 0.2f, 0.8f}}, //right forearm
	{RAD_DUDE_BOX, NE_PI / 2.0f, {0.9f, 0.28f, 0.0f}, {0.24f, 0.6f, 0.24f}, {0.2f, 0.2f, 0.8f}}, //left forearm

	{RAD_DUDE_CYLINDER, 0.0f, {-0.20f, -0.6f, 0.0f}, {0.27f, 0.7f, 0.2f}, {0.3f, 0.7f, 0.2f}}, //right thigh 
	{RAD_DUDE_CYLINDER, 0.0f, {0.20f, -0.6f, 0.0f}, {0.27f, 0.7f, 0.2f}, {0.3f, 0.7f, 0.2f}}, //left thigh 
	
	{RAD_DUDE_BOX, 0.0f, {-0.20f, -1.3f, 0.0f}, {0.3f, 0.8f, 0.3f}, {0.3f, 0.7f, 0.2f}}, //right leg
	{RAD_DUDE_BOX, 0.0f, {0.20f, -1.3f, 0.0f}, {0.3f, 0.8f, 0.3f}, {0.3f, 0.7f, 0.2f}}, //left leg
};

struct JointData
{
	s32 bodyA;
	s32 bodyB;
	neV3 pos;
	s32 type; // 0 = ball joint, 1 = hinge joint
	f32 xAxisAngle;
	f32 lowerLimit;
	f32 upperLimit;
	neBool enableLimit;
	neBool enableTwistLimit;
	f32 twistLimit;
};

JointData joints[] = 
{
	{BONE_HEAD, BONE_BODY,				{0.0f, 0.35f, 0.0f}, 1, 0.0f, -NE_PI / 4.0f, NE_PI / 4.0f, true, false, 0.0f}, //head <-> body
	
	{BONE_RIGHT_ARM, BONE_BODY,			{-0.22f, 0.28f, 0.0f}, 0, NE_PI, 0.0f, NE_PI / 2.5f, true, true, 0.1f}, //
	{BONE_LEFT_ARM, BONE_BODY,			{0.22f, 0.28f, 0.0f}, 0, 0.0f, 0.0f, NE_PI / 2.5f, true, true, 0.1f},

	{BONE_RIGHT_FOREARM, BONE_RIGHT_ARM,{-0.65f, 0.28f, 0.0f}, 1, NE_PI, 0.0f, NE_PI / 2.0f, true, false},
	{BONE_LEFT_FOREARM, BONE_LEFT_ARM,	{0.65f, 0.28f, 0.0f}, 1, 0.0f, 0.0f, NE_PI / 2.0f, true, false},

	{BONE_RIGHT_THIGH, BONE_BODY,		{-0.20f, -0.32f, 0.0f}, 0, NE_PI * 6.0f / 8.0f, 0.0f, NE_PI / 4.0f, true, true, 0.8f},
	{BONE_LEFT_THIGH, BONE_BODY,		{0.20f, -0.32f, 0.0f}, 0, NE_PI * 2.0f / 8.0f, 0.0f, NE_PI / 4.0f, true, true, 0.8f},

	{BONE_RIGHT_LEG, BONE_RIGHT_THIGH,	{-0.20f, -0.95f, 0.0f}, 1, -NE_PI * 0.5f, -NE_PI / 2.0f, 0.0f, true, false},
	{BONE_LEFT_LEG, BONE_LEFT_THIGH,	{0.20f, -0.95f, 0.0f}, 1, -NE_PI * 0.5f, -NE_PI / 2.0f, 0.0f, true, false},
};


void CSample9::MakeRadDude(neV3 position, s32 & index)
{
	const f32 groundLevel = -10.0f;

	s32 cur;

	f32 scale = 1.0f;

	for (s32 i = 0; i < BONES_PER_DUDE; i++)
	{
		cur = index + i;

		rigidBodies[cur] = sim->CreateRigidBody();

		rigidBodies[cur]->CollideConnected(true);

		neV3 inertiaTensor;

		f32 mass;
		
		mass = 8.0f;

		if (i == 0)
			mass = 20.0f;
		else if (i == 8 || i ==9)
			mass = 8.0f;

		neGeometry * geom = rigidBodies[cur]->AddGeometry();

		switch (bones[i].geometryType)
		{
		case RAD_DUDE_BOX:
			
			geom->SetBoxSize(bones[i].size[0] * scale, bones[i].size[1] * scale, bones[i].size[2] * scale);

			inertiaTensor = neBoxInertiaTensor(bones[i].size[0]* scale, 
													bones[i].size[1]* scale, 
													bones[i].size[2]* scale, mass);

			break;

		case RAD_DUDE_SPHERE:

			geom->SetSphereDiameter(bones[i].size[0] * scale);

			inertiaTensor = neSphereInertiaTensor(bones[i].size[0]* scale, mass);
			break;

		case RAD_DUDE_CYLINDER:

			geom->SetCylinder(bones[i].size[0] * scale, bones[i].size[1] * scale);

			inertiaTensor = neCylinderInertiaTensor(bones[i].size[0], bones[i].size[1], mass);
			
			break;
		}

		rigidBodies[cur]->UpdateBoundingInfo();

		rigidBodies[cur]->SetInertiaTensor(inertiaTensor);

		rigidBodies[cur]->SetMass(mass);

		neV3 pos;
		
		pos = bones[i].pos * scale+ position;

		rigidBodies[cur]->SetPos(pos);

		neQ quat;

		neV3 zAxis; zAxis.Set(0.0f, 0.0f, 1.0f);

		quat.Set(bones[i].zRotation, zAxis);

		rigidBodies[cur]->SetRotation(quat);
	}
	neJoint * joint;
		
	neT3 jointFrame;

	for (i = 0; i < JOINTS_PER_DUDE; i++)
	{
		joint = sim->CreateJoint(rigidBodies[joints[i].bodyA + index], rigidBodies[joints[i].bodyB + index]);

		jointFrame.SetIdentity();

		jointFrame.pos = joints[i].pos * scale + position;
			
		if (joints[i].type == 0)
		{
			joint->SetType(neJoint::NE_JOINT_BALLSOCKET);

			neQ q;

			neV3 zAxis; zAxis.Set(0.0f, 0.0f, 1.0f);

			q.Set(joints[i].xAxisAngle, zAxis);

			jointFrame.rot = q.BuildMatrix3();
		}
		else
		{
			joint->SetType(neJoint::NE_JOINT_HINGE);

			if (i == 3)
			{
				jointFrame.rot[0].Set(1.0f, 0.0f, 0.0f);
				jointFrame.rot[1].Set(0.0f, 1.0f, 0.0f);
				jointFrame.rot[2].Set(0.0f, 0.0f, 1.0f);
			}
			else if (i == 4)
			{
				jointFrame.rot[0].Set(-1.0f, 0.0f, 0.0f);
				jointFrame.rot[1].Set(0.0f, -1.0f, 0.0f);
				jointFrame.rot[2].Set(0.0f, 0.0f, 1.0f);
			}
			else
			{
				jointFrame.rot[0].Set(0.0f, 0.0f, -1.0f);
				jointFrame.rot[1].Set(-1.0f, 0.0f, 0.0f);
				jointFrame.rot[2].Set(0.0f, 1.0f, 0.0f);
			}
		}

		joint->SetJointFrameWorld(jointFrame);
		
		if (i == 5|| i == 6) // right
		{
			neT3 body2w = rigidBodies[joints[i].bodyB+index]->GetTransform();

			neT3 w2Body = body2w.FastInverse();

			neM3 m;

			neQ q1, q2;

			neV3 zAxis; zAxis.Set(0.0f, 0.0f, 1.0f);

			q1.Set(joints[i].xAxisAngle, zAxis);

			neV3 xAxis; xAxis.Set(1.0f, 0.0f, 0.0f);

			q2.Set(-NE_PI * 0.30f, xAxis);

			neQ q; q = q2 * q1;

			m = q.BuildMatrix3();

			neT3 frame2body;

			frame2body.rot = w2Body.rot * m;

			frame2body.pos = w2Body * jointFrame.pos;

			//neT3 frame2w = body2w * frame2body;

			joint->SetJointFrameB(frame2body);
		}
		joint->SetLowerLimit(joints[i].lowerLimit);

		joint->SetUpperLimit(joints[i].upperLimit);

		if (joints[i].enableLimit)
			joint->EnableLimit(true);

		if (joints[i].enableTwistLimit)
		{
			joint->SetLowerLimit2(joints[i].twistLimit);

			joint->EnableLimit2(true);
		}

		joint->Enable(true);

	}
	index = cur+1;
}

void CSample9::MakeStair()
{
	neM3 rot;
	neV3 r;

	r.Set(0.4f, 0.0f, 0.0f);

	rot.RotateXYZ(r);

	for (s32 i = 0; i < N_STEP; i++)
	{
		steps[i] = sim->CreateAnimatedBody();

		neGeometry * geom = steps[i]->AddGeometry();

		const f32 stepHeight = 0.5f;//1.2f;

		const f32 stepDepth = 3.f;//1.2f;

		neV3 stepSize; stepSize.Set(30.0f, stepHeight, stepDepth);

		geom->SetBoxSize(stepSize);

		steps[i]->UpdateBoundingInfo();

		neV3 pos;

		pos.Set(0.0f, 2.5f - stepHeight * i * 4.f, -1.0f + stepDepth * i * 0.8f);

		steps[i]->SetPos(pos);

		steps[i]->SetRotation(rot);
	}
}

void CSample9::MakeStack(neV3 position, s32 & index)
{
	const f32 groundLevel = -10.0f;

	s32 cur;

	for (s32 i = 0; i < STACK_HEIGHT; i++)
	{
		cur = index + i;

		rigidBodies[cur] = sim->CreateRigidBody();

		neGeometry * geom = rigidBodies[cur]->AddGeometry();

		neV3 boxSize1; boxSize1.Set(0.8f, 0.8f, 0.5f);
		
		geom->SetBoxSize(boxSize1[0],boxSize1[1],boxSize1[2]);

		rigidBodies[cur]->UpdateBoundingInfo();
		
		f32 mass = 3.0f;

		rigidBodies[cur]->SetInertiaTensor(neBoxInertiaTensor(boxSize1[0], boxSize1[1], boxSize1[2], mass));

		rigidBodies[cur]->SetMass(mass);
		
		neV3 pos;

		pos.Set(position[0], groundLevel + boxSize1[1] * (i + 0.5f) + 0.0f, position[2]);

		rigidBodies[cur]->SetPos(pos);
	}
	index = cur+1;
}

void CSample9::InititialiseTokens()
{
	s32 curToken = 0;

	int curDude = 0;

	for (s32 i = 0; i < N_BODY; i++)
	{
		rigidBodies[i]->BeginIterateGeometry();

		bool isBlock = false;

		if (i >= (BONES_PER_DUDE * N_DUDE))
			isBlock = true;

		neGeometry * geom = rigidBodies[i]->GetNextGeometry();

		s32 b = 0;

		while (geom)
		{
			{
				neSimpleToken * token = MakeToken(geom);

				geom->SetUserData((u32)token);

				if (!isBlock)
				{
					s32 boneNumber = i % BONES_PER_DUDE;

					token->SetDiffuseColour(neV4(bones[boneNumber].colour[0], 
												bones[boneNumber].colour[1], 
												bones[boneNumber].colour[2], 
												1.0f));

					token->SetAmbientColour(neV4(bones[boneNumber].colour[0] * 0.5f , 
												bones[boneNumber].colour[1] * 0.5f, 
												bones[boneNumber].colour[2] * 0.5f, 
												1.0f));
				}
				else
				{
					token->SetDiffuseColour(neV4(0.3f, 0.3f, 0.5f, 1.0f));

					token->SetAmbientColour(neV4(0.0f, 0.0f, 0.1f, 1.0f));
				}

				tokens[curToken++] = token;
			}
			geom = rigidBodies[i]->GetNextGeometry();

			b++;
		}
	}
	for (i = 0; i < N_STEP; i++)
	{
		steps[i]->BeginIterateGeometry();

		neGeometry * geom = steps[i]->GetNextGeometry();

		s32 b = 0;

		while (geom)
		{
			{
				neSimpleToken * token = MakeToken(geom);

				geom->SetUserData((u32)token);

//				token->SetDiffuseColour(neV4(0.4f, 0.5f, 0.3f, 1.0f));

//				token->SetAmbientColour(neV4(0.3f ,0.3f, 0.2f, 1.0f));

				token->SetDiffuseColour(neV4(0.2f, 0.2f, 0.3f, 1.0f));

				token->SetAmbientColour(neV4(0.2f ,0.1f, 0.1f, 1.0f));

				tokens[curToken++] = token;
			}
			geom = steps[i]->GetNextGeometry();

			b++;
		}
	}
}

