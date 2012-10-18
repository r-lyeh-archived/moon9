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

#ifndef NEINPUT_H
#define NEINPUT_H

#include "math/ne_math.h"

typedef struct neMouseState neMouseState;

struct neMouseState
{
	neBool clickLeft;
	neBool clickMid;
	neBool clickRight;

	s32 mouseX;
	s32 mouseY;
};

class CNEArcBall
{
	s32 m_iWidth;   // ArcBall's window width
	s32 m_iHeight;  // ArcBall's window height
	f32 m_fRadius;  // ArcBall's radius in screen coords
	f32 m_fRadius2; // ArcBall's radius in screen coords
	
	neQ m_qDown;               // Quaternion before button down
	neQ m_qNow;                // Composite quaternion for current drag
	neM4 m_matRotation;         // Matrix for arcball's orientation
	neM4 m_matRotationDeltaX;    // Matrix for arcball's orientation
	neM3 m_matRotationDeltaY;    // Matrix for arcball's orientation
	neM4 m_matTranslation;      // Matrix for arcball's position
	neM4 m_matTranslationDelta; // Matrix for arcball's position
	
	int  iCurMouseX;      // Saved mouse position
	int  iCurMouseY;
	neV3 m_vDown;         // Button down quaternion
	neV3 m_vCur;          // Current quaternion
	
	neV3 ScreenToVector( int sx, int sy );
	
public:
	//LRESULT HandleMouseMessages( HWND, Us32, WPARAM, LPARAM );
	bool m_bLeftDrag;               // Whether user is dragging arcball
	bool m_bRightDrag;               // Whether user is dragging arcball
	neV3 m_translate2;	
	
	neM4* GetTranslationMatrix();
	neM4* GetTranslationDeltaMatrix();
	
	void SetRadius( f32 fRadius );
	void SetWindow( s32 w, s32 h, f32 r=1.0f );
	
	void TranslateStart(int mx, int my);
	void TranslateDelta(int mx, int my, int flag);
	
	CNEArcBall();
};

class INEInput
{
public:
	CNEArcBall arcBall;
	
	virtual bool Initialise() = 0;
	virtual void Shutdown() = 0;
	virtual void GetArchBallEnable(bool & enable) = 0;
	virtual void SetArchBallEnable(bool enable) = 0;
	virtual void GetMouse(neMouseState * mouseState) = 0;
};

INEInput * NECreateInput();

#endif //NEINPUT_H