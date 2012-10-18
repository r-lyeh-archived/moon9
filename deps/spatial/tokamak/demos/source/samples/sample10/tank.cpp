/*
Here is the method that's building the tank.  Here are the declarations:

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

#define WHEEL_DIAMETER(2.0f)
#define WHEEL_WIDTH(0.3f)
#define MAX_MOTOR(100.0f)
#define TANK_MASS(300.0f)
#define WHEEL_MASS(20.0f)

*/

HRESULT CJXBTank::BuildTank ()
{
neV3 vPos;
neT3 mTrans;
vPos.Set (0.0f, 0.0f, 0.0f);
mTrans.SetIdentity ();

// Create the tank body
m_pTankBody = m_pTokSim->CreateRigidBody ();

// Add the main hull geometry
neGeometry *pHull = m_pTankBody->AddGeometry ();
neV3 vHullSize;
vHullSize.Set (6.50f, 2.9f, 3.2f);
pHull->SetBoxSize (vHullSize);

// Position the hull
neT3 hullTrans;
hullTrans.SetIdentity ();
hullTrans.pos.Set (0.0f, 1.5f, 0.0f);
pHull->SetTransform (hullTrans);
pHull->SetMaterialIndex (1);

// Put the hull at the origin in world space, to
// ease the task of adding additional bodies
m_pTankBody->SetPos (vPos);

m_pTankBody->UpdateBoundingInfo ();

// Rigid body dynamics properties
m_pTankBody->SetInertiaTensor (neBoxInertiaTensor (6.91f,3.5f,3.6f,TANK_MASS));
m_pTankBody->SetMass (TANK_MASS);
m_pTankBody->SetAngularDamping (0.01f);
m_pTankBody->SetSleepingParameter (0.01f);

m_pTankBody->SetUserData ((u32)this);

m_pTankBody->GravityEnable (FALSE);
m_pTankBody->CollideConnected (FALSE);

// We will start with 8 wheels.  Each wheel
// represented by a cylindrical rigid body
// with high friction.
DWORD dwIndex;
for (dwIndex = 0; dwIndex < JXB_TANK_WHEELS; dwIndex++)
{

// Each wheel will be 1 meters tall, 1/3 meter white
float fWheelDiameter= WHEEL_DIAMETER;
float fWheelWidth= WHEEL_WIDTH;

// Create the geometry (wheel)
m_pTankWheels [dwIndex] = m_pTokSim->CreateRigidBody ();

// Add the cylinder geometry
neGeometry *pGeoWheel = m_pTankWheels [dwIndex]->AddGeometry ();
pGeoWheel->SetMaterialIndex (1);

// Set the cylinder size
pGeoWheel->SetCylinder (fWheelDiameter, fWheelWidth);

/////
// Positioning...
// We must rotate it 90 degrees about X in order
// for the cylinder to be positioned as a wheel.
neV3 vRotate;
mTrans.SetIdentity ();
vRotate.Set (D3DX_PI/2.0f, 0.0f, 0.0f);

// Set the position and rotation
m_pTankWheels [dwIndex]->SetPos (WheelPositions[dwIndex]);
mTrans.rot.RotateXYZ (vRotate);
m_pTankWheels [dwIndex]->SetRotation (mTrans.rot);

// Set the physical paramters
m_pTankWheels [dwIndex]->SetMass (WHEEL_MASS);
m_pTankWheels [dwIndex]->SetUserData ((u32)this);
m_pTankWheels [dwIndex]->SetAngularDamping (0.00f);
m_pTankWheels [dwIndex]->SetSleepingParameter (0.00f);

m_pTankWheels [dwIndex]->UpdateBoundingInfo ();

// Set the inertia tensor
m_pTankWheels [dwIndex]->SetInertiaTensor 
(neCylinderInertiaTensor (fWheelDiameter, fWheelWidth, WHEEL_MASS));

m_pTankWheels [dwIndex]->SetPos (WheelPositions[dwIndex]);

// Disable gravity on the new wheel
m_pTankWheels [dwIndex]->GravityEnable (FALSE);
m_pTankWheels [dwIndex]->CollideConnected (FALSE);

// Attach the rigid body to the tank's hull with a 
// Hinge joint
m_pWheelJoints [dwIndex] = m_pTokSim->CreateJoint 
(m_pTankBody, m_pTankWheels[dwIndex]);

// Accuracy
m_pWheelJoints [dwIndex]->SetIteration (4);

// Create the joint frame along the Z axis (Axel)
neT3 mJointFrame;
mJointFrame.rot[0].Set(1.0f, 0.0f, 0.0f);
mJointFrame.rot[1].Set(0.0f, 0.0f, -1.0f);// ?
mJointFrame.rot[2].Set(0.0f, 1.0f, 0.0f);
mJointFrame.pos = WheelPositions[dwIndex];

// Set joint attributes
m_pWheelJoints [dwIndex]->SetJointFrameWorld (mJointFrame);
m_pWheelJoints [dwIndex]->SetType (neJoint::NE_JOINT_HINGE);
m_pWheelJoints [dwIndex]->Enable (true);
m_pWheelJoints [dwIndex]->EnableMotor (true);
m_pWheelJoints [dwIndex]->SetMotor(neJoint::NE_MOTOR_SPEED, 
0.0f, MAX_MOTOR);// Initially at Zero

}

return S_OK;
}