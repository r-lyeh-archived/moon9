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



class CSampleRigidParticlesAndTerrain
{
public:
	CSampleRigidParticlesAndTerrain() { 
		paused = false;
	}

	void Initialise();

	void Shutdown();

	void Process(XINPUT_STATE & InputState ,double fTime);

	void InititialisePhysics();

	void MakeParticle(neV3 position, s32 index);


public:
	enum
	{
		NUMBER_OF_PARTICLES = 100,

		GEOMETRY_PER_BODY = 1,
	};

	neSimulator * sim;


	neRigidBody* box[NUMBER_OF_PARTICLES];

	CRenderPrimitive boxRenderPrimitives[NUMBER_OF_PARTICLES * GEOMETRY_PER_BODY];


	neAllocatorDefault all;

	nePerformanceReport perfReport;

	bool paused;

	CRenderPrimitive groundRender;
	neAnimatedBody * ground;

	CRenderPrimitive terrainRender;

	s32 nextBullet;
};

CSampleRigidParticlesAndTerrain sample;

void CSampleRigidParticlesAndTerrain::Initialise() 
{
	InititialisePhysics();
	nextBullet = 0;
}

void CSampleRigidParticlesAndTerrain::Process(XINPUT_STATE & InputState ,double fTime)
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

		box[nextBullet]->SetPos(pos);

		box[nextBullet]->SetVelocity(dir * 20.0f);

		nextBullet = (nextBullet + 1) % NUMBER_OF_PARTICLES;

		mTimeUntilNextToggle = 10;
		
	}

	
	


}


void CSampleRigidParticlesAndTerrain::Shutdown() 
{
	sample.groundRender.mMesh.Destroy();
	sample.terrainRender.mMesh.Destroy();

	for (s32 i = 0; i < sample.NUMBER_OF_PARTICLES; i++)
	{
		sample.boxRenderPrimitives[i].mMesh.Destroy();
	}

	sim->FreeTerrainMesh();

	neSimulator::DestroySimulator(sim);



	sim = NULL;
}



void CSampleRigidParticlesAndTerrain::MakeParticle(neV3 position, s32 index)
{
	//const f32 groundLevel = -10.0f;

	s32 cur;

	cur = index;

	box[cur] = sim->CreateRigidParticle();

	neGeometry * geom = box[cur]->AddGeometry();

	neV3 boxSize1; boxSize1.Set(1.f, 1.f, 1.f);

	geom->SetBoxSize(boxSize1[0],boxSize1[1],boxSize1[2]);

	box[cur]->UpdateBoundingInfo();

	f32 mass = 0.1f;

	box[cur]->SetInertiaTensor(neBoxInertiaTensor(boxSize1[0], boxSize1[1], boxSize1[2], mass));

	box[cur]->SetMass(mass);

	box[cur]->SetPos(position);

	//graphic
	boxRenderPrimitives[cur].SetGraphicBox(boxSize1[0],boxSize1[1],boxSize1[2]);


	if ((cur % 4) == 0)
	{
		boxRenderPrimitives[cur].SetDiffuseColor(D3DXCOLOR(0.8f, 0.2f, 0.2f, 1.0f));

	}
	else if ((cur % 4) == 1)
	{
		boxRenderPrimitives[cur].SetDiffuseColor(D3DXCOLOR(0.2f, 0.4f, 0.6f, 1.0f));

	}
	else if ((cur % 4) == 2)
	{
		boxRenderPrimitives[cur].SetDiffuseColor(D3DXCOLOR(0.2f, 0.6f, 0.2f, 1.0f));
	}
	else
	{
		boxRenderPrimitives[cur].SetDiffuseColor(D3DXCOLOR(0.6f, 0.6f, 0.2f, 1.0f));
	}

}

void CSampleRigidParticlesAndTerrain::InititialisePhysics()
{
	neV3 gravity; gravity.Set(0.0f, -8.0f, 0.0f);

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidParticleCount = NUMBER_OF_PARTICLES;
	sizeInfo.animatedBodiesCount = WALL_NUMBER;
	sizeInfo.geometriesCount = NUMBER_OF_PARTICLES * GEOMETRY_PER_BODY + WALL_NUMBER;

	{ //dont need any of these
		sizeInfo.rigidBodiesCount = 0;
		sizeInfo.constraintsCount = 0;
	}

	s32 totalBody = NUMBER_OF_PARTICLES + WALL_NUMBER;
	sizeInfo.overlappedPairsCount = totalBody * (totalBody - 1) / 2;

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

	neV3 position;

	position.SetZero();

	for (s32 j = 0; j < NUMBER_OF_PARTICLES; j++)
	{
		position.Set(0.0f, 2.0f * j + 20.0f, 0.0f);
		//position.Set(13.5f, 20.0f, 1.5f);

		MakeParticle(position, j);	
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
	D3DXVECTOR3 vecAt (0.0f, 0.0f, 1.0f);
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



	// Display the boxes

	for (s32 i = 0; i < sample.NUMBER_OF_PARTICLES; i++)
	{
		t = sample.box[i]->GetTransform();
		t.MakeD3DCompatibleMatrix();
		sample.boxRenderPrimitives[i].Render(pd3dDevice,&t);
	}


	WCHAR * str[8];

	str[0] = L"Tokamak demo - Rigid Particles and Terrain Demo - (c) 2010 Tokamak Ltd";
	str[1] = L"Controls:";
	str[2] = L"'P' -> pause/unpause the simulation";
	str[3] = L"'T' -> Fire particles";	
	str[4] = L"100 rigid particles interact with arbitrary landscape mesh.";
	str[5] = L"Rigid particles are light-weight rigid bodies which doesn't";
	str[6] = L"collide with each other, designed for particle effects";
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

