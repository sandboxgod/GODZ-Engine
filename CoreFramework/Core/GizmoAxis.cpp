
#include "GizmoAxis.h"
#include <CoreFramework/Collision/RayCollider.h>

namespace GODZ
{

Gizmo::Gizmo()
: scale(50)
, thickness(10)
, up(0,1,0)
, right(1,0,0)
, depth(0,0,-1)
, start(2)
{
	//create bounds in model space
	mUp.SetMinMax( Vector3(-thickness,start,-thickness),  Vector3(thickness,thickness*scale,thickness) );

	mRight.SetMinMax( Vector3(start,-thickness,-thickness), Vector3(thickness*scale,thickness,thickness) );

	mDepth.SetMinMax( Vector3(thickness,thickness,start), Vector3(-thickness,-thickness,-thickness*scale));
}

GizmoAxis Gizmo::GetAxis(const WRay& ray, const WMatrix16f& tm)
{
	WBox boxUp = mUp;
	WBox boxRight = mRight;
	WBox boxDepth = mDepth;

	boxUp.Transform(tm);
	boxRight.Transform(tm);
	boxDepth.Transform(tm);

	if (RayAABB(ray, boxUp))
	{
		return GizmoAxis_Y;
	}

	if (RayAABB(ray, boxRight))
	{
		return GizmoAxis_X;
	}

	if (RayAABB(ray, boxDepth))
	{
		return GizmoAxis_Z;
	}

	return GizmoAxis_None;
}

}
