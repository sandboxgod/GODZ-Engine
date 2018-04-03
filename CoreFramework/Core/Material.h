/* ===========================================================
	Material

	Represents a texture or graphical sfx that can be applied 
	to a model.

	Created Apr 8,'04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#include "GenericObject.h"
#include "TextureResource.h"
#include "Color4f.h"
#include "MaterialInfo.h"
#include "ObjectStateChangeEvent.h"
#include "GodzVector.h"
#include "GenericObject.h"
#include <hash_map>
#include "atomic_ref.h"

#if !defined(_MATERIAL_H_)
#define _MATERIAL_H_

namespace GODZ
{
	class IDriver;
	class Shader;
	class ShaderResource;
	class Material;

	/*
	* Interface for Material parameters
	*/
	class GODZ_API MaterialParameter : public GenericObject
	{
	public:
		DeclareGenericBase(MaterialParameter, GenericObject)

		enum MaterialParameterType
		{
			MaterialParameterType_Resource,
			MaterialParameterType_Float,
		};

		virtual ~MaterialParameter() {}
		virtual MaterialParameterType getType() = 0;
		virtual void CreateRenderDeviceObject() = 0;
		virtual void Publish(Material* mat, HashCodeID hash) = 0;
	};

	class GODZ_API TextureMaterialParameter : public MaterialParameter
	{
	public:
		DeclareGeneric(TextureMaterialParameter, MaterialParameter)

		TextureMaterialParameter()
		{}

		//MaterialParameter interface---->
		virtual MaterialParameterType getType() { return MaterialParameterType_Resource; }

		void SetTexture(TextureResource* tex);
		TextureResource* GetTexture() { return mTex; }

		virtual void CreateRenderDeviceObject();
		virtual void Publish(Material* mat, HashCodeID hash);

	private:
		GenericReference<TextureResource> mTex;
	};

	/*
	*
	*/
	class GODZ_API FloatMaterialParameter : public MaterialParameter
	{
	public:
		DeclareGeneric(FloatMaterialParameter, MaterialParameter)

		FloatMaterialParameter();
		~FloatMaterialParameter();

		virtual MaterialParameterType getType() { return MaterialParameterType_Float; }
		virtual void CreateRenderDeviceObject() {}
		virtual void Publish(Material* mat, HashCodeID hash);

		size_t getNumParameters() { return mValues.size(); }
		void setNumParameters(size_t num);

		float getValue(size_t index);
		void addValue(float value);
		void clearValues() { mValues.clear(); }
		GodzVector<float>& GetValues() { return mValues; }

	private:
		GodzVector<float> mValues;
	};

	/*
	* Wrapper for MaterialParameters; used for queries
	*/
	struct GODZ_API MaterialParameterInfo
	{
		atomic_ptr<MaterialParameter> mParam;
		HString mParameterName;
	};

	/*
	* Material
	*/
	class GODZ_API Material : public GenericObject
	{
		DeclareGeneric(Material, GenericObject)

	public:
		//flexible shader parameter system that maps an HString to the parameter
		typedef stdext::hash_map<HashCodeID, atomic_ptr<MaterialParameter> > ParameterMap;

		Material();
		~Material();

		void				SetBitmap(ResourceObject* resource);
		ResourceObject*		GetBitmap();

		//Returns the technique assigned to the associated shader
		int					GetTechnique();
		void				SetTechnique(int tech);
		ShaderSettings*		GetShaderSettings();

		//Returns the name of the texture (parses it from a file path)
		static rstring		GetTextureName(const char* filename);

		//Applies the effect when this material is rendered
		void				SetShader(ShaderResource* shader);

		//Returns the effect applied when this material is rendered
		ShaderResource*		GetShader();

		//Notification (editor mode) this entity has possibly been updated.
		virtual void		OnPropertiesUpdated();

		//commits changes to render thread. This should be called anytime any member data changes for the material.
		//Otherwise, the updates will not be rendered (render thread caches the changes to materials)
		void				CommitChanges();

		//Returns the unique runtime id for this material
		MaterialID			GetRuntimeID() { return mMatInfo.m_id; }


		void SetParameter(HString name, MaterialParameter* parameter);
		atomic_ptr<MaterialParameter> GetParameter(HString name);
		TextureMaterialParameter* GetTextureParameter(HString name);
		void GetParameters(std::vector<MaterialParameterInfo>& parameters);
		void RemoveParameter(HString name);

		void PublishTexture(TextureResource* resource, HString type);
		void PublishFloat(FloatMaterialParameter* fp, HString name);

		ParameterMap& GetMap() { return mMap; }

		//Notification this object has completed loading
		virtual void OnPostLoad();

	protected:

		MaterialInfo		mMatInfo;

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Resources...

		GenericReference<ShaderResource>	m_pShader;				//fx applied to this material (shared resource)
	
		ParameterMap mMap;
	};
}

#endif