
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt

#include "OBBox.h"
#include "WBox.h"
#include "WSphere.h"
#include "WGeometry.h"
#include "WMatrix.h"
#include "IntersectionAxis.h"
#include <float.h>

namespace GODZ
{

OBBox::OBBox(const Vector3& _center, const Vector3& _extent)
: center(_center)
, extent(_extent)
{
	axis[0] = Vector3(1,0,0);
	axis[1] = Vector3(0,1,0);
	axis[2] = Vector3(0,0,1);
}

void OBBox::GetMinMax(Vector3& min, Vector3& max) const
{
	min = center - extent;
	max = center + extent;
}

bool OBBox::IsSeparated (float min0, float max0, float min1,
    float max1, float speed, float tmax, float& tlast, float& contactTime)
{
    float invSpeed, t;

    if (max1 < min0) // box1 initially on left of box0
    {
        if (speed <= (float)0)
        {
            // The projection intervals are moving apart.
            return true;
        }
        invSpeed = ((float)1)/speed;

        t = (min0 - max1)*invSpeed;
        if (t > contactTime)
        {
            contactTime = t;
        }

        if (contactTime > tmax)
        {
            // Intervals do not intersect during the specified time.
            return true;
        }

        t = (max0 - min1)*invSpeed;
        if (t < tlast)
        {
            tlast = t;
        }

        if (contactTime > tlast)
        {
            // Physically inconsistent times--the objects cannot intersect.
            return true;
        }
    }
    else if (max0 < min1) // box1 initially on right of box0
    {
        if (speed >= (float)0)
        {
            // The projection intervals are moving apart.
            return true;
        }
        invSpeed = ((float)1)/speed;

        t = (max0 - min1)*invSpeed;
        if (t > contactTime)
        {
            contactTime = t;
        }

        if (contactTime > tmax)
        {
            // Intervals do not intersect during the specified time.
            return true;
        }

        t = (min0 - max1)*invSpeed;
        if (t < tlast)
        {
            tlast = t;
        }

        if (contactTime > tlast)
        {
            // Physically inconsistent times--the objects cannot intersect.
            return true;
        }
    }
    else // box0 and box1 initially overlap
    {
        if (speed > (float)0)
        {
            t = (max0 - min1)/speed;
            if (t < tlast)
            {
                tlast = t;
            }

            if (contactTime > tlast)
            {
                // Physically inconsistent times--the objects cannot
                // intersect.
                return true;
            }
        }
        else if (speed < (float)0)
        {
            t = (min0 - max1)/speed;
            if (t < tlast)
            {
                tlast = t;
            }

            if (contactTime > tlast)
            {
                // Physically inconsistent times--the objects cannot
                // intersect.
                return true;
            }
        }
    }

    return false;
}

bool OBBox::Test(const OBBox& box0,const OBBox& box1)
{
    // Cutoff for cosine of angles between box axes.  This is used to catch
    // the cases when at least one pair of axes are parallel.  If this
    // happens, there is no need to test for separation along the
    // Cross(A[i],B[j]) directions.
    const float cutoff = 1.0f - EPSILON;
    bool existsParallelPair = false;
    int i;

    // Convenience variables.
    const Vector3* A = box0.axis;
	const Vector3* B = box1.axis;
	const float* EA = box0.extent;
	const float* EB = box1.extent;

    // Compute difference of box centers, D = C1-C0.
	Vector3 D = box1.center - box0.center;

    float C[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    float AbsC[3][3];  // |c_{ij}|
    float AD[3];       // Dot(A_i,D)
    float r0, r1, r;   // interval radii and distance between centers
    float r01;         // = R0 + R1

    // axis C0+t*A0
    for (i = 0; i < 3; ++i)
    {
        C[0][i] = A[0].Dot(B[i]);
        AbsC[0][i] = fabsf(C[0][i]);
        if (AbsC[0][i] > cutoff)
        {
            existsParallelPair = true;
        }
    }
    AD[0] = A[0].Dot(D);
    r = fabsf(AD[0]);
    r1 = EB[0]*AbsC[0][0] + EB[1]*AbsC[0][1] + EB[2]*AbsC[0][2];
    r01 = EA[0] + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A1
    for (i = 0; i < 3; ++i)
    {
        C[1][i] = A[1].Dot(B[i]);
        AbsC[1][i] = fabsf(C[1][i]);
        if (AbsC[1][i] > cutoff)
        {
            existsParallelPair = true;
        }
    }
    AD[1] = A[1].Dot(D);
    r = fabsf(AD[1]);
    r1 = EB[0]*AbsC[1][0] + EB[1]*AbsC[1][1] + EB[2]*AbsC[1][2];
    r01 = EA[1] + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A2
    for (i = 0; i < 3; ++i)
    {
        C[2][i] = A[2].Dot(B[i]);
        AbsC[2][i] = fabsf(C[2][i]);
        if (AbsC[2][i] > cutoff)
        {
            existsParallelPair = true;
        }
    }
    AD[2] = A[2].Dot(D);
    r = fabsf(AD[2]);
    r1 = EB[0]*AbsC[2][0] + EB[1]*AbsC[2][1] + EB[2]*AbsC[2][2];
    r01 = EA[2] + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*B0
    r = fabsf(B[0].Dot(D));
    r0 = EA[0]*AbsC[0][0] + EA[1]*AbsC[1][0] + EA[2]*AbsC[2][0];
    r01 = r0 + EB[0];
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*B1
    r = fabsf(B[1].Dot(D));
    r0 = EA[0]*AbsC[0][1] + EA[1]*AbsC[1][1] + EA[2]*AbsC[2][1];
    r01 = r0 + EB[1];
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*B2
    r = fabsf(B[2].Dot(D));
    r0 = EA[0]*AbsC[0][2] + EA[1]*AbsC[1][2] + EA[2]*AbsC[2][2];
    r01 = r0 + EB[2];
    if (r > r01)
    {
        return false;
    }

    // At least one pair of box axes was parallel, so the separation is
    // effectively in 2D where checking the "edge" normals is sufficient for
    // the separation of the boxes.
    if (existsParallelPair)
    {
        return true;
    }

    // axis C0+t*A0xB0
    r = fabsf(AD[2]*C[1][0] - AD[1]*C[2][0]);
    r0 = EA[1]*AbsC[2][0] + EA[2]*AbsC[1][0];
    r1 = EB[1]*AbsC[0][2] + EB[2]*AbsC[0][1];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A0xB1
    r = fabsf(AD[2]*C[1][1] - AD[1]*C[2][1]);
    r0 = EA[1]*AbsC[2][1] + EA[2]*AbsC[1][1];
    r1 = EB[0]*AbsC[0][2] + EB[2]*AbsC[0][0];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A0xB2
    r = fabsf(AD[2]*C[1][2] - AD[1]*C[2][2]);
    r0 = EA[1]*AbsC[2][2] + EA[2]*AbsC[1][2];
    r1 = EB[0]*AbsC[0][1] + EB[1]*AbsC[0][0];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A1xB0
    r = fabsf(AD[0]*C[2][0] - AD[2]*C[0][0]);
    r0 = EA[0]*AbsC[2][0] + EA[2]*AbsC[0][0];
    r1 = EB[1]*AbsC[1][2] + EB[2]*AbsC[1][1];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A1xB1
    r = fabsf(AD[0]*C[2][1] - AD[2]*C[0][1]);
    r0 = EA[0]*AbsC[2][1] + EA[2]*AbsC[0][1];
    r1 = EB[0]*AbsC[1][2] + EB[2]*AbsC[1][0];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A1xB2
    r = fabsf(AD[0]*C[2][2] - AD[2]*C[0][2]);
    r0 = EA[0]*AbsC[2][2] + EA[2]*AbsC[0][2];
    r1 = EB[0]*AbsC[1][1] + EB[1]*AbsC[1][0];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A2xB0
    r = fabsf(AD[1]*C[0][0] - AD[0]*C[1][0]);
    r0 = EA[0]*AbsC[1][0] + EA[1]*AbsC[0][0];
    r1 = EB[1]*AbsC[2][2] + EB[2]*AbsC[2][1];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A2xB1
    r = fabsf(AD[1]*C[0][1] - AD[0]*C[1][1]);
    r0 = EA[0]*AbsC[1][1] + EA[1]*AbsC[0][1];
    r1 = EB[0]*AbsC[2][2] + EB[2]*AbsC[2][0];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    // axis C0+t*A2xB2
    r = fabsf(AD[1]*C[0][2] - AD[0]*C[1][2]);
    r0 = EA[0]*AbsC[1][2] + EA[1]*AbsC[0][2];
    r1 = EB[0]*AbsC[2][1] + EB[1]*AbsC[2][0];
    r01 = r0 + r1;
    if (r > r01)
    {
        return false;
    }

    return true;
}

bool OBBox::Intersects(const OBBox& box0, const Vector3& velocity0, const OBBox& box1, const Vector3& velocity1, float tmax, float& contactTime)
{
	//if the boxes are stationary
    if (velocity0 == velocity1)
    {
        if (Test(box0, box1))
        {
            contactTime = 0;
            return true;
        }
        return false;
    }

    // Cutoff for cosine of angles between box axes.  This is used to catch
    // the cases when at least one pair of axes are parallel.  If this
    // happens, there is no need to include the cross-product axes for
    // separation.
    const float cutoff = 1.0f - EPSILON;
    bool existsParallelPair = false;

    // convenience variables
    const Vector3* A = box0.axis;
    const Vector3* B = box1.axis;
    const float* EA = box0.extent;
    const float* EB = box1.extent;
    Vector3 D = box1.center - box0.center;
    Vector3 W = velocity1 - velocity0;
    float C[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    float AbsC[3][3];  // |c_{ij}|
    float AD[3];       // Dot(A_i,D)
    float AW[3];       // Dot(A_i,W)
    float min0, max0, min1, max1, center, radius, speed;
    int i, j;

    contactTime = 0;
    float tlast = FLT_MAX;

    // axes C0+t*A[i]
    for (i = 0; i < 3; ++i)
    {
        for (j = 0; j < 3; ++j)
        {
            C[i][j] = A[i].Dot(B[j]);
            AbsC[i][j] = fabsf(C[i][j]);
            if (AbsC[i][j] > cutoff)
            {
                existsParallelPair = true;
            }
        }
        AD[i] = A[i].Dot(D);
        AW[i] = A[i].Dot(W);
        min0 = -EA[i];
        max0 = +EA[i];
        radius = EB[0]*AbsC[i][0] + EB[1]*AbsC[i][1] + EB[2]*AbsC[i][2];
        min1 = AD[i] - radius;
        max1 = AD[i] + radius;
        speed = AW[i];
        if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast, contactTime))
        {
            return false;
        }
    }

    // axes C0+t*B[i]
    for (i = 0; i < 3; ++i)
    {
        radius = EA[0]*AbsC[0][i] + EA[1]*AbsC[1][i] + EA[2]*AbsC[2][i];
        min0 = -radius;
        max0 = +radius;
        center = B[i].Dot(D);
        min1 = center - EB[i];
        max1 = center + EB[i];
        speed = W.Dot(B[i]);
        if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast, contactTime))
        {
            return false;
        }
    }

    // At least one pair of box axes was parallel, so the separation is
    // effectively in 2D where checking the "edge" normals is sufficient for
    // the separation of the boxes.
    if (existsParallelPair)
    {
        return true;
    }

    // axis C0+t*A0xB0
    radius = EA[1]*AbsC[2][0] + EA[2]*AbsC[1][0];
    min0 = -radius;
    max0 = +radius;
    center = AD[2]*C[1][0] - AD[1]*C[2][0];
    radius = EB[1]*AbsC[0][2] + EB[2]*AbsC[0][1];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[2]*C[1][0] - AW[1]*C[2][0];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast, contactTime))
    {
        return false;
    }

    // axis C0+t*A0xB1
    radius = EA[1]*AbsC[2][1] + EA[2]*AbsC[1][1];
    min0 = -radius;
    max0 = +radius;
    center = AD[2]*C[1][1] - AD[1]*C[2][1];
    radius = EB[0]*AbsC[0][2] + EB[2]*AbsC[0][0];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[2]*C[1][1] - AW[1]*C[2][1];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast, contactTime))
    {
        return false;
    }

    // axis C0+t*A0xB2
    radius = EA[1]*AbsC[2][2] + EA[2]*AbsC[1][2];
    min0 = -radius;
    max0 = +radius;
    center = AD[2]*C[1][2] - AD[1]*C[2][2];
    radius = EB[0]*AbsC[0][1] + EB[1]*AbsC[0][0];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[2]*C[1][2] - AW[1]*C[2][2];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast, contactTime))
    {
        return false;
    }

    // axis C0+t*A1xB0
    radius = EA[0]*AbsC[2][0] + EA[2]*AbsC[0][0];
    min0 = -radius;
    max0 = +radius;
    center = AD[0]*C[2][0] - AD[2]*C[0][0];
    radius = EB[1]*AbsC[1][2] + EB[2]*AbsC[1][1];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[0]*C[2][0] - AW[2]*C[0][0];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast, contactTime))
    {
        return false;
    }

    // axis C0+t*A1xB1
    radius = EA[0]*AbsC[2][1] + EA[2]*AbsC[0][1];
    min0 = -radius;
    max0 = +radius;
    center = AD[0]*C[2][1] - AD[2]*C[0][1];
    radius = EB[0]*AbsC[1][2] + EB[2]*AbsC[1][0];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[0]*C[2][1] - AW[2]*C[0][1];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast, contactTime))
    {
        return false;
    }

    // axis C0+t*A1xB2
    radius = EA[0]*AbsC[2][2] + EA[2]*AbsC[0][2];
    min0 = -radius;
    max0 = +radius;
    center = AD[0]*C[2][2] - AD[2]*C[0][2];
    radius = EB[0]*AbsC[1][1] + EB[1]*AbsC[1][0];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[0]*C[2][2] - AW[2]*C[0][2];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast, contactTime))
    {
        return false;
    }

    // axis C0+t*A2xB0
    radius = EA[0]*AbsC[1][0] + EA[1]*AbsC[0][0];
    min0 = -radius;
    max0 = +radius;
    center = AD[1]*C[0][0] - AD[0]*C[1][0];
    radius = EB[1]*AbsC[2][2] + EB[2]*AbsC[2][1];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[1]*C[0][0] - AW[0]*C[1][0];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast, contactTime))
    {
        return false;
    }

    // axis C0+t*A2xB1
    radius = EA[0]*AbsC[1][1] + EA[1]*AbsC[0][1];
    min0 = -radius;
    max0 = +radius;
    center = AD[1]*C[0][1] - AD[0]*C[1][1];
    radius = EB[0]*AbsC[2][2] + EB[2]*AbsC[2][0];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[1]*C[0][1] - AW[0]*C[1][1];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast, contactTime))
    {
        return false;
    }

    // axis C0+t*A2xB2
    radius = EA[0]*AbsC[1][2] + EA[1]*AbsC[0][2];
    min0 = -radius;
    max0 = +radius;
    center = AD[1]*C[0][2] - AD[0]*C[1][2];
    radius = EB[0]*AbsC[2][1] + EB[1]*AbsC[2][0];
    min1 = center - radius;
    max1 = center + radius;
    speed = AW[1]*C[0][2] - AW[0]*C[1][2];
    if (IsSeparated(min0, max0, min1, max1, speed, tmax, tlast, contactTime))
    {
        return false;
    }

    return true;
}

bool OBBox::Intersects(const Vector3 triangleVerts[3]) const
{
    float min0, max0, min1, max1;
    Vector3 D, edge[3];

    // Test direction of triangle normal.
    edge[0] = triangleVerts[1] - triangleVerts[0];
    edge[1] = triangleVerts[2] - triangleVerts[0];
    D = edge[0].Cross(edge[1]);
    min0 = D.Dot(triangleVerts[0]);
    max0 = min0;
	IntersectionAxis::GetProjection(D, *this, min1, max1);
    if (max1 < min0 || max0 < min1)
    {
        return false;
    }

    // Test direction of box faces.
    for (int i = 0; i < 3; ++i)
    {
        D = axis[i];
        IntersectionAxis::GetProjection(D, triangleVerts, min0, max0);
        float DdC = D.Dot(center);
        min1 = DdC - extent[i];
        max1 = DdC + extent[i];
        if (max1 < min0 || max0 < min1)
        {
            return false;
        }
    }

    // Test direction of triangle-box edge cross products.
    edge[2] = edge[1] - edge[0];
    for (int i0 = 0; i0 < 3; ++i0)
    {
        for (int i1 = 0; i1 < 3; ++i1)
        {
            D = edge[i0].Cross(axis[i1]);
            IntersectionAxis::GetProjection(D, triangleVerts, min0, max0);
			IntersectionAxis::GetProjection(D, *this, min1, max1);
            if (max1 < min0 || max0 < min1)
            {
                return false;
            }
        }
    }

    return true;
}

bool OBBox::Intersects (float tmax, const Vector3 tverts[3], const Vector3& velocity0, const Vector3& velocity1, float& contactTime) const
{
    // Process as if triangle is stationary, box is moving.
    Vector3 relVelocity = velocity1 - velocity0;
    contactTime = 0;
    float tlast = FLT_MAX;

    // Test direction of triangle normal.
    Vector3 edge[3] =
    {
        tverts[1] - tverts[0],
        tverts[2] - tverts[0],
        tverts[2] - tverts[1]
    };
    Vector3 D = edge[0].Cross(edge[1]);
    if (!IntersectionAxis::Test(D, tverts, *this, relVelocity, tmax,
        contactTime, tlast))
    {
        return false;
    }

    // Test direction of box faces.
    for (int i = 0; i < 3; ++i)
    {
        D = axis[i];
        if (!IntersectionAxis::Test(D, tverts, *this, relVelocity, tmax,
            contactTime, tlast))
        {
            return false;
        }
    }

    // Test direction of triangle-box edge cross products.
    for (int i0 = 0; i0 < 3; ++i0)
    {
        for (int i1 = 0; i1 < 3; ++i1)
        {
            D = edge[i0].Cross(axis[i1]);
            if (!IntersectionAxis::Test(D, tverts, *this, relVelocity,
                tmax, contactTime, tlast))
            {
                return false;
            }
        }
    }

    return true;
}

//bool IntrBox3Box3<Real>::Find (Real tmax, const Vector3<Real>& velocity0,
//    const Vector3<Real>& velocity1)
bool OBBox::Find(float tmax, const Vector3& velocity0,
    const Vector3& velocity1, const OBBox& box0, const OBBox& box1, int& quantity, float& contactTime, Vector3 point[8])
{
    quantity = 0;

    contactTime = 0;
    float tlast = FLT_MAX;

    // Relative velocity of box1 relative to box0.
    Vector3 relVelocity = velocity1 - velocity0;

    int i0, i1;
    int side = IntrConfiguration::NONE;
    IntrConfiguration box0Cfg, box1Cfg;
    Vector3 axis;

    // box 0 normals
    for (i0 = 0; i0 < 3; ++i0)
    {
        axis = box0.axis[i0];
        if (!IntersectionAxis::Find(axis, box0, box1, relVelocity, tmax,
            contactTime, tlast, side, box0Cfg, box1Cfg))
        {
            return false;
        }
    }

    // box 1 normals
    for (i1 = 0; i1 < 3; ++i1)
    {
        axis = box1.axis[i1];
        if (!IntersectionAxis::Find(axis, box0, box1, relVelocity, tmax,
            contactTime, tlast, side, box0Cfg, box1Cfg))
        {
            return false;
        }
    }

    // box 0 edges cross box 1 edges
    for (i0 = 0; i0 < 3; ++i0)
    {
        for (i1 = 0; i1 < 3; ++i1)
        {
            axis = box0.axis[i0].Cross(box1.axis[i1]);

            // Since all axes are unit length (assumed), then can just compare
            // against a constant (not relative) epsilon.
            if (axis.GetLengthSQ() <= EPSILON)
            {
                // Axis i0 and i1 are parallel.  If any two axes are parallel,
                // then the only comparisons that needed are between the faces
                // themselves.  At this time the faces have already been
                // tested, and without separation, so all further separation
                // tests will show only overlaps.
                FindContactSet(box0, box1, side, box0Cfg, box1Cfg,
                    velocity0, velocity1, contactTime, quantity, point);
                return true;
            }

            if (!IntersectionAxis::Find(axis, box0, box1, relVelocity,
                tmax, contactTime, tlast, side, box0Cfg, box1Cfg))
            {
                return false;
            }
        }
    }

    // velocity cross box 0 edges
    for (i0 = 0; i0 < 3; ++i0)
    {
        axis = relVelocity.Cross(box0.axis[i0]);
        if (!IntersectionAxis::Find(axis, box0, box1, relVelocity, tmax,
            contactTime, tlast, side, box0Cfg, box1Cfg))
        {
            return false;
        }
    }

    // velocity cross box 1 edges
    for (i1 = 0; i1 < 3; ++i1)
    {
        axis = relVelocity.Cross(box1.axis[i1]);
        if (!IntersectionAxis::Find(axis, box0, box1, relVelocity, tmax,
            contactTime, tlast, side, box0Cfg, box1Cfg))
        {
            return false;
        }
    }

    if (contactTime <= 0 || side == IntrConfiguration::NONE)
    {
        return false;
    }

    FindContactSet(box0, box1, side, box0Cfg, box1Cfg,
        velocity0, velocity1, contactTime, quantity, point);

    return true;
}

//Tip #1 - //http://www.gamedev.net/topic/429443-obb-ray-and-obb-plane-intersection/:
//What I prefer to use, is the standard ray-axis aligned box test. For an OBB, you
//can simply translate the test into an ABB test, by applying the inverse-transform
//of the box to the ray.
//Tip #2 - http://www.gamedev.net/topic/486342-finding-intersection-point-rayobb/:
//Transform the ray into the OBB's space (i.e., multiply the ray's origin and 
//direction by the inverse of the OBB's local-to-world transformation). Now you're
//back to Ray vs. AABB.
bool OBBox::Intersects(const WRay& worldSpaceRay, float& tmin, Vector3& q) const
{
	/*
	Matrix inverse_rotate = Matrix(D[0], D[1], D[2]).Transpose();
	Vector inverse_scale = Vector(1 / E.x, 1 / E.y, 1/ E.z)
	Vector inverse_translate = Vector(-P.x, -P.y, -P.z)
	
	V = ((V' + inverse_translate) * inverse_rotate).Scale(inverse_scale);
	*/

	WMatrix m;
	GetTransform(m);

	WMatrix inverseMat;
	m.Inverse(inverseMat);

	WRay localSpaceRay = worldSpaceRay;
	inverseMat.Mul(localSpaceRay.origin);
	inverseMat.Mul(localSpaceRay.dir);

	WBox b = *this;
	if (b.Intersects(localSpaceRay, tmin, q))
	{
		//Transform hit coord to world space (because our ray was in local space)
		m.Mul(q);
		return true;
	}

	return false;
}

void OBBox::SetTransformUnscaled(const WMatrix16f& m)
{
	center = m.GetTranslation();
	m.GetAxes(axis[0], axis[1], axis[2]);
}

void OBBox::TransformBy(const WMatrix16f& m)
{
	center += m.GetTranslation();

	/*
	WMatrix mat(1);
	mat.SetX(axis[0]);
	mat.SetY(axis[1]);
	mat.SetZ(axis[2]);
	WMatrix final = mat * m;
	final.GetAxes(axis[0], axis[1], axis[2]);
	*/
	

	WMatrix mat = m;
	mat.SetTranslation(ZERO_VECTOR);
	mat.Mul(axis[0]);
	mat.Mul(axis[1]);
	mat.Mul(axis[2]);
	//mat.Mul(center);

}

void OBBox::MakeIdentity(void)
{
	axis[0] = Vector3(1,0,0);
	axis[1] = Vector3(0,1,0);
	axis[2] = Vector3(0,0,1);
}

OBBox::operator WBox() const
{
	Vector3 p[8];
	GetAABBPoints(p);
	return WBox(p, 8);
}

OBBox::operator WSphere() const
{
	WSphere s;
	s.center = center;
	s.radius = extent.GetLength() * 0.5f;
	return s;
}

void OBBox::GetTransform(WMatrix& m) const
{
	WMatrix rot(1);
	rot.SetX(axis[0]);
	rot.SetY(axis[1]);
	rot.SetZ(axis[2]);

	WMatrix16f scaleMat(1);
	scaleMat.SetScale(extent);

	WMatrix tm(1);
	tm.SetTranslation(center);

	//scale * rotation
	WMatrix16f sr;
	WMatrixMul(scaleMat, rot, sr);

	//combine scale / rotation / translation matrices
	WMatrixMul(sr,tm,m);
}

void OBBox::GetAABBPoints(Vector3 vertex[8]) const
{
    Vector3 extAxis0 = extent[0]*axis[0];
    Vector3 extAxis1 = extent[1]*axis[1];
    Vector3 extAxis2 = extent[2]*axis[2];

    vertex[0] = center - extAxis0 - extAxis1 - extAxis2;
    vertex[1] = center + extAxis0 - extAxis1 - extAxis2;
    vertex[2] = center + extAxis0 + extAxis1 - extAxis2;
    vertex[3] = center - extAxis0 + extAxis1 - extAxis2;
    vertex[4] = center - extAxis0 - extAxis1 + extAxis2;
    vertex[5] = center + extAxis0 - extAxis1 + extAxis2;
    vertex[6] = center + extAxis0 + extAxis1 + extAxis2;
    vertex[7] = center - extAxis0 + extAxis1 + extAxis2;
}

/*
* reference: http://www.gamedev.net/topic/551816-finding-the-aabb-surface-normal-from-an-intersection-point-on-aabb/
* Do you mean a function that, given an AABB and a point on the AABB (and no 
* other information), returns the normal corresponding to that point?
* If so, I think such a function should be fairly easy to construct. You might 
* start by transforming the query point to the AABB's local space by subtracting the
* AABB center from the point. You can then identify which 'slab' the point corresponds
* to by examining the elements of the transformed point in turn; the element whose magnitude
* is closest to the box extent in that direction will correspond to the slab. Then, the
* sign of the element will give you the side of the slab, and you have your normal.
* (I haven't actually tried this method, but it seems correct.)
*/
Vector3 OBBox::GetNormalFromPoint(const Vector3& cpoint) const
{
	Vector3 normal(0,0,0);

	//make local copy
	Vector3 point = cpoint;
	point -= center;

	float bestDist = FLT_MAX;
	for (int i = 0; i < 3; ++i)
	{
		Vector3 extAxis = extent[i]*axis[i];

		//Test the negative axis (this way we account for all 6 slabs of the box)

		float distance = ( extAxis - point[i] ).GetLengthSQ();
		if (distance < bestDist) 
		{
			bestDist = distance;

			//We use the sign so that we can account for all 6 axis of the box
			normal = sign(point[i]) * axis[i];
		}
	}

	//Test the negative axis (this way we account for all 6 slabs of the box)
	for (int i = 0; i < 3; ++i)
	{
		Vector3 extAxis = extent[i]*axis[i];
		extAxis.Invert();

		float distance = ( extAxis - point[i] ).GetLengthSQ();
		if (distance < bestDist) 
		{
			bestDist = distance;

			//We use the sign so that we can account for all 6 axis of the box
			normal = sign(point[i]) * axis[i];
		}
	}

	normal.Normalize();
	return normal;
}


}