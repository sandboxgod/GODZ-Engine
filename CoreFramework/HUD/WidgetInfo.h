/* ===========================================================
	WidgetInfo

	Copyright (c) 2008, Richard Osborne
	========================================================== 
*/

#pragma once

#include <CoreFramework/Core/MaterialInfo.h>
#include <CoreFramework/Core/ObjectStateChangeEvent.h>
#include <CoreFramework/Math/Vector2.h>
#include <CoreFramework/Math/Vector3.h>

namespace GODZ
{
	typedef size_t WidgetId;

	//render thread representation of a sprite
	class GODZ_API WidgetInfo : public MaterialInfoObserver
	{
	public:
		WidgetInfo();
		~WidgetInfo();
		void OnDestroyed(MaterialInfo* mat);

		WidgetId			m_id;				//allocation id # - widget identifier
		Vector3				m_pos;
		Vector2				m_scale;
		Color4f				m_color;
		MaterialID			m_matInfo;			//material runtime ID

		// Render Thread only stuff --------------------------------------------------------- //
		MaterialInfo* m_cachedMaterial;

		friend GDZArchive& operator<<(GDZArchive& ar, WidgetInfo& data)
		{
			ar << data.m_scale << data.m_color  << data.m_pos << data.m_id; // << m_matInfo;
		}
	};

	/*
	* Sends over all of the widget properties.
	*/
	class WidgetCreationEvent : public ObjectStateChangeEvent
	{
	public:
		WidgetCreationEvent();

		WidgetId			m_id;				//allocation id # - widget identifier	
		Vector3				m_pos;
		Vector2				m_scale;
		Color4f				m_color;
	};

	class WidgetUpdateEvent : public ObjectStateChangeEvent
	{
	public:
		WidgetUpdateEvent()
			: ObjectStateChangeEvent(ObjectStateChangeEventID_WidgetUpdate)
		{
		}

		WidgetId			m_id;				//allocation id # - widget identifier	
		Vector3				m_pos;
		Vector2				m_scale;
		Color4f				m_color;
	};

	class WidgetDestructionEvent : public ObjectStateChangeEvent
	{
	public:
		WidgetDestructionEvent(WidgetId id);
		WidgetId			m_id;			//id of the widget being destroyed
	};

	class WidgetMaterialChangeEvent : public ObjectStateChangeEvent
	{
	public:
		WidgetMaterialChangeEvent(WidgetId id)
			: m_id(id)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_WidgetMaterialChange)
		{
		}

		WidgetId			m_id;
		MaterialID			m_matid;
	};

}
