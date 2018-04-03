/* ===========================================================
	CollisionUtil.h

	Copyright (c) 2011
	========================================================== 
*/

#pragma once

#include "CollisionPrimitive.h"

namespace GODZ
{
	class GODZ_API CollisionUtil
	{
	public:
		static CollisionPrimitive* GetInstance(CollisionPrimitive::CollisionType type);
	};
}

