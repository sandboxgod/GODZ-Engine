/* ===========================================================
	MaterialInfo

	========================================================== 
*/

#if !defined(__MATERIALINFO_H__)
#define __MATERIALINFO_H__

#include "Godz.h"
#include "TextureResource.h"
#include "ShaderResource.h"
#include "ObjectStateChangeEvent.h"
#include "Color4f.h"
#include "SlotIndex.h"
#include <hash_map>

namespace GODZ
{
	// Handle to a Material
	typedef SlotIndex MaterialID;

	class ShaderSettings;
	struct MaterialInfo;

	//commonly used texture parameter names
	class TextureTypes
	{
	public:
		static const unsigned int Height = 4074889273;
		static const unsigned int Ambient = 2360138580;
		static const unsigned int Opacity = 2353026298;
		static const unsigned int Normal = 640249298;
		static const unsigned int Diffuse = 1629286381;
	};

	//Color settings (Material) coming in from 3dsmax during import
	class ParameterTypes
	{
	public:
		static const unsigned int DiffuseColor	= 2417308959;
		static const unsigned int AmbientColor	= 3691332413;
		static const unsigned int SpecularColor	= 3015922117;
		static const unsigned int EmissiveColor	= 2675060850;
		static const unsigned int OpacityColor	= 114355555;
		static const unsigned int SpecularPowerColor = 3773494587;
	};


	class GODZ_API ParameterInfo
	{
	public:
		enum ParameterInfoType
		{
			ParameterInfoType_Texture,
			ParameterInfoType_Float
		};

		virtual ~ParameterInfo() {}
		virtual ParameterInfoType getType() = 0;
	};

	class GODZ_API TextureParameterInfo : public ParameterInfo
	{
	public:
		virtual ParameterInfoType getType() { return ParameterInfoType_Texture; }

		Future<GenericReference<RenderDeviceObject> > m_tex;
	};

	class GODZ_API FloatParameterInfo : public ParameterInfo
	{
	public:
		virtual ParameterInfoType getType() { return ParameterInfoType_Float; }

		size_t numFloats() { return mValues.size(); }
		float getFloat(size_t index) { return mValues[index]; }
		void add(float value) { mValues.push_back(value); }

	private:
		std::vector<float> mValues;
	};

	/*
	* MaterialInfo Observer
	*/
	class GODZ_API MaterialInfoObserver
	{
	public:
		//notification the material is going out of scope
		virtual void OnDestroyed(MaterialInfo* mat) = 0;
	};

	struct GODZ_API MaterialInfo
	{
		~MaterialInfo();

		//assigns texture to the texture slot indicated by the textureinfo->type. will overwrite anything else at that slot
		void				AddTexture(HString name, Future<GenericReference<RenderDeviceObject> > textureFuture);
		Future<GenericReference<RenderDeviceObject> >	GetTexture(HString textureName);

		void				AddFloatParameter(HString name, FloatParameterInfo& fp);
		ParameterInfo*		GetParameter(HString parameterName);

		void				AddObserver(MaterialInfoObserver* observer);
		void				DropObserver(MaterialInfoObserver* observer);
		void				CopyFrom(const MaterialInfo& src)
		{
			m_pShader = src.m_pShader;
			mMap = src.mMap;
		}

		void Clear();
		MaterialID			m_id;
		
		Future<GenericReference<RenderDeviceObject> >		m_pShader;				//fx applied to this material (shared resource)
		
	private:
		typedef stdext::hash_map<HashCodeID, ParameterInfo*> ParameterMap;
		ParameterMap mMap;
		std::vector<MaterialInfoObserver*> m_observers;		//observers
	};

	struct GODZ_API MaterialQuad
	{
		MaterialID m_id;
		float x;
		float y;
		float z;
		int width;
		int height;
	};


	/*
	* Material Update/Creation event
	*/
	class GODZ_API MaterialCreationEvent : public ObjectStateChangeEvent
	{
	public:
		MaterialCreationEvent(MaterialInfo& minfo);

		MaterialInfo mInfo;
	};

	class GODZ_API MaterialUpdateEvent : public ObjectStateChangeEvent
	{
	public:
		MaterialUpdateEvent()
			: ObjectStateChangeEvent(ObjectStateChangeEventID_MaterialUpdate)
		{
		}


		MaterialID			m_id;
		Future<GenericReference<RenderDeviceObject> >		m_pShader;				//fx applied to this material (shared resource)
	};

	class GODZ_API MaterialDestructionEvent : public ObjectStateChangeEvent
	{
	public:
		MaterialDestructionEvent(const MaterialID& key);

		MaterialID m_id;			//ID for this material
	};

	class GODZ_API MaterialTexUpdateEvent : public ObjectStateChangeEvent
	{
	public:
		MaterialTexUpdateEvent(MaterialID id)
			: ObjectStateChangeEvent(ObjectStateChangeEventID_MaterialTextureUpdate)
			, m_id(id)
		{
		}

		MaterialID m_id;
		TextureParameterInfo mTex;
		HString mName;
	};

	class GODZ_API MaterialFloatUpdateEvent : public ObjectStateChangeEvent
	{
	public:
		MaterialFloatUpdateEvent(MaterialID id, FloatParameterInfo& fp)
			: ObjectStateChangeEvent(ObjectStateChangeEventID_MaterialFloatUpdate)
			, m_id(id)
			, mFloatParameter(fp)
		{
		}

		MaterialID m_id;
		FloatParameterInfo mFloatParameter;
		HString mName;
	};

	/*
	* Creates a material quad
	*/
	class GODZ_API MaterialQuadCreationEvent : public ObjectStateChangeEvent
	{
	public:
		MaterialQuadCreationEvent(MaterialQuad& quad)
			: ObjectStateChangeEvent(ObjectStateChangeEventID_MaterialQuadCreate)
			, m_quad(quad)
		{
		}

		MaterialQuad m_quad;
	};

	class GODZ_API MaterialQuadClearEvent : public ObjectStateChangeEvent
	{
	public:
		MaterialQuadClearEvent()
			: ObjectStateChangeEvent(ObjectStateChangeEventID_MaterialQuadsClear)
		{
		}
	};
}

#endif