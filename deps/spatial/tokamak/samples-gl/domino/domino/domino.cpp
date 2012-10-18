/*
 *	Copyright (c) 2002 Tokamak Limited
 *
 *	This sample demonstrate how to use the custom collision 
 *	detection callback function. Custom collision detection
 *	callback functions allow the user to define their own
 *	collision detection algorithm.
 *	
 *	The two custom CD callback use in this samples are
 *	
 *		CustomCDRB2ABCallback and
 *		CustomCDRB2RBCallback
 *		
 *	The first function define the callback for collision
 *	between RigidBody and AnimatedBody, and the second defines
 *	the function between RigidBody and RigidBody
 *
 *	Each function must return true if the 2 bodies intersect,
 *	and must fill in the cdInfo struct. The contact normal return
 *	must point away from body B, a positive penetration depth if
 *	intersecting, and the point on the surface of body A and 
 *	body B (defined in world space
 *
 *	About ::UseCustomCollisionDetection()
 *	Use this function to tell Tokamak that this body uses custom
 *	collision detection callback function. The second parameter is
 *	a neT3. This transform should represent the OBB which enclose 
 *	the entire body. This OBB is defined in the body's space.
 *	The last parameter is a approximate bounding radius of the body.
 *
 *	Note: when using custom collision detect function, you are still
 *	relying on Tokamak's internal broadphase detection. This is reason
 *	why you need to supply the OBB of the body.
 *
 */ 
	
#include <render9/render.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>       
//#include <GL/glut.h>    
#include "tokamak.h"

#define N_RIGIDBODY 20
#define N_ANIMATED_BODY 1
#define N_GEOMETRY (N_RIGIDBODY + N_ANIMATED_BODY)
#define TIME_STEP (1.0f/ 60.0f)

//globals
char * SampleTitle = "Tokamak Sample: Dominos (Press 'Q' to start)";

neSimulator * sim = NULL;
neAllocatorDefault all;
neRigidBody * boxBody[N_RIGIDBODY];


#define DOMINO_WIDTH 2.0f
#define DOMINO_HEIGHT 4.f
#define DOMINO_DEPTH 0.5f

neBool pause = false;

void sampleInit()
{
	// create simulator
	neV3 gravity; gravity.Set(0.0f, -9.f, 0.0f);

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidBodiesCount = N_RIGIDBODY;
	sizeInfo.animatedBodiesCount = N_ANIMATED_BODY;
	sizeInfo.geometriesCount = N_GEOMETRY;
	sizeInfo.overlappedPairsCount = (N_RIGIDBODY * N_ANIMATED_BODY) + N_RIGIDBODY * (N_RIGIDBODY - 1) / 2;
	sizeInfo.rigidParticleCount = 0;
	sizeInfo.constraintsCount = 0;
	sizeInfo.terrainNodesStartCount = 0;

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

	// create a box
	neV3 pos;

	for (int i = 0; i < N_RIGIDBODY; i++)
	{
		boxBody[i] = sim->CreateRigidBody();

		neGeometry * geom = boxBody[i]->AddGeometry();

		geom->SetBoxSize(DOMINO_DEPTH, DOMINO_HEIGHT, DOMINO_WIDTH);

		boxBody[i]->UpdateBoundingInfo();

		boxBody[i]->SetMass(1.0f);

		boxBody[i]->SetInertiaTensor(neBoxInertiaTensor(DOMINO_DEPTH, DOMINO_HEIGHT, DOMINO_WIDTH, 1.0f));

		pos.Set(DOMINO_DEPTH * 5.0f * i - DOMINO_DEPTH * 2.5f*N_RIGIDBODY, DOMINO_HEIGHT / 2.0f, 0.0f);
		
		boxBody[i]->SetPos(pos);
	}
	// create the floor
	
	neAnimatedBody * ab = sim->CreateAnimatedBody();

	neGeometry * geom = ab->AddGeometry();

	geom->SetBoxSize(100, 1.0f, 100);

	ab->UpdateBoundingInfo();

	pos.Set(0, -0.5f, 0);

	ab->SetPos(pos);
}

void sampleDraw()

{
	glPushMatrix();

	for (int i = 0; i < N_RIGIDBODY; i++)
	{
		glPushMatrix();

		neT3 frame = boxBody[i]->GetTransform();

		glMultMatrixf((GLfloat*)&frame);

		GLfloat skinColor[] = {0.5f - 0.1f * i, 1.0, 0.5f + 0.1f * i, 1.0};

		glMaterialfv(GL_FRONT, GL_DIFFUSE, skinColor);

		glPushMatrix();

		glScalef(DOMINO_DEPTH, DOMINO_HEIGHT, DOMINO_WIDTH);

		glutSolidCube(1.0f);

		glPopMatrix();

		glPopMatrix();
	}  
	glPopMatrix();
}

void sampleKeyFunc(unsigned char c, int x, int y)
{
	switch (c)
	{
	case 27:
		exit(0);
		break;
	case 'q':
		{
			neV3 impulse; impulse.Set(-5.0f, 0.0f, 0.0f);

			neV3 pos; pos = boxBody[N_RIGIDBODY-1]->GetPos();

			pos[1] += 1.0f;
			
			boxBody[N_RIGIDBODY-1]->ApplyImpulse(impulse, pos);
		}
		break;
	case 'p':
		{
			pause = !pause;
		}
	}
}

void sampleUpdate()
{
	if (!pause)
		sim->Advance(TIME_STEP);
}

void sampleShutdown()
{
	neSimulator::DestroySimulator(sim);
}

