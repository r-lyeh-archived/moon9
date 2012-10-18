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

#ifndef OS_H
#define OS_H

#include "simplefsm.h"
#include "ne_ostype.h"
//#include "ne_gameworld.h"
#include "ne_display.h"
#include "ne_input.h"
#include <sstream>
#include "ne_debug_stream.h"

class neGameWorld;
class IScriptEngine;
class OS;

// Frequently use functions
#define GETOS (OS::GetOS())
#define GETDISPLAY (GETOS->GetDisplaySystem())
#define GETINPUT (GETOS->GetInput())
#define GETLUA ((CLua*)GETOS->GetScriptEngine())

OS * neNewOS();

class CConsoleStream : public stringstream
{
public:
	enum{
		CONSOLE_FORMAT_INPUT,
		CONSOLE_FORMAT_OUTPUT,
		CONSOLE_FORMAT_ERROR,
	};
	Flush(int format = CONSOLE_FORMAT_INPUT);
};

typedef struct TMouse TMouse;

struct TMouse
{
	f32 x;
	f32 y;
	bool leftDown;
	bool rightDown;
};

class OS : public CSimpleFSM
{
public:
	
	OS();
	~OS() { delete &G_ERR; } 
	CConsoleStream conStream;
	int argc;
	char** argv;

private:
	neDisplaySystem * displaySystem;
	INEInput * input;
	neGameWorld* gameWorld;
	OSTime clockFreq; //how many ticks per sec
	IScriptEngine* scriptEngine;
	bool frameLock;	
	OSTime lastTime;

public:
	DECLARE_STATE_BEGIN
		DECLARE_STATE(OS_STATE_START)
		DECLARE_STATE(OS_STATE_INITIALISE)
		DECLARE_STATE(OS_STATE_RUN)
		DECLARE_STATE(OS_STATE_EXIT)
	DECLARE_STATE_END

	DECLARE_EVENT_BEGIN
		DECLARE_EVENT(OS_EVENT_INITIALISE)
		DECLARE_EVENT(OS_EVENT_RUN)
		DECLARE_EVENT(OS_EVENT_STOP)
	DECLARE_EVENT_END

protected:
	DECLARE_FSM_ACTION(StartAction)
	DECLARE_VIRTUAL_FSM_ACTION(InitialiseAction)
	DECLARE_FSM_ACTION(RunAction)
	DECLARE_VIRTUAL_FSM_ACTION(ExitAction)
	
	DECLARE_STATE_ACTION_BEGIN
		DECLARE_STATE_ACTION(OS_STATE_START, StartAction)
		DECLARE_STATE_ACTION(OS_STATE_INITIALISE, InitialiseAction)
		DECLARE_STATE_ACTION(OS_STATE_RUN, RunAction)
		DECLARE_STATE_ACTION(OS_STATE_EXIT, ExitAction)
	DECLARE_STATE_ACTION_END

	DECLARE_MAP_EVENT_BEGIN
		DECLARE_MAP_EVENT(OS_STATE_START, OS_EVENT_INITIALISE, OS_STATE_INITIALISE)
		DECLARE_MAP_EVENT(OS_STATE_INITIALISE, OS_EVENT_RUN, OS_STATE_RUN)
		DECLARE_MAP_EVENT(OS_STATE_RUN, OS_EVENT_STOP, OS_STATE_EXIT)
	DECLARE_MAP_EVENT_END

public:
	// operation
	void Run();

	void FrameLock(bool lock);
	
	bool FrameLock();
	
	bool FileExist(const char * filename);
	
	string	GetGlobalTableStr(const char * tableName, const char * key);

	double	GetGlobalTableVal(const char * tableName, const char * key);
	
	string	GetGlobalStr(const char * key);
	
	double	GetGlobalVal(const char * key);

	string	GetScriptPath();
	
	string	GetEffectPath();
	
	string	GetModelPath();

	void SetGameWorld(neGameWorld* gw){gameWorld = gw;}
	
	neGameWorld * GetGameWorld() { return gameWorld; }

	bool GetMouse(TMouse * mouse);

	// retrieving subsystems
	neDisplaySystem * GetDisplaySystem(){return displaySystem;}

	INEInput * GetInput(){return input;}
	
	IScriptEngine *	GetScriptEngine(){return scriptEngine;}
	
	static OS *	GetOS() 
	{
		static OS * os = NULL;

		if (os == NULL)
			os = neNewOS();

		return os;
	}

private:
	void ExposeFunction();

	void SetDefaultVariables();

	void DisplayText();

protected:
	virtual void UpdateKeyInput() = 0;

public:
	virtual bool IsKeyPress(int code) = 0;
};

class IScriptEngine
{
public:
	virtual bool Initialise() = 0;
	
	virtual void ExecuteString(const char * cmd) = 0;
	
	virtual void RunScript(const char * filename) = 0;
	
	virtual void Shutdown() = 0;
};

string OsStrGetFilePath(string & s);
string OsStrGetFilename(string & s);

#endif //OS_H