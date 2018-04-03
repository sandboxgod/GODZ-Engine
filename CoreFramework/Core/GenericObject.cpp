
#include "GenericObject.h"
#include "SceneManager.h"
#include "GenericPackage.h"
#include "WindowManager.h"
#include "GenericObjData.h"
#include "crc32.h"
#include "SceneData.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>
#include "StringTokenizer.h"


using namespace std;


namespace GODZ
{



ImplementGeneric(GenericObject)
REGISTER_HASHSTRING(GenericObject, Name)

//---------------------------------------------------
//declare the static members
//---------------------------------------------------

IGenericConfig*				GlobalConfig::m_pConfig=NULL;

//---------------------------------------------------
//declare globals
//---------------------------------------------------

//All access to g_pObjData must be double checked in most builds. Every genericobject,
//upon creation, is stored into the global list...
GenericObjData g_pObjData;

//---------------------------------------------------
//namespace functions
//---------------------------------------------------

//Stores Class instances to a global list
void GODZ::PushGenericObject(GenericObject* pObj)
{
	//Thread check: Note, if a DLL is registered later on and it attempts to push new Classes into the registry we will get
	//a thread violation here (if its getting registered from a different thread). So, this is the deal, only register
	//new Classes in the main thread. NEVER attempt to register a Class from another thread	

	//g_pObjData.m_pClass is supposed to be read-only after the engine has started up....
	godzassert (!pObj->IsAObject());
	
	//Make sure no Classes are getting dynamically generated during run time. Class registration must end before game loop starts.
	//Otherwise we risk another thread trying to register a Class while a resource loading thread is trying to instantiate a new object.
	godzassert(!GenericObjData::mIsClassListBuilt);


	g_pObjData.m_pClass[g_pObjData.counter] = static_cast<GenericClass*>(pObj);
	g_pObjData.m_pClass[g_pObjData.counter]->SetReference( g_pObjData.m_pClass[g_pObjData.counter] );
	g_pObjData.counter++;
	godzassert(g_pObjData.counter < g_pObjData.MAX_OBJECT_SIZE); //"There are more classes than the max allowed number. Please increment MAX_CLASS_SIZE in CGenericObject.h");

}

//Note: should be modified to accept a package
GODZ_API GenericClass* GODZ::StaticLoadObject(const HString& hashString)
{
	GenericClass* ret = NULL;

	//Searches the object list for a static class instance that matches
	//the requested name.
	for(udword i=0; i < g_pObjData.counter; i++)
	{
		GenericClass* cclass = g_pObjData.m_pClass[i];

		if ( cclass->GetHashCode() == hashString.getHashCode() )
		{
			ret = g_pObjData.m_pClass[i];
			break;
		}
	}


	return ret;
}

//Should be called to shutdown the engine after all threads have been halted/removed....
void GenericObject::DestroyAllObjects()
{
	Log("Begin shutdown\n");
	//Log("Classes %d bytes\n", GlobalHeaps::m_pClassHeap->GetUsedMemory());
	//Log("Objects %d bytes\n", GlobalHeaps::m_pObjectHeap->GetUsedMemory());
	//Log("Application %d bytes - allocated on %d heaps\n", GlobalHeaps::m_pAppHeap->GetUsedMemory(), GlobalHeaps::m_pAppHeap->GetNumHeaps() + 1);	

	//drop levels before we drop all the packages....
	if (SceneManager::IsInstanced())
	{
		SceneManager* smgr = SceneManager::GetInstance();
		smgr->Shutdown();
	}

	//drop all loaded packages...
	Log("Unloading all object packages\n");
	GenericObjData::m_packageList.DropPackages();

	//destroy all registered classes
	for(udword i=0;i<g_pObjData.counter;i++)
	{
		g_pObjData.m_pClass[i] = NULL;
	}

	//set this before singletons are destroyed. this way if a singleton sort of depends on another for some odd reason, it can
	//check this variable to see what's up
	EngineStatus::engineRunning = false; //singletons are no longer valid, etc....

	//release all the global singleton objects
	DestroySingletons();
	
	//release the application window object
	Log("Release WindowManager\n");
	WindowManager::Release(); 

	//release all the global data, This should occur last after all of the genericobjects are destroyed. This way, we can be kinda sure no new
	//events from objects are sent out.
	SceneData::Shutdown();

}


GODZ_API size_t GODZ::GetClassCount()
{
	return GenericObjData::counter;
}

//////////////////////////////////////////////////////////////////////

//Used to copy over Map Property values
class PropertyCopierCallback : public MapCallback
{
public:
	PropertyCopierCallback(ClassProperty* cp, GenericObject* src, GenericObject* dest)
		: mSource(src)
		, mDest(dest)
		, mProperty(cp)
	{
	}

	virtual void analyzeMapPair(GenericReference<PropertyValue> key, GenericReference<PropertyValue> value)
	{
		mProperty->AddElement(mDest, key->clone(), value->clone());
	}

private:
	GenericObject* mSource;
	GenericObject* mDest;
	ClassProperty* mProperty;
};

//---------------------------------------------------
// member functions
//---------------------------------------------------

GenericObject::GenericObject()
{
}

GenericObject::~GenericObject()
{
}

GUID GenericObject::CreateGUID()
{
	GUID id;
	HRESULT hr = CoCreateGuid(&id);
	godzassert( !FAILED( hr ) );
	return id;
}

GUID GenericObject::GetEmptyGUID()
{
	static GUID empty = GUID_NULL; 
	return empty;
}

// Convience function that checks to see if this Object was assigned a GUID
bool GenericObject::GetGUIDFromPackage(GUID & id)
{
	GenericPackage* p = GenericObjData::m_packageList.GetPackage(this);
	if ( p != NULL)
	{
		id = p->GetGUIDForObject(this);
		return id != GUID_NULL;
	}

	return false;
}

GenericPackage* GenericObject::GetPackage()
{
	return GenericObjData::m_packageList.GetPackage(this);
}

void GenericObject::Serialize(GDZArchive& ar)
{
}

atomic_ptr<GenericObject> GenericObject::GetClone()
{
	GenericClass* pClass = GetRuntimeClass();
	atomic_ptr<GenericObject> dest( pClass->GetInstance() );
	CopyTo(dest);

	return dest;
}

void GenericObject::CopyTo(GenericObject* pObj)
{
	GenericClass* pClass = GetRuntimeClass();

	std::vector<ClassProperty*> cpList;
	pClass->GetProperties(cpList);

	size_t n = cpList.size();
	for(size_t i=0;i<n;i++)
	{
		EPropertyType t = cpList[i]->GetPropertyType();
		switch(t)
		{
		default:
			{
				godzassert(0); //unknown property type
			}
			break;
		case PT_BOOL:
			{
				bool flag = cpList[i]->GetBool(this);
				cpList[i]->SetBool(pObj, flag);
			}
			break;
		case PT_HASHSTRING:
			{
				HString s = cpList[i]->GetHashString(this);
				cpList[i]->SetHashString(pObj, s);
			}
			break;
		case PT_HASHCODE:
			{
				HashCodeID hash = cpList[i]->GetHashCode(this);
				cpList[i]->SetHashCode(pObj, hash);
			}
			break;
		case PT_FLOAT:
			{
				float val = cpList[i]->GetFloat(this);
				cpList[i]->SetFloat(pObj, val);
			}
			break;
		case PT_INT:
			{
				int val = cpList[i]->GetInt(this);
				cpList[i]->SetInt(pObj, val);
			}
			break;
		case PT_VECTOR:
			{
				const Vector3& val = cpList[i]->GetVector(this);
				cpList[i]->SetVector(pObj, val);
			}
			break;
		case PT_STRING:
			{
				const char* val = cpList[i]->GetString(this);
				cpList[i]->SetString(pObj, val);
			}
			break;
		case PT_OBJECT:
			{
				GenericObject* val = cpList[i]->getObject(this);

				if (val != NULL)
				{
					atomic_ptr<GenericObject> objPtr( val->GetClone() );
					cpList[i]->setObject(pObj, objPtr);
				}
			}
			break;
		case PT_BOX:
			{
				const WBox& box = cpList[i]->GetBox(this);
				cpList[i]->SetBox(pObj, box);
			}
			break;
		case PT_MATRIX:
			{
				const WMatrix& m = cpList[i]->GetMatrix(this);
				cpList[i]->SetMatrix(pObj, m);
			}
			break;
		case PT_MATRIX3:
			{
				const Matrix3& m = cpList[i]->GetMatrix3(this);
				cpList[i]->SetMatrix3(pObj, m);
			}
			break;
		case PT_RESOURCE:
			{
				ResourceObject* resource = cpList[i]->GetResource(this);
				cpList[i]->SetResource(pObj, resource);
			}
			break;
		case PT_UINT32:
			{
				UInt32 val = cpList[i]->GetUInt32(this);
				cpList[i]->SetUInt32(pObj, val);
			}
			break;
		case PT_FLOAT4:
			{
				const Color4f& color = cpList[i]->GetColor(this);
				cpList[i]->SetColor(pObj, color);
			}
			break;
		case PT_ARRAY:
			{
				cpList[i]->Clear(pObj);
				size_t n = cpList[i]->GetSize(this);
				for(size_t i=0;i<n;i++)
				{
					GenericReference<PropertyValue> value = cpList[i]->GetValueAt(this, i);
					cpList[i]->AddValue(pObj, value->clone());
				}
			}
			break;
		case PT_MAP:
			{
				cpList[i]->Clear(pObj);
				PropertyCopierCallback iter(cpList[i], this, pObj);
				cpList[i]->IteratePairs(this, &iter);
			}
			break;
		}
	}
}

bool GenericObject::IsA(GenericClass* type)
{
	GenericClass* Class = GetRuntimeClass();
	godzassert(Class != NULL); 
	
	if (Class == type)
	{
		return true;
	}

	GenericClass* sObj = Class->GetSuperClass();
	while(sObj != NULL)
	{
		if (sObj == type)
			return true;

		sObj = sObj->GetSuperClass();
	}

	return false;
}

bool GenericObject::IsA(HString type)
{
	GenericClass* Class = GetRuntimeClass();
	godzassert(Class != NULL); 
	
	if (Class->GetHashCode() == type)
	{
		return true;
	}

	GenericClass* sObj = Class->GetSuperClass();
	while(sObj != NULL)
	{
		if (sObj->GetHashCode() == type)
			return true;

		sObj = sObj->GetSuperClass();
	}

	return false;
}

bool GenericObject::IsAObject()
{
	return true;
}

void GenericObject::OnPropertiesUpdated()
{
}

//converts String-to-Hash
void GenericObject::SetName(HString name)
{
	m_HashCode = name;
}

HString GenericObject::GetName()
{
	return m_HashCode;
}

void GenericObject::OnPostLoad()
{
}

} // namespace GODZ