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


class CSampleBreakageObjects
{
public:
	CSampleBreakageObjects() { 
		paused = false;
	}

	void Initialise();

	void Shutdown();

	void Process(XINPUT_STATE & InputState ,double fTime);

	void InititialisePhysics();

	void MakeTable(neV3 position, s32 index);


public:
	enum
	{
		GEOMETRY_PER_TABLE = 5,

		NUMBER_OF_TABLES = 15,

		NUMBER_OF_BODIES = NUMBER_OF_TABLES * GEOMETRY_PER_TABLE,
	};

	neSimulator * sim;

	neRigidBody * rigidBodies[NUMBER_OF_BODIES];

	CRenderPrimitive boxRenderPrimitives[NUMBER_OF_BODIES];


	neAllocatorDefault all;

	nePerformanceReport perfReport;

	bool paused;

	CRenderPrimitive groundRender;
	neAnimatedBody * ground;

	s32 currentBodyCount;
};

CSampleBreakageObjects sample;

void CSampleBreakageObjects::Initialise() 
{
	InititialisePhysics();

	currentBodyCount = NUMBER_OF_TABLES;
}

void CSampleBreakageObjects::Process(XINPUT_STATE & InputState ,double fTime)
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


	neV3 force;

	neBool dontSet = false;

	f32 v = 5.0f;

	if (GetAsyncKeyState('T'))
	{
		force.Set (0.0f,v,0.0f);
	}
	else
	{
		dontSet = true;
	}

	if (!dontSet)
	{
		for (s32 i = 0; i < NUMBER_OF_TABLES; i++)
			rigidBodies[i]->SetVelocity(force);
	}


}


void CSampleBreakageObjects::Shutdown() 
{
	sample.groundRender.mMesh.Destroy();

	for (s32 i = 0; i < sample.NUMBER_OF_BODIES; i++)
	{
		sample.boxRenderPrimitives[i].mMesh.Destroy();
	}

	neSimulator::DestroySimulator(sim);

	sim = NULL;
}


void BreakageCallbackFn(neByte * originalBody, neBodyType bodyType, neGeometry * brokenGeometry, neRigidBody * newBody)
{
	sample.rigidBodies[sample.currentBodyCount] = (neRigidBody*)newBody;

	sample.currentBodyCount++;
}

void CSampleBreakageObjects::InititialisePhysics()
{
	neV3 gravity; gravity.Set(0.0f, -9.0f, 0.0f);

	neSimulatorSizeInfo sizeInfo;

	sizeInfo.rigidBodiesCount = NUMBER_OF_BODIES;
	sizeInfo.animatedBodiesCount = WALL_NUMBER;
	sizeInfo.geometriesCount = NUMBER_OF_BODIES + WALL_NUMBER;
	sizeInfo.overlappedPairsCount = MAX_OVERLAPPED_PAIR;//totalBody * (totalBody - 1) / 2;
	{ //dont need any of these
		sizeInfo.rigidParticleCount = 0;
		sizeInfo.constraintsCount = 0;
		sizeInfo.terrainNodesStartCount = 0;
	}

	sim = neSimulator::CreateSimulator(sizeInfo, &all, &gravity);

	sim->SetBreakageCallback(BreakageCallbackFn);

	for (s32 i = 0; i < NUMBER_OF_BODIES; i++)
	{
		rigidBodies[i] = NULL;
	}

	neV3 position;

	position.SetZero();

	for (s32 j = 0; j < NUMBER_OF_TABLES; j++)
	{
		position.Set(20.0f + 4.0f * j, 30.0f + 5.0f * j, 0.0f);

		MakeTable(position, j);	
	}


	//SetUpRoom

	ground = sim->CreateAnimatedBody();

	neGeometry * geom = ground->AddGeometry();	 

	geom->SetBoxSize(gFloor.boxSize);

	ground->UpdateBoundingInfo();

	ground->SetPos(gFloor.pos);

	groundRender.SetGraphicBox(gFloor.boxSize[0], gFloor.boxSize[1], gFloor.boxSize[2]);


}
struct TableData
{
	neV3 size;
	neV3 pos;
	neV3 colour;
	neGeometry::neBreakFlag breakFlag;
	f32 breakageMass;
	f32 breakageMagnitude;
	f32 breakageAbsorption;
};

TableData tableData[CSampleBreakageObjects::GEOMETRY_PER_TABLE] = 
{
	{{3.0f, 0.3f, 3.0f}, {0.0f, 0.0f, 0.0f}, {0.8f, 0.1f, 0.1f}, neGeometry::NE_BREAK_DISABLE, 2.0f, 0.0f, 0.0f},
	{{0.4f, 1.0f, 0.4f}, {1.3f, -0.65f, 1.3f}, {0.1f, 0.1f, 0.8f}, neGeometry::NE_BREAK_NORMAL, 0.4f, 5.0f, 0.3f},
	{{0.4f, 1.0f, 0.4f}, {-1.3f, -0.65f, 1.3f}, {0.1f, 0.1f, 0.8f}, neGeometry::NE_BREAK_NORMAL, 0.4f, 5.0f, 0.3f},
	{{0.4f, 1.0f, 0.4f}, {1.3f, -0.65f, -1.3f}, {0.1f, 0.1f, 0.8f}, neGeometry::NE_BREAK_NORMAL, 0.4f, 5.0f, 0.3f},
	{{0.4f, 1.0f, 0.4f}, {-1.3f, -0.65f, -1.3f}, {0.1f, 0.1f, 0.8f}, neGeometry::NE_BREAK_NORMAL, 0.4f, 5.0f, 0.3f},

};

void CSampleBreakageObjects::MakeTable(neV3 position, s32 index)
{
	//const f32 groundLevel = -10.0f;

	neV3 tableSize;

	tableSize.Set(3.0f, 1.0f, 3.0f);

	s32 cur;

	cur = index;

	rigidBodies[cur] = sim->CreateRigidBody();

	rigidBodies[cur]->SetInertiaTensor(neBoxInertiaTensor(tableSize, 2.0f));

	rigidBodies[cur]->SetMass(2.0f);

	//position[1] = groundLevel + 0.95f * (1 + cur) + (0.15f * cur);

	rigidBodies[cur]->SetPos(position);

	neQ rot;

	rot.X = ((f32)rand()) /RAND_MAX;
	rot.Y = ((f32)rand()) /RAND_MAX;
	rot.Z = ((f32)rand()) /RAND_MAX;
	rot.W = ((f32)rand()) /RAND_MAX;

	rot.Normalize();

	rigidBodies[cur]->SetRotation(rot);

	for (s32 i = 0; i < GEOMETRY_PER_TABLE; i++)
	{
		neGeometry * geom = rigidBodies[cur]->AddGeometry();

		geom->SetUserData((u32) (cur * GEOMETRY_PER_TABLE + i));

		geom->SetBoxSize(tableData[i].size);

		neT3 trans;

		trans.SetIdentity();

		trans.pos = tableData[i].pos;

		geom->SetTransform(trans);

		geom->SetBreakageFlag(tableData[i].breakFlag);

		geom->SetBreakageMass(tableData[i].breakageMass);

		geom->SetBreakageInertiaTensor(neBoxInertiaTensor(tableData[i].size, tableData[i].breakageMass));

		geom->SetBreakageMagnitude(tableData[i].breakageMagnitude);

		geom->SetBreakageAbsorption(tableData[i].breakageAbsorption);

		neV3 plane;

		plane.Set(0.0f, 1.0f, 0.0f);

		geom->SetBreakagePlane(plane);

		//graphic
		boxRenderPrimitives[geom->GetUserData()].SetGraphicBox(tableData[i].size.X(),tableData[i].size.Y(),tableData[i].size.Z());

		if (i % 3 == 0)
		{
			boxRenderPrimitives[geom->GetUserData()].SetDiffuseColor(D3DXCOLOR(0.8f, 0.2f, 0.2f, 1.0f));
		}
		else if (i % 3 == 1)
		{
			boxRenderPrimitives[geom->GetUserData()].SetDiffuseColor(D3DXCOLOR(0.3f,	0.3f, 1.0f, 1.0f));
		}
		else
		{
			boxRenderPrimitives[geom->GetUserData()].SetDiffuseColor(D3DXCOLOR(0.2f,	0.8f, 0.2f, 1.0f));
		}
	}
	rigidBodies[cur]->UpdateBoundingInfo();

}

void MyAppInit()
{
	// TODO: Perform any application-level initialization here


	D3DXVECTOR3 vecEye (-10.0f, 5.0f, 45.0f);
	D3DXVECTOR3 vecAt (47.0f, 0.0f, 1.0f);
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


	for (s32 i = 0; i < sample.currentBodyCount; i++)
	{
		neT3 trans = sample.rigidBodies[i]->GetTransform();

		sample.rigidBodies[i]->BeginIterateGeometry();

		neGeometry * geom = sample.rigidBodies[i]->GetNextGeometry();

		while (geom)
		{
			u32 index = geom->GetUserData();

			neT3 geomTrans = geom->GetTransform();

			neT3 worldTrans = trans * geomTrans;

			worldTrans.MakeD3DCompatibleMatrix();

			sample.boxRenderPrimitives[index].Render(pd3dDevice,&worldTrans);

			geom = sample.rigidBodies[i]->GetNextGeometry();
		}
	}


	//display text

	WCHAR * str[9];

	str[0] = L"Tokamak demo - build-in of breakage function - (c) 2010 Tokamak Ltd";
	str[1] = L"Controls:";
	str[2] = L"'P' -> pause/unpause the simulation";
	str[3] = L"'T' -> Lift tables";	
	str[4] = L"Demonstrate the build in breaking function of Tokamak.";
	str[5] = L"A rigid body is assemble with any number of geometries.";
	str[6] = L"Each geometry can be told to break off at a certain";
	str[7] = L"collision force. Tokamak will automatically spawn the";
	str[8] = L"new fragment objects when the breaking occurs.";	


	MyRenderText(str, 9);
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

