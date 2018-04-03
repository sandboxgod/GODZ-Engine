// Implements Vertex, Vector3, WBox geometry structures

#include "WPlane.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <CoreFramework/Core/VertexTypes.h>
#include <CoreFramework/Math/WGeometry.h>
#include <CoreFramework/Math/vectormath.h>


using namespace GODZ;


GODZ_API float GODZ::ComputeAngle(const Vector3& u, const Vector3& v)
{
	//determine angle theta between u & v
	//cos T = (u dot v) / (||u|| * ||v||)
	float angle = u.Dot(v) / (u.GetLength() * v.GetLength());

	//NaN protection
	if (angle > 1.0f)
	{
		angle = 1.0f;
	}
	else if (angle < -1.0f)
	{
		angle = -1.0f;
	}

	return acosf(angle);
}

//returns the angle between two vectors in radians. Note, to compute axis vector simply
//take cross product between u and v
GODZ_API float GODZ::ComputeAngle(const Vector3& u, const Vector3& v, Vector3& axis)
{
	u.Cross(v, axis);	//computes angle-axis
	
	if ( axis.GetLength() < EPSILON)
	{
		return 0.0f;
	}

	axis.Normalize(); //axis must be normalized before use

	//determine angle theta between u & v
	//cos T = (u dot v) / (||u|| * ||v||)
	return atan2f(axis.GetLength(),u.Dot(v));

	//acos' formula gives the correct answer with unit vectors as it stands, but it encounters an accuracy problem for angles that are near 0
	//or pi. This is the main reason for my preference for the 'atan2' method.
}

// Offset pIn by pOffset into pOut
void VectorOffset (Vector3* pIn, Vector3* pOffset, Vector3* pOut)
{
   pOut->x = pIn->x - pOffset->x;
   pOut->y = pIn->y - pOffset->y;
   pOut->z = pIn->z - pOffset->z;
}

// Compute the cross product a X b into pOut
void VectorGetNormal (Vector3* a, Vector3* b, Vector3* pOut)
{
   pOut->x = a->y * b->z - a->z * b->y;
   pOut->y = a->z * b->x - a->x * b->z;
   pOut->z = a->x * b->y - a->y * b->x;
}


GODZ_API Vector3 GODZ::ComputeFaceNormal(Vector3 v[3])
{
   // Uses p2 as a new origin for p1,p3
   Vector3 a;
   VectorOffset(&v[2], &v[1], &a);
   Vector3 b;
   VectorOffset(&v[0], &v[1], &b);
   // Compute the cross product a X b to get the face normal
   Vector3 pn;
   VectorGetNormal(&a, &b, &pn);
   // Return a normalized vector
   pn.Normalize();

   return pn;
}

GODZ_API Vector3 GODZ::ComputeMidPoint(const Vector3 m_points[3])
{ 
	return (m_points[0]+m_points[1]+m_points[2])*(1.0f/3.0f); 
}

GODZ_API float GODZ::FDot(const Vector3& p1, const Vector3& p2)
{
		float x,y,z;
		float v;
		
		//just experimenting to determine speed differences between hand written assembly
		//and the compiler....
		_asm 
		{
			mov     esi,[p1]
			mov     edx,[p2]     

			fld dword ptr [esi+0];	//push p1.x
			fld dword ptr [edx+0];	//push p2.x
			fmul;
			fstp x;		//x = p1.x * p2.x

			fld dword ptr [esi+4];  //push p1.y
			fld dword ptr [edx+4];	//push p2.y
			fmul;
			fstp y;		//y = p1.y * p2.y

			fld dword ptr [esi+8];
			fld dword ptr [edx+8];
			fmul;
			fstp z;		//z = p1.z * p2.z

			fld x;
			fld y;
			fadd;		//store result of x + y
			fld z;		//push z
			fadd;
			fst v;		//v += z
		}
		
		

		return v;
}

GODZ_API float GODZ::Signed3DTriArea(const Vector3& a, const Vector3& b, const Vector3& c)
	{
		//The area of a 3D triangle defined by the vertex V0, V1, V2 is half the magnitude of the cross product of the two edge vectors:
		//0.5 * | V0V1 * V0V2 |
		return 0.5f * ((b-a).Cross(c-a)).GetLength();
	}

GODZ_API float GODZ::Signed2DTriArea(const Vector3& a, const Vector3& b, const Vector3& c)
{
	return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
}

GODZ_API float GODZ::GetSquaredLine3(const WRay& m_rkLine0, const WRay& m_rkLine1, Vector3& m_kClosestPoint0, Vector3& m_kClosestPoint1)
{
    Vector3 kDiff = m_rkLine0.origin - m_rkLine1.origin;
    float fA01 = -m_rkLine0.dir.Dot(m_rkLine1.dir);
    float fB0 = kDiff.Dot(m_rkLine0.dir);
    float fC = kDiff.GetLength();
    float fDet = fabs((float)1.0 - fA01*fA01);
    float fB1, fS0, fS1, fSqrDist;

    if (fDet >= 0)
    {
        // lines are not parallel
        fB1 = -kDiff.Dot(m_rkLine1.dir);
        float fInvDet = ((float)1.0)/fDet;
        fS0 = (fA01*fB1-fB0)*fInvDet;
        fS1 = (fA01*fB0-fB1)*fInvDet;
        fSqrDist = fS0*(fS0+fA01*fS1+((float)2.0)*fB0) +
            fS1*(fA01*fS0+fS1+((float)2.0)*fB1)+fC;
    }
    else
    {
        // lines are parallel, select any closest pair of points
        fS0 = -fB0;
        fS1 = (float)0.0;
        fSqrDist = fB0*fS0+fC;
    }

    m_kClosestPoint0 = m_rkLine0.origin + m_rkLine0.dir * fS0;
    m_kClosestPoint1 = m_rkLine1.origin + m_rkLine1.dir * fS1;
//    m_fLine0Parameter = fS0;
//    m_fLine1Parameter = fS1;
    return fabs(fSqrDist);
}

GODZ_API Vector3 GODZ::ClosestPtPointTriangle(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c)
{
	//check if P in vertex region outside A
	Vector3 ab = b - a;
	Vector3 ac = c - a;
	Vector3 ap = p - a;
	float d1 = ab.Dot(ap);
	float d2 = ac.Dot(ap);

	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		return a; //barycentric coords (1,0,0)
	}

	//check if P in vertex region outside B
	Vector3 bp = p - b;
	float d3 = ab.Dot(bp);
	float d4 = ac.Dot(bp);
	if (d3 >= 0.0f && d4 <= d3)
	{
		return b; //bary coords (0,1,0)
	}

	//check if P in edge of region of AB, if so return proj of P onto AB
	float vc = d1*d4 - d3*d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		return a + v * ab; //bary coords (1-v,v,0)
	}

	//check if P in vertex region outside C
	Vector3 cp = p - c;
	float d5 = ab.Dot(cp);
	float d6 = ac.Dot(cp);
	if (d6 >= 0.0f && d5 <= d6)
	{
		return c; //bary coords (0,0,1)
	}

	//check if P is in edge region of AC, if so return proj of P onto AC
	float vb = d5*d2 - d1*d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		return a + w * ac; //bary coords (1-w,0,w)
	}

	//check if P in edge region of BC, if so return projection of P onto BC
	float va = d3*d6 - d5*d4;
	if (va <= 0.0f && (d4 -d3) >= 0 && (d5-d6) >= 0)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return b + w * (c-b); //bary coords (0,1-w,w)
	}

	//P inside face region. Compute Q through its barycentric coords (u,v,w)
	float denom = 1 / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	return a + ab * v + ac * w; //= u*a + v*b + w*c, u = va * denom = 1.0f - v - w
}

GODZ_API bool GODZ::IsNaN(float f)
{
	return f != f;
}

GODZ_API bool GODZ::IsPositionInViewingArea(const Vector3& afViewPosition3, const Vector3& afViewForward3, float viewAngle, const Vector3& afPositionToTest3)
{
	Vector3 dir = (afPositionToTest3 - afViewPosition3);
	float angle = ComputeAngle(afViewForward3, dir);

	return angle < viewAngle;
}