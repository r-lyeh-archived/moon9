#ifndef TOKAMAK_SAMPLE_APP
#define TOKAMAK_SAMPLE_APP

#include "tokamak.h" 

#define NUM_LIGHT 2
#define GROUND_Y 3.0f
#define CAMERA_SIZE 0.2f

// MinBound and MaxBound are the bounding box representing the cell mesh.

extern ID3DXEffect*            g_pEffect;
extern D3DXHANDLE              g_hDiffuse;             // Handles to material parameters in effect
extern D3DXHANDLE              g_hSpecular;
extern D3DXHANDLE              g_hTex;
extern D3DXHANDLE              g_hPower;
extern D3DXHANDLE				g_hAmbient;
extern D3DXHANDLE				g_hLightTokamak[NUM_LIGHT];
extern D3DXHANDLE				g_hLightColorTokamak[NUM_LIGHT];
extern D3DXHANDLE				g_hViewPos;
extern D3DXHANDLE              g_hMatWVP;
extern D3DXHANDLE              g_hMatWV;
extern D3DXHANDLE              g_hMatW;
extern CFirstPersonCamera		g_Camera;               // Camera

extern D3DXHANDLE g_hShaderTechTokamak;
extern D3DXVECTOR4 vLightWorld[NUM_LIGHT];
extern D3DXVECTOR4 vLightColor[NUM_LIGHT];
extern IDirect3DDevice9 * g_pD3dDevice;

class CRenderPrimitive
{
public:
	
	// for display
	CDXUTMesh mMesh;
	D3DXMATERIAL mMaterial;
	s32 mIsCylinder;
//	D3DXMATRIX mWorld;

	CRenderPrimitive();
	void SetDiffuseColor(const D3DXCOLOR &c);
	void SetGraphicBox(float Width, float Height, float Depth);
	void SetGraphicSphere(float radius);
	void SetGraphicCylinder(float radius, float length);
	void SetGraphicMesh(LPCWSTR strFilename);
	void Render(IDirect3DDevice9* pd3dDevice, neT3 * matrix);
};

#endif //TOKAMAK_SAMPLE_HEADER