
#include "WSphere.h"
#include "WPlane.h"
#include "vectormath.h"
#include "WBox.h"
#include "Vector3.h"
#include <CoreFramework/ozlib/intr_spheretri.h>

namespace GODZ
{

WSphere::operator WBox() const
{
	Vector3 min;
	min.x = center.x - radius;
	min.y = center.y - radius;
	min.z = center.z - radius;

	Vector3 max;
	max.x = center.x + radius;
	max.y = center.y + radius;
	max.z = center.z + radius;

	return WBox(min, max);
}

void WSphere::GetBottom(Vector3& bottom) const
{
	bottom = Vector3(center.x, center.y - radius, center.z);
}

bool WSphere::IntersectsPlane(const Vector3& va, const Vector3& vb, const WPlane& p)
{
	float adist = va.Dot(p.N) - p.D;
	float bdist = vb.Dot(p.N) - p.D;

	//intersects if on diff side of plane (different signs)
	if (adist * bdist < 0)
		return true;
	//intersects if start or end pos is within radius from plane
	if (abs(adist) <= radius || abs(bdist) <= radius)
		return true;

	return 0;
}

bool WSphere::IntersectsPlane(const Vector3& v, const WPlane& p, float& t, Vector3& q) const
{
	float dist = p.N.Dot(center) - p.D;
	if (fabsf(dist) <= radius)
	{
		//sphere is overlapping the plane
		t=0;
		q=center;
		return true;
	}
	
	float denom = p.N.Dot(v);
	if (denom * dist >= 0)
	{
		//no intersection
		return 0;
	}
	else
	{
		//sphere is moving towards plane
		float r = dist > 0 ? radius : -radius;
		t = (radius - dist) / denom;
		q = center + t * v - radius * p.N;
		return true;
	}
}

bool WSphere::IntersectsSphere(const WSphere& s1, const Vector3& v0, const Vector3& v1, float& t)
{
	Vector3 s = s1.center - center;
	Vector3 v = v1 - v0;
	float r = s1.radius + radius;
	float c = s.Dot(s) - r * r;
	if (c < 0)
	{
		t=0;
		return true;
	}

	float a = v.Dot(v);
	if (a < EPSILON)
		return false;

	float b = v.Dot(s);
	if (b>=0)
		return false;

	float d = b * b - a * c;
	if (d < 0)
		return false;


	t = (-b - sqrtf(d)) / a;
	return true;
}

bool WSphere::IntersectTriangle(const Vector3& va, const Vector3& vb, const Vector3& vc, Vector3& p)
{
	p = ClosestPtPointTriangle(center, va, vb, vc);

	Vector3 v = p - center;
	return v.Dot(v) <= radius * radius;
}

bool WSphere::Intersects(const WRay& ray, float& dist)
{
	//return intersectRaySphere(ray.origin, ray.dir, center, radius, dist);

	Vector3 dst = ray.origin - center;
	float B = dot(dst, ray.dir);
	float C = dot(dst, dst) - radius * radius;

	if (C > 0.0f && B > 0.0f)
		return false;

	float D = B*B - C;
	if (D < 0.0f)
		return false;

	dist = -B - sqrtf(D);
	return true;
}

bool WSphere::IntersectTriangle(const Vector3& va, const Vector3& vb, const Vector3& vc, const Vector3& planeNormal, const Vector3& vel, Vector3& hitLocation, float& dist) const
{
	/*
	bool testIntersectionTriSphere(const Vec3f triPts[3],
								 const Vec3f &triNormal,
								 const Sphere &sphere,
								 const Vec3f &sphereVel,
								 float &distTravel,
								 Vec3f *reaction);
								 */

	ozlib::Vec3f triPts[3];

	triPts[0].x = va.x;
	triPts[0].y = va.y;
	triPts[0].z = va.z;

	triPts[1].x = vb.x;
	triPts[1].y = vb.y;
	triPts[1].z = vb.z;

	triPts[2].x = vc.x;
	triPts[2].y = vc.y;
	triPts[2].z = vc.z;

	ozlib::Vec3f normal(planeNormal);
	ozlib::Sphere sphere(center, radius);
	ozlib::Vec3f sphereVelocity(vel);

	ozlib::Vec3f reaction;

	bool hasCollided = ozlib::testIntersectionTriSphere(triPts, normal, sphere, sphereVelocity, dist, &reaction);

	hitLocation.x = reaction.x;
	hitLocation.y = reaction.y;
	hitLocation.z = reaction.z;

	return hasCollided;
}

}