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


neV3 WheelPositions[8] = 
{
	// Left
	{-3.4f, 0.8f, -1.75f},
	{-1.0f, 0.8f, -1.75f},
	{1.0f, 0.8f, -1.75f},
	{3.4f, 0.8f, -1.75f},
	// Right
	{-3.4f, 0.8f, 1.75f},
	{-1.0f, 0.8f, 1.75f},
	{1.0f, 0.8f, 1.75f},
	{3.4f, 0.8f, 1.75f}
};

#define WHEEL_DIAMETER (2.0f)
#define WHEEL_WIDTH (0.3f)
#define MAX_MOTOR (100.0f)
#define TANK_MASS (300.0f)
#define WHEEL_MASS (2.0f)

class CSampleTank
{
public:
	CSampleTank() { 
		paused = false;
	}

	void Initialise();

	void Shutdown();

	void Process(XINPUT_STATE & InputState ,double fTime);

	void InititialisePhysics();

	void Reset();

	void DisplayRigidBodies(neRigidBody ** rb, s32 count , IDirect3DDevice9* pd3dDevice);
	void DisplayAnimatedBodies(neAnimatedBody ** ab, s32 count , IDirect3DDevice9* pd3dDevice);



public:
	enum
	{
		N_BODY =9,

		N_TOTAL_BODIES = N_BODY,
	};

	neSimulator * sim;

	neRigidBody * rigidBodies[N_TOTAL_BODIES];

	CRenderPrimitive renderPrimitives[N_TOTAL_BODIES];

	neJoint * lastJoint;

	neAllocatorDefault all;

	nePerformanceReport perfReport;

	bool paused;

	CRenderPrimitive groundRender;
	neAnimatedBody * ground;
};

CSampleTank sample;

void CSampleTank::DisplayRigidBodies(neRigidBody ** rb, s32 count , IDirect3DDevice9* pd3dDevice)
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

void CSampleTank::DisplayAnimatedBodies(neAnimatedBody ** ab, s32 count , IDirect3DDevice9* pd3dDevice)
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
void CSampleTank::Initialise() 
{
	InititialisePhysics();
}

void CSampleTank::Process(XINPUT_STATE & InputState ,double fTime)
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


	s32 actOnBody = 0;//N_TOTAL_BODIES - 1;// -20;

	if (GetAsyncKeyState('T') && mTimeUntilNextToggle <= 0)
	{
		neV3 vel;

		vel.Set (0.0f, 30.0f, 0.0f);

		rigidBodies[actOnBody]->SetVelocity(vel);

		mTimeUntilNextToggle = 40;
	}


}


void CSampleTank::Shutdown() 
{
	sample.groundRender.mMesh.Destroy();

	for (s32 i = 0; i < sample.N_TOTAL_BODIES; i++)
	{
		sample.renderPrimitives[i].mMesh.Destroy();
	}

	neSimulator::DestroySimulator(sim);

	sim = NULL;
}

void CSampleTank::Reset()
{
	Shutdown();

	InititialisePhysics();
}


void CSampleTank::InititialisePhysics()
{

	f32 ypos = -7;

	neV3 gravity; gravity.Set(0.0f, -9.0f, 0.0f);
	//neV3 gravity; gravity.Set(-9.0f, 0.0f, 0.0f);

	//f32 linkLength = 2.2f;
	f32 linkLength = -0.0f;

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidBodiesCount = N_TOTAL_BODIES;
	sizeInfo.animatedBodiesCount = WALL_NUMBER;
	sizeInfo.geometriesCount = N_TOTAL_BODIES + WALL_NUMBER;
	sizeInfo.overlappedPairsCount = 2000;//MAX_OVERLAPPED_PAIR;

	{ //dont need any of these
		sizeInfo.terrainNodesStartCount = 0;
		sizeInfo.rigidParticleCount = 0;
	}

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

	//create the kart body

	f32 mass = TANK_MASS;

	rigidBodies[0] = sim->CreateRigidBody();

	rigidBodies[0]->SetMass(mass);

	neGeometry * geom = rigidBodies[0]->AddGeometry();

	neV3 boxSize; boxSize.Set(6.5f, 2.9f, 3.2f);

	geom->SetBoxSize(boxSize);

	neT3 hullTrans; 

	hullTrans.SetIdentity();

	hullTrans.pos.Set(0.0f, 1.5, 0.0f);

	geom->SetTransform(hullTrans);

	rigidBodies[0]->UpdateBoundingInfo();

	rigidBodies[0]->SetInertiaTensor(neBoxInertiaTensor(6.91f,3.5f,3.6f, mass));

	rigidBodies[0]->SetMass(mass);

	//rigidBodies[0]->GravityEnable(false);

	rigidBodies[0]->CollideConnected(false);

	rigidBodies[0]->SetSleepingParameter (0.01f);

	//create the 4 wheels (sphere)

	s32 j = 1;

	ypos -= 2.0f;

	for (int i = 0; i < 8; i++)
	{
		neRigidBody * rb;

		rigidBodies[j++] = rb = sim->CreateRigidBody();

		geom = rb->AddGeometry();

		geom->SetCylinder(WHEEL_DIAMETER, WHEEL_WIDTH);

		neV3 rotate; rotate.Set(NE_PI/2.0f, 0.0f, 0.0f);

		neT3 trans; trans.SetIdentity();

		trans.rot.RotateXYZ(rotate);

		rb->SetRotation(trans.rot);

		rb->SetMass(WHEEL_MASS);

		rb->UpdateBoundingInfo();

		rb->SetInertiaTensor(neCylinderInertiaTensor(WHEEL_DIAMETER, WHEEL_WIDTH, mass));

		rb->SetPos(WheelPositions[i]);

		//rb->GravityEnable(false);

		rb->CollideConnected(false);

		neJoint * joint = sim->CreateJoint(rigidBodies[0], rb);

		joint->SetIteration(4);

		neT3 jointFrame;

		jointFrame.rot[0].Set(1.0f, 0.0f, 0.0f);
		jointFrame.rot[1].Set(0.0f, 0.0f, -1.0f);
		jointFrame.rot[2].Set(0.0f, 1.0f, 0.0f);
		jointFrame.pos = WheelPositions[i];

		joint->SetJointFrameWorld(jointFrame);
		joint->SetType(neJoint::NE_JOINT_HINGE);
		joint->Enable(true);
		joint->EnableMotor(true);
		joint->SetMotor(neJoint::NE_MOTOR_SPEED, 10.1f, 50.f);
	}


	for (int i = 0; i < N_BODY; i++)
	{
		rigidBodies[i]->BeginIterateGeometry();

		neGeometry * geom = rigidBodies[i]->GetNextGeometry();


		neV3 scale;
		if (geom->GetBoxSize(scale))
		{
			renderPrimitives[i].SetGraphicBox(scale.X(),scale.Y(),scale.Z());				
		}
		else
		{
			f32 height, diameter;

			if (geom->GetCylinder(diameter, height))
			{	
				renderPrimitives[i].SetGraphicCylinder(diameter/2.0f,height);
			}
			else if (geom->GetSphereDiameter(diameter))
			{
				renderPrimitives[i].SetGraphicSphere(diameter/2.0);
			}
		}

		geom->SetUserData((u32)&renderPrimitives[i]);
	}


	//SetUpRoom
	{

		ground = sim->CreateAnimatedBody();

		neGeometry * geom = ground->AddGeometry();	 

		geom->SetBoxSize(gFloor.boxSize);

		ground->UpdateBoundingInfo();

		ground->SetPos(gFloor.pos);

		groundRender.SetGraphicBox(gFloor.boxSize[0], gFloor.boxSize[1], gFloor.boxSize[2]);

	}

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

	sample.DisplayRigidBodies(sample.rigidBodies, CSampleTank::N_TOTAL_BODIES,pd3dDevice);

	WCHAR * str[7];

	str[0] = L"Tokamak demo - Hinge joint with Motor turned on - (c) 2010 Tokamak Ltd";
	str[1] = L"Controls:";
	str[2] = L"'P' -> pause/unpause the simulation";
	str[3] = L"'T' -> lift";	
	str[4] = L"'R' -> Reset";
	str[5] = L"This sample demonstrate the hinge joint motor.";
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

