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
	
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>       
#include <GL/glut.h>    
#include "tokamak.h"

#define N_RIGIDBODY 10
#define N_GEOMETRY 1
#define N_ANIMATED_BODY 1
#define TIME_STEP (1.0f/ 60.0f)

//globals
char * SampleTitle = "Tokamak Sample: Custom Collision Detection";

neSimulator * sim = NULL;
neAllocatorDefault all;
neRigidBody * boxBody[N_RIGIDBODY];


#define SPHERE_PER_BODY 4
#define SPHERE_OFFSET 0.5f
#define SPHERE_RADIUS 0.7f

neV3 sphereOffset[SPHERE_PER_BODY] = 
{	{SPHERE_OFFSET, 0.0f, SPHERE_OFFSET},
	{-SPHERE_OFFSET, 0.0f, SPHERE_OFFSET},
	{SPHERE_OFFSET, 0.0f, -SPHERE_OFFSET},
	{-SPHERE_OFFSET, 0.0f, -SPHERE_OFFSET},
};

neBool pause = true;//false;

neBool CustomCDRB2ABCallback(neRigidBody * bodyA, neAnimatedBody * bodyB, neCustomCDInfo & cdInfo);

neBool CustomCDRB2RBCallback(neRigidBody * bodyA, neRigidBody * bodyB, neCustomCDInfo & cdInfo);

void sampleInit()
{
	// create simulator
	neV3 gravity; gravity.Set(0.0f, -9.0f, 0.0f);

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidBodiesCount = N_RIGIDBODY;
	sizeInfo.animatedBodiesCount = N_ANIMATED_BODY;
	sizeInfo.geometriesCount = N_GEOMETRY;
	sizeInfo.overlappedPairsCount = (N_RIGIDBODY * N_ANIMATED_BODY) + N_RIGIDBODY * (N_RIGIDBODY - 1) / 2;
	sizeInfo.rigidParticleCount = 0;
	sizeInfo.constraintsCount = 0;
	sizeInfo.terrainNodesStartCount = 0;

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

	sim->SetCustomCDRB2ABCallback(CustomCDRB2ABCallback);

	sim->SetCustomCDRB2RBCallback(CustomCDRB2RBCallback);
	
	// create a box
	neT3 obb;	
	neV3 pos;

	for (int i = 0; i < N_RIGIDBODY; i++)
	{
		boxBody[i] = sim->CreateRigidBody();

		f32 extend = (SPHERE_RADIUS + SPHERE_OFFSET) * 2;

		obb.SetIdentity();
		obb.rot[0].Set(extend, 0, 0);
		obb.rot[1].Set(0, SPHERE_RADIUS * 2, 0);
		obb.rot[2].Set(0, 0, extend);

		boxBody[i]->UseCustomCollisionDetection(true, &obb, extend);

		boxBody[i]->SetMass(1.0f);

		boxBody[i]->SetInertiaTensor(neBoxInertiaTensor(extend, SPHERE_RADIUS * 2, extend, 1.0f));

		pos.Set(0.0f, 5.0f + 4.0f * i, 0.0f);
		
		boxBody[i]->SetPos(pos);
	}
	// create the floor
	
	neAnimatedBody * ab = sim->CreateAnimatedBody();

	obb.rot[0].Set(100, 0, 0);
	obb.rot[1].Set(0, 1, 0);
	obb.rot[2].Set(0, 0, 100);

	ab->UseCustomCollisionDetection(true, &obb, 100);

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

		for (int i = 0; i < SPHERE_PER_BODY; i++)
		{
			glPushMatrix();

			glTranslatef(sphereOffset[i][0], sphereOffset[i][1], sphereOffset[i][2]);
		
			glutSolidSphere(SPHERE_RADIUS, 5, 5);

			glPopMatrix();
		}
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
			neV3 vel; vel.Set(0.0f, 10.0f, 0.0f);

			boxBody[0]->SetVelocity(vel);
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

neBool CustomCDRB2ABCallback(neRigidBody * bodyA, neAnimatedBody * bodyB, neCustomCDInfo & cdInfo)
{
	neT3 t = bodyA->GetTransform();

	f32 depth = 0.0f;

	for (int i = 0; i < SPHERE_PER_BODY; i++)
	{
		neV3 pos = t * sphereOffset[i];

		if (pos[1] >= SPHERE_RADIUS)
			continue;

		f32 d = SPHERE_RADIUS - pos[1];

		if (d > depth)
		{
			depth = d;
			cdInfo.collisionNormal.Set(0.0f, 1.0f, 0.0f); // normal must point away from body B
			cdInfo.materialIdA = 0;
			cdInfo.materialIdB = 0;
			cdInfo.penetrationDepth = d;
			cdInfo.worldContactPointA.Set(pos[0], pos[1] - SPHERE_RADIUS, pos[2]);
			cdInfo.worldContactPointB.Set(pos[0], 0.0f, pos[2]);
		}
	}

	return (depth > 0.0f);
}

neBool CustomCDRB2RBCallback(neRigidBody * bodyA, neRigidBody * bodyB, neCustomCDInfo & cdInfo)
{
	f32 depth = 0;

	for (int i = 0; i < SPHERE_PER_BODY; i++)
	{
		neV3 posA;

		posA = bodyA->GetTransform() * sphereOffset[i];

		for (int j = 0; j < SPHERE_PER_BODY; j++)
		{
			neV3 posB;

			posB = bodyB->GetTransform() * sphereOffset[j];

			neV3 sub = posA - posB;

			f32 dot = sub.Dot(sub);

			f32 totalLen = SPHERE_RADIUS * 2;

			totalLen *= totalLen;

			if (dot >= totalLen)
			{
				continue;
			}

			if (neIsConsiderZero(dot))
			{
				continue;
			}
			f32 len = sub.Length();

			sub *= 1.0f / len;

			f32 thisDepth = SPHERE_RADIUS * 2 - len;

			if (thisDepth < depth)
				continue;

			depth = thisDepth;

			cdInfo.collisionNormal = sub; // normal must point away from body B
			cdInfo.materialIdA = 0;
			cdInfo.materialIdB = 0;
			cdInfo.penetrationDepth = thisDepth;

			cdInfo.worldContactPointA = posA - sub * SPHERE_RADIUS;
			cdInfo.worldContactPointB = posB + sub * SPHERE_RADIUS;
		}
	}
	
	return (depth > 0.0f);
}
