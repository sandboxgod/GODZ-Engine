
#include "Godz.h"
#include "GenericPackage.h"
#include <tchar.h>
#include "GenericObject.h"
#include "crc32.h"
#include "ResourceObject.h"
#include "Material.h"

namespace GODZ
{

void PackageDependencies::Reserve(size_t num)
{
	m_dependencies.reserve(num);
}

void PackageDependencies::Add(HashCodeID hash)
{
	m_dependencies.push_back(hash);
}

size_t PackageDependencies::Size() const
{
	return m_dependencies.size();
}

HashCodeID PackageDependencies::Get(size_t index) const
{
	return m_dependencies[index];
}

bool PackageDependencies::Contains(HashCodeID hash)
{
	size_t num = m_dependencies.size();
	for(size_t i=0;i<num;i++)
	{
		if (m_dependencies[i] == hash)
			return true;
	}

	return false;
}

//---------------------------------------------------
//declare member functions
//---------------------------------------------------

GenericPackage::GenericPackage(const char* name, EPackageFlag flag)
: packageFlag(flag)
, packageName(name)
{
	//TODO: enable this constructor only if EDITOR compile flag set, etc
	HString temp( name );
	mHash = temp.getHashCode();
}

GenericPackage::GenericPackage(const char* name, HashCodeID hash, EPackageFlag flag)
: packageFlag(flag)
, packageName(name)
, mHash(hash)
{
}

GenericPackage::~GenericPackage()
{
	//bad news is we cant check any of the objects we're releasing here to see if they're leaking since
	//there might be an object from another package that still points to something

	Clear();
}

void GenericPackage::Clear()
{
	Lock();
	m_obj.clear();
	m_resourceList.clear();
	Unlock();
}

void GenericPackage::AddDependency(HashCodeID hash)
{
	Lock();
	m_dependencies.Add(hash);
	Unlock();
}

//NOTE: This function is not thread-safe per se. Would need to return a copy for thread safety
const PackageDependencies& GenericPackage::GetPackageDependencies()
{
	return m_dependencies;
}

bool GenericPackage::IsDependantOn(HashCodeID hash)
{
	return m_dependencies.Contains(hash);
}

GenericObject* GenericPackage::GetObjectAt(size_t index) 
{
	GenericObject* p = NULL;

	Lock();
	godzassert(index < m_obj.size()); 

	p = m_obj[index].mObject;

	//unlock
	Unlock();

	return p;
}

atomic_ptr<GenericObject> GenericPackage::GetReferenceAt(size_t index) 
{
	atomic_ptr<GenericObject> ptr;

	Lock();
	godzassert(index < m_obj.size()); 

	ptr = m_obj[index].mObject;

	//unlock
	Unlock();

	return ptr;
}

void GenericPackage::AddObject(const atomic_ptr<GenericObject> obj) 
{ 
	godzassert( !FindObject(obj) ); //make sure we dont already own this object

	Lock();
	PackageObject packageObject;
	packageObject.mObject = obj;

	// We don't bother giving IDs to annoymous objects. Otherwise, objects like 
	// Classes and such would get a brand new ID everytime the Core DLL runs.
	packageObject.mId = GUID_NULL;
	m_obj.push_back( packageObject ); 

	Unlock();
}

void GenericPackage::AddObject(const atomic_ptr<GenericObject> obj, GUID id) 
{ 
	godzassert( !FindObject(obj) ); //make sure we dont already own this object

	Lock();

	PackageObject packageObject;
	packageObject.mObject = obj;
	packageObject.mId = id;
	m_obj.push_back( packageObject ); 

	Unlock();
}

bool GenericPackage::RemoveObject(GenericObject* obj)
{
	bool result = false;

	Lock();

	EntityList::iterator iter = m_obj.begin();
	for(; iter != m_obj.end(); iter++)
	{
		GenericObject* ref = iter->mObject;
		if (ref == obj)
		{
			m_obj.erase(iter);
			result = true;
			break;
		}
	}

	Unlock();

	return result;
}

bool GenericPackage::FindObject(GenericObject* obj)
{
	size_t index = 0;
	return FindObject(obj, index);
}

bool GenericPackage::FindObject(GenericObject* obj, size_t &index)
{
	bool found = false;

	Lock();

	size_t len = m_obj.size();
	for(index=0;index<len;index++)
	{
		if (obj == m_obj[index].mObject)
		{
			found = true;
			break;
		}
	}

	Unlock();

	return found;
}

atomic_ptr<GenericObject> GenericPackage::FindObjectRef(HashCodeID hash)
{
	atomic_ptr<GenericObject> ptr;

	Lock();

	size_t len = m_obj.size();
	for(size_t index=0;index<len;index++)
	{
		if (m_obj[index].mObject->GetHashCode() == hash)
		{
			ptr = m_obj[index].mObject;
			break;
		}
	}

	Unlock();

	return ptr;
}

// Returns the shared_ptr to the object
atomic_ptr<GenericObject> GenericPackage::FindObjectRef(GenericObject* obj)
{
	atomic_ptr<GenericObject> ptr;

	Lock();

	size_t len = m_obj.size();
	for(size_t index=0;index<len;index++)
	{
		if (m_obj[index].mObject == obj)
		{
			ptr = m_obj[index].mObject;
			break;
		}
	}

	Unlock();

	return ptr;
}

GenericObject* GenericPackage::FindObject(HashCodeID hash)
{
	Lock();

	GenericObject* obj = NULL;

	size_t len = m_obj.size();
	for(size_t index=0;index<len;index++)
	{
		if (m_obj[index].mObject->GetHashCode() == hash)
		{
			obj = m_obj[index].mObject;
			break;
		}
	}

	Unlock();

	return obj;
}

GenericObject* GenericPackage::FindObjectByGUID(GUID id)
{
	Lock();

	GenericObject* obj = NULL;

	size_t len = m_obj.size();
	for(size_t index=0;index<len;index++)
	{
		if (m_obj[index].mId == id)
		{
			obj = m_obj[index].mObject;
			break;
		}
	}

	Unlock();

	return obj;
}

atomic_ptr<GenericObject> GenericPackage::FindObjectRefByGUID(GUID id)
{
	atomic_ptr<GenericObject> ptr;

	Lock();

	size_t len = m_obj.size();
	for(size_t index=0;index<len;index++)
	{
		if (m_obj[index].mId == id)
		{
			ptr = m_obj[index].mObject;
			break;
		}
	}

	Unlock();

	return ptr;
}

GUID GenericPackage::GetGUIDForObject(GenericObject* obj)
{
	GUID id = GUID_NULL;

	Lock();

	size_t len = m_obj.size();
	for(size_t index=0;index<len;index++)
	{
		if (m_obj[index].mObject == obj)
		{
			id = m_obj[index].mId;
			break;
		}
	}

	Unlock();

	return id;
}

EPackageFlag GenericPackage::GetPackageFlag() const
{
	return packageFlag;
}

const char* GenericPackage::GetName() const
{
	return packageName.c_str();
}

void GenericPackage::GetObjectsOfType(GenericClass* type, GenericObjectList& objects)
{
	Lock();

	size_t len = m_obj.size();
	for(size_t i=0;i<len;i++)
	{
		if (m_obj[i].mObject->IsA(type))
		{
			objects.Add(m_obj[i].mObject);
		}
	}

		
	Unlock();
}

//only expected to be used by Editor
void GenericPackage::SetName(const char* newName)
{
	packageName = newName;
	HString temp(newName);
	mHash = temp.getHashCode();
}

//GenericPackage is setup to be a synchronized object however, we don't really ever expect
//this functionality to be needed. So we throw an error if we ever detect 2 threads competing
//to access data
void GenericPackage::Lock()
{
#ifdef _DEBUG
	bool error_printed = false;

	while(!m_packageLock.Set())
	{
		if (!error_printed)
		{
			error_printed = true;
			Log("GenericPackage::Lock() -- Thread collision detected at GenericPackage %s\n", packageName.c_str());
			//godzassert(0);
		}
	}
#endif //_DEBUG
}

void GenericPackage::Unlock()
{
#ifdef _DEBUG
	m_packageLock.UnSet();
#endif //_DEBUG
}


void GenericPackage::AddResource(ResourceObject *resource)
{
	HashCodeID hash = resource->GetHashCode();
	godzassert( hash != INVALID_HASH );
	m_resourceList[hash] = GenericReference<ResourceObject>(resource);
}

ResourceObject* GenericPackage::findResource(HashCodeID hash)
{
	return m_resourceList[hash];
}

void GenericPackage::ReserveObjects(size_t num)
{
	Lock();
	m_obj.reserve(num);
	Unlock();
}

void GenericPackage::GetResourceIterators(GenericPackage::ResourceList::iterator& begin, GenericPackage::ResourceList::iterator& end)
{
	begin = m_resourceList.begin();
	end = m_resourceList.end();
}

void GenericPackage::OnLoaded()
{
	Lock();
	for(size_t i = 0; i < m_obj.size(); i++)
	{
		m_obj[i].mObject->OnPostLoad();
	}
	Unlock();
}

} //namespace GODZ