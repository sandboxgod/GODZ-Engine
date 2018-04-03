
#include "WPlane.h"

namespace GODZ
{

bool WPlane::operator==(const WPlane& Other) const
{
	float x = fabsf(Other.D - D);
	if (x>EPSILON)
		return false;
	if (Other.N != N)
		return false;

	return true;
}

//basically we will just convert it into a ray and do the hit detection that way...
bool WPlane::IntersectsLineSegment(const Vector3& start, const Vector3& end, float& t, Vector3& q) const
{
	//Compute the t value for the directed line ab intersecting the plane
	Vector3 dir = end - start;
	return RayIntersection(start, dir, t, q);
}

}

