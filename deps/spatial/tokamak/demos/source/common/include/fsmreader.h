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

#ifndef FSMREADER_H
#define FSMREADER_H

#include "fsm.h"

using namespace std;

typedef enum 
{
	EFSMF_BASE_CLASS,
	EFSMF_STATE_COUNT,
	EFSMF_STATE_NAME,
	EFSMF_EVENT_COUNT,
	EFSMF_EVENT_NAME,
	EFSMF_NEXT_STATE,
	EFSMF_DONE,
	EFSMF_ERROR,
}EFSMFileEventID;

class CFSMFile;

class CFSMFileState: public CFSMState
{
public:
	string ReadString();
	int ReadInt(int& ii);
	void WriteString(string& ss);
	void WriteInt(int ii);
};
//======================================================
//	FSM file read/write states
//		
//		FSMFileStart - start state
//		GetStateBaseClass
//		GetStateCount
//		GetStateName
//		GetEventCount
//		GetEventName
//		GetNextState
//		FSMFileError - error state
//		
//======================================================
class CFSMFileStart: public CFSMFileState
{
public:
	CFSMFileStart()
	{
		id = 0;
	}
	int Action(CFSMEvent &event, CFSMEvent &newEvent){return 0;}
};

class CGetStateBaseClass: public CFSMFileState
{
public:
	CGetStateBaseClass()
	{
		id = 1;
	}
	int Action(CFSMEvent &event, CFSMEvent &newEvent);
};
class CGetStateCount: public CFSMFileState
{
public:
	CGetStateCount()
	{
		id = 2;
	}
	int Action(CFSMEvent &event, CFSMEvent &newEvent);
};
class CGetStateName: public CFSMFileState
{
public:
	CGetStateName()
	{
		id = 3;
	}
	int Action(CFSMEvent &event, CFSMEvent &newEvent);
};
class CGetEventCount: public CFSMFileState
{
public:
	CGetEventCount()
	{
		id = 4;
	}
	int Action(CFSMEvent &event, CFSMEvent &newEvent);
};
class CGetEventName: public CFSMFileState
{
public:
	CGetEventName()
	{
		id = 5;
	}
	int Action(CFSMEvent &event, CFSMEvent &newEvent);
};
class CGetNextState: public CFSMFileState
{
public:
	CGetNextState()
	{
		id = 6;
	}
	int Action(CFSMEvent &event, CFSMEvent &newEvent);
};

class CFSMFile: public CFSM
{
public:

	CFSMFileStart start;
	CGetStateBaseClass stateBaseClass;
	CGetStateCount stateCount;
	CGetStateName stateName;
	CGetEventCount eventCount;
	CGetEventName eventName;
	CGetNextState nextState;

	string _baseClass;
	int _stateCount;
	string _stateName;
	int _eventCount;
	string _eventName;
	string _nextState;

	fstream *str;

public:
	CFSMFile();
	fstream & GetStream() {return *str;}
	void SaveFSM(fstream & ostr);
	CFSM* LoadFSM(fstream & istr);
};

#endif