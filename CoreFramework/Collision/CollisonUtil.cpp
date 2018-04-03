/* ===========================================================
	CollisionUtil.h

	Copyright (c) 2011
	========================================================== 
*/


#include "CollisionUtil.h"
#include <CoreFramework/Collision/AABBCollisionPrimitive.h>
#include <CoreFramework/Collision/OBBCollisionPrimitive.h>
#include <CoreFramework/Collision/SphereCollisionPrimitive.h>

namespace GODZ
{

CollisionPrimitive* CollisionUtil::GetInstance(CollisionPrimitive::CollisionType type)
{
	switch(type)
	{
	default:
		{
			godzassert(0); //unknown type
		}
		break;
	case CollisionPrimitive::CollisionType_AABB:
		{
			return new AABBCollisionPrimitive();
		}
		break;
	case CollisionPrimitive::CollisionType_OBB:
		{
			return new OBBCollisionPrimitive();
		}
		break;
	case CollisionPrimitive::CollisionType_Sphere:
		{
			return new SphereCollisionPrimitive();
		}
		break;
	}

	return NULL;
}

}

