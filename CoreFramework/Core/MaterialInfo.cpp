
#include "MaterialInfo.h"


namespace GODZ
{

/////////////////////////////////////////////////////////////////////////////////////////

MaterialCreationEvent::MaterialCreationEvent(MaterialInfo& info)
: mInfo(info)
, ObjectStateChangeEvent(ObjectStateChangeEventID_MaterialCreate)
{
}

MaterialDestructionEvent::MaterialDestructionEvent(const MaterialID& key)
: m_id(key)
, ObjectStateChangeEvent(ObjectStateChangeEventID_MaterialDestroy)
{
}

//////////////////////////////////////////////////////////////////////////////////////////

MaterialInfo::~MaterialInfo()
{
	Clear();
}

void MaterialInfo::Clear()
{
	GenericReference<RenderDeviceObject> empty;
	m_pShader.setValue(empty);

	std::vector<MaterialInfoObserver*>::iterator iter;
	for( iter = m_observers.begin(); iter != m_observers.end(); iter++)
	{
		MaterialInfoObserver* ob = *iter;
		ob->OnDestroyed(this);
	}
	m_observers.clear();

	//delete the parameters we allocated...
	ParameterMap::iterator iter2;
	for(iter2 = mMap.begin(); iter2 != mMap.end(); iter2++)
	{
		ParameterInfo* ptr = (iter2->second);
		delete ptr;
	}

	mMap.clear();
}

void MaterialInfo::DropObserver(MaterialInfoObserver* observer)
{
	std::vector<MaterialInfoObserver*>::iterator iter;
	for( iter = m_observers.begin(); iter != m_observers.end(); iter++)
	{
		if (*iter == observer)
		{
			m_observers.erase(iter);
			break;
		}
	}
}

void MaterialInfo::AddObserver(MaterialInfoObserver* observer)
{
	m_observers.push_back(observer);
}

void MaterialInfo::AddTexture(HString name, Future<GenericReference<RenderDeviceObject> > textureFuture)
{
	TextureParameterInfo* tex = new TextureParameterInfo;
	tex->m_tex = textureFuture;

	mMap[name.getHashCode()] = tex;
}

Future<GenericReference<RenderDeviceObject> > MaterialInfo::GetTexture(HString name)
{
	ParameterInfo* p = mMap[name.getHashCode()];

	if (p != NULL && p->getType() == ParameterInfo::ParameterInfoType_Texture)
	{
		TextureParameterInfo* tex = (TextureParameterInfo*)p;
		return tex->m_tex;
	}

	return Future<GenericReference<RenderDeviceObject> >();
}

void MaterialInfo::AddFloatParameter(HString name, FloatParameterInfo& fpi)
{
	FloatParameterInfo* flt = new FloatParameterInfo();
	*flt = fpi;

	mMap[name.getHashCode()] = flt;
}

ParameterInfo* MaterialInfo::GetParameter(HString name)
{
	return mMap[name.getHashCode()];
}

} //namespace GODZ