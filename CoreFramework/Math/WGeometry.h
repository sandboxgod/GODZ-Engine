/* ===================================================================
	Defines math and collision routines

	Sources Cited:

	[1] Elementary Linear Algebra: Applications Version by Howard Anton
	and Chris Rorres

	[2] http://www.cs.unc.edu/~hoff/research/vfculler/viewcull.html 
	by Kenny Hoff

	[3] http://www2.ravensoft.com/users/ggribb/plane%20extraction.pdf

	[4] Compute Face Normals
	http://www.codeguru.com/Cpp/G-M/opengl/article.php/c2681/

	Notes:
	"Orthogonal" means "oriented at 90° to each other". To keep things 
	consistent, the zero vector is regarded as orthogonal to all other 
	vectors since 0 · v = 0.0 for all vectors v .

	Two lines, vectors, planes, etc., are said to be perpendicular if 
	they meet at a right angle. Two vectors a and b are 
	perpendicular if their dot product = 0.

	Created Nov 5, 2003 by Richard Osborne
	Copyright (c) 2003, Richard Osborne
	==================================================================
*/

#pragma once

#include <CoreFramework/Core/CoreFramework.h>
#include <CoreFramework/Core/GDZArchive.h>
#include "Vector3.h"
#include "GodzMath.h"
#include "WRay.h"
#include <math.h>

namespace GODZ
{
	//! 32 bit floating point variable.
	/** This is a typedef for float, it ensures portability of the engine. */
	typedef float				f32; 

	//! 64 bit floating point variable.
	/** This is a typedef for double, it ensures portability of the engine. */
	typedef double				f64; 

	//! 32 bit unsigned variable.
	/** This is a typedef for unsigned int, it ensures portability of the engine. */
	typedef unsigned int		u32;

	//Discovers the min/max bound within a list of vectors.
	template<typename T>
	void SetMinMaxBound(T* V, size_t count, Vector3& min, Vector3& max)
	{
		//Initialize!
		min = V[0];
		max = min;
		for (size_t i=1;i<count;i++)
		{
			if (V[i].x < min.x)
				min.x = V[i].x;
			if (V[i].y < min.y)
				min.y = V[i].y;
			if (V[i].z < min.z)
				min.z = V[i].z;

			if (V[i].x > max.x)
				max.x = V[i].x;
			if (V[i].y > max.y)
				max.y = V[i].y;
			if (V[i].z > max.z)
				max.z = V[i].z;
		}
	}

	//Discovers the min/max bound within a list of vertices.
	template<typename T>
	void SetMinMaxBoundVerts(T* V, size_t count, Vector3& min, Vector3& max)
	{
		//Initialize!
		min=V[0].pos;
		max=min;

		for (size_t i = 1;i < count;i++)
		{
			if (V[i].pos.x < min.x)
				min.x = V[i].pos.x;
			if (V[i].pos.y < min.y)
				min.y = V[i].pos.y;
			if (V[i].pos.z < min.z)
				min.z = V[i].pos.z;

			if (V[i].pos.x > max.x)
				max.x = V[i].pos.x;
			if (V[i].pos.y > max.y)
				max.y = V[i].pos.y;
			if (V[i].pos.z > max.z)
				max.z = V[i].pos.z;
		}
	}

	//forces the value to be between [min...max]
	static float Clamp(float value, float min, float max)
	{
		if (value < min + EPSILON)
		{
			return min;
		}

		if (value > max - EPSILON)
		{
			return max;
		}

		return value;
	}

	static float Wrap360(float value)
	{
		if (value > 6.28f)
		{
			return value - 6.28f;
		}
		if (value < -6.28f)
		{
			return value + 6.28f;
		}

		return value;
	}

	struct GODZ_API Point
	{
		int x, y;

		friend GDZArchive& operator<<(GDZArchive& ar, Point& v)
		{
			ar << v.x << v.y;
			return ar;
		}
	};

	static const float NEARPLANE = 0.1f;
	static const float FARPLANE = 100000;

	// clamp variables of any type
	template<class Type> inline Type Clamp(Type A, Type Min, Type Max)
	{
		if(A<Min) return Min;
		if(A>Max) return Max;
		return A;
	}

	struct GODZ_API Triangle
	{
		//! Returns if a point is inside the triangle. This method is an implementation
		//! of the example used in a paper by Kasper Fauerby original written
		//! by Keidy from Mr-Gamemaker.
		//! \param p: Point to test. Assumes that this point is already on the plane
		//! of the triangle.
		//! \return Returns true if the point is inside the triangle, otherwise false.
		bool isPointInsideFast(const Vector3& p) const
		{
			Vector3 f = pointB - pointA;
			Vector3 g = pointC - pointA;

			f32 a = f.Dot(f);
			f32 b = f.Dot(g);
			f32 c = g.Dot(g);

			f32 ac_bb = (a*c)-(b*b);
			Vector3 vp = p - pointA;

			f32 d = vp.Dot(f);
			f32 e = vp.Dot(g);
			f32 x = (d*c)-(e*b);
			f32 y = (e*a)-(d*b);
			f32 z = x+y-ac_bb;

			return (( ((u32&)z)& ~(((u32&)x)|((u32&)y))) & 0x80000000)!=0;
		}

		void set(const Vector3& a, const Vector3& b, const Vector3& c)
		{
			pointA = a;
			pointB = b;
			pointC = c;
		}

		Vector3 pointA;
		Vector3 pointB;
		Vector3 pointC;
	};

	GODZ_API bool IsNaN(float f);

	//Returns the angle between two vectors (in radians)...
	GODZ_API float ComputeAngle(const Vector3& u, const Vector3& v);
	GODZ_API float ComputeAngle(const Vector3& u, const Vector3& v, Vector3& axis);

	GODZ_API bool IsPositionInViewingArea(const Vector3& afViewPosition3, const Vector3& afViewForward3, float fMaximumViewAngle, const Vector3& afPositionToTest3);

	//Computes the mid point of a triangle
	GODZ_API Vector3 ComputeMidPoint(const Vector3 points[3]);

	//computes face normal of 3 vertices
	GODZ_API Vector3 ComputeFaceNormal(Vector3 verts[3]); 

	//Computes dot product between two vectors (after running several benchmarks I've actually
	//found it's a tad bit faster to do a Vector3.Dot(Vector3) call).
	GODZ_API inline float FDot(const Vector3& p1, const Vector3& p2);

	//area of a 3d triangle
	GODZ_API float Signed3DTriArea(const Vector3& a, const Vector3& b, const Vector3& c);

	//area of a 2d triangle
	GODZ_API float Signed2DTriArea(const Vector3& a, const Vector3& b, const Vector3& c);

	//get closest point between two lines
	GODZ_API float GetSquaredLine3(const WRay& m_rkLine0, const WRay& m_rkLine1, Vector3& m_kClosestPoint0, Vector3& m_kClosestPoint1);

	//Returns the Closest point (p) on a triangle (a,b,c) to a point. 
	GODZ_API Vector3 ClosestPtPointTriangle(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c);
}
