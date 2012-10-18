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

class CControllerCB: public neRigidBodyControllerCallback
{
public:
	
	void RigidBodyControllerCallback(neRigidBodyController * controller);
};

class CSample8;

class CSampleCar
{
public:
	void MakeCar(neSimulator * sim, neV3 & pos);

	void MakeParts(neSimulator * sim, neV3 & pos);

	void CarController(neRigidBodyController * controller);

	void Process();

public:
	enum
	{
		N_BODY_BOXES = 2,
		N_PARTS = 3,

		N_TOKENS = (N_BODY_BOXES + N_PARTS),
	};

	CSampleCar()
	{
		id = -1;

		accel = 0.0f;

		steer = 0.0f;

		slide = 0.0f;

		gameWorld = NULL;
	}

	neRigidBody * carRigidBody;

	neRigidBody * carParts[N_PARTS];
	
	neSimpleToken * carToken[N_TOKENS];

	neSimpleToken * wheelToken[4];

	neV3 displayLines[4][2];

	f32 suspensionLength[4];

	CSample8 * gameWorld;

	s32 id;
	
	f32 accel;

	f32 steer;

	f32 slide;

	neV3 steerDir;
};

class CSample8: public CSampleBase
{
public:
	CSample8() { 
		paused = false;
	}
	
	void Initialise();
	
	void Shutdown();
	
	void Process(OSTime time);
	
	void DisplayText();

	void InititialisePhysics();

	void LoadWheelModels(s32 whichCar);

public:
	enum
	{
		N_CARS = 10,
		TARGET_CAR = 0,
	};

	neSimulator * sim;

	neAnimatedBody * ramp;

	neSimpleToken * rampToken;

	CSampleCar car[N_CARS];

	neAllocatorDefault all;

	nePerformanceReport perfReport;
};
