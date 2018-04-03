#pragma once

#include "ObjectStateChangeEvent.h"
#include "GizmoAxis.h"
#include "EntityPass.h"
#include "GeometryInfo.h"
#include <CoreFramework/Math/ViewFrustum.h>
#include <CoreFramework/Math/WMatrix.h>

namespace GODZ
{
	class GODZ_API EntityCreationEvent : public ObjectStateChangeEvent
	{
	public:
		EntityCreationEvent(const GeometryInfo& info)
			: m_info(info)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_EntityCreate)
		{
		}

		virtual rstring getDebug() { return GetString("EntityCreationEvent actor %lu", m_info.m_id.GetUInt64()); }

		GeometryInfo m_info;
	};

	class GODZ_API EntityUpdateEvent : public ObjectStateChangeEvent
	{
	public:
		EntityUpdateEvent(const GeometryInfo& info)
			: m_info(info)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_EntityUpdate)
		{
		}

		virtual rstring getDebug() { return GetString("EntityUpdateEvent actor %lu", m_info.m_id.GetUInt64()); }

		GeometryInfo m_info;
	};

	class GODZ_API EntityShadowCasterEvent : public ObjectStateChangeEvent
	{
	public:
		EntityShadowCasterEvent(VisualID id, bool castShadows)
			: m_id(id)
			, m_castShadows(castShadows)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_EntityCastShadows)
		{
		}

		virtual bool isMoreRecent(ObjectStateChangeEvent* other)
		{
			EntityShadowCasterEvent* event = (EntityShadowCasterEvent*)other;
			return event->m_id == m_id;
		}

		virtual rstring getDebug() { return GetString("EntityShadowCasterEvent actor %lu", m_id.GetUInt64()); }

		VisualID m_id;
		bool m_castShadows;
	};

	class GODZ_API EntityDestroyEvent : public ObjectStateChangeEvent
	{
	public:
		EntityDestroyEvent(VisualID id)
			: m_id(id)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_EntityDestroy)
		{
		}

		VisualID m_id;
	};

	class GODZ_API EntityBonesUpdateEvent : public ObjectStateChangeEvent
	{
	public:
		EntityBonesUpdateEvent(VisualID id)
			: m_id(id)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_EntityUpdateBones)
		{
		}

		virtual bool isMoreRecent(ObjectStateChangeEvent* other)
		{
			EntityBonesUpdateEvent* event = (EntityBonesUpdateEvent*)other;
			return event->m_id == m_id;
		}

		virtual rstring getDebug()
		{
			return GetString("EntityBonesUpdateEvent actor %lu", m_id.GetUInt64());
		}

		VisualID m_id;
		std::vector<WMatrix16f> m_bones;
	};

	class GODZ_API EntityTransformUpdateEvent : public ObjectStateChangeEvent
	{
	public:
		EntityTransformUpdateEvent(VisualID id, Matrix4& m)
			: m_id(id)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_EntityUpdateTransform)
			, m_mat(m)
		{
		}

		virtual bool isMoreRecent(ObjectStateChangeEvent* other)
		{
			EntityTransformUpdateEvent* t = (EntityTransformUpdateEvent*)other;
			return t->m_id == m_id;
		}

		virtual rstring getDebug()
		{
			return GetString("EntityTransformUpdateEvent actor %lu", m_id.GetUInt64());
		}

		VisualID m_id;
		Matrix4 m_mat;
	};

	class GODZ_API EntityMaterialsUpdateEvent : public ObjectStateChangeEvent
	{
	public:
		EntityMaterialsUpdateEvent(VisualID id)
			: m_id(id)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_EntityMaterialsChanged)
		{
		}

		virtual bool isMoreRecent(ObjectStateChangeEvent* other)
		{
			EntityMaterialsUpdateEvent* t = (EntityMaterialsUpdateEvent*)other;
			return t->m_id == m_id;
		}

		VisualID m_id;
		std::vector<MaterialID> m_materials;

		typedef stdext::hash_map<UInt64, RenderDeviceContainer > RenderDeviceMap;
		RenderDeviceMap mMap;
	};

	class GODZ_API EntityVisibilityPassEvent : public ObjectStateChangeEvent
	{
	public:
		EntityVisibilityPassEvent()
			: ObjectStateChangeEvent(ObjectStateChangeEventID_EntityVisibilityPass)
		{
		}

		//we only want to send one EntityVisibilityPassEvent so always defer to the more recent event
		virtual bool isMoreRecent(ObjectStateChangeEvent* other) { return true; }

		std::vector<EntityPassList> m_actors;
		WMatrix mSunlightView; //debug
		ViewFrustum mLightFrustum;
	};

	class GODZ_API EntityRenderDeviceUpdateEvent : public ObjectStateChangeEvent
	{
	public:
		EntityRenderDeviceUpdateEvent(VisualID id)
			: m_id(id)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_EntityRenderDeviceEvent)
		{
		}

		virtual rstring getDebug() { return GetString("EntityRenderDeviceUpdateEvent actor %lu", m_id.GetUInt64()); }

		VisualID m_id;
		typedef stdext::hash_map<UInt64, RenderDeviceContainer > RenderDeviceMap;
		RenderDeviceMap mMap;
	};

	class GODZ_API EntitySelectedEvent : public ObjectStateChangeEvent
	{
	public:
		EntitySelectedEvent(VisualID newActorid, VisualID oldActorID)
			: m_id(newActorid)
			, m_PreviousID(oldActorID)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_EntitySelected)
		{
		}

		VisualID m_id;
		VisualID m_PreviousID;
	};

	class GODZ_API EntityDeselectedEvent : public ObjectStateChangeEvent
	{
	public:
		EntityDeselectedEvent(VisualID newActorid)
			: m_id(newActorid)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_EntityDeselected)
		{
		}

		VisualID m_id;
	};

	class GODZ_API EntityAxisChangedEvent : public ObjectStateChangeEvent
	{
	public:
		EntityAxisChangedEvent(VisualID id, GizmoAxis axis)
			: m_id(id)
			, m_Axis(axis)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_EntityAxisChanged)
		{
		}

		VisualID m_id;
		GizmoAxis m_Axis;;
	};

	/*
	* Computes a ray in world space at the screen-space point
	*/
	class GODZ_API EntityScreenRayEvent : public ObjectStateChangeEvent
	{
	public:
		EntityScreenRayEvent(int x, int y, Future<WRay> future)
			: mX(x)
			, mY(y)
			, mFuture(future)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_EntityScreenPick)
		{
		}

		int mX;
		int mY;
		Future<WRay> mFuture;
	};

	//This is just a generic message for an entity to the renderer to update the 
	//renderable data for this entity.
	class GODZ_API EntityStatusChangedEvent : public ObjectStateChangeEvent
	{
	public:
		//List of Events
		enum EEntityChangeEvent
		{
			kEntityChangeEvent_ClearEditorMaterial
		};

		EntityStatusChangedEvent(VisualID id, EEntityChangeEvent ev)
			: m_id(id)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_EntityStatusChangeEvent)
			, mEvent(ev)
		{
		}

		virtual bool isMoreRecent(ObjectStateChangeEvent* other)
		{
			EntityStatusChangedEvent* t = (EntityStatusChangedEvent*)other;

			// If the 'other' Event is for the same actor + contains updated data for this
			// event then it should get used instead
			return t->m_id == m_id && mEvent == t->mEvent;
		}

		virtual rstring getDebug()
		{
			return GetString("EntityStatusChangedEvent actor %lu", m_id.GetUInt64() );
		}

		VisualID m_id;
		EEntityChangeEvent mEvent;
	};
}
