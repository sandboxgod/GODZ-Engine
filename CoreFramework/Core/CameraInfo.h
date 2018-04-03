/* ===========================================================
	CameraInfo.h

	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__CAMERAINFO__H__)
#define __CAMERAINFO__H__


#include <CoreFramework/Math/ViewMatrixInfo.h>
#include <CoreFramework/Math/WMatrix.h>
#include "ObjectStateChangeEvent.h"

namespace GODZ
{
	typedef size_t CameraID;

	enum ECameraType
	{
		CameraType_LightSource,
		CameraType_Main
	};

	struct GODZ_API CameraInfo
	{
		CameraID		m_id;
		WMatrix			m_viewMatrix;
		ViewMatrixInfo	m_viewMatrixInfo;
		Vector3			m_camLocation;		//camera location provided for shaders

		ECameraType mType;
	};

	/*
	* Notification for render to update info for this cam
	*/
	class GODZ_API CameraCreationEvent : public ObjectStateChangeEvent
	{
	public:
		CameraCreationEvent(CameraInfo& info)
			: m_info(info)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_CameraCreateUpdate)
		{
		}

		virtual bool isMoreRecent(ObjectStateChangeEvent* other)
		{
			CameraCreationEvent* event = (CameraCreationEvent*)other;
			return event->m_info.m_id == m_info.m_id;
		}

		virtual bool shouldReplace()
		{
			//CameraType_Main is the main camera event which must always be last in the list.
			//Thus, we never want to replace a former entry. Instead, just place us last in the list...
			return m_info.mType != CameraType_Main;
		}

		virtual rstring getDebug()
		{
			rstring type = "Camera_Main";

			if (m_info.mType == CameraType_LightSource)
			{
				type = "Light";
			}

			return GetString("CameraCreationEvent actor %d type: %s", m_info.m_id, type.c_str());
		}

		CameraInfo m_info;
	};

	/*
	* Notification for render to update info for this camera's focus actor
	*/
	class GODZ_API CameraFocusEvent : public CameraCreationEvent
	{
	public:
		CameraFocusEvent(CameraInfo& info, VisualID focusID, const WMatrix& sot)
			: CameraCreationEvent(info)
			, mFocusActorID(focusID)
			, m_SOT(sot)
		{
			mType = ObjectStateChangeEventID_CameraFocusUpdate;
		}

		VisualID mFocusActorID;
		WMatrix m_SOT;
	};

	//Main scene camera set
	class GODZ_API CameraActivateEvent : public ObjectStateChangeEvent
	{
	public:
		CameraActivateEvent(CameraID id)
			: m_id(id)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_CameraActivate)
		{
		}

		virtual bool isMoreRecent(ObjectStateChangeEvent* other)
		{
			return true;
		}

		CameraID m_id;
	};

	class GODZ_API CameraDestroyEvent : public ObjectStateChangeEvent
	{
	public:
		CameraDestroyEvent(CameraID id)
			: m_id(id)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_CameraDestroy)
		{
		}

		CameraID m_id;
	};
}

#endif //__CAMERAINFO__H__
