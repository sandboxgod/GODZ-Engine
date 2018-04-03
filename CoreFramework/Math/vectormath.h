//-----------------------------------------------------------------------------
// File: Vectormath.h
//
// Desc: A collection of vector math related functions.
//
// Copyright (c) 2000 Telemachos of Peroxide
// www.peroxide.dk
//-----------------------------------------------------------------------------
#ifndef VECTORMATH_H
#define VECTORMATH_H

#include <CoreFramework/Math/Vector3.h>
#include <math.h>

namespace GODZ
{

#define PLANE_BACKSIDE 0x000001
#define PLANE_FRONT    0x000010
#define ON_PLANE       0x000100
const float PI = 3.14159265f;

// basic vector operations (inlined)
inline float dot(const GODZ::Vector3& v1, const GODZ::Vector3& v2) {
 return ( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z );	
}

inline void normalizeVector(GODZ::Vector3& v) {
 float len = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
 v.x /= len;
 v.y /= len;
 v.z /= len;
}

inline float lengthOfVector(const GODZ::Vector3& v) {
 return (sqrtf(v.x * v.x + v.y * v.y + v.z * v.z));
}

inline void setLength(GODZ::Vector3& v, float l) {
 float len = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
 v.x *= l/len;
 v.y *= l/len;
 v.z *= l/len;
} 

inline bool isZERO_VECTOR(const GODZ::Vector3& v) {
 if ((v.x == 0.0f) && (v.y == 0.0f) && (v.z == 0.0f))
   return true;
   
  return false;
}

inline GODZ::Vector3 wedge(const GODZ::Vector3& v1, const GODZ::Vector3& v2) {
 GODZ::Vector3 result;
 
 result.x = (v1.y * v2.z) - (v2.y * v1.z);
 result.y = (v1.z * v2.x) - (v2.z * v1.x);
 result.z = (v1.x * v2.y) - (v2.x * v1.y);
 
 return (result);
}



// ray intersections. All return -1.0 if no intersection, otherwise the distance along the 
// ray where the (first) intersection takes place
GODZ_API float      intersectRayPlane(const GODZ::Vector3& rOrigin, const GODZ::Vector3& rVector, const GODZ::Vector3& pOrigin, const GODZ::Vector3& pNormal);
GODZ_API bool		intersectRaySphere(const GODZ::Vector3& rO, const GODZ::Vector3& rV, const GODZ::Vector3& sO, float sR, float& dist);


// Distance to line of triangle
GODZ::Vector3  closestPointOnLine(GODZ::Vector3& a, GODZ::Vector3& b, GODZ::Vector3& p);
GODZ::Vector3  closestPointOnTriangle(GODZ::Vector3 a, GODZ::Vector3 b, GODZ::Vector3 c, GODZ::Vector3 p);

// point inclusion
bool CheckPointInTriangle(GODZ::Vector3 point ,GODZ::Vector3 a, GODZ::Vector3 b, GODZ::Vector3 c);
bool CheckPointInSphere(const GODZ::Vector3& point, const GODZ::Vector3& sO, double sR);

// Normal generation
GODZ::Vector3 tangentPlaneNormalOfEllipsoid(const GODZ::Vector3& point, const GODZ::Vector3& eO, const GODZ::Vector3& eR);

// Point classification
unsigned long classifyPoint(const GODZ::Vector3& point, const GODZ::Vector3& pO, const GODZ::Vector3& pN);

}
#endif // VECTORMATH_H