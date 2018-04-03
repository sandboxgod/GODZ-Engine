/* ===========================================================

	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#pragma once

#include "Godz.h"
#include "GenericObjectList.h"
#include "NavigationMesh.h"
#include "GenericObject.h"


namespace GODZ
{

	class GODZ_API NavMeshInfo : public GenericObject
	{
		DeclareGeneric(NavMeshInfo, GenericObject)

	public:
		void Serialize(GDZArchive& ar);

		void SetNavigationMesh(const NavigationMesh& navMesh) { m_navMesh = navMesh; }
		NavigationMesh& GetNavigationMesh() { return m_navMesh; }

	private:
		NavigationMesh m_navMesh;
	};
}

