
#pragma once

#include <CoreFramework/Core/CoreFramework.h>
#include <CoreFramework/Core/GDZArchive.h>
#include "Vector3.h"
#include "WRay.h"

namespace GODZ
{
	struct WBox;

	//sphere primitive
	struct GODZ_API WSphere
	{
		Vector3 center;
		float radius;

		WSphere()
		{
		}

		//returns the bottom of this volume
		void GetBottom(Vector3& bottom) const;

		//tests moving sphere going from a to b against a Plane (very fast because t isn't computed)
		bool IntersectsPlane(const Vector3& pos_a, const Vector3& pos_b, const WPlane& p);
		bool IntersectsPlane(const Vector3& v, const WPlane& p, float& t, Vector3& pointOfCollision) const;
		bool IntersectsSphere(const WSphere& a, const Vector3& va, const Vector3& vb, float& t);
		bool IntersectTriangle(const Vector3& vert1, const Vector3& vert2, const Vector3& vert3, Vector3& p);

		//intersects moving sphere against a triangle
		bool IntersectTriangle(const Vector3& vert1, const Vector3& vert2, const Vector3& vert3, const Vector3& planeNormal, const Vector3& vel, Vector3& hitLocation, float& dist) const;
   
		// Notes : Normalized directional vectors expected
		// Return: distance to sphere in world units if true
		bool Intersects(const WRay& ray, float& dist);

		friend GDZArchive& operator<<(GDZArchive& ar, WSphere& s)
		{
			ar << s.center << s.radius;
			return ar;
		}

		operator WBox() const;
	};

}
