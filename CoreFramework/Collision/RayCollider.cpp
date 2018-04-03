
#include "RayCollider.h"
#include <math.h>

using namespace GODZ;

//! Integer representation of a floating-point value.
#define IR(x)	((udword&)x)

#define USE_MINMAX


#define CROSS(dest,v1,v2) \
          dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define SUB(dest,v1,v2) \
          dest[0]=v1[0]-v2[0]; \
          dest[1]=v1[1]-v2[1]; \
          dest[2]=v1[2]-v2[2]; 

/* the original jgt code */
GODZ_API bool GODZ::intersect_triangle(const float orig[3], const float dir[3],
		       const float vert0[3], const float vert1[3], const float vert2[3],
		       float *t, float *u, float *v)
{
   float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
   float det,inv_det;

   /* find vectors for two edges sharing vert0 */
   SUB(edge1, vert1, vert0);
   SUB(edge2, vert2, vert0);

   /* begin calculating determinant - also used to calculate U parameter */
   CROSS(pvec, dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   det = DOT(edge1, pvec);

   if (det > -EPSILON && det < EPSILON)
     return false;
   inv_det = 1.0 / det;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec) * inv_det;
   if (*u < 0.0 || *u > 1.0)
     return false;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

   /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec) * inv_det;
   if (*v < 0.0 || *u + *v > 1.0)
     return false;

   /* calculate t, ray intersects triangle */
   *t = DOT(edge2, qvec) * inv_det;

   return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A method to compute a ray-AABB intersection.
 *	Original code by Andrew Woo, from "Graphics Gems", Academic Press, 1990
 *	Optimized code by Pierre Terdiman, 2000 (~20-30% faster on my Celeron 500)
 *	Epsilon value added by Klaus Hartmann. (discarding it saves a few cycles only)
 *
 *	Hence this version is faster as well as more robust than the original one.
 *
 *	Should work provided:
 *	1) the integer representation of 0.0f is 0x00000000
 *	2) the sign bit of the float is the most significant one
 *
 *	Report bugs: p.terdiman@codercorner.com
 *
 *	\param		aabb		[in] the axis-aligned bounding box
 *	\param		origin		[in] ray origin
 *	\param		dir			[in] ray direction
 *	\param		coord		[out] impact coordinates
 *	\return		true if ray intersects AABB
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define RAYAABB_EPSILON 0.00001f
GODZ_API bool GODZ::RayAABB(const WBox& aabb, const Vector3& _origin, const Vector3& _dir, Vector3& coord)
{
	bool Inside = true;
	Vector3 MinB = aabb.GetCenter() - aabb.GetExtent();
	Vector3 MaxB = aabb.GetCenter() + aabb.GetExtent();
	Vector3 MaxT;
	MaxT.x=MaxT.y=MaxT.z=-1.0f;

	//this sucks, need to figure out a better way to use the params passed in
	Vector3 origin = _origin;
	Vector3 dir = _dir;

	// Find candidate planes.
	for(udword i=0;i<3;i++)
	{
		if(origin.Get(i) < MinB.Get(i))
		{
			coord.Set(i, MinB.Get(i));
			Inside		= false;

			// Calculate T distances to candidate planes
			float dir_f = dir.Get(i);
			if(IR(dir_f))	
			{
				MaxT.Set(i, (MinB.Get(i) - origin.Get(i)) / dir.Get(i));
			}
		}
		else if(origin.Get(i) > MaxB.Get(i))
		{
			coord.Set(i, MaxB.Get(i));
			Inside		= false;

			// Calculate T distances to candidate planes
			float dir_f = dir.Get(i);
			if(IR(dir_f))	
			{
				MaxT.Set(i, (MaxB.Get(i) - origin.Get(i)) / dir.Get(i));
			}
		}
	}

	// Ray origin inside bounding box
	if(Inside)
	{
		coord = origin;
		return true;
	}

	// Get largest of the maxT's for final choice of intersection
	udword WhichPlane = 0;
	if(MaxT.Get(1) > MaxT.Get(WhichPlane))	WhichPlane = 1;
	if(MaxT.Get(2) > MaxT.Get(WhichPlane))	WhichPlane = 2;

	// Check final candidate actually inside box
	float t_plane = MaxT.Get(WhichPlane);
	if(IR(t_plane)&0x80000000) return false;

	for(unsigned int i=0;i<3;i++)
	{
		if(i!=WhichPlane)
		{
			coord.Set(i, origin.Get(i) + MaxT.Get(WhichPlane) * dir.Get(i));
#ifdef RAYAABB_EPSILON
			if(coord.Get(i) < MinB.Get(i) - RAYAABB_EPSILON || coord.Get(i) > MaxB.Get(i) + RAYAABB_EPSILON)	return false;
#else
			if(coord.Get(i) < MinB.Get(i) || coord.Get(i) > MaxB.Get(i))	return false;
#endif
		}
	}
	return true;	// ray hits box
}



GODZ_API void FIND_CANDIDATE_PLANE(int i, const WBox& aabb, const Vector3& origin, const Vector3& dir, Vector3& coord, Vector3& MaxT, bool &Inside)		
{
	Vector3 min = aabb.GetMin();
	Vector3 max = aabb.GetMax();

		if(origin.Get(i) < min.Get(i))										
		{																	
			/* Calculate T distances to candidate planes */		
			float dir_f = dir.Get(i);
			if(IR(dir_f))
			{
				MaxT.Set(i, (min.Get(i) - origin.Get(i)) / dir.Get(i));
			}
																			
			Inside		= false;											
			coord.Set(i, min.Get(i));	
		}																	
		else if(origin.Get(i) > max.Get(i))									
		{																	
			/* Calculate T distances to candidate planes */		
			float dir_f = dir.Get(i);
			if(IR(dir_f))	MaxT.Set(i, (max.Get(i) - origin.Get(i)) / dir.Get(i));
																			
			Inside		= false;											
			coord.Set(i, max.Get(i));									
		}
}

// Unroll loop
#ifndef USE_MINMAX

	#ifndef RAYAABB_EPSILON
		#define COMPUTE_INTERSECT(i)											\
			if(i!=WhichPlane)													\
			{																	\
				coord[i] = origin[i] + MaxT[WhichPlane] * dir[i];				\
				if(coord[i] < MinB[i] || coord[i] > MaxB[i])	return false;	\
			}
	#else
		#define COMPUTE_INTERSECT(i)											\
			if(i!=WhichPlane)													\
			{																	\
				coord[i] = origin[i] + MaxT[WhichPlane] * dir[i];				\
				if(coord[i] < MinB[i] - RAYAABB_EPSILON || coord[i] > MaxB[i] + RAYAABB_EPSILON)	return false;	\
			}
	#endif

#else

	#ifndef RAYAABB_EPSILON
		#define COMPUTE_INTERSECT(i)											\
			if(i!=WhichPlane)													\
			{																	\
				coord.Set(i, origin.Get(i) + MaxT.Get(WhichPlane) * dir.Get(i));				\
				if(coord.Get(i) < aabb.GetMin(i) || coord.Get(i) > aabb.GetMax(i))	return false;	\
			}
	#else
		#define COMPUTE_INTERSECT(i)											\
			if(i!=WhichPlane)													\
			{																	\
				coord.Set(i,  origin.Get(i) + MaxT.Get(WhichPlane) * dir.Get(i));				\
				if(coord.Get(i) < aabb.GetMin(i) - RAYAABB_EPSILON || coord.Get(i) > aabb.GetMax(i) + RAYAABB_EPSILON)	return false;	\
			}
	#endif

#endif



/*
	Updated October, 5, 2001 :
		- Below is an alternative version, up to ~25% faster than the one above on my Celeron.
		- Not fully tested so handle with care. It's not that much different anyway.

	Updated October, 9, 2001:
		- Fixed a slight bug......
		- Compiles for (Min, Max) or (Center, Extents) boxes
		- I did some tests with Adam Moravanszky, and the fastest version apparently depends on the machine. See for yourself.
		- You should also take a look at Tim Schröder's version in GDMag. It's sometimes faster thanks to early exits, sometimes
		slower (at least on my machine!).
*/

GODZ_API bool GODZ::RayAABB4(const WBox& aabb, const Vector3& origin, const Vector3& dir, Vector3& coord)
{
	bool Inside = true;
#ifndef USE_MINMAX
	Vector3 MinB; MinB=aabb.GetMin();
	Vector3 MaxB; MaxB=aabb.GetMax();
#endif
	Vector3 MaxT;
	MaxT.x=MaxT.y=MaxT.z=-1.0f;

	// Find candidate planes.
	FIND_CANDIDATE_PLANE(0, aabb, origin, dir, coord, MaxT,Inside);
	FIND_CANDIDATE_PLANE(1, aabb, origin, dir, coord, MaxT,Inside);
	FIND_CANDIDATE_PLANE(2, aabb, origin, dir, coord, MaxT,Inside);

	// Ray origin inside bounding box
	if(Inside)
	{
		coord = origin;
		return true;
	}

	// Get largest of the MaxT's for final choice of intersection
	// - this version without FPU compares
	// - but branch prediction might suffer
	// - a bit faster on my Celeron, duno how it behaves on something like a P4
	udword WhichPlane;
	if(IR(MaxT[0])&0x80000000)
	{
		// T[0]<0
		if(IR(MaxT[1])&0x80000000)
		{
			// T[0]<0
			// T[1]<0
			if(IR(MaxT[2])&0x80000000)
			{
				// T[0]<0
				// T[1]<0
				// T[2]<0
				return false;
			}
			else
			{
				WhichPlane = 2;
			}
		}
		else if(IR(MaxT[2])&0x80000000)
		{
			// T[0]<0
			// T[1]>0
			// T[2]<0
			WhichPlane = 1;
		}
		else
		{
			// T[0]<0
			// T[1]>0
			// T[2]>0
			if(IR(MaxT[2]) > IR(MaxT[1]))
			{
				WhichPlane = 2;
			}
			else
			{
				WhichPlane = 1;
			}
		}
	}
	else
	{
		// T[0]>0
		if(IR(MaxT[1])&0x80000000)
		{
			// T[0]>0
			// T[1]<0
			if(IR(MaxT[2])&0x80000000)
			{
				// T[0]>0
				// T[1]<0
				// T[2]<0
				WhichPlane = 0;
			}
			else
			{
				// T[0]>0
				// T[1]<0
				// T[2]>0
				if(IR(MaxT[2]) > IR(MaxT[0]))
				{
					WhichPlane = 2;
				}
				else
				{
					WhichPlane = 0;
				}
			}
		}
		else if(IR(MaxT[2])&0x80000000)
		{
			// T[0]>0
			// T[1]>0
			// T[2]<0
			if(IR(MaxT[1]) > IR(MaxT[0]))
			{
				WhichPlane = 1;
			}
			else
			{
				WhichPlane = 0;
			}
		}
		else
		{
			// T[0]>0
			// T[1]>0
			// T[2]>0
			WhichPlane = 0;
			if(IR(MaxT[1]) > IR(MaxT[WhichPlane]))	WhichPlane = 1;
			if(IR(MaxT[2]) > IR(MaxT[WhichPlane]))	WhichPlane = 2;
		}
	}

	// Old code below:
/*
	// Get largest of the maxT's for final choice of intersection
	udword WhichPlane = 0;
	if(MaxT[1] > MaxT[WhichPlane])	WhichPlane = 1;
	if(MaxT[2] > MaxT[WhichPlane])	WhichPlane = 2;

	// Check final candidate actually inside box
	if(IR(MaxT[WhichPlane])&0x80000000) return false;
*/

	COMPUTE_INTERSECT(0)
	COMPUTE_INTERSECT(1)
	COMPUTE_INTERSECT(2)

	return true;	// ray hits box
}


// For a ray
GODZ_API bool GODZ::RayAABB(const WRay& ray, const WBox& aabb)
{
	Vector3 BoxExtents, Diff;

	Vector3 center = aabb.GetCenter();
	Vector3 extents = aabb.GetExtent();

	Diff.x = ray.origin.x - center.Get(0);
	BoxExtents.x = extents.Get(0);
	if(fabsf(Diff.x)>BoxExtents.x && Diff.x*ray.dir.x>=0.0f)	return false;

	Diff.y = ray.origin.y - center.Get(1);
	BoxExtents.y = extents.Get(1);	
	if(fabsf(Diff.y)>BoxExtents.y && Diff.y*ray.dir.y>=0.0f)	return false;

	Diff.z = ray.origin.z - center.Get(2);
	BoxExtents.z = extents.Get(2);
	if(fabsf(Diff.z)>BoxExtents.z && Diff.z*ray.dir.z>=0.0f)	return false;

	float fAWdU[3];
	fAWdU[0] = fabsf(ray.dir.x);
	fAWdU[1] = fabsf(ray.dir.y);
	fAWdU[2] = fabsf(ray.dir.z);

	float f;
	f = ray.dir.y * Diff.z - ray.dir.z * Diff.y;	if(fabsf(f)>BoxExtents.y*fAWdU[2] + BoxExtents.z*fAWdU[1])	return false;
	f = ray.dir.z * Diff.x - ray.dir.x * Diff.z;	if(fabsf(f)>BoxExtents.x*fAWdU[2] + BoxExtents.z*fAWdU[0])	return false;
	f = ray.dir.x * Diff.y - ray.dir.y * Diff.x;	if(fabsf(f)>BoxExtents.x*fAWdU[1] + BoxExtents.y*fAWdU[0])	return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Computes a ray-triangle intersection test.
 *	Original code from Tomas Möller's "Fast Minimum Storage Ray-Triangle Intersection".
 *	It's been optimized a bit with integer code, and modified to return a non-intersection if distance from
 *	ray origin to triangle is negative.
 *
 *	\param		vert0	[in] triangle vertex
 *	\param		vert1	[in] triangle vertex
 *	\param		vert2	[in] triangle vertex
 *	\return		true on overlap. mStabbedFace is filled with relevant info.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GODZ::RayTriOverlap(const WRay &ray, const Vector3& vert0, const Vector3& vert1, const Vector3& vert2, CollisionFace &mStabbedFace, bool mCulling)
{
	// Stats
	//mNbRayPrimTests++;

	// Find vectors for two edges sharing vert0
	Vector3 edge1 = vert1 - vert0;
	Vector3 edge2 = vert2 - vert0;

	// Begin calculating determinant - also used to calculate U parameter
	Vector3 pvec = ray.dir.Cross(edge2);

	// If determinant is near zero, ray lies in plane of triangle
	float det = edge1.Dot(pvec);

	if(mCulling)
	{
		if(det<LOCAL_EPSILON)
			return 0;
		// From here, det is > 0. So we can use integer cmp.

		// Calculate distance from vert0 to ray origin
		Vector3 tvec = ray.origin - vert0;

		// Calculate U parameter and test bounds
		mStabbedFace.mU = tvec.Dot(pvec);
//		if(IR(u)&0x80000000 || u>det)					return FALSE;
		if(IS_NEGATIVE_FLOAT(mStabbedFace.mU) || IR(mStabbedFace.mU)>IR(det))
			return 0;

		// Prepare to test V parameter
		//Vector3 qvec = tvec^edge1;
		Vector3 qvec = tvec.Cross(edge1);

		// Calculate V parameter and test bounds
		mStabbedFace.mV = ray.dir.Dot(qvec);
		if(IS_NEGATIVE_FLOAT(mStabbedFace.mV) || mStabbedFace.mU+mStabbedFace.mV>det)	
			return 0;

		// Calculate t, scale parameters, ray intersects triangle
		mStabbedFace.mDistance = edge2.Dot(qvec);
		// Det > 0 so we can early exit here
		// Intersection point is valid if distance is positive (else it can just be a face behind the orig point)
		if(IS_NEGATIVE_FLOAT(mStabbedFace.mDistance))
			return 0;
		// Else go on
		float OneOverDet = 1.0f / det;
		mStabbedFace.mDistance *= OneOverDet;
		mStabbedFace.mU *= OneOverDet;
		mStabbedFace.mV *= OneOverDet;
	}
	else
	{
		// the non-culling branch
		if(det>-LOCAL_EPSILON && det<LOCAL_EPSILON)	
			return 0;
		float OneOverDet = 1.0f / det;

		// Calculate distance from vert0 to ray origin
		Vector3 tvec = ray.origin - vert0;

		// Calculate U parameter and test bounds
		mStabbedFace.mU = (tvec.Dot(pvec)) * OneOverDet;
//		if(IR(u)&0x80000000 || u>1.0f)					return FALSE;
		if(IS_NEGATIVE_FLOAT(mStabbedFace.mU) || IR(mStabbedFace.mU)>IEEE_1_0)
			return 0;

		// prepare to test V parameter
		Vector3 qvec = tvec.Cross(edge1);

		// Calculate V parameter and test bounds
		mStabbedFace.mV = (ray.dir.Dot(qvec)) * OneOverDet;
		if(IS_NEGATIVE_FLOAT(mStabbedFace.mV) || mStabbedFace.mU+mStabbedFace.mV>1.0f)	
			return 0;

		// Calculate t, ray intersects triangle
		mStabbedFace.mDistance = (edge2.Dot(qvec)) * OneOverDet;
		// Intersection point is valid if distance is positive (else it can just be a face behind the orig point)
		if(IS_NEGATIVE_FLOAT(mStabbedFace.mDistance))								
			return 0;
	}
	return true;
}

//! TO BE DOCUMENTED
bool GODZ::planeBoxOverlap(const Vector3& normal, const float d, const Vector3& maxbox)
{
	Vector3 vmin, vmax;
	for(udword q=0;q<=2;q++)
	{
		if(normal[q]>0.0f)	{ vmin[q]=-maxbox[q]; vmax[q]=maxbox[q]; }
		else				{ vmin[q]=maxbox[q]; vmax[q]=-maxbox[q]; }
	}
	if((normal.Dot(vmin))+d>0.0f) return 0;
	if((normal.Dot(vmax))+d>=0.0f) return true;

	return true;
}

//! TO BE DOCUMENTED
#define AXISTEST_X01(a, b, fa, fb)							\
	min = a*v0.y - b*v0.z;									\
	max = a*v2.y - b*v2.z;									\
	if(min>max) {const float tmp=max; max=min; min=tmp;	}	\
	rad = fa * extents.y + fb * extents.z;					\
	if(min>rad || max<-rad) return FALSE;

//! TO BE DOCUMENTED
#define AXISTEST_X2(a, b, fa, fb)							\
	min = a*v0.y - b*v0.z;									\
	max = a*v1.y - b*v1.z;									\
	if(min>max) {const float tmp=max; max=min; min=tmp;	}	\
	rad = fa * extents.y + fb * extents.z;					\
	if(min>rad || max<-rad) return FALSE;

//! TO BE DOCUMENTED
#define AXISTEST_Y02(a, b, fa, fb)							\
	min = b*v0.z - a*v0.x;									\
	max = b*v2.z - a*v2.x;									\
	if(min>max) {const float tmp=max; max=min; min=tmp;	}	\
	rad = fa * extents.x + fb * extents.z;					\
	if(min>rad || max<-rad) return FALSE;

//! TO BE DOCUMENTED
#define AXISTEST_Y1(a, b, fa, fb)							\
	min = b*v0.z - a*v0.x;									\
	max = b*v1.z - a*v1.x;									\
	if(min>max) {const float tmp=max; max=min; min=tmp;	}	\
	rad = fa * extents.x + fb * extents.z;					\
	if(min>rad || max<-rad) return FALSE;

//! TO BE DOCUMENTED
#define AXISTEST_Z12(a, b, fa, fb)							\
	min = a*v1.x - b*v1.y;									\
	max = a*v2.x - b*v2.y;									\
	if(min>max) {const float tmp=max; max=min; min=tmp;	}	\
	rad = fa * extents.x + fb * extents.y;					\
	if(min>rad || max<-rad) return FALSE;

//! TO BE DOCUMENTED
#define AXISTEST_Z0(a, b, fa, fb)							\
	min = a*v0.x - b*v0.y;									\
	max = a*v1.x - b*v1.y;									\
	if(min>max) {const float tmp=max; max=min; min=tmp;	}	\
	rad = fa * extents.x + fb * extents.y;					\
	if(min>rad || max<-rad) return FALSE;

// compute triangle edges
// - edges lazy evaluated to take advantage of early exits
// - fabs precomputed (half less work, possible since extents are always >0)
// - customized macros to take advantage of the null component
// - axis vector discarded, possibly saves useless movs
#define IMPLEMENT_CLASS3_TESTS						\
	float rad;										\
	float min, max;									\
													\
	const float fey0 = fabsf(e0.y);					\
	const float fez0 = fabsf(e0.z);					\
	AXISTEST_X01(e0.z, e0.y, fez0, fey0);			\
	const float fex0 = fabsf(e0.x);					\
	AXISTEST_Y02(e0.z, e0.x, fez0, fex0);			\
	AXISTEST_Z12(e0.y, e0.x, fey0, fex0);			\
													\
	const float fey1 = fabsf(e1.y);					\
	const float fez1 = fabsf(e1.z);					\
	AXISTEST_X01(e1.z, e1.y, fez1, fey1);			\
	const float fex1 = fabsf(e1.x);					\
	AXISTEST_Y02(e1.z, e1.x, fez1, fex1);			\
	AXISTEST_Z0(e1.y, e1.x, fey1, fex1);			\
													\
	const Point e2 = mLeafVerts[0] - mLeafVerts[2];	\
	const float fey2 = fabsf(e2.y);					\
	const float fez2 = fabsf(e2.z);					\
	AXISTEST_X2(e2.z, e2.y, fez2, fey2);			\
	const float fex2 = fabsf(e2.x);					\
	AXISTEST_Y1(e2.z, e2.x, fez2, fex2);			\
	AXISTEST_Z12(e2.y, e2.x, fey2, fex2);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Triangle-Box overlap test using the separating axis theorem.
 *	This is the code from Tomas Möller, a bit optimized:
 *	- with some more lazy evaluation (faster path on PC)
 *	- with a tiny bit of assembly
 *	- with "SAT-lite" applied if needed
 *	- and perhaps with some more minor modifs...
 *
 *	\param		center		[in] box center
 *	\param		extents		[in] box extents
 *	\return		true if triangle & box overlap
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GODZ::TriBoxOverlap(const Vector3& center, const Vector3& extents, const Vector3 mLeafVerts[3])
{
	// use separating axis theorem to test overlap between triangle and box 
	// need to test for overlap in these directions: 
	// 1) the {x,y,z}-directions (actually, since we use the AABB of the triangle 
	//    we do not even need to test these) 
	// 2) normal of the triangle 
	// 3) crossproduct(edge from tri, {x,y,z}-directin) 
	//    this gives 3x3=9 more tests 

	// move everything so that the boxcenter is in (0,0,0) 
	Vector3 v0, v1, v2;
	v0.x = mLeafVerts[0].x - center.x;
	v1.x = mLeafVerts[1].x - center.x;
	v2.x = mLeafVerts[2].x - center.x;

	// First, test overlap in the {x,y,z}-directions
	float min,max;
	// Find min, max of the triangle in x-direction, and test for overlap in X
	FINDMINMAX(v0.x, v1.x, v2.x, min, max);
	if(min>extents.x || max<-extents.x) return false;

	// Same for Y
	v0.y = mLeafVerts[0].y - center.y;
	v1.y = mLeafVerts[1].y - center.y;
	v2.y = mLeafVerts[2].y - center.y;

	FINDMINMAX(v0.y, v1.y, v2.y, min, max);
	if(min>extents.y || max<-extents.y) return false;

	// Same for Z
	v0.z = mLeafVerts[0].z - center.z;
	v1.z = mLeafVerts[1].z - center.z;
	v2.z = mLeafVerts[2].z - center.z;

	FINDMINMAX(v0.z, v1.z, v2.z, min, max);
	if(min>extents.z || max<-extents.z) return false;

	// 2) Test if the box intersects the plane of the triangle
	// compute plane equation of triangle: normal*x+d=0
	// ### could be precomputed since we use the same leaf triangle several times
	const Vector3 e0 = v1 - v0;
	const Vector3 e1 = v2 - v1;
	const Vector3 normal = e0.Cross(e1);
	const float d = -normal.Dot(v0);
	if(!planeBoxOverlap(normal, d, extents)) return false;

	return true;
}


