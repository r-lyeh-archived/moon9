#include "dxut\dxstdafx.h"
#include "resource.h"
#include "d3dapp.h"
#include "tokamaksampleApp.h"

#define PYRAMID
//#define COLLISION_CALLBACK_ON

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



class CSampleStackingObjects
{
public:
	CSampleStackingObjects() { 
		paused = false;
	}

	void Initialise();

	void Shutdown();

	void Process(XINPUT_STATE & InputState ,double fTime);

	void InititialisePhysics();

	void MakeStack(neV3 position, s32 & index);

	void MakeBullet(int);

	void Reset();


public:
	enum
	{
		STACK_HEIGHT = 10,//4,

		N_STACKS = 5,

		N_DEPTH = 5,

		N_BULLET = 5,

#ifdef PYRAMID
		N_BODY = 55 + N_BULLET,
#else
		N_BODY = STACK_HEIGHT * N_STACKS * N_DEPTH + N_BULLET,
#endif
	};

	neSimulator * sim;

	neRigidBody * box[N_BODY];

	CRenderPrimitive boxRenderPrimitives[N_BODY];


	neAllocatorDefault all;

	nePerformanceReport perfReport;

	bool paused;

	CRenderPrimitive groundRender;
	neAnimatedBody * ground;
};

CSampleStackingObjects sample;

void CSampleStackingObjects::Initialise() 
{
	InititialisePhysics();
}

void CSampleStackingObjects::Process(XINPUT_STATE & InputState ,double fTime)
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


	if (GetAsyncKeyState('R'))
	{
		Reset();
		return;
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

		pos = pos + dir * 3.0f;

		s32 bullet = N_BODY - N_BULLET + nextBullet;

		box[bullet]->SetPos(pos);

		box[bullet]->SetVelocity(dir * 40.0f);

		nextBullet = (nextBullet + 1) % N_BULLET;

		mTimeUntilNextToggle = 500;
	}

	
	


}


void CSampleStackingObjects::Shutdown() 
{
	sample.groundRender.mMesh.Destroy();

	for (s32 i = 0; i < sample.N_BODY; i++)
	{
		sample.boxRenderPrimitives[i].mMesh.Destroy();
	}

	neSimulator::DestroySimulator(sim);

	sim = NULL;
}

void CSampleStackingObjects::Reset()
{
	Shutdown();

	InititialisePhysics();
}

void CSampleStackingObjects::MakeStack(neV3 position, s32 & index)
{
	const f32 groundLevel = -10.0f;

	s32 cur = 0;;

#ifndef PYRAMID 

	for (s32 i = 0; i < STACK_HEIGHT; i++)
	{
		cur = index + i;

		box[cur] = sim->CreateRigidBody();

		neGeometry * geom = box[cur]->AddGeometry();

		neV3 boxSize1; boxSize1.Set(0.8f, 0.8f, 0.8f);
		//neV3 boxSize1; boxSize1.Set(0.3, 1.9, 0.3);

		geom->SetBoxSize(boxSize1[0],boxSize1[1],boxSize1[2]);

		box[cur]->UpdateBoundingInfo();

		f32 mass = 1.0f;

		box[cur]->SetInertiaTensor(neBoxInertiaTensor(boxSize1[0], boxSize1[1], boxSize1[2], mass));

		box[cur]->SetMass(mass);

		neV3 pos;

		pos.Set(position[0], groundLevel + boxSize1[1] * (i + 0.5f) + 0.0f, position[2]);

		box[cur]->SetPos(pos);

		boxRenderPrimitives[cur].SetGraphicBox(boxSize1[0],boxSize1[1],boxSize1[2]);

		if ((cur % 4) == 0)
		{
			boxRenderPrimitives[cur].SetDiffuseColor(D3DXCOLOR(0.8f, 0.2f, 0.2f, 1.0f));
		}
		else if ((cur % 4) == 1)
		{
			boxRenderPrimitives[cur].SetDiffuseColor(D3DXCOLOR(0.2f, 0.1f, 0.8f, 1.0f));
		}
		else if ((cur % 4) == 2)
		{
			boxRenderPrimitives[cur].SetDiffuseColor(D3DXCOLOR(0.0f, 0.8f, 0.0f, 1.0f));
		}
		else
		{
			boxRenderPrimitives[cur].SetDiffuseColor(D3DXCOLOR(0.8f, 0.8f, 0.2f, 1.0f));
		}

		box[cur]->SetUserData(reinterpret_cast<u32>(&boxRenderPrimitives[cur]));
	}
	index = cur+1;

#else

	neV3 boxRadii; boxRadii.Set(.5f, .5f, .5f);
	neV3 boxSize; boxSize = boxRadii * 2.f;
	f32 stepX    = boxSize[0] + 0.05f;
	s32 baseSize = 10;

	neV3 startPos; startPos.Set( 0.0f, groundLevel + boxSize[1] * 0.5f, 0.0f);

	int k = 0;

	for(int i=0; i<baseSize; i++)
	{
		int rowSize = baseSize - i;

		neV3 start; start.Set(-rowSize * stepX * 0.5f + stepX * 0.5f, i * boxSize[1], 0);

		for(int j=0; j< rowSize; j++)
		{
			neV3 boxPos; boxPos.Set(start);

			neV3 shift; shift.Set(j * stepX, 0.0f, 0.0f);

			boxPos += shift;
			boxPos += startPos;

			cur = index + k;

			box[cur] = sim->CreateRigidBody();

			neGeometry * geom = box[cur]->AddGeometry();

			geom->SetBoxSize(boxSize[0],boxSize[1],boxSize[2]);

			box[cur]->UpdateBoundingInfo();

			f32 mass = 1.f;

			box[cur]->SetInertiaTensor(neBoxInertiaTensor(boxSize[0], boxSize[1], boxSize[2], mass));

			box[cur]->SetMass(mass);

			box[cur]->SetPos(boxPos);

			boxRenderPrimitives[cur].SetGraphicBox(boxSize[0],boxSize[1],boxSize[2]);

			if ((cur % 4) == 0)
			{
				boxRenderPrimitives[cur].SetDiffuseColor(D3DXCOLOR(0.8f, 0.2f, 0.2f, 1.0f));
			}
			else if ((cur % 4) == 1)
			{
				boxRenderPrimitives[cur].SetDiffuseColor(D3DXCOLOR(0.2f, 0.1f, 0.8f, 1.0f));
			}
			else if ((cur % 4) == 2)
			{
				boxRenderPrimitives[cur].SetDiffuseColor(D3DXCOLOR(0.0f, 0.8f, 0.0f, 1.0f));
			}
			else
			{
				boxRenderPrimitives[cur].SetDiffuseColor(D3DXCOLOR(0.8f, 0.8f, 0.2f, 1.0f));
			}

			box[cur]->SetUserData(reinterpret_cast<u32>(&boxRenderPrimitives[cur]));

			k++;
		}
	}
	index = cur+1;
#endif
}

void CSampleStackingObjects::MakeBullet(s32 index)
{

	neV3 boxSize;
	boxSize.Set(1.0f,1.0f,1.0f);

	s32 cur = N_BODY - 1 - index;

	box[cur] = sim->CreateRigidBody();

	neGeometry * geom = box[cur]->AddGeometry();

	geom->SetBoxSize(boxSize);

	

	box[cur]->UpdateBoundingInfo();

	f32 mass = 0.6f;

	box[cur]->SetInertiaTensor(neBoxInertiaTensor(boxSize, mass));

	box[cur]->SetMass(mass);

	neV3 pos;

	pos.Set(-10.0f, -8.75f + index * 3.0f+3, 0.0f);

	box[cur]->SetPos(pos);

	//graphic
	boxRenderPrimitives[cur].SetGraphicBox(boxSize[0],boxSize[1],boxSize[2]);
	boxRenderPrimitives[cur].SetDiffuseColor(D3DXCOLOR(0.8f,0.8f,0.8f,1.0f));

	box[cur]->SetUserData(reinterpret_cast<u32>(&boxRenderPrimitives[cur]));

}

void CollisionCallback (neCollisionInfo & collisionInfo)
{
	if (collisionInfo.typeA == NE_RIGID_BODY)
	{
		neRigidBody * rbA = (neRigidBody *)collisionInfo.bodyA;

		s32 data = rbA->GetUserData();

		if (data == 0)
		{
			return;
		}

		CRenderPrimitive* render = reinterpret_cast<CRenderPrimitive*>(data);
		render->SetDiffuseColor(D3DXCOLOR(1.0,0,0,1.0f));
	}
	if (collisionInfo.typeB == NE_RIGID_BODY)
	{
		neRigidBody * rbB = (neRigidBody *)collisionInfo.bodyB;

		s32 data = rbB->GetUserData();

		if (data == 0)
		{
			return;
		}

		CRenderPrimitive* render = reinterpret_cast<CRenderPrimitive*>(data);
		render->SetDiffuseColor(D3DXCOLOR(0,0.5f,0.5,1.0f));
	}
}

void CSampleStackingObjects::InititialisePhysics()
{
	neV3 gravity; gravity.Set(0.0f, -9.0f, 0.0f);

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidBodiesCount = N_BODY;
	sizeInfo.animatedBodiesCount = WALL_NUMBER;
	sizeInfo.geometriesCount = N_BODY + WALL_NUMBER;
	s32 totalBody = N_BODY + WALL_NUMBER;
	sizeInfo.overlappedPairsCount = totalBody * (totalBody - 1) / 2;
	{ //dont need any of these
		sizeInfo.rigidParticleCount = 0;
		sizeInfo.constraintsCount = 0;
		sizeInfo.terrainNodesStartCount = 0;
	}

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

#ifdef COLLISION_CALLBACK_ON

	sim->GetCollisionTable()->Set(0, 0, neCollisionTable::RESPONSE_IMPULSE_CALLBACK);

	sim->SetCollisionCallback(CollisionCallback);

#endif

	neV3 position;

	position.SetZero();

	s32 i = 0;

#ifdef PYRAMID

	MakeStack(position, i);	

#else
	f32 gap = 2.f;

	for (s32 j = 0; j < N_STACKS; j++)
	{
		//position.Set(3.05f * j, 0.0f, 0.0f);

		for (s32 k = 0; k < N_DEPTH; k++)
		{
			position.Set(gap * j, 0.0f, gap * k);

			MakeStack(position, i);	
		}
	}
#endif

	for (s32 jj = 0; jj < N_BULLET; jj++)
		MakeBullet(jj);


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



	D3DXVECTOR3 vecEye (-10.0f, 5.0f, 40.0f);
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

	neT3 t;
	t = sample.ground->GetTransform();
	t.MakeD3DCompatibleMatrix();
	sample.groundRender.Render(pd3dDevice,&t);

	// Display the boxes

	for (s32 i = 0; i < sample.N_BODY; i++)
	{
		t = sample.box[i]->GetTransform();
		t.MakeD3DCompatibleMatrix();
		sample.boxRenderPrimitives[i].Render(pd3dDevice,&t);
	}

	WCHAR * str[7];

	str[0] = L"Tokamak demo - stacking of objects - (c) 2010 Tokamak Ltd";
	str[1] = L"Controls:";
	str[2] = L"'P' -> pause/unpause the simulation";
	str[3] = L"'T' -> Fire block";	
	str[4] = L"'R' -> Reset";
	str[5] = L"Demonstration of fast and stable stacking.";
	str[6] = L"";



	MyRenderText(str, 7);
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

