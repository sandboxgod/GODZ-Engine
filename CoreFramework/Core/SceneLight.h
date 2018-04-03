/* ===========================================================
	RenderData.h

	Defines structures used for "Game" thread to communicate
	with "Render"

	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__SCENELIGHT__H__)
#define __SCENELIGHT__H__

#include <CoreFramework/Math/ViewMatrixInfo.h>
#include <CoreFramework/Math/Vector3.h>
#include <CoreFramework/Math/ProjectionMatrixInfo.h>
#include "ObjectStateChangeEvent.h"
#include "Color4f.h"
#include "SlotIndex.h"

namespace GODZ
{
	typedef size_t LightID;

	enum GODZ_API LightType
	{
		LT_None,
		LT_SunLight,				//The main lighting type that highlights our scene
		LT_SpotLight,				//This type of light brigthens a room, etc
		LT_ShadowSpotLight,
		LT_PointLight,
		LT_MAX
	};

	struct GODZ_API SceneLight
	{
		LightID				m_id;				//id of the camera
		VisualID			m_visualID;
		Vector3				m_viewerPosition;	//view target position
		ViewMatrixInfo		m_viewMatrixInfo;
		LightType			m_lightType;
		Vector3				m_lightDir;
		Vector3				m_forward;
		Color4f				m_lightColor;
		ProjectionMatrixInfo mProj;
		float				m_lightRange;

		bool isSpotLight()
		{
			switch (m_lightType)
			{
			default:
				return false;
				break;
			case LT_ShadowSpotLight:
			case LT_SpotLight:
				{
					return true;
				}
				break;
			}
		}
	};

	/*
	* Notification for render to update info for this light
	*/
	class GODZ_API LightCreationEvent : public ObjectStateChangeEvent
	{
	public:
		LightCreationEvent(SceneLight& light)
			: m_light(light)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_LightCreateUpdate)
		{
		}

		virtual bool isMoreRecent(ObjectStateChangeEvent* other)
		{
			LightCreationEvent* event = (LightCreationEvent*)other;
			return event->m_light.m_id == m_light.m_id;
		}

		virtual rstring getDebug()
		{
			rstring type = "Light_Undefined";

			switch(m_light.m_lightType)
			{
			case LT_SunLight:
				{
					type = "Sunlight";
				}
				break;

			case LT_SpotLight:
				{
					type = "SpotLight";
				}
				break;

			case LT_ShadowSpotLight:
				{
					type = "ShadowSpotLight";
				}
				break;
			case LT_PointLight:
				{
					type = "PointLight";
				}
				break;
			}

			return GetString("LightCreationEvent actor %d type: %s", m_light.m_id, type.c_str());
		}

		SceneLight m_light;
	};

	//Activates this light as the source of sunlight (outdoor scene light)
	class GODZ_API SunLightActivateEvent : public ObjectStateChangeEvent
	{
	public:
		SunLightActivateEvent(LightID id)
			: m_light(id)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_LightActivate)
		{
		}

		virtual bool isMoreRecent(ObjectStateChangeEvent* other)
		{
			return true;
		}

		LightID m_light;
	};

	class GODZ_API LightDestroyEvent : public ObjectStateChangeEvent
	{
	public:
		LightDestroyEvent(size_t id)
			: m_id(id)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_LightDestroy)
		{
		}

		size_t m_id;
	};
}

#endif //__SCENELIGHT__H__
