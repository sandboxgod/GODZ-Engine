/* ===========================================================
	Static Class Instance used to identify a type of object.

	Created Nov 11, 2003 by Richard Osborne
	Copyright 2010, Richard Osborne
	========================================================== 
*/

#include "GenericClass.h"
#include "ClassProperty.h"
#include <CoreFramework/Core/GenericObjData.h>
#include <CoreFramework/Core/crc32.h>
#include <CoreFramework/Core/Material.h>


namespace GODZ
{

GenericClass::GenericClass(const char* _Name, GenericClass* parent)
: Super(NULL)
, ClassFlags(0)
, m_package(NULL)
{
	//set a flag to indicate this is a STATIC CLASS
	//ClassFlags |= GF_StaticClass;

	if (_Name != NULL || parent != NULL)
		Setup(_Name, parent);
}

GenericClass::~GenericClass()
{
}

bool GenericClass::IsParent(GenericClass* godzClass)
{
	GenericClass* gc = Super;
	while (gc != NULL)
	{
		if (gc == godzClass)
			return true;

		gc = gc->GetSuperClass();
	}

	return false;
}

GenericObject* GenericClass::GetInstance(const char* Name, GenericClass* parent)
{
	return NULL;
}

void GenericClass::Setup(const char* NewObjectName, GenericClass* parent)
{
	godzassert(GenericObjData::m_pCurrentPackage != NULL); // "Class instanced before package set"

	m_package = GenericObjData::m_pCurrentPackage;

	mName = NewObjectName;

	Super=parent;

	//TODO: Should ensure no other classes already have this hash code registered?
	m_HashCode = ECRC32::Get_CRC(NewObjectName);
}

void GenericClass::SetReference(const atomic_ptr<GenericClass>& ptr)
{
	GenericObjData::m_pCurrentPackage->AddObject(ptr);
}

atomic_ptr<GenericClass> GenericClass::GetReference()
{
	return m_package->FindObjectRef(this);
}

GenericClass* GenericClass::GetRuntimeClass()
{
	return NULL;
}

int GenericClass::GetClassFlags()
{
	return ClassFlags;
}

GenericClass* GenericClass::GetSuperClass()
{
	return Super;
}


void GenericClass::AddProperty(ClassProperty* property)
{
	m_propertyList[property->GetHashCode()] = property;
}

ClassProperty* GenericClass::GetProperty(HString propertyName)
{
	ClassProperty* cp = m_propertyList[ propertyName.getHashCode() ];
	if ( cp != NULL )
	{
		return cp;
	}

	if (Super != NULL)
	{
		return Super->GetProperty(propertyName);
	}

	return NULL;
}

size_t GenericClass::GetNumProperties()
{
	size_t c = 0;

	if (Super != NULL)
	{
		c += Super->GetNumProperties();
	}
	
	PropertyList::iterator iter;
	for(iter = m_propertyList.begin(); iter != m_propertyList.end(); iter++)
	{
		if (iter->second != NULL)
		{
			c++;
		}
	}

	return c;
}

ClassProperty* GenericClass::GetPropertyAt(HashCodeID hash)
{
	return m_propertyList[hash];
}

void GenericClass::GetProperties(std::vector<ClassProperty*>& list)
{
	PropertyList::iterator iter;
	for(iter = m_propertyList.begin(); iter != m_propertyList.end(); iter++)
	{
		if (iter->second != NULL)
		{
			list.push_back(iter->second);
		}
	}

	if (Super != NULL)
	{
		Super->GetProperties(list);
	}
}

// Returns properties that only belong to this class
void GenericClass::GetPropertiesNonRecurse(std::vector<ClassProperty*>& list)
{
	PropertyList::iterator iter;
	for(iter = m_propertyList.begin(); iter != m_propertyList.end(); iter++)
	{
		if (iter->second != NULL)
		{
			list.push_back(iter->second);
		}
	}
}

bool GenericClass::IsPlaceable()
{
	return (ClassFlags & ClassFlag_Placeable) == ClassFlag_Placeable;
}


atomic_ptr<Material> GenericClass::GetMaterial()
{
	if (m_mat != NULL)
	{
		return m_mat;
	}
	else if (Super != NULL)
	{
		return Super->GetMaterial();
	}

	return atomic_ptr<Material>();
}

void GenericClass::SetMaterial(atomic_ptr<Material> m)
{
	m_mat = m;
}


}
