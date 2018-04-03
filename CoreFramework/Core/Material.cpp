
#include "Material.h"
#include "StringTokenizer.h"
#include "SceneData.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>


namespace GODZ
{

ImplementGeneric(Material)
REGISTER_RESOURCE(Material, Shader, ShaderResource)
REGISTER_MAP(Material, Map)

ImplementGeneric(MaterialParameter)

ImplementGeneric(TextureMaterialParameter)
REGISTER_RESOURCE(TextureMaterialParameter, Texture, TextureResource)

ImplementGeneric(FloatMaterialParameter)
REGISTER_ARRAY(FloatMaterialParameter, Values, float )

/////////////////////////////////////////////////////////////////////////////////////////

void TextureMaterialParameter::SetTexture(TextureResource* tex)
{
	mTex = tex;
	mTex->CreateRenderDeviceObject();
}

void TextureMaterialParameter::CreateRenderDeviceObject()
{
	if (mTex != NULL)
	{
		mTex->CreateRenderDeviceObject();
	}
}

void TextureMaterialParameter::Publish(Material* mat, HashCodeID hash)
{
	mat->PublishTexture(mTex, hash);
}

/////////////////////////////////////////////////////////////////////////////////////////

FloatMaterialParameter::FloatMaterialParameter()
{
}

FloatMaterialParameter::~FloatMaterialParameter()
{
}

void FloatMaterialParameter::Publish(Material* mat, HashCodeID hash)
{
	mat->PublishFloat(this, hash);
}

void FloatMaterialParameter::setNumParameters(size_t num)
{
	mValues.reserve(num);
}

float FloatMaterialParameter::getValue(size_t index)
{
	godzassert(index < mValues.size());
	return mValues[index];
}

void FloatMaterialParameter::addValue(float value)
{
	mValues.push_back(value);
}

/////////////////////////////////////////////////////////////////////////////////////////
Material::Material()
{
	//materials need a unique id
	mMatInfo.m_id = SceneData::GetNextMaterialID();

	MaterialCreationEvent* ev = new MaterialCreationEvent(mMatInfo);
	SceneData::AddObjectStateChangeEvent(ev);
}

Material::~Material()
{
	MaterialDestructionEvent* ev = new MaterialDestructionEvent(mMatInfo.m_id);
	SceneData::AddObjectStateChangeEvent(ev);
}

ShaderResource* Material::GetShader()
{
	return m_pShader;
}

void Material::SetShader(ShaderResource* shader)
{
	m_pShader = shader;

	//send over matinfo again... TODO: Make it so we only send over the future-- without entire struct
	mMatInfo.m_pShader = shader->GetRenderDeviceObject();
	CommitChanges();
}

rstring Material::GetTextureName(const char* filename)
{
	StringTokenizer st(filename, "\\.");
	
	int size = static_cast<int>(st.size());
	rstring text = st[size - 2];

	return text;
}

void Material::PublishTexture(TextureResource* resource, HString type)
{
	//set texture slot on render
	TextureParameterInfo tex;
	if (resource != NULL)
	{
		tex.m_tex = resource->GetRenderDeviceObject();
	}

	mMatInfo.AddTexture(type, tex.m_tex);

	//copy over texture updates
	MaterialTexUpdateEvent* texEvent = new MaterialTexUpdateEvent(mMatInfo.m_id);
	texEvent->mTex = tex;
	texEvent->mName = type;
	SceneData::AddObjectStateChangeEvent(texEvent);
}

void Material::PublishFloat(FloatMaterialParameter* fp, HString name)
{
	FloatParameterInfo floatInfo;
	size_t num = fp->getNumParameters();
	for(size_t i = 0; i < num; i++)
	{
		floatInfo.add( fp->getValue(i) );
	}

	MaterialFloatUpdateEvent* ev = new MaterialFloatUpdateEvent(mMatInfo.m_id, floatInfo);
	ev->mName = name;
	SceneData::AddObjectStateChangeEvent(ev);
}

void Material::OnPostLoad()
{
	ParameterMap::iterator iter = mMap.begin();
	for(; iter != mMap.end(); iter++)
	{
		iter->second->Publish(this, iter->first);
	}
}


int	Material::GetTechnique()
{
	return 0; //this code is pretty much not completed yet :(
}


void Material::SetTechnique(int tech)
{
	//this code is pretty much not completed yet :(
}

//commit changes to render thread
void Material::OnPropertiesUpdated()
{
	CommitChanges();
}

void Material::CommitChanges()
{
	//update/replace render thread material data
	MaterialUpdateEvent* ev = new MaterialUpdateEvent();

	ev->m_id = mMatInfo.m_id;
	ev->m_pShader = mMatInfo.m_pShader;

	SceneData::AddObjectStateChangeEvent(ev);
}

//accessor method
void Material::SetBitmap(ResourceObject* resource)
{
	godzassert(resource->GetType() == ResourceObject::RF_Texture);

	MaterialParameter* mp = GetParameter(TextureTypes::Diffuse);
	TextureMaterialParameter* tex = NULL;

	if (mp == NULL)
	{
		tex = new TextureMaterialParameter();
	}
	else
	{
		tex = (TextureMaterialParameter*)mp;
	}

	TextureResource* tr = (TextureResource*)resource;
	tex->SetTexture(tr);
	SetParameter(TextureTypes::Diffuse, tex);
}

ResourceObject*	Material::GetBitmap()
{
	MaterialParameter* mp = GetParameter(TextureTypes::Diffuse);
	if (mp == NULL)
	{
		return NULL;
	}

	TextureMaterialParameter* tex = (TextureMaterialParameter*)mp;
	return tex->GetTexture();
}

TextureMaterialParameter* Material::GetTextureParameter(HString name)
{
	ParameterMap::iterator iter = mMap.find(name);
	if (iter != mMap.end())
	{
		MaterialParameter* mp = iter->second;
		if (mp->getType() == MaterialParameter::MaterialParameterType_Resource)
		{
			TextureMaterialParameter* tm = (TextureMaterialParameter*)mp;
			return tm;
		}
	}

	return NULL;
}

void Material::SetParameter(HString name, MaterialParameter* parameter)
{
	mMap[name] = parameter;

	if (parameter->getType() == MaterialParameter::MaterialParameterType_Resource)
	{
		TextureMaterialParameter* tmp = (TextureMaterialParameter*)parameter;
		TextureResource* tr = tmp->GetTexture();
		PublishTexture(tr, name);
		tmp->CreateRenderDeviceObject();
	}
	else if (parameter->getType() == MaterialParameter::MaterialParameterType_Float)
	{
		FloatMaterialParameter* fp = (FloatMaterialParameter*)parameter;
		PublishFloat(fp, name);
	}
}

atomic_ptr<MaterialParameter> Material::GetParameter(HString name)
{
	ParameterMap::iterator iter = mMap.find(name);
	if (iter != mMap.end())
	{
		return iter->second;
	}

	return atomic_ptr<MaterialParameter>();
}

void Material::RemoveParameter(HString name)
{
	ParameterMap::iterator iter = mMap.find(name);
	if (iter != mMap.end())
	{
		mMap.erase(iter);
	}
}

//Intended to be used by the Editor only
void Material::GetParameters(std::vector<MaterialParameterInfo>& parameters)
{
	parameters.reserve(mMap.size());

	ParameterMap::iterator iter;
	for(iter = mMap.begin(); iter != mMap.end(); iter++)
	{
		MaterialParameterInfo info;
		info.mParam = iter->second;
		info.mParameterName = iter->first;
		parameters.push_back(info);
	}
}

}