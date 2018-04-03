/* ===========================================================
RenderDeviceEvent

Copyright (c) 2008, Richard Osborne
=============================================================
*/

#include "RenderDeviceObject.h"

namespace GODZ
{
	RenderDeviceObject::RenderDeviceObjMap RenderDeviceObject::mRenderObjectMap;

	RenderDeviceObject::RenderDeviceObject()
	{
		mRenderObjectMap[this] = this;
	}

	RenderDeviceObject::~RenderDeviceObject()
	{
		RenderDeviceObjMap::iterator iter = mRenderObjectMap.find(this);
		if(iter != mRenderObjectMap.end())
		{
			mRenderObjectMap.erase(iter);
		}
	}
}