
#include "HUD.h"
#include "Widget.h"
#include <CoreFramework/Core/GodzAtomics.h>
#include <CoreFramework/Core/SceneManager.h>
#include <CoreFramework/Core/SceneData.h>


namespace GODZ
{


HUD::HUD()
{
}

HUD::~HUD()
{
}


Widget& HUD::CreateWidget()
{
	size_t index = SceneData::CreateRuntimeID();

	Widget widget(index);
	m_widgets.push_back(widget);
	return m_widgets[m_widgets.size() - 1];
}


void HUD::RemoveWidget(WidgetId id)
{
	bool found = false;

	std::vector<Widget>::iterator iter;
	for(iter=m_widgets.begin();iter!=m_widgets.end();++iter)
	{
		if (iter->GetId() == id)
		{
			iter->Release();

			found = true;
			m_widgets.erase(iter);
			break;
		}
	}

	godzassert(found == true); //Did not find the widget? Did it belong to another HUD?
}

void HUD::Update(float dt)
{
	std::vector<Widget>::iterator iter;
	for(iter=m_widgets.begin();iter!=m_widgets.end();++iter)
	{
		iter->Update(dt);
	}
}

void HUD::Clear()
{
	std::vector<Widget>::iterator iter;
	for(iter=m_widgets.begin();iter!=m_widgets.end();++iter)
	{
		iter->Release();
	}

	m_widgets.clear();
}

} //namespace