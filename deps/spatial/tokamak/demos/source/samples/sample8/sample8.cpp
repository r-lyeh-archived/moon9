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

#include "sample8.h"
#include "windows.h"

CSample8 gw;

CControllerCB cb;

#define MAX_OVERLAPPED_PAIR 1000

#define CAMERA_DIST 60

#define WHEEL_DIAMETER 0.9f

#define WHEEL_WIDTH 0.3f

#define MAX_SPEED 5.0f

#define MAX_REVERSE_SPEED -2.0f

#define MAX_STEER 0.7f

#define MAX_SLIDE 0.8f

struct SensorData
{
	neV3 pos;
};

SensorData sensorData[4] = 
{
	{2.0f, 0.0f, 1.5f},
	{2.0f, 0.0f, -1.5f},
	{-2.0f, 0.0f, 1.5f},
	{-2.0f, 0.0f, -1.5f},
};

void CSample8::Initialise() 
{
	CLuaValue &titleLUA = CLuaValue("Tokamak demo - simple car demo - (c) 2003 Tokamak Ltd");
	
	titleLUA.SetGlobal(GETLUA->GetLuaState(), "WindowTitle");

	CSampleBase::Initialise();

	targetCam.SetCamera(GETDISPLAY->GetCamera());
	
	neV3 tmp; tmp.Set(0,5,CAMERA_DIST);

	GETDISPLAY->GetCamera()->SetPos(tmp);

	perfReport.Reset();

	perfReport.SetReportType(nePerformanceReport::NE_PERF_RUNNING_AVERAGE);

	InititialisePhysics();

	targetCam.xDir = 0.1f;

	targetCam.yDir = -0.2f;

	for (int i = 0; i < N_CARS; i++)
	{
		car[i].gameWorld = this;

		LoadWheelModels(i);
	}
}

void CSample8::Process(OSTime time)
{
	CSampleBase::Process(time);

	{
		neV3 up, at;

		up.Set(0.0f, 1.0f, 0.0f);

		at = car[TARGET_CAR].carRigidBody->GetPos();

		at[1] = -10.0f;

		targetCam.cam->SetViewMatrixMethod1(targetCam.cam->GetPos(),
											at,
											up);
	}

	if (GETOS->IsKeyPress('P'))
	{
		paused = !paused;
	}

	car[TARGET_CAR].Process();
	////////////////////////////////////////////////////////
	// Display the boxes

	for (int i = 0; i < N_CARS; i++)
	{
		DisplayRigidBodies(&car[i].carRigidBody, 1);

		DisplayRigidBodies(&car[i].carParts[0], CSampleCar::N_PARTS);
	}

	DisplayAnimatedBodies(&ramp, 1);

	neV3 colour; colour.Set(1.0f);


	for (int j = 0; j < N_CARS; j++)
	{
		for (i = 0; i < 4; i++)
		{
			//GETDISPLAY->DrawLine(2, colour, &displayLines[i][0]);

			neT3 wheel2World, suspension2Body, tire2Suspension, cylinder2Tire;

			suspension2Body.SetIdentity();

			suspension2Body.pos.Set(0.0f, -car[j].suspensionLength[i] + WHEEL_DIAMETER / 2.0f, 0.0f);

			suspension2Body.pos += sensorData[i].pos;

			cylinder2Tire.SetIdentity();

			neV3 rot; rot.Set(NE_PI * 0.5f, 0.0f, 0.0f);

			cylinder2Tire.rot.RotateXYZ(rot);

			tire2Suspension.SetIdentity();

			if (i == 0 || i == 1)
			{
				rot.Set(0.0f, car[j].steer, 0.0f);

				tire2Suspension.rot.RotateXYZ(rot);
			}

			wheel2World = car[j].carRigidBody->GetTransform() * suspension2Body * tire2Suspension * cylinder2Tire;

			if (i == 0 || i == 1)
			{
				car[j].steerDir = wheel2World.rot[0];
			}

			car[j].wheelToken[i]->SetMatrix(wheel2World);

			car[j].wheelToken[i]->AddToDisplay();
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
		sim->Advance(1.0f / 60.0f, 1, &perfReport);
	}
}

void CSample8::DisplayText()
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

	GETDISPLAY->PrintText(4, 4, "'Q' -> lift the car");	

	GETDISPLAY->PrintText(4, 5, "'S' + 'X' -> accel/decel car");	

	GETDISPLAY->PrintText(4, 6, "'K' + 'L' -> steer car");	

	GETDISPLAY->PrintText(0, 7, "This sample demonstrate the simple implementation of a car.");

	GETDISPLAY->PrintText(0, 8, "The wheels are modellded as a ray-cast sensors with controller callback.");

	GETDISPLAY->PrintText(0, 9, "A bonnet and doors are added with hinge joint to make it abit more interesting.");
	
	if (counter == 100)
	{
		perfReport.Reset();

		counter = 0;
	}
}

void CSample8::Shutdown() 
{
	for (s32 j = 0; j < N_CARS; j++)
	{
		for (s32 i = 0; i < CSampleCar::N_TOKENS; i++)
		{
			if (car[j].carToken[i])
				car[j].carToken[i]->Free();
		}
	}

	neSimulator::DestroySimulator(sim);
	
	CSampleBase::Shutdown();
}

void CSample8::InititialisePhysics()
{
	neV3 gravity; gravity.Set(0.0f, -8.0f, 0.0f);

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidBodiesCount = (CSampleCar::N_PARTS + 1) * N_CARS;
	sizeInfo.animatedBodiesCount = WALL_NUMBER + 1;
	sizeInfo.geometriesCount = (CSampleCar::N_TOKENS + 1) * N_CARS + WALL_NUMBER;
	sizeInfo.overlappedPairsCount = MAX_OVERLAPPED_PAIR;
	{ //dont need any of these
		sizeInfo.rigidParticleCount = 0;
		sizeInfo.terrainNodesStartCount = 200;
	}

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

	neV3 carPos;

	carPos.Set(0.0f, -1.0f, 10.0f);

	for (int i = 0; i < N_CARS; i++)
	{
		car[i].id = i;
		
		car[i].MakeCar(sim, carPos);

		carPos[0] -= 7;
	}

	SetUpRoom(sim);

	ramp = sim->CreateAnimatedBody();

	neGeometry * geom = ramp->AddGeometry();

	geom->SetBoxSize(10.0f, 3.0f, 40.0f);

	ramp->UpdateBoundingInfo();

	rampToken = MakeToken(geom);

	geom->SetUserData((u32)rampToken);

	neT3 trans;

	neV3 tmp; tmp.Set(0.0f, 0.0f, NE_PI * 0.1f);

	trans.rot.RotateXYZ(tmp);

	trans.pos.Set(20.0f, -10.0f, 0.0f);

	ramp->SetPos(trans.pos);

	ramp->SetRotation(trans.rot);

	//SetupTerrain(sim);
}

void CSample8::LoadWheelModels(s32 whichCar)
{
	for (s32 i = 0; i < 4; i++)
	{
		neSimpleTokenParam param;

		neSimpleToken * token;

		neV3 scale;

		param.mesh = "cylinder2.x";

		param.resourceID = "cylinder";

		token = (neSimpleToken *)MakeInstance(NE_SIMPLE_TOKEN, &param, true);

		scale.Set(WHEEL_DIAMETER, WHEEL_WIDTH, WHEEL_DIAMETER);

		token = (neSimpleToken *)MakeInstance(NE_SIMPLE_TOKEN, &param, true);

		token->SetDrawScale(scale);

		car[whichCar].wheelToken[i] = token;
	}
}

void CSampleCar::MakeCar(neSimulator * sim, neV3 & pos)
{
	f32 mass = 1.0f;

	neRigidBody * rigidBody = sim->CreateRigidBody();

//	rigidBody->SetSleepingParameter(0.01f);

	rigidBody->CollideConnected(true);

	neGeometry * geom = rigidBody->AddGeometry();

	neV3 boxSize; 
	{
		// the car consist of two boxes

		boxSize.Set(6.0f, 1.0f, 3.2f);

		geom->SetBoxSize(boxSize);

		neT3 t; 
		
		t.SetIdentity();

		t.pos.Set(0.0f, 0.25f, 0.0f);

		geom->SetTransform(t);

		geom = rigidBody->AddGeometry();

		boxSize.Set(2.5f, 1.0f, 2.8f);

		geom->SetBoxSize(boxSize[0],boxSize[1],boxSize[2]);

		t.pos.Set(-0.6f, 1.0f, 0.0f);

		geom->SetTransform(t);
	}
	{ // add 4 sensors to the rigid body
		neSensor * sn;
		
		for (s32 si = 0; si < 4; si++)
		{
			sn =rigidBody->AddSensor();

			neV3 snPos, snDir;

			snDir.Set(0.0f, -1.0f, 0.0f);

			snPos.Set(sensorData[si].pos);

			sn->SetLineSensor(snPos, snDir);
		}

		// add a controller to the rigid body

		neRigidBodyController * controller; 

		controller = rigidBody->AddController(&cb, 0);
	}

	neV3 tensorSize;

	tensorSize = boxSize;

	rigidBody->UpdateBoundingInfo();
	
	rigidBody->SetInertiaTensor(neBoxInertiaTensor(tensorSize, mass));

	rigidBody->SetMass(mass);

	rigidBody->SetUserData((u32)this);

	carRigidBody = rigidBody;

	{ // setup the display boxes for the car

		s32 i = 0;
		
		rigidBody->BeginIterateGeometry();

		while (geom = rigidBody->GetNextGeometry())
		{
			neSimpleToken * token = gw.MakeToken(geom);

			token->SetDiffuseColour(neV4(0.2f, 0.2f, 0.8f, 1.0f));

			token->SetAmbientColour(neV4(0.0f, 0.2f, 0.3f, 1.0f));

			neV3 boxSize;
			
			geom->GetBoxSize(boxSize);

			token->SetDrawScale(boxSize);

			carToken[i++] = token;

			geom->SetUserData((u32)token);
		}
	}
	// set the car position
	
	carRigidBody->SetPos(pos);

	// add bonnet to the car

	MakeParts(sim, pos);
}

struct PartData
{
	neV3 boxSize;
	neV3 position;
	neV3 jointPos;
	neV3 jointRot;
	f32 lowerLimit;
	f32 upperLimit;
};

PartData parts[] =
{
	{	// Bonet
		{2.0f, 0.1f, 3.0f},
		{1.65f, 0.8f, 0.0f},
		{0.65f, 0.8f, 0.0f},
		{NE_PI * 0.5f, 0.0f, 0.0f},
		0.0f,
		NE_PI * 0.4f,
	},
	{	//Left Door
		{2.0f, 0.8f, 0.1f},
		{-0.2f, 0.25f, 1.6f},
		{0.65f, 0.25f, 1.6f},
		{0.0f, 0.0f, 0.0f},
		0.0f,
		NE_PI * 0.4f,
	},
	{	//Right Door
		{2.0f, 0.8f, 0.1f},
		{-0.2f, 0.25f, -1.6f},
		{0.65f, 0.25f, -1.6f},
		{0.0f, 0.0f, 0.0f},
		-NE_PI * 0.4f,
		0.0f,
	},
};

void CSampleCar::MakeParts(neSimulator * sim, neV3 & pos)
{
	s32 i;

	for (i = 0; i < N_PARTS; i++)
	{
		neRigidBody * rb = sim->CreateRigidBody();

		if (id == 0 ) //make it harder to sleep
			rb->SetSleepingParameter(0.1f);

		rb->SetPos(parts[i].position + pos);

		neGeometry * geom = rb->AddGeometry();

		geom->SetBoxSize(parts[i].boxSize);

		rb->SetMass(0.01f);

		rb->UpdateBoundingInfo();

		rb->SetInertiaTensor(neBoxInertiaTensor(parts[i].boxSize, 0.01f));

		neJoint * joint = sim->CreateJoint(rb, carRigidBody);

		neT3 trans;

		trans.pos = parts[i].jointPos + pos;

		trans.rot.RotateXYZ(parts[i].jointRot);

		joint->SetJointFrameWorld(trans);

		joint->SetType(neJoint::NE_JOINT_HINGE);

		joint->SetLowerLimit(parts[i].lowerLimit);

		joint->SetUpperLimit(parts[i].upperLimit);

		joint->SetEpsilon(0.0f);

		joint->SetIteration(4);

		joint->EnableLimit(true);

		joint->Enable(true);

		carParts[i] = rb;

		neSimpleToken * token = gw.MakeToken(geom);

		geom->SetUserData((u32)token);

		carToken[N_BODY_BOXES + i] = token;
	}
}

void CSampleCar::Process()
{
	neV3 vel;

	if (GETOS->IsKeyPress('Q'))
	{
		vel.Set (0.0f,5.0f,0.0f);

		carRigidBody->SetVelocity(vel);
	}
	if (GetAsyncKeyState('S'))
	{
		accel += 0.1f;

		if (accel > MAX_SPEED)
			accel = MAX_SPEED;
	} 
	else if (GetAsyncKeyState('X'))
	{
		accel -= 0.1f;

		if (accel < MAX_REVERSE_SPEED)
			accel = MAX_REVERSE_SPEED;
	}
	else 
	{
		accel *= 0.99f;
	}
	if (GetAsyncKeyState('K'))
	{
		steer += 0.02f;

		if (steer > MAX_STEER)
		{
			steer = MAX_STEER;

			slide += 0.05f;
		}
	}
	else if (GetAsyncKeyState('L'))
	{
		steer -= 0.02f;

		if (steer < -MAX_STEER)
		{
			steer = -MAX_STEER;

			slide += 0.05f;
		}
	}
	else
	{
		steer *= 0.9f;

		slide *= 0.9f;
	}
	if (slide > MAX_SLIDE)
	{
		slide = MAX_SLIDE;
	}
}

void CSampleCar::CarController(neRigidBodyController * controller)
{
	neT3 body2World = carRigidBody->GetTransform();

	carRigidBody->BeginIterateSensor();

	neSensor * sn;

	neV3 force, torque;

	force.SetZero();

	torque.SetZero();

	s32 i = 0;

	while (sn = carRigidBody->GetNextSensor())
	{
		f32 k = 6.0f; //spring constant

		f32 u = 0.8f; //damping constant

		if (i == 2 || i == 3)
		{
			k = 10.0f;
		}

		// add spring force

		f32 depth = sn->GetDetectDepth();

		//gw.car.suspensionLength[i] = 1.0f - depth;
		suspensionLength[i] = 1.0f - depth;

		if (depth == 0.0f)
		{
			i++;
			continue;
		}
		//if (depth > 0.7f)
		//	depth = 0.7f;
		
		neV3 groundNormal = sn->GetDetectNormal();

		neV3 lineNormal = body2World.rot * sn->GetLineUnitVector();

		neV3 linePos = body2World * sn->GetLinePos();

		f32 dot = lineNormal.Dot(groundNormal) * -1.0f;

		//if (dot <= 0.7f)
		//	continue;

		neV3 f = depth * lineNormal * -k ;//* dot;

		force += f;

		neV3 r = linePos - carRigidBody->GetPos();

		torque += r.Cross(f);

		// add damping force
		
		f32 speed = carRigidBody->GetVelocityAtPoint(r).Dot(lineNormal);

		f = -speed * lineNormal * u;

		force += f;

		torque += r.Cross(f);

		// add friction force

		neV3 vel = carRigidBody->GetVelocityAtPoint(sn->GetDetectContactPoint() - carRigidBody->GetPos());

		vel.RemoveComponent(groundNormal);

		if (i == 0 || i == 1)
		{
			//steering

			vel.RemoveComponent(steerDir);
		}
		else
		{
			vel.RemoveComponent(body2World.rot[0]); //rear wheel always parallel to car body
		}


		f = vel;

		f.Normalize();

		f *= -2.0f;

		if (i == 2 || i ==3)
		{
			f *= (1.0f - slide);
		}
		f[1] = 0.0f;
		
		force += f;

		r = sn->GetDetectContactPoint() - carRigidBody->GetPos();

		torque += r.Cross(f);

		// driving force

		if (i == 2 || i == 3) // rear wheel
		{
			f = body2World.rot[0];

			f *= accel;

			force += f;

			torque += r.Cross(f);
		}
/*
		gw.car.displayLines[i][0] = linePos;

		gw.car.displayLines[i][1] = sn->GetDetectContactPoint();
*/
		i++;
	}

	// drag
	f32 dragConstant = 0.5f;

	neV3 vel = carRigidBody->GetVelocity();

	f32 dot = vel.Dot(body2World.rot[0]);

	neV3 drag = dot * body2World.rot[0] * -dragConstant;

	force += drag;

	controller->SetControllerForce(force);

	controller->SetControllerTorque(torque);
}

void CControllerCB::RigidBodyControllerCallback(neRigidBodyController * controller)
{
	neRigidBody * rb = controller->GetRigidBody();

	CSampleCar * car = (CSampleCar *)rb->GetUserData();

	car->CarController(controller);
}


