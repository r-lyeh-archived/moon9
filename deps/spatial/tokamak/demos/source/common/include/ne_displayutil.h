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

#ifndef NE_DISPLAY_UTIL_H
#define NE_DISPLAY_UTIL_H

#include "math/ne_math.h"
#include "ne_display.h"
#include "ne_ostype.h"

#define MAKE4CC(ch0, ch1, ch2, ch3)                              \
		((u32)(neByte)(ch0) | ((u32)(neByte)(ch1) << 8) |   \
		((u32)(neByte)(ch2) << 16) | ((u32)(neByte)(ch3) << 24 ))

void NEGetStringFrom4CC(char * res, u32 fcc);

bool NESetViewMatrix(neM4& M, 
						neV3& vFrom, 
						neV3& vAt,
						neV3& vWorldUp );

bool NESetViewMatrix( neM4&    M, 
						f32        Dist,
						neRadian     Rx,
						neRadian      Ry,
						neV3&    vAt,
						neV3&    vWorldUp );

void NEProjectionMatrix(neM4& M, 
							float NearPlane, 
							float FarPlane, 
							float YFov, 
							float Aspect);

class CTargetCamera
{
public:
	neCamera * cam;

	f32 xDir;

	f32 yDir;

public:
	CTargetCamera() 
	{
		xDir = 0.0f;
		yDir = 0.0f;
		cam = NULL; 
	}

	void SetCamera(neCamera * camera) { cam = camera; }

	void Process(OSTime time, neV3 * target);

	void Process();

	neT3 GetTransform();
};

#endif