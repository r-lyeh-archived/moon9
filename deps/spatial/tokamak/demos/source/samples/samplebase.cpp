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

#include "samplebase.h"
#include "windows.h"
extern "C"
{
#include "../qhull-2003.1/src/qhull_a.h"
}

struct DemoData
{
	neV3 pos;
	neV3 boxSize;
	neV3 colour;
};

void DrawLine(const neV3 & colour, neV3 * startpoint, s32 count)
{
	GETDISPLAY->DrawLine(count, colour, startpoint);
}

DemoData wallData[CSampleBase::WALL_NUMBER] = 
{
	{
		{-50.5f,0.0f,0.0f}, {3.0f,60.0f,200.0f}, {0.3f,0.3f,0.6f},
	},
	{
		{50.5f,0.0f,0.0f}, {3.0f,60.0f,200.0f}, {0.3f,0.3f,0.6f},
	},
	{
		{0.0f,0.0f,-50.5f}, {200.0f,60.0f,3.0f}, {0.3f,0.3f,0.6f},
	},
	{
		{0.0f,0.0f,50.5f}, {200.0f,60.0f,3.0f}, {0.3f,0.3f,0.6f},
	},
	{
		{0.0f,31.0f,0.0f}, {200.0f,2.0f,200.0f}, {0.3f,0.3f,0.6f},
	},
	{
		{0.0f,-11.0f,0.0f}, {200.0f,2.0f,200.0f}, {0.3f,0.3f,0.6f},
//		{0.0f,-301.5f, 0.0f}, {200.0f,2.0f,200.0f}, {0.3f,0.3f,0.6f},
	},
};

char * PerformanceReportString[] =
{
	{"Total"},
	{"DYNAMIC"},
	{"POSITION"},
	{"CONTRAIN_SOLVING_1"},
	{"CONTRAIN_SOLVING_2"},
	{"COLLISION_DETECTION"},
	{"COLLISION_CULLING"},
	{"TERRAIN_CULLING"},
	{"TERRAIN"},
	{"CONTROLLER_CALLBACK"},
};

void CSampleBase::Initialise() 
{
	neGameWorld::Initialise();
	
	neDisplaySystem* ds = GETDISPLAY;

	effectMan.Initialise(ds);
	
	ds->SetEffectManager(&effectMan);

	focusPos.SetZero();

	for (s32 i = 0; i < WALL_NUMBER; i++)
	{
		walls[i] = NULL;
		
		wallTokens[i] = NULL;
	}
	landToken = NULL;

	counter = 0;
}

void CSampleBase::Reset()
{
	counter = 0;

	Free();

	for (s32 i = 0; i < WALL_NUMBER; i++)
	{
		walls[i] = NULL;
	}
}

void CSampleBase::Free()
{
	for (s32 i = 0; i < WALL_NUMBER; i++)
	{
		if (wallTokens[i])
		{
			wallTokens[i]->Free();
		}
		wallTokens[i] = NULL;
	}
	if (landToken)
		landToken->Free();

	landToken = NULL;
}

void CSampleBase::Process(OSTime time)
{
	neDisplaySystem * ds = GETDISPLAY;
	
	neCamera * cam = ds->GetCamera();

	neV3 diff = focusPos - targetCam.cam->GetPos();

	f32 len = diff.Length();

	diff = diff * (1.0f / len);

	neV3 newPos;

	if (GETOS->IsKeyPress('A')) // closer
	{
		neV3 dir; targetCam.cam->GetMatrix()->GetColumn(dir, 2);

		dir[2] *= -1.0f;

		neV3 pos = targetCam.cam->GetPos();

		newPos = pos + dir * 0.8f;

		targetCam.cam->SetPos(newPos);
	}
	else if (GETOS->IsKeyPress('Z')) //further
	{
		neV3 dir; cam->GetMatrix()->GetColumn(dir, 2);

		dir[2] *= -1.0f;

		neV3 pos = cam->GetPos();

		newPos = pos - dir * 0.8f;

		targetCam.cam->SetPos(newPos);
	}
	
	targetCam.Process();

	if (wallTokens[5])
	{
		wallTokens[5]->SetMatrix(walls[5]->GetTransform());

		wallTokens[5]->AddToDisplay();
	}
}

void CSampleBase::DisplayProfileReport()
{
	f32 processtime = perfReport.time[nePerformanceReport::NE_PERF_TOTAL_TIME];
	f32 percentage =  processtime * 60.0f;

	char ss[256];
	
	sprintf(ss, "physics time %.3f ms, %.2f", processtime * 1000.0f, percentage * 100.0f);

	strcat(ss, "%% of a 1/60 second frame");

	GETDISPLAY->PrintText2(0, 0, ss);	

	f32 t;
	
	for (s32 i = 1; i < nePerformanceReport::NE_PERF_LAST; i++)
	{
		t = perfReport.time[i];
		sprintf(ss, "%s %.3f %%", PerformanceReportString[i], t);	
		GETDISPLAY->PrintText2(4, i, ss);	
	}
}

void CSampleBase::Shutdown() 
{
	Free();
	
	effectMan.Shutdown();
	
	neGameWorld::Shutdown();
}

void CSampleBase::SetUpRoom(neSimulator * sim)
{
	//for (s32 i = 0; i < WALL_NUMBER; i++)
	s32 i = 5;
	{
		neGeometry * con;
		
		walls[i] = sim->CreateAnimatedBody();

		con = walls[i]->AddGeometry();

		assert(con);

		DemoData &d = wallData[i];

		con->SetBoxSize(d.boxSize[0], d.boxSize[1], d.boxSize[2]);

		walls[i]->UpdateBoundingInfo();

		walls[i]->SetPos(d.pos);
	}
	neSimpleTokenParam param;

	param.mesh = "cube3.x";
	param.resourceID = "cube";
	param.trans.SetIdentity();
	wallTokens[5] = (neSimpleToken *)MakeInstance(NE_SIMPLE_TOKEN, &param, true);

	ASSERT(wallTokens[5]);

	wallTokens[5]->SetDiffuseColour(neV4(0.2f, 0.2f, 0.2f, 1.0f));

	wallTokens[5]->SetAmbientColour(neV4(0.2f, 0.3f, 0.2f, 1.0f));

	DemoData &d = wallData[5];

	neV3 wallSize; wallSize.Set(d.boxSize[0], d.boxSize[1], d.boxSize[2]);

	wallTokens[5]->SetDrawScale(wallSize);
}

void CSampleBase::SetupTerrain(neSimulator * sim)
{
	neSimpleTokenParam param;

	param.mesh = "landscape2.x";
	//param.mesh = "landscape3.x";
	//param.mesh = "plane.x";
	//param.mesh = "triangle2.x";

	param.resourceID = "landscape";

	param.trans.SetIdentity();

	landToken = (neSimpleToken *)MakeInstance(NE_SIMPLE_TOKEN, &param, true);

	ASSERT(landToken);

	neFrame * f = landToken->GetFrame();

	neMesh * mesh = f->GetMesh();

	neTriangleMesh triMesh;

	triMesh.vertexCount = mesh->GetVertexCount();

	triMesh.triangleCount = mesh->GetTriangleCount();

	neV3 * verts = new neV3[triMesh.vertexCount];

	mesh->GetVerticesList(verts);

	triMesh.vertices = verts;

	neTriangle * tri = new neTriangle[triMesh.triangleCount];

	s32 * triindex = new s32[triMesh.triangleCount * 3];

	mesh->GetTriangleList(triindex);

	for (s32 i = 0; i < triMesh.triangleCount; i++)
	{
		tri[i].indices[0] = triindex[i * 3];
		tri[i].indices[1] = triindex[i * 3 + 1];
		tri[i].indices[2] = triindex[i * 3 + 2];
		tri[i].materialID = 0;
		tri[i].flag = neTriangle::NE_TRI_TRIANGLE;
		//tri[i].flag = neTriangle::NE_TRI_HEIGHT_MAP;
	}
	triMesh.triangles = tri;

	sim->SetTerrainMesh(&triMesh);
}

neSimpleToken * CSampleBase::MakeToken(neGeometry * geom)
{
	neSimpleTokenParam param;

	neSimpleToken * token;

	neV3 scale;

	if (geom->GetBoxSize(scale))
	{
		param.mesh = "cube3.x";

		param.resourceID = "cube";

		param.trans.SetIdentity();

		token = (neSimpleToken *)MakeInstance(NE_SIMPLE_TOKEN, &param, true);
	}
	else
	{
		f32 height, diameter;

		if (geom->GetCylinder(diameter, height))
		{
			param.mesh = "cylinder2.x";

			param.resourceID = "cylinder";

			token = (neSimpleToken *)MakeInstance(NE_SIMPLE_TOKEN, &param, true);

			scale.Set(diameter, height, diameter);
		}
		else if (geom->GetSphereDiameter(diameter))
		{
			param.mesh = "sphere2.x";

			param.resourceID = "sphere";

			token = (neSimpleToken *)MakeInstance(NE_SIMPLE_TOKEN, &param, true);

			scale.Set(diameter, diameter, diameter);
		}
	}
	token->SetDrawScale(scale);

	return token;
}

void CSampleBase::DisplayRigidBodies(neRigidBody ** rb, s32 count)
{
	while (count-- && *rb)
	{
		neRigidBody * body = *rb;

		neT3 trans = body->GetTransform();

		body->BeginIterateGeometry();

		neGeometry * geom = body->GetNextGeometry();

		while (geom)
		{
			neSimpleToken * t = (neSimpleToken *)geom->GetUserData();

			if (t)
			{
				neT3 geomTrans = geom->GetTransform();

				neT3 worldTrans = trans * geomTrans;

				t->SetMatrix(worldTrans);

				t->AddToDisplay();
			}
			geom = body->GetNextGeometry();
		}
		rb++;
	}
}

void CSampleBase::DisplayAnimatedBodies(neAnimatedBody ** ab, s32 count)
{
	while (count-- && *ab)
	{
		neAnimatedBody * body = *ab;

		neT3 trans = body->GetTransform();

		body->BeginIterateGeometry();

		neGeometry * geom = body->GetNextGeometry();

		while (geom)
		{
			neSimpleToken * t = (neSimpleToken *)geom->GetUserData();

			if (t)
			{
				neT3 geomTrans = geom->GetTransform();

				neT3 worldTrans = trans * geomTrans;

				t->SetMatrix(worldTrans);

				t->AddToDisplay();
			}
			geom = body->GetNextGeometry();
		}
		ab++;
	}
}

struct DCDFace
{
	neByte *neighbourFaces;
	neByte *neighbourVerts;
	neByte *neighbourEdges;
};

struct DCDVert
{
	neByte * neighbourEdges;
};

struct DCDEdge
{
	neByte f1;
	neByte f2;
	neByte v1;
	neByte v2;
};

bool ReadConvexData(char * filename, neByte *& adjacency)
{
	FILE * ff =	fopen(filename, "r");

	if (ff == NULL)
		return false;

	fseek(ff, 0, SEEK_END);
	
	fpos_t pos ;

	fgetpos(ff, &pos);

	fclose(ff);

	ff = fopen(filename, "rb");

	neByte * d = new neByte[pos];

	//fseek(ff, 0, SEEK_SET);

	int r = fread(d,1, pos, ff);

	fclose(ff);

	s32 numFaces = *(int*)d;

	s32 numVerts = *((int*)d + 1);

	s32 numEdges = *((int*)d + 2);

	DCDFace * f = (DCDFace *)(d + sizeof(int) * 4 + numFaces * sizeof(f32) * 4 + numVerts * sizeof(f32) * 4);

	DCDVert * v = (DCDVert *)(f + numFaces);

	for (s32 i  = 0; i < numFaces; i++)
	{
		f[i].neighbourEdges += (int)d;
		f[i].neighbourVerts += (int)d;
		f[i].neighbourFaces += (int)d;
	}
	for (i = 0; i < numVerts; i++)
	{
		v[i].neighbourEdges += (int)d;
	}
	adjacency = d;
	
	DCDEdge * e = (DCDEdge*)(&v[numVerts]);

	int diff = (neByte*)e - d;
	return true;
}

