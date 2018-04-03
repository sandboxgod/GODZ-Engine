
#include "WidgetInfo.h"


namespace GODZ
{

WidgetInfo::WidgetInfo()
: m_cachedMaterial(NULL)
{
}

WidgetInfo::~WidgetInfo()
{
	if (m_cachedMaterial != NULL)
	{
		m_cachedMaterial->DropObserver(this);
		m_cachedMaterial = NULL;
	}
}

void WidgetInfo::OnDestroyed(MaterialInfo* mat)
{
	if (m_cachedMaterial == mat)
	{
		m_cachedMaterial = NULL;
	}
}

WidgetCreationEvent::WidgetCreationEvent()
	: ObjectStateChangeEvent(ObjectStateChangeEventID_WidgetCreation)
{
}

WidgetDestructionEvent::WidgetDestructionEvent(WidgetId id)
	: ObjectStateChangeEvent(ObjectStateChangeEventID_WidgetDestruction)
	, m_id(id)
{
}




}
