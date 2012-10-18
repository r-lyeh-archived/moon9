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

#ifndef FSM_H
#define FSM_H

#pragma warning(disable:4786)
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include "factory.h"

using namespace std;

class CFSMEvent;
class CFSMState;
class CFSM;

//======================================================
//	CFSMEvent
//======================================================
typedef int TEventId;

class CFSMEvent
{
public:
	TEventId id;
	void * param;
	string name;
public:
	CFSMEvent()
	{
		id = -1;
	}

	//Debug function
	string ReportNameId();
};

//======================================================
//	CFSMState
//======================================================
typedef map<TEventId, CFSMState*, less<TEventId> > TTransitionMap;	
typedef TTransitionMap TTransMap;

class CFSMState
{
public:
	int id;
	int baseId; //id of the base state class
	TTransitionMap transition;
	CFSM* fsm;

public:
	CFSMState()
	{
		id = -1;
		baseId = -1;
	}	
	virtual CFSMState * Input(CFSMEvent &event);
	virtual int Action(CFSMEvent &event, CFSMEvent &newEvent)
	{
#ifdef _DEBUG
		cout << "entering state " << ReportNameId() << endl;
#endif
		return 0; //0 = did not trigger newEvent
	}
	
	//Debug function
	string CFSMState::ReportNameId();
};

//======================================================
//	class CFSM - Finite State Machine
//
//	to do: check for duplicate states
//======================================================

typedef map<TEventId, string, less<TEventId> > TEventNameMap;	

class CFSM : public IPersistant
{
public:
	vector<CFSMState*> states;
	string name;
	TEventNameMap eventNames;
	CFSMState * currentState;
	CFSMState * startState;
	int error;

public:
	CFSM()
	{
		startState = NULL;
		Reset();
	};
	void Reset() 
	{
		currentState = startState;
		error = 0;
	}
	void AddState(CFSMState &state);
	virtual void Input(CFSMEvent& event);
	//IPersistant interface
	virtual void Save(ostream & ostr)
	{
	};
	virtual void Load(istream & istr)
	{
	};
};

//======================================================
// class CFSMFactory
//======================================================
class CFSMFactory : public CObjectFactory
{
public:
	char * MakeInstance(int objType);
};

//======================================================
// implementation
//======================================================
inline string CFSMEvent::ReportNameId()
{
	string ret;
	string myName;
	static char buf[10];

	if (name == "")
		myName = "<no name>";
	else 
		myName = name;

	ret = "(" + myName + ", ";
	ret += itoa(id, buf, 10);
	ret += ")";
	return ret;
}

inline CFSMState * CFSMState::Input(CFSMEvent &event)
{
	CFSMState * newState;
	TTransitionMap::iterator iter;

	iter = transition.find(event.id);
	if (iter != transition.end())
	{
		newState = (*iter).second;
#ifdef _DEBUG
		if (newState == NULL)
		{
			cerr << "CFSMState::Input - undefined new state for event " 
			<< event.ReportNameId() 
			<< " in state " 
			<< ReportNameId() 
			<< endl;
		}
#endif
	}
	else
	{
		newState = NULL;
#ifdef _DEBUG
		cerr << "CFSMState::Input - state " 
			<< ReportNameId()
			<< " don't know how to handle event " 
			<< event.ReportNameId() 
			<< endl;
#endif
	}
	return newState;
};

inline string CFSMState::ReportNameId()
{
	string ret;
	string myName;
	static char buf[10];
	
	const type_info& type = typeid(*this);

	myName = type.name();
	ret = "(" + myName + ", ";
	ret += itoa(baseId, buf, 10);
	ret += ", ";
	ret += itoa(id, buf, 10);
	ret += ")";
	return ret;
}

inline void CFSM::AddState(CFSMState &state)
{
	vector<CFSMState*>::iterator iter;
	for (iter = states.begin(); iter != states.end(); iter++)
	{
		if ((*iter)->id == state.id)
		{
			const type_info& typeA = typeid(**iter);
			const type_info& typeB = typeid(state);

			if (typeA == typeB)
			{
				cerr << "CFSM::AddState - duplicate state " 
					<< typeA.name() 
					<< " for state machine "
					<< name << endl;
				return;
			}
		}
	}
	states.push_back(&state);
	state.fsm = this;
}

inline void CFSM::Input(CFSMEvent& event)
{
	CFSMEvent newEvent;
	int isNewEvent;

	if (currentState != NULL)
	{
		currentState = currentState->Input(event);
		if (currentState)
		{
			isNewEvent = currentState->Action(event, newEvent);
			while (isNewEvent) 
			{
				//the action of the currentState has trigger a new event 
				//which is feed back to the fsm
				currentState = currentState->Input(newEvent);
				if (currentState)
				{
					CFSMEvent e;
					isNewEvent = currentState->Action(newEvent, e);
					if (isNewEvent)
						newEvent = e;
				}
			}
		}
	}
#ifdef _DEBUG
	else
		cerr << "CFSM::Input - current state undefined" << endl;
#endif
};

#endif