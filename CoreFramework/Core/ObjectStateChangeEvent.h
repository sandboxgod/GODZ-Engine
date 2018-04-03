/* ===========================================================
	ObjectStateChangeEvent

	Copyright (c) 2009, Richard Osborne
	========================================================== 
*/

#if !defined(__ObjectStateChangeEvent__H__)
#define __ObjectStateChangeEvent__H__

#include "Godz.h"
#include <vector>

namespace GODZ
{
	enum ObjectStateChangeEventID
	{
		ObjectStateChangeEventID_None,

		ObjectStateChangeEventID_WidgetCreation,
		ObjectStateChangeEventID_WidgetUpdate,
		ObjectStateChangeEventID_WidgetMaterialChange,
		ObjectStateChangeEventID_WidgetDestruction,

		ObjectStateChangeEventID_MaterialCreate,			//create / update cached material data
		ObjectStateChangeEventID_MaterialUpdate,
		ObjectStateChangeEventID_MaterialDestroy,
		ObjectStateChangeEventID_MaterialTextureUpdate,
		ObjectStateChangeEventID_MaterialFloatUpdate,
		ObjectStateChangeEventID_MaterialQuadCreate,
		ObjectStateChangeEventID_MaterialQuadsClear,

		ObjectStateChangeEventID_ViewportCreation,
		ObjectStateChangeEventID_ViewportActivate,			//viewport activated
		ObjectStateChangeEventID_ViewportDestroy,
		ObjectStateChangeEventID_ViewportSetBackgroundColor,

		ObjectStateChangeEventID_LightCreateUpdate,
		ObjectStateChangeEventID_LightDestroy,
		ObjectStateChangeEventID_LightActivate,

		ObjectStateChangeEventID_CameraCreateUpdate,
		ObjectStateChangeEventID_CameraDestroy,
		ObjectStateChangeEventID_CameraActivate,
		ObjectStateChangeEventID_CameraFocusUpdate,

		ObjectStateChangeEventID_EntityCreate,
		ObjectStateChangeEventID_EntityCastShadows,
		ObjectStateChangeEventID_EntityDestroy,
		ObjectStateChangeEventID_EntityUpdateBones,
		ObjectStateChangeEventID_EntityUpdateTransform,			//update transform
		ObjectStateChangeEventID_EntityMaterialsChanged,
		ObjectStateChangeEventID_EntityVisibilityPass,
		ObjectStateChangeEventID_EntityRenderDeviceEvent,
		ObjectStateChangeEventID_EntitySelected,				//editor -- select
		ObjectStateChangeEventID_EntityDeselected,
		ObjectStateChangeEventID_EntityAxisChanged,				//editor -- Movement 3d axis gizmo event
		ObjectStateChangeEventID_EntityScreenPick,
		ObjectStateChangeEventID_EntityDrawScale,
		ObjectStateChangeEventID_EntityOrientationUpdate,
		ObjectStateChangeEventID_EntityUpdate,
		ObjectStateChangeEventID_EntityStatusChangeEvent,

		ObjectStateChangeEventID_RenderSwitch,
		ObjectStateChangeEventID_CreateSkyDome,
		ObjectStateChangeEventID_RemoveSkyDome,

		ObjectStateChangeEventID_Collection,					//stores collection of change events

		ObjectStateChangeEventID_MAX,							//last member, stores count
	};

	/*
	* Base class for events that broadcast changes to object state
	*/
	class GODZ_API ObjectStateChangeEvent
	{
	public:
		//Declared virtual so that derived classes will be properly destroyed
		virtual ~ObjectStateChangeEvent() {}

		ObjectStateChangeEventID getType() { return mType; }
		virtual rstring getDebug();

		//query to determine if the more recent ObjectStateChangeEvent can replace the existing one...
		virtual bool isMoreRecent(ObjectStateChangeEvent* other) { return false; }

		//query to determine if this new entry should replace obsolete entries. Return false if a new slot
		//should always be assigned.
		virtual bool shouldReplace() { return true; }

		ObjectStateChangeEventID mType;

	protected:
		ObjectStateChangeEvent(ObjectStateChangeEventID id);
	};

	class GODZ_API ObjectStateChangeEventList
	{
	public:
		ObjectStateChangeEventList();

		//Adds the event to this container. It will query any other preexisting events of its type to determine if it's more recent and
		//should be used instead...
		void Add(ObjectStateChangeEvent* ev);
		size_t GetNumObjects();
		ObjectStateChangeEvent* operator[](size_t index);
		void Clear();

	private:
		std::vector<ObjectStateChangeEvent*> mList;
		std::vector<size_t> mQuickIndex[ObjectStateChangeEventID_MAX];
	};

	/*
	* Stores a list of object state change events. Sometimes, it's more convient to pass a collection
	* of related events all at once
	*/
	class GODZ_API ObjectStateEventCollection : public ObjectStateChangeEvent
	{
	public:
		ObjectStateEventCollection()
			: ObjectStateChangeEvent(ObjectStateChangeEventID_Collection)
		{
		}

		virtual bool isMoreRecent(ObjectStateChangeEvent* other);

		ObjectStateChangeEventList mList;
	};
}

#endif //__ObjectStateChangeEvent__H__
