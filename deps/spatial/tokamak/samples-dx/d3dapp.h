#ifndef D3DAPP_H
#define D3DAPP_H

#include "tokamak.h"

void MyAppInit();

void MyRenderText(WCHAR ** StrList, int NumStr);

HRESULT CALLBACK OnMyAppCreateDevice	( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );

void CALLBACK OnMyAppFrameMove			( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );

void CALLBACK OnMyAppFrameRender		( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );

HRESULT CALLBACK OnMyAppResetDevice		( IDirect3DDevice9* pd3dDevice, 
											const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );

LRESULT CALLBACK MyAppMsgProc			( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
											bool* pbNoFurtherProcessing, void* pUserContext );

void CALLBACK OnMyAppLostDevice			( void* pUserContext );

void CALLBACK OnMyAppDestroyDevice		( void* pUserContext );

void CALLBACK MyAppKeyboardProc			( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );


#endif // D3DAPP_H