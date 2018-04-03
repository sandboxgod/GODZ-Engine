
#include "GUIMenu.h"
#include "GUIControl.h"
#include <CoreFramework/Reflection/GenericClass.h>


using namespace GODZ;

ImplementGeneric(GUIMenu)

void GUIMenu::AddControl(GUIControl* pControl)
{
	m_controls.push_back(pControl);
}

void GUIMenu::OnClose()
{
}

void GUIMenu::OnDisplay()
{
}

