//-----------------------------------------------------------------------------
// File: Vectormath.cpp
//
// Desc: A collection of vector math related functions.
//
// Copyright (c) 2000 Telemachos of Peroxide
// www.peroxide.dk
//-----------------------------------------------------------------------------

//#define D3D_OVERLOADS
//#define STRICT
#include <math.h>
#include <stdio.h>
#include <CoreFramework/Math/vectormath.h>


namespace GODZ
{

// ----------------------------------------------------------------------
// Name  : intersectRayPlane()
// Input : rOrigin - origin of ray in world space
//         rVector - vector describing direction of ray in world space
//         pOrigin - Origin of plane 
//         pNormal - Normal to plane
// Notes : Normalized directional vectors expected
// Return: distance to plane in world units, -1 if no intersection.
// -----------------------------------------------------------------------  
GODZ_API float GODZ::intersectRayPlane(const Vector3& rOrigin, const Vector3& rVector, const Vector3& pOrigin, const Vector3& pNormal)
{  
  float d = - (dot(pNormal,pOrigin));
 
  float numer = dot(pNormal,rOrigin) + d;
  float denom = dot(pNormal,rVector);
  
  if (denom == 0)  // normal is orthogonal to vector, cant intersect
   return (-1.0f);
   
  return -(numer / denom);	
}


// ----------------------------------------------------------------------
// Name  : intersectRaySphere()
// Input : rO - origin of ray in world space
//         rV - vector describing direction of ray in world space
//         sO - Origin of sphere 
//         sR - radius of sphere
// Notes : Normalized directional vectors expected
// Return: distance to sphere in world units, -1 if no intersection.
// -----------------------------------------------------------------------  

GODZ_API bool intersectRaySphere(const Vector3& rO, const Vector3& rV, const Vector3& sO, float sR, float& dist) 
{
	
   Vector3 Q = sO-rO;
   
   float c = lengthOfVector(Q);
   float v = dot(Q,rV);
   float d = sR*sR - (c*c - v*v);

   // If there was no intersection, return -1
   if (d < 0.0) 
	   return false;

   // Return the distance to the [first] intersecting point
   dist = (v - sqrtf(d));
   return true;
}




// ----------------------------------------------------------------------
// Name  : CheckPointInTriangle()
// Input : point - point we wish to check for inclusion
//         a - first vertex in triangle
//         b - second vertex in triangle 
//         c - third vertex in triangle
// Notes : Triangle should be defined in clockwise order a,b,c
// Return: TRUE if point is in triangle, FALSE if not.
// -----------------------------------------------------------------------  

bool GODZ::CheckPointInTriangle(Vector3 point, Vector3 a, Vector3 b, Vector3 c)
{
  
  double total_angles = 0.0f;
       
  // make the 3 vectors
  Vector3 v1 = point-a;
  Vector3 v2 = point-b;
  Vector3 v3 = point-c;
  
  normalizeVector(v1);
  normalizeVector(v2);
  normalizeVector(v3);

  total_angles += acos(dot(v1,v2));
  total_angles += acos(dot(v2,v3));
  total_angles += acos(dot(v3,v1)); 
     
  if (fabs(total_angles-2*PI) <= 0.005)
   return (1);
     
  return(0);
}



// ----------------------------------------------------------------------
// Name  : closestPointOnLine()
// Input : a - first end of line segment
//         b - second end of line segment
//         p - point we wish to find closest point on line from 
// Notes : Helper function for closestPointOnTriangle()
// Return: closest point on line segment
// -----------------------------------------------------------------------  

Vector3 GODZ::closestPointOnLine(Vector3& a, Vector3& b, Vector3& p) {
	
   // Determine t (the length of the vector from ‘a’ to ‘p’)
   Vector3 c = p-a;
   Vector3 V = b-a; 
      
   float d = lengthOfVector(V);
      
   normalizeVector(V);  
   float t = dot(V,c);

   
   // Check to see if ‘t’ is beyond the extents of the line segment
   if (t < 0.0f) return (a);
   if (t > d) return (b);
 
  
   // Return the point between ‘a’ and ‘b’
   //set length of V to t. V is normalized so this is easy
   V.x = V.x * t;
   V.y = V.y * t;
   V.z = V.z * t;
           
   return (a+V);	
}




// ----------------------------------------------------------------------
// Name  : closestPointOnTriangle()
// Input : a - first vertex in triangle
//         b - second vertex in triangle 
//         c - third vertex in triangle
//         p - point we wish to find closest point on triangle from 
// Notes : 
// Return: closest point on line triangle edge
// -----------------------------------------------------------------------  

Vector3 GODZ::closestPointOnTriangle(Vector3 a, Vector3 b, Vector3 c, Vector3 p) {
	
  Vector3 Rab = closestPointOnLine(a, b, p);
  Vector3 Rbc = closestPointOnLine(b, c, p);
  Vector3 Rca = closestPointOnLine(c, a, p);
    
  double dAB = lengthOfVector(p-Rab);
  double dBC = lengthOfVector(p-Rbc);
  double dCA = lengthOfVector(p-Rca);
  
  
  double min = dAB;
  Vector3 result = Rab;
  
  if (dBC < min) {
    min = dBC;
    result = Rbc;
    }
 
  if (dCA < min)
   result = Rca;
  
    
  return (result);	
}



// ----------------------------------------------------------------------
// Name  : CheckPointInTriangle()
// Input : point - point we wish to check for inclusion
//         sO - Origin of sphere
//         sR - radius of sphere 
// Notes : 
// Return: TRUE if point is in sphere, FALSE if not.
// -----------------------------------------------------------------------  

bool GODZ::CheckPointInSphere(const Vector3& point, const Vector3& sO, double sR)
{
 float d = lengthOfVector(point-sO);
 
 if(d<= sR) return true;
 return false;
}




// ----------------------------------------------------------------------
// Name  : tangentPlaneNormalOfEllipsoid()
// Input : point - point we wish to compute normal at 
//         eO - Origin of ellipsoid
//         eR - radius vector of ellipsoid 
// Notes : 
// Return: a unit normal vector to the tangent plane of the ellipsoid in the point.
// -----------------------------------------------------------------------  
Vector3 GODZ::tangentPlaneNormalOfEllipsoid(const Vector3& point, const Vector3& eO, const Vector3& eR) {

 Vector3 p = point - eO;

 float a2 = eR.x * eR.x;
 float b2 = eR.y * eR.y;
 float c2 = eR.z * eR.z;

 
 Vector3 res;
 res.x = p.x / a2;
 res.y = p.y / b2;
 res.z = p.z / c2;

 normalizeVector(res);
 return (res);
}



// ----------------------------------------------------------------------
// Name  : classifyPoint()
// Input : point - point we wish to classify 
//         pO - Origin of plane
//         pN - Normal to plane 
// Notes : 
// Return: One of 3 classification codes
// -----------------------------------------------------------------------  

unsigned long GODZ::classifyPoint(const Vector3& point, const Vector3& pO, const Vector3& pN) {

 Vector3 dir = pO - point;
 double d = dot(dir, pN);
 
 if (d<-0.001f)
  return PLANE_FRONT;
 else
 if (d>0.001f)
  return PLANE_BACKSIDE;

return ON_PLANE;
}




}