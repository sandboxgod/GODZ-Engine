
#include "GUIController.h"
//#include "GenericObject.h"
#include "GUIMenu.h"
#include <CoreFramework/Reflection/GenericClass.h>

using namespace GODZ;

GUIController::GUIController()
{
	m_pCurrMenu=0;
}

GUIMenu* GUIController::AddMenu(const char* menuName)
{
	GenericClass* pClass = StaticLoadObject(menuName);
	if (!pClass)
	{
		Log("Error: Could not find the menu class %s \n", menuName);
		return NULL;
	}

	GenericObject* pObj = pClass->GetInstance();
	GUIMenu* pMenu = SafeCast<GUIMenu>(pObj);
	m_menus.push_back(pMenu);

	return pMenu;
}

GUIMenu* GUIController::FindMenu(HString menuName)
{
	std::vector<GUIMenu*>::iterator menuIter;
	for(menuIter = m_menus.begin();menuIter!=m_menus.end();menuIter++)
	{
		GUIMenu* pMenu = (*menuIter);

		/*
		if (pMenu->GetName() == menuName)
		{
			return pMenu;
		}
		*/
	}

	return NULL;
}

