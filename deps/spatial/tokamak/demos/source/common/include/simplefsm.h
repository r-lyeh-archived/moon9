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

#ifndef SIMPLEFSM_H
#define SIMPLEFSM_H

#include "fsm.h"
///////////////////////////////////////////////////////////////////
#define DECLARE_STATE_BEGIN								\
	enum {

#define DECLARE_STATE(sid)	sid,

#define DECLARE_STATE_END								\
	};

#define DECLARE_EVENT_BEGIN								\
	enum {

#define DECLARE_EVENT(eid)	eid,

#define DECLARE_EVENT_END								\
	};

///////////////////////////////////////////////////////////////////
#define DECLARE_STATE_ACTION_BEGIN							\
	void EnterState(int sid, CFSMEvent& event) {		\
		SetState(sid);									\
		switch(sid)	{									

#define DECLARE_STATE_ACTION(sid, stateAction)				\
		case sid: stateAction(event); break;			

#define DECLARE_STATE_ACTION_END							\
		default: cerr << "Unknown State " << sid << endl;}	\
	};													
///////////////////////////////////////////////////////////////////

#define DECLARE_FSM_ACTION(stateAction)					\
	void stateAction(CFSMEvent& event);

#define DECLARE_VIRTUAL_FSM_ACTION(stateAction)					\
	virtual void stateAction(CFSMEvent& event);

///////////////////////////////////////////////////////////////////
#define DECLARE_MAP_EVENT_BEGIN	\
	int MapEvent(int eid) {

#define DECLARE_MAP_EVENT(curState, eventID, newState) \
		if (stateID == curState && eid == eventID) return newState;

#define DECLARE_MAP_EVENT_END \
		return -1;};

///////////////////////////////////////////////////////////////////
class CSimpleFSM
{
public:
	int		GetState(){return stateID;}
	void	SetState(int sid) {stateID = sid;}
	void	ReceiveFSMEvent(CFSMEvent& event)
	{
		int nextState = MapEvent(event.id);
		if (nextState == -1) return;
		EnterState(nextState, event);
	}
protected:
	int		stateID;
	virtual void	EnterState(int sID, CFSMEvent& event) = 0;
	virtual int		MapEvent(int eid) = 0;
};

#endif;