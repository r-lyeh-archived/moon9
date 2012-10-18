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

#ifndef INTERFACE_H
#define INTERFACE_H

#pragma warning(disable:4786)
#include <iostream>
#include <string>
#include <vector>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CObjectFactory
//	CMemObject
//	CObjectSetupParam
//
//	These 3 objects work togather to provide the ObjectFactory 
//	pattern.
//	
//	ObjectFactory is the object handling the request.
//		It also maintain the list of object it has created
//
//	CMemObject is base class of all object created by ObjectFactory
//		it contain information to allow garabage collection. This
//		is done by inheriting all objects created by ObjectFactory
//		from CMemObject, so that when a object is created, it is 
//		inserted into a list in the ObjectFactory. When the object
//		is deleted, the virtual destructor will then remove itself
//		from that list. 
//
//	FactoryParam contain parameters require for the creation of
//		a object. This parameter is pass to the ObjectFactory
//		as part of the request.
//
///////////////////////////////////////////////////////////////////
class CObjectFactory;
class CObjectSetupParam;

class CMemObject
{
friend class CObjectFactory;
public:
	CMemObject(CObjectFactory * factory, bool unique);
	virtual ~CMemObject();
	void Free();
	virtual bool Setup(CObjectSetupParam * param) = 0;
	string ResourceID();

protected:
	CObjectFactory * factory;
	bool isUnique;
	int refCount;
	string resourceID;
};

///////////////////////////////////////////////////////////////////
class CObjectSetupParam
{
public:
	string resourceID;

// force it to become polymorphic
	virtual void Dummy(){};
};

///////////////////////////////////////////////////////////////////
typedef vector<CMemObject *>::iterator TMemObjectIter;

class CObjectFactory
{
friend class CMemObject;
public:
	CMemObject * MakeInstance(int objType, CObjectSetupParam * param, bool unique = true);
	virtual ~CObjectFactory();
	vector<CMemObject *> & GetAllInstances() {return allInstances;}
	
protected:
	vector<CMemObject *> allInstances;
	bool FreeObject(CMemObject& mem);
	TMemObjectIter FindMemObject(const CMemObject& mem);
	TMemObjectIter FindMemObjectByResource(const string& resourceID);

protected:
	virtual CMemObject * _MakeInstance(int objType, bool unique = true) = 0;
	virtual void DeleteMemObject(CMemObject& mem);
};

///////////////////////////////////////////////////////////////////
// 
//	Persistant interface
//
///////////////////////////////////////////////////////////////////
class IPersistant
{
public:
	virtual void Save(ostream & ostr) = 0;
	virtual void Load(istream & istr) = 0;
};

#endif //INTERFACE_H