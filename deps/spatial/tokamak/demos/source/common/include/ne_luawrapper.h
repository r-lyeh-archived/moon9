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

#ifndef NE_LUAWRAPPER_H
#define NE_LUAWRAPPER_H

#include <assert.h>
#include <vector>

using namespace std;

extern "C" {
#include "lua.h"
}

#define DEFAULT_LUA_STACK_SIZE 0

class CLuaState;
class CLuaValue;
class CLuaFunction;
//////////////////////////////////////////////////////////////////
//
// CLuaValueBase
//
//////////////////////////////////////////////////////////////////
class CLuaValueBase
{
protected:
	int vtype;
public:
	int GetType(){return vtype;}
	virtual void Push(CLuaState * ls) = 0;
	virtual void Pop(CLuaState * ls) = 0;
	void SetGlobal(CLuaState * ls, const char * name);
};

//////////////////////////////////////////////////////////////////
//
// CLuaState
//
//
//////////////////////////////////////////////////////////////////
class CLuaState 
{
protected:
	lua_State * luaState;
	int stackSize;
	vector<CLuaValueBase*> popArray;

public:
	CLuaState() {
		stackSize = DEFAULT_LUA_STACK_SIZE;
		luaState = lua_open(stackSize);
	}
	CLuaState(int stack){
		stackSize = stack;
		luaState = lua_open(stackSize);
	}
	~CLuaState(){
		int i;
		for (i = 0; i < popArray.size(); i++)
			delete popArray[i];
		
		lua_close(luaState);
	}
	
	lua_State * GetState() {
		return luaState;
	}
	
	CLuaValueBase * Pop();

	void Pop(string & str) {
		if (lua_type(luaState, -1) != LUA_TNIL)
		{
			assert(lua_type(luaState, -1) == LUA_TSTRING);
			str = lua_tostring(luaState, -1);
		}
		else
		{
			
		}
		lua_pop(luaState, 1);
	}

	void Pop(double & value) {
		if (lua_type(luaState, -1) != LUA_TNIL)
		{
			assert(lua_type(luaState, -1) == LUA_TNUMBER);
			value = lua_tonumber(luaState, -1);
		}
		else
		{
			
		}
		lua_pop(luaState, 1);
	}
	void Pop(u32 & value) {
		double d;
		Pop(d);
		value = (u32)d;
	}
	void Pop(CLuaFunction & fun);
	
	void PopOnly(int n) 
	{
		lua_pop(luaState, n);
	}
	
/*	
	void PopN(int n) {
		int i;
		for (i = 0; i < n; i++)
		{
			CLuaValueBase * v = Pop();
			popArray.push_back(v);
		}
	}
*/	
	CLuaValueBase * PopResult() {
		assert(popArray.size() > 0);
		CLuaValueBase * ret = popArray[popArray.size()-1];
		popArray.pop_back();
		return ret;
	}

	operator lua_State * () const {
		return luaState;
	}
	
	void SetGlobal(const char * varname) {
		lua_setglobal(*this, varname);
	}
	
	CLuaValueBase * PopGlobal(const char * varname) {
		lua_getglobal(*this, varname);
		return Pop();
	}

	void GetGlobal(const char * varname, CLuaValueBase * value) {
		lua_getglobal(*this, varname);
		value->Pop(this);
	}
	
	int Call(int nargs/*, int nresults*/) {
		int ret = lua_call(luaState, nargs, LUA_MULTRET);
		//if (nresults > 0)
		//	PopN(nresults);
		return ret;
	}
};

//////////////////////////////////////////////////////////////////
//
// CLuaValue (nil, number, string)
//
//////////////////////////////////////////////////////////////////
class CLuaValue: public CLuaValueBase
{
protected:
	union {
		const char * valueString;
		double	valueNum;
	};
public:
	//constructor
	CLuaValue(const char * str) {*this = str;}
	CLuaValue(double val) {*this = val;}
	CLuaValue(float val) {*this = val;}
	CLuaValue(int val) {*this = val;}
	CLuaValue(unsigned int val) {*this = val;}
	CLuaValue(long val) {*this = val;}
	CLuaValue(unsigned long val) {*this = val;}

	CLuaValue(){vtype = LUA_TNIL;}

	//operator
	CLuaValue & operator = (const char * str){
		valueString = str;
		vtype = LUA_TSTRING;
		return *this;
	}
	CLuaValue & operator = (double val){
		valueNum = val;
		vtype = LUA_TNUMBER;
		return *this;
	}
	CLuaValue & operator = (float val){
		valueNum = (double)val;
		vtype = LUA_TNUMBER;
		return *this;
	}
	CLuaValue & operator = (int val){
		valueNum = (double)val;
		vtype = LUA_TNUMBER;
		return *this;
	}
	CLuaValue & operator = (unsigned int val){
		valueNum = (double)val;
		vtype = LUA_TNUMBER;
		return *this;
	}
	CLuaValue & operator = (long val){
		valueNum = (double)val;
		vtype = LUA_TNUMBER;
		return *this;
	}
	CLuaValue & operator = (unsigned long val){
		valueNum = (double)val;
		vtype = LUA_TNUMBER;
		return *this;
	}
	operator const char * () const{
		if (vtype == LUA_TSTRING) 
			return valueString;
		else
			return NULL;
	}
	operator double () const{
		if (vtype == LUA_TNUMBER)
			return valueNum;
		else if (vtype == LUA_TSTRING)
			return atof(valueString);
		else 
			return 0.0;
	}
	operator float () const{
		if (vtype == LUA_TNUMBER)
			return (float)valueNum;
		else if (vtype == LUA_TSTRING)
			return (float)atof(valueString);
		else 
			return 0.0f;
	}

	//function
	void SetNil() {vtype = LUA_TNIL;}
	void Push(CLuaState * ls) {
		Push(*ls);
	}
	void Push(lua_State * luaState) {
		switch (vtype)
		{
		case LUA_TNIL:
			lua_pushnil(luaState);
			break;
		case LUA_TNUMBER:
			lua_pushnumber(luaState, valueNum);
			break;
		case LUA_TSTRING:
			lua_pushstring(luaState, valueString);
			break;
		default:
			assert(0);
		}
	}
	void Pop(CLuaState * ls) 
	{
		Pop(*ls);
	}
	void Pop(lua_State * luaState) {
		int t = lua_type(luaState, -1);
		switch (t)
		{
		case LUA_TNIL:
			SetNil();
			break;
		case LUA_TNUMBER:
			*this = lua_tonumber(luaState, -1);
			break;
		case LUA_TSTRING:
			*this = lua_tostring(luaState, -1);
			break;
		}
		lua_pop(luaState, 1);
	}
};

//////////////////////////////////////////////////////////////////
//
// CLuaFunction
//
//////////////////////////////////////////////////////////////////
class CLuaFunction: public CLuaValueBase
{
friend CLuaState;

protected:
	string funcname;
	lua_CFunction valueFunc;
	int nresults;

public:
	//constructor
	CLuaFunction() {
		valueFunc = NULL;
		vtype = LUA_TFUNCTION;
		nresults = 0;
	}
	CLuaFunction(lua_CFunction f, int nres = 0) {
		valueFunc = f;
		vtype = LUA_TFUNCTION;
		nresults = nres;
	}
	CLuaFunction(const char * fname, lua_CFunction f, int nres = 0) {
		valueFunc = f;
		funcname = fname;
		vtype = LUA_TFUNCTION;
		nresults = nres;
	}
	CLuaFunction(const char * fname, int nres = 0) {
		valueFunc = NULL;
		funcname = fname;
		vtype = LUA_TFUNCTION;
		nresults = nres;
	}
	//operator
	int operator () (CLuaState * ls) { 
		int tos0 = lua_gettop(*ls);
		if (PushForCall(ls)) 
			lua_call(*ls, 0, LUA_MULTRET);
		return lua_gettop(*ls) - tos0;
	}
	int operator () (CLuaState * ls, CLuaValueBase * arg1) {
		int tos0 = lua_gettop(*ls);
		if (PushForCall(ls))
		{
			arg1->Push(ls);
			ls->Call(1);
		}
		return lua_gettop(*ls) - tos0;
	}
	int operator () (CLuaState * ls, CLuaValueBase * arg1, CLuaValueBase * arg2) {
		int tos0 = lua_gettop(*ls);
		if (PushForCall(ls))
		{
			arg1->Push(ls);
			arg2->Push(ls);
			ls->Call(2);
		}
		return lua_gettop(*ls) - tos0;
	}
	int operator () (CLuaState * ls, CLuaValueBase * arg1, CLuaValueBase * arg2, CLuaValueBase * arg3) {
		int tos0 = lua_gettop(*ls);
		if (PushForCall(ls))
		{
			arg1->Push(ls);
			arg2->Push(ls);
			arg3->Push(ls);
			ls->Call(3);
		}
		return lua_gettop(*ls) - tos0;
	}	
	int operator () (CLuaState * ls, CLuaValueBase * arg1, CLuaValueBase * arg2, CLuaValueBase * arg3, CLuaValueBase * arg4) {
		int tos0 = lua_gettop(*ls);
		if (PushForCall(ls)) 
		{
			arg1->Push(ls);
			arg2->Push(ls);
			arg3->Push(ls);
			arg4->Push(ls);
			ls->Call(4);
		}
		return lua_gettop(*ls) - tos0;
	}	
	int operator () (CLuaState * ls, int argCount, ...) {
		int tos0 = lua_gettop(*ls);
		if (PushForCall(ls)) 
		{
			va_list valist;

			va_start(valist, argCount);
			for (int i = 0; i < argCount; i++)
			{
				CLuaValueBase * arg = va_arg(valist, CLuaValueBase*);
				arg->Push(ls);
			}
			va_end(valist);
			ls->Call(argCount);
		}
		return lua_gettop(*ls) - tos0;
	}

	//function
	void SetNResults(int n) {
		nresults = n;
	}
	int GetNResults() {
		return nresults;
	}
	void Push(CLuaState * ls) {
		assert(vtype == LUA_TFUNCTION);
		assert(valueFunc != NULL);
		lua_pushcclosure(*ls, valueFunc, 0);
	}
	void PushAndRegister(CLuaState * ls) {
		assert(funcname != "");
		Push(ls);
		ls->SetGlobal(funcname.c_str());
	}
	void PushClosure(CLuaState * ls, int closureCount, ...) {
		assert(vtype == LUA_TFUNCTION);
		assert(valueFunc != NULL);

		va_list vlist;
		va_start(vlist, closureCount);
		int i;
		for (i = 0; i < closureCount; i++)
		{
			CLuaValueBase* v = va_arg(vlist, CLuaValueBase*);
			v->Push(ls);
		}
		va_end(vlist);
		lua_pushcclosure(*ls, valueFunc, closureCount);
	}
	void PushClosureAndRegister(CLuaState * ls, int closureCount, ...) {
		assert(funcname != "");
		va_list arglist;
		va_start(arglist, closureCount);
		
		PushClosure(ls, closureCount, arglist);
		//va_end(arglist);
		ls->SetGlobal(funcname.c_str());
	}
	bool PushForCall(CLuaState * ls)
	{
		if ((funcname == "") && (valueFunc == NULL))
			return false;
		if (valueFunc == NULL)
		{
			lua_getglobal(*ls, funcname.c_str());
			
			if (lua_type(*ls, -1) == LUA_TFUNCTION)
			{
				return true;
			}
		} 
		else 
		{
			Push(ls);
			return true;
		}
		return false;
	}
	void Pop(CLuaState * ls) {
		if (lua_type(*ls, -1) == LUA_TFUNCTION)
			valueFunc = lua_tocfunction(*ls, -1);

		lua_pop(*ls, 1);
	}
};

//////////////////////////////////////////////////////////////////
//
// CLuaUserData
//
//////////////////////////////////////////////////////////////////
class CLuaUserData: public CLuaValue
{
protected:
	neByte * valueUserData;
	int tag;

public:
	//constructor
	CLuaUserData(int tg = LUA_ANYTAG){tag = tg;}
	CLuaUserData(neByte * ptr, int tg = LUA_ANYTAG) {
		valueUserData = ptr;
		tag = tg;
		vtype = LUA_TUSERDATA;
	}
	
	//operator

	//function
	void Push(lua_State * lua)
	{
		lua_pushusertag(lua, valueUserData, tag);
	}
	void Push(CLuaState * ls){
		Push(*ls);
	}
	void Pop(lua_State * lua){
		if (lua_isuserdata(lua, -1))
		{
			valueUserData = (neByte*)lua_touserdata(lua, -1);
			tag = lua_tag(lua, -1);
		}
		lua_pop(lua, 1);
	}
	
	void Pop(CLuaState * ls){
		Pop(*ls);
	}
	void SetTag(int tg) {tag = tg;}
	int GetTag() {return tag;}
	operator neByte * () const {
		return valueUserData;
	}
};

//////////////////////////////////////////////////////////////////
//
// CLuaTable
//
//////////////////////////////////////////////////////////////////
class CLuaTable
{
protected:
	int index; //on the stack where the table reside
	CLuaState * luaState;

public:
	CLuaTable() {luaState = NULL; index = -1;}

	CLuaTable(CLuaState * ls, int i) {
		luaState = ls;
		index = i;
	}
	~CLuaTable() {
		if (index != -1)
		{
			lua_remove(*luaState, index);
		}
	}
	CLuaTable(CLuaState * ls, const char * tableName) {
		luaState = ls;
		GetTable(tableName);
	}
	CLuaTable(CLuaState * ls) {
		index = -1;
		SetLuaState(ls, 1);
	}
	void SetLuaState(CLuaState* ls, int flag = 0) { 
		luaState = ls;
		if (flag && lua_type(*ls, -1) == LUA_TTABLE)
			index = lua_gettop(*ls);
	}
	void GetTable(const char * tableName) {
		lua_getglobal(*luaState, tableName);
		assert(lua_type(*luaState, -1) == LUA_TTABLE);
		index = lua_gettop(*luaState);
	}
	void Get(CLuaValueBase * key) {
		key->Push(luaState);
		lua_gettable(*luaState, index);
	}
	CLuaValueBase * Pop(CLuaValueBase * key) {
		Get(key);
		return luaState->Pop();
	}
	void Pop(CLuaValueBase * key, string & str){
		Get(key);
		luaState->Pop(str);
	}
	void Pop(CLuaValueBase * key, double & value){
		Get(key);
		luaState->Pop(value);
	}
	void Pop(CLuaValueBase * key, u32 & value){
		Get(key);
		luaState->Pop(value);
	}
	void Pop(CLuaValueBase * key, CLuaTable & tab){
		Get(key);
		tab.SetLuaState(luaState, 1);
	}
	void Pop(CLuaValueBase * key, CLuaFunction & fun){
		Get(key);
		luaState->Pop(fun);
	}
	void Set(CLuaValueBase * key, CLuaValueBase * value) {
		key->Push(luaState);
		value->Push(luaState);
		lua_settable(*luaState, index);
	}
};

//////////////////////////////////////////////////////////////////
//
// C++ to Lua binding using template (base on Luna)
//
//////////////////////////////////////////////////////////////////
template <class T> class Luna
{
public:
	/* member function map */
	struct RegType { 
		  const char* name; 
		  const int(T::*mfunc)(lua_State*);
	};      

	/* register class T */
	static void Register(lua_State* L) {
		lua_pushcfunction(L, &Luna<T>::constructor);
		lua_setglobal(L, T::className);
      
		if (otag == 0) {
			otag = lua_newtag(L);
			lua_pushcfunction(L, &Luna<T>::gc_obj);
			lua_settagmethod(L, otag, "gc"); /* tm to release objects */
		}
	}

private:
	static int otag; /* object tag */

	/* member function dispatcher */
	static int thunk(lua_State* L) {
		/* stack = closure(-1), [args...], 'self' table(1) */
		int i = static_cast<int>(lua_tonumber(L,-1));
		lua_pushnumber(L, 0); /* userdata object at index 0 */
		lua_gettable(L, 1);
		T* obj = static_cast<T*>(lua_touserdata(L,-1));
		lua_pop(L, 2); /* pop closure value and obj */
		return (obj->*(T::Register[i].mfunc))(L);
	}
	   
	/* constructs T objects */
	static int constructor(lua_State* L) {
		T* obj= new T(L); /* new T */
		/* user is expected to remove any values from stack */

		lua_newtable(L); /* new table object */
		lua_pushnumber(L, 0); /* userdata obj at index 0 */
		lua_pushusertag(L, obj, otag); /* have gc call tm */
		lua_settable(L, -3);

		/* register the member functions */
		for (int i=0; T::Register[i].name; i++) {
		   lua_pushstring(L, T::Register[i].name);
		   lua_pushnumber(L, i);
		   lua_pushcclosure(L, &Luna<T>::thunk, 1);
		   lua_settable(L, -3);
		}
		return 1; /* return the table object */
	}
	   
	/* releases objects */
	static int gc_obj(lua_State* L) {
		T* obj = static_cast<T*>(lua_touserdata(L, -1));
		delete obj;
		return 0;
	}

protected: 
	Luna(); /* hide default constructor */
};
template <class T> int Luna<T>::otag = 0;

//////////////////////////////////////////////////////////////////
//
// inlines
//
//////////////////////////////////////////////////////////////////

inline CLuaValueBase * CLuaState::Pop() {
	CLuaValueBase * ret;
	switch (lua_type(luaState, -1))
	{
	case LUA_TNONE:
		assert(0);
		break;
	case LUA_TUSERDATA:
		assert(0);
		break;
	case LUA_TNIL:
		ret = new CLuaValue();
		lua_pop(luaState, 1);
		break;
	case LUA_TNUMBER:
		ret = new CLuaValue(lua_tonumber(luaState, -1));
		lua_pop(luaState, 1);
		break;
	case LUA_TSTRING:
		ret = new CLuaValue(lua_tostring(luaState, -1));
		lua_pop(luaState, 1);
		break;
	case LUA_TTABLE:
		lua_pop(luaState, 1);
		return NULL;
		break;
	case LUA_TFUNCTION:
		ret = new CLuaFunction(lua_tocfunction(luaState, -1));
		lua_pop(luaState, 1);
		break;
	}
	return ret;
}

inline void CLuaState::Pop(CLuaFunction & fun) {
	if (lua_type(luaState, -1) != LUA_TNIL)
	{
		fun.funcname = lua_tostring(luaState, -1);
		fun.valueFunc = NULL;
	}
	else
	{
		//fun.valueFunc = NULL;
		//fun.funcname = "";
	}
	lua_pop(luaState, 1);
}

inline void CLuaValueBase::SetGlobal(CLuaState * ls, const char * name)
{
	Push(ls);
	ls->SetGlobal(name);
}
#endif //NE_LUAWRAPPER_H