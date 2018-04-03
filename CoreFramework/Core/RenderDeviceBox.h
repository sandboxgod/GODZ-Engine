
#if !defined(__RENDERDEVICEBOX_H__)
#define __RENDERDEVICEBOX_H__

#include "Godz.h"
#include "MaterialInfo.h"
#include <CoreFramework/Math/WBox.h>

namespace GODZ
{
class RenderDeviceObject;

struct GODZ_API RenderDeviceBox
{
	RenderDeviceObject* mObj;
	WBox box;
	MaterialID matID;		//material runtime ID
	size_t matIndex;		//material array index

	bool operator==(const RenderDeviceBox& other) const
	{
		if (mObj != other.mObj)
		{
			return false;
		}

		//not gonna bother checking the bounds

		return true;
	}

	bool operator!=(const RenderDeviceBox& other) const
	{
		return !(*this == other);
	}
};


}


#endif //__RENDERDEVICEBOX_H__