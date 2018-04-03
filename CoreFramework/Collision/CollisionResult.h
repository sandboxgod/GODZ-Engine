/* ===========================================================
	CollsionResult

	Copyright (c) 2010
	========================================================== 
*/

#pragma once

#include <CoreFramework/Core/Godz.h>
#include <CoreFramework/Math/Vector3.h>
#include <CoreFramework/Math/WPlane.h>
#include "PhysicsObjectProxy.h"


namespace GODZ
{
	// collision result
	struct CollisionResult
	{
		float m_distance;			//distance to collision point
		Vector3 m_hitLocation;		//point of collision
		PhysicsObjectProxy model;	//collision model that was hit
		Vector3 m_normal;			//normal of the plane we collided with
		bool m_bStuck;				//embedded in the poly
		WPlane m_plane;				//Plane that was collided with (if applicable)

		CollisionResult()
			: model(NULL)
			, m_bStuck(false)
			, m_distance(0)
			, m_normal(Vector3::Zero())
			, m_hitLocation(Vector3::Zero())
			, m_plane(WPlane::Zero())
		{
		}


		void Reset()
		{
			m_hitLocation=ZERO_VECTOR;
			m_bStuck=false;
			m_distance=0;
			m_normal = Vector3(0,0,0);
			m_plane.N.MakeZero();
			m_plane.D = 0;
		}
	};
}
