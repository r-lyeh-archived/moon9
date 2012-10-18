#include "dxut\dxstdafx.h"
#include "resource.h"
#include "d3dapp.h"
#include "tokamaksampleApp.h"


D3DXHANDLE g_hShaderTechTokamak;

D3DXVECTOR4 vLightWorld[NUM_LIGHT] = {	D3DXVECTOR4(1.f,2.f,1.f,0.f),
D3DXVECTOR4(-1.f,1.f,1.f,0.f)};

D3DXVECTOR4 vLightColor[NUM_LIGHT] = {	D3DXVECTOR4(0.7f,0.7f,0.7f,0.f),
D3DXVECTOR4(0.5f,0.5f,0.5f,0.f)};

const s32 MAX_OVERLAPPED_PAIR = 300;
const s32 WALL_NUMBER = 1;
const f32 EPSILON  = 0.1f;

struct DemoData
{
	neV3 pos;
	neV3 boxSize;
	neV3 colour;
};

DemoData gFloor = {	{0.0f,-11.0f,0.0f}, {200.0f,2.0f,200.0f}, {0.3f,0.3f,0.6f}};


void BreakageCallbackFn(neByte * originalBody, neBodyType bodyType, neGeometry * brokenGeometry, neRigidBody * newBody);


class CSampleSCCollision
{
public:
	CSampleSCCollision() { 
		paused = false;
	}

	void Initialise();

	void Shutdown();

	void Process(XINPUT_STATE & InputState ,double fTime);

	void InititialisePhysics();

	void InititialiseRenderPrimitives();

	void MakeParticle(neV3 position, s32 index);

	void MakeTree(neV3 position, s32 index);

	void CSampleSCCollision::DisplayAnimatedBodies(neAnimatedBody ** ab, s32 count , IDirect3DDevice9* pd3dDevice);

	void CSampleSCCollision::DisplayRigidBodies(neRigidBody ** rb, s32 count , IDirect3DDevice9* pd3dDevice);


public:
	enum
	{
		NUMBER_OF_PARTICLES = 100,

		NUMBER_OF_TREES = 5,

		BRANCH_PER_TREE = 4, 

		GEOMETRY_PER_TREE = BRANCH_PER_TREE + 2,

		NUMBER_OF_BODIES = NUMBER_OF_PARTICLES + NUMBER_OF_TREES * BRANCH_PER_TREE,

		NUMBER_OF_TOKENS = NUMBER_OF_PARTICLES + NUMBER_OF_TREES * GEOMETRY_PER_TREE,
	};

	neSimulator * sim;


	neRigidBody * particles[NUMBER_OF_PARTICLES];

	neRigidBody * branches[NUMBER_OF_TREES * BRANCH_PER_TREE];

	neAnimatedBody * trees[NUMBER_OF_TREES];

	CRenderPrimitive renderPrimitives[NUMBER_OF_TOKENS];


	neAllocatorDefault all;

	nePerformanceReport perfReport;

	bool paused;

	CRenderPrimitive groundRender;
	neAnimatedBody * ground;

	CRenderPrimitive terrainRender;

	s32 nextBullet;

	s32 nextFreeRenderPrimitive;

	s32 nextFreeBranch;

};

CSampleSCCollision sample;

void CSampleSCCollision::Initialise() 
{
	InititialisePhysics();
	InititialiseRenderPrimitives();
	nextBullet = 0;
	nextFreeBranch = 0;
	nextFreeRenderPrimitive = 0;
}

void CSampleSCCollision::Process(XINPUT_STATE & InputState ,double fTime)
{

	static double mTimeUntilNextToggle = 0;

	if (mTimeUntilNextToggle >= 0)
		mTimeUntilNextToggle -= fTime;


	// Zero value if thumbsticks are within the dead zone 
	if( (InputState.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && 
		InputState.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) && 
		(InputState.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && 
		InputState.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) )
	{	
		InputState.Gamepad.sThumbLX = 0;
		InputState.Gamepad.sThumbLY = 0;
	}

	if( (InputState.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && 
		InputState.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) && 
		(InputState.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && 
		InputState.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ) 
	{
		InputState.Gamepad.sThumbRX = 0;
		InputState.Gamepad.sThumbRY = 0;
	}


	if (GetAsyncKeyState('P') && mTimeUntilNextToggle <= 0)
	{
		paused = !paused;

		mTimeUntilNextToggle = 500;
	}


	static s32 nextBullet = 0;

	if (GetAsyncKeyState('T') && mTimeUntilNextToggle <= 0)
	{
		neV3 pos;
		neV3 dir;
		const D3DXVECTOR3* lookatpt;
		const D3DXVECTOR3* eyept;
		eyept = g_Camera.GetEyePt();
		lookatpt = g_Camera.GetLookAtPt();
		D3DXVECTOR3 d3ddir;
		d3ddir = *lookatpt - *eyept;

		pos.Set(eyept->x,eyept->y,eyept->z);
		dir.Set(d3ddir.x,d3ddir.y,d3ddir.z);
		dir.Normalize();

		pos = pos + dir * 10.0f;

		particles[nextBullet]->SetPos(pos);

		particles[nextBullet]->SetVelocity(dir * 30.0f);

		nextBullet = (nextBullet + 1) % NUMBER_OF_PARTICLES;

		mTimeUntilNextToggle = 40;
		
	}

	
	


}


void CSampleSCCollision::Shutdown() 
{
	sample.groundRender.mMesh.Destroy();
	sample.terrainRender.mMesh.Destroy();

	for (s32 i = 0; i < sample.NUMBER_OF_TOKENS; i++)
	{
		sample.renderPrimitives[i].mMesh.Destroy();
	}

	sim->FreeTerrainMesh();

	neSimulator::DestroySimulator(sim);

	sim = NULL;
}


void CSampleSCCollision::MakeParticle(neV3 position, s32 index)
{
	//const f32 groundLevel = -10.0f;

	s32 cur;

	cur = index;

	particles[cur] = sim->CreateRigidParticle();

	particles[cur]->GravityEnable(false);

	neGeometry * geom = particles[cur]->AddGeometry();

	geom->SetSphereDiameter(0.8f);

	particles[cur]->UpdateBoundingInfo();

	f32 mass = 1.0f;

	particles[cur]->SetInertiaTensor(neSphereInertiaTensor(0.8f, mass));

	particles[cur]->SetMass(mass);

	particles[cur]->SetPos(position);

}

struct TreeData
{
	f32 diameter;
	f32 height;
	neV3 pos;
	neV3 rotation;
	neV3 colour;
	neGeometry::neBreakFlag breakFlag;
	f32 breakageMass;
	f32 breakageMagnitude;
	f32 breakageAbsorption;
};

TreeData treeData[CSampleSCCollision::GEOMETRY_PER_TREE] = 
{
	{1.5f, 10.0f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.5f, 0.3f, 0.1f}, neGeometry::NE_BREAK_DISABLE, 2.0f, 0.0f, 0.0f},
	{1.2f, 10.0f, {0.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.5f, 0.3f, 0.1f}, neGeometry::NE_BREAK_DISABLE, 2.0f, 0.0f, 0.0f},

	{0.8f, 7.0f, {-4.0f, 3.0f, 0.0f}, {0.0f, 0.0f, -NE_PI / 4.0f}, {0.4f, 0.5f, 0.1f}, neGeometry::NE_BREAK_NORMAL, 5.0f, 1.0f, 0.3f},
	{0.8f, 7.0f, {4.0f, 6.0f, 0.0f}, {0.0f, 0.0f, NE_PI / 4.0f}, {0.4f, 0.5f, 0.1f}, neGeometry::NE_BREAK_NORMAL, 5.0f, 1.0f, 0.3f},

	{0.8f, 6.0f, {-3.5f, 10.0f, 0.0f}, {0.0f, 0.0f, -NE_PI / 4.0f}, {0.4f, 0.5f, 0.1f}, neGeometry::NE_BREAK_NORMAL, 5.0f, 2.0f, 0.3f},
	{0.8f, 6.0f, {3.5f, 12.0f, 0.0f}, {0.0f, 0.0f, NE_PI / 4.0f}, {0.4f, 0.5f, 0.1f}, neGeometry::NE_BREAK_NORMAL, 5.0f, 2.0f, 0.3f},

};

void CSampleSCCollision::MakeTree(neV3 position, s32 index)
{
	//const f32 groundLevel = -10.0f;

	s32 cur;

	cur = index;

	trees[cur] = sim->CreateAnimatedBody();

	trees[cur]->SetPos(position);

	for (s32 i = 0; i < GEOMETRY_PER_TREE; i++)
	{
		neGeometry * geom = trees[cur]->AddGeometry();

		geom->SetCylinder(treeData[i].diameter, treeData[i].height);

		neT3 trans;

		trans.SetIdentity();

		trans.pos = treeData[i].pos;

		trans.rot.RotateXYZ(treeData[i].rotation);

		geom->SetTransform(trans);

		geom->SetBreakageFlag(treeData[i].breakFlag);

		geom->SetBreakageMass(treeData[i].breakageMass);

		geom->SetBreakageInertiaTensor(neCylinderInertiaTensor(treeData[i].height, treeData[i].height, treeData[i].breakageMass));

		geom->SetBreakageMagnitude(treeData[i].breakageMagnitude);

		geom->SetBreakageAbsorption(treeData[i].breakageAbsorption);

		neV3 plane;

		plane.Set(0.0f, 1.0f, 0.0f);

		geom->SetBreakagePlane(plane);


	}
	trees[cur]->UpdateBoundingInfo();

}


void BreakageCallbackFn(neByte * originalBody, neBodyType bodyType, neGeometry * brokenGeometry, neRigidBody * newBody)
{
	sample.branches[sample.nextFreeBranch] = (neRigidBody*)newBody;

	sample.nextFreeBranch++;
}

neV3 treePosition[CSampleSCCollision::NUMBER_OF_TREES] =
{
	{0.0f, 5.0f, 0.0f},
	{25.0f, 5.0f, -20.0f},
	{-20.0f, 0.0f, 30.0f},
	{30.0f, 5.0f, 10.0f},
	{-30.0f, 0.0f, -20.0f},
};

void CSampleSCCollision::InititialiseRenderPrimitives()
{
	s32 curToken = 0;

	for (s32 i = 0; i < NUMBER_OF_PARTICLES; i++)
	{
		particles[i]->BeginIterateGeometry();

		neGeometry * geom = particles[i]->GetNextGeometry();

		while (geom)
		{

			f32 diameter;
			geom->GetSphereDiameter(diameter);
			renderPrimitives[curToken].SetGraphicSphere(diameter/2.0f);

			geom->SetUserData((u32)&renderPrimitives[curToken]);

			if ((curToken % 4) == 0)
			{
				renderPrimitives[curToken].SetDiffuseColor(D3DXCOLOR(0.8f, 0.2f, 0.2f, 1.0f));
			}
			else if ((curToken % 4) == 1)
			{
				renderPrimitives[curToken].SetDiffuseColor(D3DXCOLOR(0.2f, 0.4f, 0.6f, 1.0f));
			}
			else if ((curToken % 4) == 2)
			{
				renderPrimitives[curToken].SetDiffuseColor(D3DXCOLOR(0.2f, 0.6f, 0.2f, 1.0f));
			}
			else
			{
				renderPrimitives[curToken].SetDiffuseColor(D3DXCOLOR(0.6f, 0.6f, 0.2f, 1.0f));
			}
			
			curToken++;
			geom = particles[i]->GetNextGeometry();
		}
	}

	for (int i = 0; i < NUMBER_OF_TREES; i++)
	{
		trees[i]->BeginIterateGeometry();

		neGeometry * geom = trees[i]->GetNextGeometry();
		s32 b = 0;
		while (geom)
		{
			{		
				neV3 scale;
				if (geom->GetBoxSize(scale))
				{
					renderPrimitives[curToken].SetGraphicBox(scale.X(),scale.Y(),scale.Z());				
				}
				else
				{
					f32 height, diameter;

					if (geom->GetCylinder(diameter, height))
					{	
						renderPrimitives[curToken].SetGraphicCylinder(diameter/2.0f,height);
					}
					else if (geom->GetSphereDiameter(diameter))
					{
						renderPrimitives[curToken].SetGraphicSphere(diameter/2.0);
					}
				}

				geom->SetUserData((u32)&renderPrimitives[curToken]);

				if (geom->GetBreakageFlag() == neGeometry::NE_BREAK_DISABLE)
				{
					renderPrimitives[curToken].SetDiffuseColor(D3DXCOLOR(treeData[b].colour[0], 
						treeData[b].colour[1], 
						treeData[b].colour[2], 
						1.0f));
				}
				else if (geom->GetBreakageFlag() == neGeometry::NE_BREAK_NORMAL)
				{
					renderPrimitives[curToken].SetDiffuseColor(D3DXCOLOR(treeData[b].colour[0], 
						treeData[b].colour[1], 
						treeData[b].colour[2], 
						1.0f));
				}
				


			
			}
			curToken++;
			geom = trees[i]->GetNextGeometry();

			b++;
		}
	}
	nextFreeRenderPrimitive = curToken;
}
void CSampleSCCollision::DisplayRigidBodies(neRigidBody ** rb, s32 count , IDirect3DDevice9* pd3dDevice)
{
	while (count-- && *rb)
	{
		neRigidBody * body = *rb;

		neT3 trans = body->GetTransform();

		body->BeginIterateGeometry();

		neGeometry * geom = body->GetNextGeometry();

		while (geom)
		{
			CRenderPrimitive * t = (CRenderPrimitive *)geom->GetUserData();

			if (t)
			{
				neT3 geomTrans = geom->GetTransform();

				neT3 worldTrans = trans * geomTrans;

				worldTrans.MakeD3DCompatibleMatrix();

				t->Render(pd3dDevice,&worldTrans);			

			}
			geom = body->GetNextGeometry();
		}
		rb++;
	}
}

void CSampleSCCollision::DisplayAnimatedBodies(neAnimatedBody ** ab, s32 count , IDirect3DDevice9* pd3dDevice)
{
	while (count-- && *ab)
	{
		neAnimatedBody * body = *ab;

		neT3 trans = body->GetTransform();

		body->BeginIterateGeometry();

		neGeometry * geom = body->GetNextGeometry();

		while (geom)
		{
			CRenderPrimitive * t = (CRenderPrimitive *)geom->GetUserData();

			if (t)
			{
				neT3 geomTrans = geom->GetTransform();

				neT3 worldTrans = trans * geomTrans;

				worldTrans.MakeD3DCompatibleMatrix();

				t->Render(pd3dDevice,&worldTrans);

			}
			geom = body->GetNextGeometry();
		}
		ab++;
	}
}
void CSampleSCCollision::InititialisePhysics()
{

	for (s32 i = 0; i < NUMBER_OF_BODIES; i++)
	{
		branches[i] = NULL;
	}
	neV3 gravity; gravity.Set(0.0f, -10.0f, 0.0f);

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidBodiesCount = NUMBER_OF_TREES * BRANCH_PER_TREE;
	sizeInfo.rigidParticleCount = NUMBER_OF_PARTICLES;
	sizeInfo.animatedBodiesCount = WALL_NUMBER + NUMBER_OF_TREES;
	sizeInfo.geometriesCount = NUMBER_OF_TOKENS + WALL_NUMBER;

	{ //dont need any of these

		sizeInfo.constraintsCount = 0;
	}

	s32 totalBody = sizeInfo.rigidBodiesCount + sizeInfo.animatedBodiesCount;

	sizeInfo.overlappedPairsCount = totalBody * (totalBody - 1) / 2;

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

	sim->SetBreakageCallback(BreakageCallbackFn);

	neV3 position;

	position.SetZero();

	for (s32 j = 0; j < NUMBER_OF_PARTICLES; j++)
	{
		position.Set(2.0f * j, 2.0f, 100.0f);

		MakeParticle(position, j);	
	}
	for (s32 j = 0; j < NUMBER_OF_TREES; j++)
	{
		MakeTree(treePosition[j], j);	
	}

	//SetUpTerrain

	terrainRender.SetGraphicMesh(L"model\\landscape2.x");
	terrainRender.SetDiffuseColor(D3DXCOLOR(0.1f,0.5f,0.1f,1.0f));

	LPD3DXMESH lpterrainD3Dmesh = terrainRender.mMesh.GetMesh();

	neTriangleMesh triMesh;

	triMesh.vertexCount = lpterrainD3Dmesh->GetNumVertices();

	triMesh.triangleCount = lpterrainD3Dmesh->GetNumFaces();	

	neV3 * verts = new neV3[triMesh.vertexCount];

	//
	DWORD dwFVF			= lpterrainD3Dmesh->GetFVF();
	DWORD dwOptions		= lpterrainD3Dmesh->GetOptions();
	DWORD dwNumFaces	= lpterrainD3Dmesh->GetNumFaces();
	DWORD dwNumVertices = lpterrainD3Dmesh->GetNumVertices();
	DWORD dwBytes		= lpterrainD3Dmesh->GetNumBytesPerVertex();

	LPDIRECT3DVERTEXBUFFER9 pVB;
	lpterrainD3Dmesh->GetVertexBuffer(&pVB);

	byte* pBuffer;
	pVB->Lock(0, 0, (void**)&pBuffer, 0);

	byte* pPointer = pBuffer;

	for (int i = 0;i< triMesh.vertexCount;i++)
	{
		if (dwFVF & D3DFVF_XYZ)
		{
			D3DVECTOR *d3dvector;
			d3dvector = (D3DVECTOR*)pPointer;
			verts[i].Set(d3dvector->x,d3dvector->y,d3dvector->z);
			pPointer += sizeof(D3DVECTOR);
		}
		//if (dwFVF & D3DFVF_NORMAL)
		//{
		//	//don't care the NORMAL data
		//	pPointer += sizeof(D3DVECTOR);
		//}
		//if (dwFVF & D3DFVF_TEX1)
		//{
		//	pPointer += 8;
		//}
		pPointer += dwBytes - sizeof(D3DVECTOR);
		

	}


	pVB->Unlock();
	pVB->Release();

	//

	triMesh.vertices = verts;

	neTriangle * tri = new neTriangle[triMesh.triangleCount];

	s32 * triindex = new s32[triMesh.triangleCount * 3];

	//

	LPDIRECT3DINDEXBUFFER9 pIB;
	lpterrainD3Dmesh->GetIndexBuffer(&pIB);

	D3DINDEXBUFFER_DESC kDesc;
	pIB->GetDesc(&kDesc);

	dwBytes = 0;
	if (kDesc.Format & D3DFMT_INDEX16)
	{
		dwBytes = 2 * sizeof(byte);
	}
	else if (kDesc.Format & D3DFMT_INDEX32)
	{
		dwBytes = 4 * sizeof(byte);
	}

	pIB->Lock(0, 0, (void**)&pBuffer, 0);

	pPointer = pBuffer;
	while ((pPointer - pBuffer) < kDesc.Size)
	{
		if (dwBytes == 2*sizeof(byte))
		{
			//16bit
			triindex[(pPointer-pBuffer)/dwBytes] = *((s16*)pPointer);
		}
		else if (dwBytes == 4*sizeof(byte))
		{
			//32bit
			triindex[(pPointer-pBuffer)/dwBytes] = *((s32*)pPointer);
		}
		pPointer += dwBytes;
	}

	pIB->Unlock();
	pIB->Release();

	//

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

	//SetUpRoom

	ground = sim->CreateAnimatedBody();

	neGeometry * geom = ground->AddGeometry();	 

	geom->SetBoxSize(gFloor.boxSize);

	ground->UpdateBoundingInfo();

	ground->SetPos(gFloor.pos);

	groundRender.SetGraphicBox(gFloor.boxSize[0], gFloor.boxSize[1], gFloor.boxSize[2]);


}


void MyAppInit()
{
	// TODO: Perform any application-level initialization here



	D3DXVECTOR3 vecEye (-10.0f, 25.0f, 40.0f);
	D3DXVECTOR3 vecAt (0.0f, 10.0f, 1.0f);
	g_Camera.SetViewParams( &vecEye, &vecAt );

	g_Camera.SetEnableYAxisMovement( true );
	g_Camera.SetRotateButtons( false, false, true );
	g_Camera.SetScalers( 0.01f, 50.0f );



	for (s32 i = 0; i < NUM_LIGHT; i++)
		D3DXVec4Normalize(&vLightWorld[i], &vLightWorld[i]);

	//OnMyAppDestroyDevice(g_pD3dDevice);

	sample.Initialise();

};

void CALLBACK OnMyAppFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{


	DWORD dwResult;    

	XINPUT_STATE state;

	ZeroMemory( &state, sizeof(XINPUT_STATE) );

	// Simply get the state of the controller from XInput.
	dwResult = XInputGetState( 0, &state );

	////////////////////////////////////////////////////////
	sample.Process(state , fTime);

	if (!sample.paused)
	{
		sample.sim->Advance(1.0f / 60.0f,1);
	}
}

void CALLBACK OnMyAppFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	neT3 terrainTrans;
	terrainTrans.SetIdentity();
	terrainTrans.MakeD3DCompatibleMatrix();
	sample.terrainRender.Render(pd3dDevice,&terrainTrans);

	neT3 t;
	t = sample.ground->GetTransform();
	t.MakeD3DCompatibleMatrix();
	sample.groundRender.Render(pd3dDevice,&t);


	sample.DisplayRigidBodies(sample.particles,CSampleSCCollision::NUMBER_OF_PARTICLES,pd3dDevice);
	sample.DisplayRigidBodies(sample.branches, CSampleSCCollision::NUMBER_OF_TREES * CSampleSCCollision::BRANCH_PER_TREE,pd3dDevice);
	sample.DisplayAnimatedBodies(sample.trees, CSampleSCCollision::NUMBER_OF_TREES,pd3dDevice);




	WCHAR * str[8];

	str[0] = L"Tokamak demo - Sphere and Cylinder collision Demo - (c) 2010 Tokamak Ltd";
	str[1] = L"Controls:";
	str[2] = L"'P' -> pause/unpause the simulation";
	str[3] = L"'T' -> Fire particles";	
	str[4] = L"100 sphere rigid particles interact with arbitrary landscape mesh.";
	str[5] = L"Trees constructed with cylinders and branches are specified";
	str[6] = L"to break off when hit.";
	str[7] = L"";



	MyRenderText(str, 8);
}

LRESULT CALLBACK MyAppMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
							  bool* pbNoFurtherProcessing, void* pUserContext )
{
	return 0;
}

void CALLBACK OnMyAppDestroyDevice( void* pUserContext )
{

	SAFE_RELEASE( g_pEffect );

	sample.Shutdown();


}

void CALLBACK MyAppKeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	if( bKeyDown )
	{
		switch( nChar )
		{
		case 'R':
			{
				//OnMyAppDestroyDevice(g_pD3dDevice);
			}
			break;

		default:
			break;
		}
	}
}

