/* ===========================================================
	Physics

	Jan 21, 2010
	========================================================== 
*/

#if !defined(__PHYSICS_H__)
#define __PHYSICS_H__

#include <CoreFramework/Math/Vector3.h>
#include <CoreFramework/Math/WMatrix3.h>

namespace GODZ
{
	/*
	* Current collision state
	*/
	enum CollisionStatus
	{
		CollisionStatus_NotSet,
		CollisionStatus_BaseFound
	};

	/*
	* Be careful changing the order, these types are saved out to .gsa files
	*/
	enum GODZ_API MotionType
	{
		//does not have velocity, however it can be collided with by other objects
		MotionType_Static,
		
		//will perform collide-and-slide when it collides with an object; collision results not cached
		//May be collided with
		MotionType_Character,

		//will exit the physics simulation automatically once it hits an object; hits are cached
		MotionType_Projectile,

		//phantoms can hit other objects, etc. These will continue along their
		//trajectory even after hitting something. Hits are cached
		MotionType_Phantom,

		//will perform collide-and-slide when it collides with an object; collision results not cached
		//Also it cannot be collided with (works like a phantom)
		MotionType_MovementQuery,
	};

	struct TransformState
	{
		Matrix3 m_rot;
		Vector3 m_pos;
	};
}

#endif __PHYSICS_H__