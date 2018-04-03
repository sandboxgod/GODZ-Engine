
#include "GUIMenu.h"
#include "GUIControl.h"


using namespace GODZ;

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

