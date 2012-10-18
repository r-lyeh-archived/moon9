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

#ifndef OS_WIN_H
#define OS_WIN_H

#include "ne_os.h"
#include "d3dapp.h"

#define GETD3DAPP (&((neOSWin*)GETOS)->d3dapp)

class neD3DApp : public CD3DApplication
{
public:
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT ConfirmDevice(D3DCAPS8* pCaps,DWORD dwBehavior,D3DFORMAT Format)   
	{ 
		if( dwBehavior & D3DCREATE_PUREDEVICE )
			return E_FAIL; // GetTransform doesn't work on PUREDEVICE
		
		return S_OK; 
	}
};

class neOSInitialiseEvent: public CFSMEvent
{
public:
	HINSTANCE hInst;
};


class neOSWin: public OS
{
public:
	HINSTANCE hInst;
	neD3DApp d3dapp;
	
public:
	HINSTANCE GetHInstance() {return hInst;}
	
	void InitialiseAction(CFSMEvent& event);
	
	void ExitAction(CFSMEvent& event);

	LPDIRECT3DDEVICE8 GetD3DDevice() {return d3dapp.m_pd3dDevice;}

	bool IsKeyPress(int code);

protected:
	void UpdateKeyInput();
};

#endif