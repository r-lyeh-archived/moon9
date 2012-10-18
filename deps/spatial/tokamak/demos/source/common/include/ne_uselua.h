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

#ifndef USELUA_H
#define USELUA_H

#include "ne_os.h"
#include "ne_luawrapper.h"

extern "C" {
#include "lua.h"
#include "luadebug.h"
#include "lualib.h"
}

///////////////////////////////////////////////////////////////////
//
//	CLua
//
//	The class wrapper for the Lua state
//
///////////////////////////////////////////////////////////////////

class CLua : public IScriptEngine
{
friend class OS;

private:
	int toclose;
	int stacksize;
	lua_Hook old_linehook;
	lua_Hook old_callhook;
	//lua_State *L;
	CLuaState * luaState;
	void OpenLibrary();

public:
	CLua();
	bool Initialise();
	void ExecuteString(const char * cmd);
	void RunScript(const char * filename);
	void Shutdown();
	CLuaState * GetLuaState() {
		return luaState;
	}
	int GetGCCount();
	int GetGCThreshold();
};


#endif //USELUA_H