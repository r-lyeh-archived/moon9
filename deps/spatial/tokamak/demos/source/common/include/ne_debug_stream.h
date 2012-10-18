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

#ifndef NE_DEBUG_STREAM_H
#define NE_DEBUG_STREAM_H

#include <sstream>	

class CDebugStream : public std::stringstream
{
public:
	static CDebugStream& GetErrorStream()
	{
		static CDebugStream* oneAndOnly = NULL;
		if (!oneAndOnly)
			oneAndOnly = new CDebugStream;
		return *oneAndOnly;
	}

	void Flush();
};

#define G_ERR (CDebugStream::GetErrorStream())

#endif //NE_DEBUG_STREAM_H