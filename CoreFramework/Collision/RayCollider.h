/* ===================================================================
	Defines some collision routines

	Sources Cited:
	OPCODE 1.3 at http://www.codercorner.com/Opcode.htm 

	Created Feb 8, 2004 by Richard Osborne
	Copyright (c) 2010
	==================================================================
*/

#if !defined(_RAY_COLLIDER_H_)
#define _RAY_COLLIDER_H_

#include <CoreFramework/Math/WGeometry.h>
#include <CoreFramework/Math/WBox.h>

namespace GODZ
{	
	class GODZ_API CollisionFace
	{
		public:
		//! Constructor
		CollisionFace()
		{
			mFaceID=0;
			mDistance=0;
			mU=0;
			mV=0;
		}

		udword		mFaceID;				//!< Index of touched face
		float		mDistance;				//!< Distance from collider to hitpoint
		float		mU, mV;					//!< Impact barycentric coordinates
	};

	GODZ_API bool intersect_triangle(const float orig[3], const float dir[3],
		       const float vert0[3], const float vert1[3], const float vert2[3],
		       float *t, float *u, float *v);

	//! Integer-based comparison of a floating point value.
	//! Don't use it blindly, it can be faster or slower than the FPU comparison, depends on the context.
	#define IS_NEGATIVE_FLOAT(x)	(IR(x)&0x80000000)
	#define LOCAL_EPSILON 0.000001f
	#define IEEE_1_0				0x3f800000					//!< integer representation of 1.0

//! This macro quickly finds the min & max values among 3 variables
#define FINDMINMAX(x0, x1, x2, min, max)	\
	min = max = x0;							\
	if(x1<min) min=x1;						\
	if(x1>max) max=x1;						\
	if(x2<min) min=x2;						\
	if(x2>max) max=x2;


	GODZ_API bool RayAABB(const WBox& aabb, const Vector3& origin, const Vector3& dir, Vector3& coord);
	GODZ_API bool RayAABB4(const WBox& aabb, const Vector3& origin, const Vector3& dir, Vector3& coord);

	//Ray-AABB intersection
	GODZ_API bool RayAABB(const WRay& ray, const WBox& aabb);

	//Ray vs Triangle
	//[out] CollisionFace - information about the collision
	//[in]  bCulling - Stab culled faces or not
	GODZ_API bool RayTriOverlap(const WRay &ray, const Vector3& vert0, const Vector3& vert1, const Vector3& vert2, CollisionFace &collFace, bool bCulling = 0);

	//AABB vs Triangle
	GODZ_API bool TriBoxOverlap(const Vector3& center, const Vector3& extents, const Vector3 mLeafVerts[3]);
	bool planeBoxOverlap(const Vector3& normal, const float d, const Vector3& maxbox);
}

#endif