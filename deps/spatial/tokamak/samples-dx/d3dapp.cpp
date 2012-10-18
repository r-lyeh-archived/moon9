//--------------------------------------------------------------------------------------
// File: EmptyProject.cpp
//
// Empty starting point for new Direct3D applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include ".\dxut\dxstdafx.h"
#include "resource.h"
#include "d3dapp.h"

ID3DXFont*              g_pFont = NULL;         // Font for drawing text
bool					g_bLeftMouseDown = false;
CFirstPersonCamera		g_Camera;               // Camera
ID3DXSprite*			g_pTextSprite = NULL;   // Sprite for batching draw text calls


HRESULT CALLBACK	OnCreateDevice		( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK		OnFrameMove			( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK		OnFrameRender		( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
bool CALLBACK		IsDeviceAcceptable	( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
											D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
bool CALLBACK		ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext );
HRESULT CALLBACK	OnResetDevice		( IDirect3DDevice9* pd3dDevice, 
											const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
LRESULT CALLBACK	MsgProc				( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
											bool* pbNoFurtherProcessing, void* pUserContext );
void CALLBACK		OnLostDevice		( void* pUserContext );
void CALLBACK		OnDestroyDevice		( void* pUserContext );
void CALLBACK		KeyboardProc		( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );

HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;

    // Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Arial", &g_pFont ) );



    return OnMyAppCreateDevice( pd3dDevice, pBackBufferSurfaceDesc, pUserContext );
}

void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	// Update the camera's position based on user input 
    g_Camera.FrameMove( fElapsedTime );

	OnMyAppFrameMove( pd3dDevice, fTime, fElapsedTime, pUserContext );
}

void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;

	// Clear the render target and the zbuffer 
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		OnMyAppFrameRender( pd3dDevice, fTime, fElapsedTime, pUserContext );

        V( pd3dDevice->EndScene() );
    }
}

//--------------------------------------------------------------------------------------
// Rejects any devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Typically want to skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3DObject(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;

    if( g_pFont )
        V_RETURN( g_pFont->OnResetDevice() );
//    if( g_pEffect )
//        V_RETURN( g_pEffect->OnResetDevice() );

//    g_Cell.RestoreDeviceObjects( pd3dDevice );

	V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pTextSprite ) );

    return OnMyAppResetDevice( pd3dDevice, 
                          pBackBufferSurfaceDesc, pUserContext );
}





//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
    // Pass all remaining windows messages to camera so it can respond to user input
    g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

    switch( uMsg )
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
            g_bLeftMouseDown = true;
            break;
        case WM_LBUTTONUP:
            g_bLeftMouseDown = false;
            break;
        case WM_CAPTURECHANGED:
            if( (HWND)lParam != hWnd )
                g_bLeftMouseDown = false;
            break;
    }

    return MyAppMsgProc(hWnd, uMsg, wParam, lParam, 
                          pbNoFurtherProcessing, pUserContext);
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnResetDevice callback here 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
    if( g_pFont )
        g_pFont->OnLostDevice();
//    if( g_pEffect )
//        g_pEffect->OnLostDevice();
    SAFE_RELEASE( g_pTextSprite );
//    g_Cell.InvalidateDeviceObjects();

	OnMyAppLostDevice( pUserContext );
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnCreateDevice callback here
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
    SAFE_RELEASE( g_pFont );
//    SAFE_RELEASE( g_pDefaultTex );
//    g_Cell.Destroy();
	OnMyAppDestroyDevice( pUserContext );
}

void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    if( bKeyDown )
    {
//        switch( nChar )
//      {
//            case VK_F1: g_bShowHelp = !g_bShowHelp; break;
//		default:
//			break;
//        }
    }
	MyAppKeyboardProc( nChar, bKeyDown, bAltDown, pUserContext );
}

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------

INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // Set the callback functions
    DXUTSetCallbackDeviceCreated( OnCreateDevice );
    DXUTSetCallbackDeviceReset( OnResetDevice );
    DXUTSetCallbackDeviceLost( OnLostDevice );
    DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( KeyboardProc );
    DXUTSetCallbackFrameRender( OnFrameRender );
    DXUTSetCallbackFrameMove( OnFrameMove );
   
    // Initialize world matrices
//    D3DXMatrixScaling( &g_mCellWorld, 1.0f, GROUND_Y / 3.0f, 1.0f );

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"d3dapp" );
	DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 480, IsDeviceAcceptable, ModifyDeviceSettings );

	MyAppInit();
    // Start the render loop
    DXUTMainLoop();

    // TODO: Perform any application-level cleanup here

    return DXUTGetExitCode();
}

