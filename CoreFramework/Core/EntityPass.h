/* ===========================================================
	EntityPass

	========================================================== 
*/

#if !defined(__ENTITYPASS_H__)
#define __ENTITYPASS_H__

#include "Godz.h"
#include <CoreFramework/Math/WBox.h>
#include "RenderDeviceBox.h"
#include <vector>


namespace GODZ
{

class GODZ_API EntityPassList
{
public:
	bool operator==(const EntityPassList& other) const
	{
		if (m_id != other.m_id)
		{
			return false;
		}

		if (m_visibleResources.size() != other.m_visibleResources.size())
		{
			return false;
		}

		size_t len = m_visibleResources.size();
		for(size_t i=0; i<len; i++)
		{
			if (m_visibleResources[i] != other.m_visibleResources[i])
			{
				return false;
			}
		}

		return true;
	}

	VisualID m_id;
	std::vector<RenderDeviceBox> m_visibleResources; //list of visible resources this frame
};

struct VisibilityChangeInfo
{
	//set to true if an actor was altered / updated
	bool m_requiresPass;

	VisibilityChangeInfo()
		: m_requiresPass(false)
	{
	}
};

}

#endif