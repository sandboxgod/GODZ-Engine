/* ===================================================================
	WPlane.h

	==================================================================
*/

#if !defined(__WPLANE_H_)
#define __WPLANE_H_

#include <CoreFramework/Core/Godz.h>
#include "GodzMath.h"
#include "Vector3.h"

namespace GODZ
{

	enum Halfspace
	{
		Halfspace_BEHIND, //behind the plane
		Halfspace_ON_PLANE, 
		Halfspace_FRONT, //in front of plane
	};

	//Represents a plane defined in world space Ax + By + Cz + D = 0, Normal=(A,B,C), Point=(x,y,z)
	//Note: any functions added to this class, we will have to make sure to subtract D.
	struct GODZ_API WPlane
	{
		Vector3 N;	//Normal of the plane
		float D;	//Distance from a point on the plane

		WPlane()
		{
		}
		
		//[in] P is a point on the plane
		//[in] N is the normal of the plane
		WPlane(const Vector3& P, const Vector3& N)
		{
			this->N=N;

			//note- we're not negating D, so we will need to subtract it everywhere else
			D = N.Dot(P);
		}

		//Creates a plane based on 3 vectors (intended for the verts of a triangle)
		WPlane(const Vector3& A, const Vector3& B, const Vector3& C)
		{
			BuildPlane(A,B,C);
		}

		//Creates a plane based on 3 vectors (intended for the verts of a triangle)
		void BuildPlane(const Vector3& A, const Vector3& B, const Vector3& C)
		{
			N = (B-A).Cross(C-A);  // NORMAL = AB X AC
			N.Normalize();      // REQUIRES NORMALIZED NORMAL

			//note- we're not negating D, so we will need to subtract it everywhere else
			D = N.Dot(A);            // EVALUATE EQUATION WITH NORM AND PT ON PLANE TO GET D
		}

		// RETURNS THE DIST TO PT FROM PLANE (+ in dir of normal, - in opposite dir)
		float DistanceTo(const Vector3& P) const 
		{ 
			return( (N.Dot(P)) - (D) ); 
		}

		float DistanceToNormalized(const Vector3& P) const
		{
			return (N.Dot(P) - D) / N.GetLength();
		}

		//Performs a hit test between a ray and a plane
		//reference: http://www.siggraph.org/education/materials/HyperGraph/raytrace/rayplane_intersection.htm
		bool RayIntersection(const Vector3& Start, const Vector3& Dir, float& t, Vector3& HitPt) const
		{
			float vd = N.Dot(Dir);					// DOT-PROD BETWEEN NORMAL AND RAY DIR

			//If Vd > 0 then the normal of the plane is pointing away from the ray
			//If Vd==0 RAY AND PLANE ARE PARALLEL
			if (vd>=0)
				return(false);

			float v0 = -(N.Dot(Start) - D);
			t = v0/vd;								// CALC PARAMETRIC T-VAL
			if (t>0)								// IF INTERSECTION NOT "BEHIND" RAY
			{
				HitPt = Start + Dir*t;				// CALC HIT POINT
				return(true);                       // INTERSECTION FOUND!
			}
			return(false);
		}

		bool IntersectsLineSegment(const Vector3& start, const Vector3& end, float& t, Vector3& q) const;

		Halfspace ClassifyPoint(const Vector3& pt) const
		{
			// d = N.Dot(pt) - D;
			float d = N.x*pt.x + N.y*pt.y + N.z*pt.z - D;
			if (d < -EPSILON) return Halfspace_BEHIND;
			if (d > EPSILON) return Halfspace_FRONT;
			return Halfspace_ON_PLANE;
		}

		Halfspace ClassifySphere(const Vector3& center, float radius) const
		{
			float test = (N.Dot(center)) - D;
			if (test >= 0)
				return (test < radius) ? Halfspace_ON_PLANE : Halfspace_FRONT;
			else
				return (-test < radius) ? Halfspace_ON_PLANE : Halfspace_BEHIND;
		}

		Halfspace ClassifyBox(const Vector3& min, const Vector3& max) const
		{
			Halfspace space1 = ClassifyPoint(min);
			Halfspace space2 = ClassifyPoint(max);

			if (space1 == space2)
			{
				return space1;
			}

			//what to return if one point is on_plane and the other differs?
			//if the points differ we just return on_plane
			return Halfspace_ON_PLANE;
		}

		Vector3 Project(const Vector3& _p)
		{
			float h = DistanceTo(_p);
			return Vector3(_p.x - N.x * h,
					 _p.y - N.y * h,
					 _p.z - N.z * h);
		}

		//normalizes this plane
		void Normalize()
		{
			float mag;

			mag = N.GetLength();
			if (mag != 0)
			{
				N.x = N.x / mag;
				N.y = N.y / mag;
				N.z = N.z / mag;
				D   =  D  / mag;
			}
		}

		//inverts this plane
		void Invert()
		{
			N.Invert();
		}

		static WPlane Zero( void )
		{
			WPlane p;
			p.N.MakeZero();
			p.D = 0;
			return p;
		}

		bool operator==(const WPlane& Other) const;

		friend GDZArchive& operator<<(GDZArchive& ar, WPlane& p)
		{
			ar << p.N << p.D;
			return ar;
		}
	};
}

#endif