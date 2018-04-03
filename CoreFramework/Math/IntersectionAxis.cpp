
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt

#include "IntersectionAxis.h"
#include "WGeometry.h"
#include "OBBox.h"
#include <float.h>

namespace GODZ
{

//See IntrAxis<Real>::GetProjection() in wildmagic5.1
void IntersectionAxis::GetProjection (const Vector3& axis, const OBBox& box, float& imin, float& imax)
{
    float origin = axis.Dot(box.center);
    float maximumExtent =
        fabsf(box.extent[0]*axis.Dot(box.axis[0])) +
        fabsf(box.extent[1]*axis.Dot(box.axis[1])) +
        fabsf(box.extent[2]*axis.Dot(box.axis[2]));

    imin = origin - maximumExtent;
    imax = origin + maximumExtent;
}


//See IntrAxis<Real>::GetProjection() in wildmagic5.1
void IntersectionAxis::GetProjection (const Vector3& axis, const Vector3 verts[3], float& imin, float& imax)
{
    float dot[3] =
    {
        axis.Dot(verts[0]),
        axis.Dot(verts[1]),
        axis.Dot(verts[2])
    };

    imin = dot[0];
    imax = imin;

    if (dot[1] < imin)
    {
        imin = dot[1];
    }
    else if (dot[1] > imax)
    {
        imax = dot[1];
    }

    if (dot[2] < imin)
    {
        imin = dot[2];
    }
    else if (dot[2] > imax)
    {
        imax = dot[2];
    }
}

//See IntrAxis<Real>::Test() in wildmagic5.1
bool IntersectionAxis::Test (const Vector3& axis, const Vector3 triangle[3], const OBBox& box, const Vector3& velocity,
			float tmax, float& tfirst, float& tlast)
{
    float min0, max0;
    GetProjection(axis, triangle, min0, max0);

    float min1, max1;
    GetProjection(axis, box, min1, max1);
    
    return Test(axis, velocity, min0, max0, min1, max1, tmax, tfirst, tlast);
}

bool IntersectionAxis::Test (const Vector3& axis, const Vector3& velocity, float min0, float max0, float min1,
		float max1, float tmax, float& tfirst, float& tlast)
{
    // Static separating axis test.  Returns false iff object0 and object1
    // do not intersect in the interval [0,TMax] on any separating axis
    // ( TFirst > TLast || TFirst > TMax ) during the time interval, that is,
    // a quick out.  Returns true otherwise.
    //
    // min0, max0, min1, and max1 are the minimal and maximal points of
    // whatever object object0 and object1 are projected onto the test axis.
    //
    // velocity is Velocity1 - Velocity0

    float t;
    float speed = axis.Dot(velocity);
    
    if (max1 < min0) // object1 on left of object0
    {
        if (speed <= 0) // object1 moving away from object0
        {
            return false;
        }

        // find first time of contact on this axis
        t = (min0 - max1)/speed;
        if (t > tfirst)
        {
            tfirst = t;
        }

        // quick out: intersection after desired interval
        if (tfirst > tmax)
        {
            return false;   
        }

        // find last time of contact on this axis
        t = (max0 - min1)/speed;
        if (t < tlast)
        {
            tlast = t;
        }

        // quick out: intersection before desired interval
        if (tfirst > tlast)
        {
            return false; 
        }
    }
    else if (max0 < min1)   // object1 on right of object0
    {
        if (speed >= 0) // object1 moving away from object0
        {
            return false;
        }

        // find first time of contact on this axis
        t = (max0 - min1)/speed;
        if (t > tfirst)
        {
            tfirst = t;
        }

        // quick out: intersection after desired interval
        if (tfirst > tmax)
        {
            return false;   
        }

        // find last time of contact on this axis
        t = (min0 - max1)/speed;
        if (t < tlast)
        {
            tlast = t;
        }

        // quick out: intersection before desired interval
        if (tfirst > tlast)
        {
            return false; 
        }

    }
    else // object1 and object0 on overlapping interval
    {
        if (speed > 0)
        {
            // find last time of contact on this axis
            t = (max0 - min1)/speed;
            if (t < tlast)
            {
                tlast = t;
            }

            // quick out: intersection before desired interval
            if (tfirst > tlast)
            {
                return false; 
            }
        }
        else if (speed < 0)
        {
            // find last time of contact on this axis
            t = (min0 - max1)/speed;
            if (t < tlast)
            {
                tlast = t;
            }

            // quick out: intersection before desired interval
            if (tfirst > tlast)
            {
                return false;
            }
        }
    }
    return true;
}

//----------------------------------------------------------------------------
bool IntersectionAxis::Find (const Vector3& axis,
    const OBBox& box0, const OBBox& box1,
    const Vector3& velocity, float tmax, float& tfirst,
    float& tlast, int& side, IntrConfiguration& box0CfgFinal,
    IntrConfiguration& box1CfgFinal)
{
    IntrConfiguration box0CfgStart;
    GetConfiguration(axis,box0,box0CfgStart);

    IntrConfiguration box1CfgStart;
    GetConfiguration(axis,box1,box1CfgStart);

    return Find(axis, velocity, box0CfgStart, box1CfgStart, tmax, side,
        box0CfgFinal, box1CfgFinal, tfirst, tlast);
}

//----------------------------------------------------------------------------
bool IntersectionAxis::Find (const Vector3& axis,
    const Vector3& velocity, const IntrConfiguration& cfg0Start,
    const IntrConfiguration& cfg1Start, float tmax, int& side,
    IntrConfiguration& cfg0Final, IntrConfiguration& cfg1Final,
    float& tfirst, float& tlast)
{
    // Constant velocity separating axis test.  The configurations cfg0Start
    // and cfg1Start are the current potential configurations for contact,
    // and cfg0Final and cfg1Final are improved configurations.

    float t;
    float speed = axis.Dot(velocity);

    if (cfg1Start.mMax < cfg0Start.mMin) // object1 left of object0
    {
        if (speed <= 0) // object1 moving away from object0
        {
            return false;
        }

        // find first time of contact on this axis
        t = (cfg0Start.mMin - cfg1Start.mMax)/speed;

        // If this is the new maximum first time of contact, set side and
        // configuration.
        if (t > tfirst)
        {
            tfirst = t;
            side = IntrConfiguration::LEFT;
            cfg0Final = cfg0Start;
            cfg1Final = cfg1Start;
        }

        // quick out: intersection after desired interval
        if (tfirst > tmax)
        {
            return false;
        }

        // find last time of contact on this axis
        t = (cfg0Start.mMax - cfg1Start.mMin)/speed;
        if (t < tlast)
        {
            tlast = t;
        }

        // quick out: intersection before desired interval
        if (tfirst > tlast)
        {
            return false;
        }
    }
    else if (cfg0Start.mMax < cfg1Start.mMin)  // obj1 right of obj0
    {
        if (speed >= 0) // object1 moving away from object0
        {
            return false;
        }

        // find first time of contact on this axis
        t = (cfg0Start.mMax - cfg1Start.mMin)/speed;

        // If this is the new maximum first time of contact,  set side and
        // configuration.
        if (t > tfirst)
        {
            tfirst = t;
            side = IntrConfiguration::RIGHT;
            cfg0Final = cfg0Start;
            cfg1Final = cfg1Start;
        }

        // quick out: intersection after desired interval
        if (tfirst > tmax)
        {
            return false;   
        }

        // find last time of contact on this axis
        t = (cfg0Start.mMin - cfg1Start.mMax)/speed;
        if (t < tlast)
        {
            tlast = t;
        }

        // quick out: intersection before desired interval
        if (tfirst > tlast)
        {
            return false;
        }
    }
    else // object1 and object0 on overlapping interval
    {
        if (speed > 0)
        {
            // find last time of contact on this axis
            t = (cfg0Start.mMax - cfg1Start.mMin)/speed;
            if (t < tlast)
            {
                tlast = t;
            }

            // quick out: intersection before desired interval
            if (tfirst > tlast)
            {
                return false; 
            }
        }
        else if (speed < 0)
        {
            // find last time of contact on this axis
            t = (cfg0Start.mMin - cfg1Start.mMax)/speed;
            if (t < tlast)
            {
                tlast = t;
            }

            // quick out: intersection before desired interval
            if (tfirst > tlast)
            {
                return false; 
            }
        }
    }
    return true;
}

//----------------------------------------------------------------------------
void IntersectionAxis::GetConfiguration (const Vector3& axis, 
    const OBBox& box, IntrConfiguration& cfg)
{
    // Description of coordinate ordering scheme for IntrConfiguration.mIndex.
    //
    // Vertex number (up/down) vs. sign of extent (only matters in mapping
    // back)
    //   012
    // 0 ---
    // 1 +--
    // 2 -+-
    // 3 ++-
    // 4 --+
    // 5 +-+
    // 6 -++
    // 7 +++
    //
    // When it returns an ordering in the IntrConfiguration, it is also
    // guarenteed to be in-order (if 4 vertices, then they are guarenteed in
    // an order that will create a box, e.g. 0,1,3,2).

    float axes[3] =
    {
        axis.Dot(box.axis[0]),
        axis.Dot(box.axis[1]),
        axis.Dot(box.axis[2])
    };

    float absAxes[3] =
    {
        fabsf(axes[0]),
        fabsf(axes[1]),
        fabsf(axes[2])
    };

    float maxProjectedExtent;

    if (absAxes[0] < EPSILON)
    {
        if (absAxes[1] < EPSILON)
        {
            // face-face
            cfg.mMap = IntrConfiguration::m44;

            maxProjectedExtent = absAxes[2]*box.extent[2];

            // faces have normals along axis[2]
            if (axes[2] > (float)0)
            {       
                cfg.mIndex[0] = 0;
                cfg.mIndex[1] = 1;
                cfg.mIndex[2] = 3;
                cfg.mIndex[3] = 2;

                cfg.mIndex[4] = 6;
                cfg.mIndex[5] = 7;
                cfg.mIndex[6] = 5;
                cfg.mIndex[7] = 4;
            }
            else
            {
                cfg.mIndex[0] = 6;
                cfg.mIndex[1] = 7;
                cfg.mIndex[2] = 5;
                cfg.mIndex[3] = 4;

                cfg.mIndex[4] = 0;
                cfg.mIndex[5] = 1;
                cfg.mIndex[6] = 3;
                cfg.mIndex[7] = 2;
            }
        }
        else if (absAxes[2] < EPSILON)
        {
            // face-face
            cfg.mMap = IntrConfiguration::m44;

            maxProjectedExtent = absAxes[1]*box.extent[1];

            // faces have normals along axis[1]
            if (axes[1] > (float)0) 
            {
                cfg.mIndex[0] = 4;
                cfg.mIndex[1] = 5;
                cfg.mIndex[2] = 1;
                cfg.mIndex[3] = 0;

                cfg.mIndex[4] = 2;
                cfg.mIndex[5] = 3;
                cfg.mIndex[6] = 7;
                cfg.mIndex[7] = 6;
            }
            else
            {
                cfg.mIndex[0] = 2;
                cfg.mIndex[1] = 3;
                cfg.mIndex[2] = 7;
                cfg.mIndex[3] = 6;

                cfg.mIndex[4] = 4;
                cfg.mIndex[5] = 5;
                cfg.mIndex[6] = 1;
                cfg.mIndex[7] = 0;
            }
        }
        else // only axes[0] is equal to 0
        {
            // seg-seg
            cfg.mMap = IntrConfiguration::m2_2;

            maxProjectedExtent = absAxes[1]*box.extent[1] +
                absAxes[2]*box.extent[2];

            // axis 0 is perpendicular to axis
            if (axes[1] > (float)0)
            {
                if (axes[2] > (float)0) 
                {
                    cfg.mIndex[0] = 0;
                    cfg.mIndex[1] = 1;

                    cfg.mIndex[6] = 6;
                    cfg.mIndex[7] = 7;
                }
                else 
                {
                    cfg.mIndex[0] = 4;
                    cfg.mIndex[1] = 5;

                    cfg.mIndex[6] = 2;
                    cfg.mIndex[7] = 3;
                }
            }
            else // axes[1] < 0
            {
                if (axes[2] > (float)0)
                {
                    cfg.mIndex[0] = 2;
                    cfg.mIndex[1] = 3;

                    cfg.mIndex[6] = 4;
                    cfg.mIndex[7] = 5;
                }
                else
                {
                    cfg.mIndex[0] = 6;
                    cfg.mIndex[1] = 7;

                    cfg.mIndex[6] = 0;
                    cfg.mIndex[7] = 1;
                }
            }
        }
    }
    else if (absAxes[1] < EPSILON)
    {
        if (absAxes[2] < EPSILON)
        {
            // face-face
            cfg.mMap = IntrConfiguration::m44;

            maxProjectedExtent = absAxes[0]*box.extent[0];

            // faces have normals along axis[0]
            if (axes[0] > (float)0)
            {
                cfg.mIndex[0] = 0;
                cfg.mIndex[1] = 2;
                cfg.mIndex[2] = 6;
                cfg.mIndex[3] = 4;

                cfg.mIndex[4] = 5;
                cfg.mIndex[5] = 7;
                cfg.mIndex[6] = 3;
                cfg.mIndex[7] = 1;
            }
            else
            {
                cfg.mIndex[4] = 0;
                cfg.mIndex[5] = 2;
                cfg.mIndex[6] = 6;
                cfg.mIndex[7] = 4;

                cfg.mIndex[0] = 5;
                cfg.mIndex[1] = 7;
                cfg.mIndex[2] = 3;
                cfg.mIndex[3] = 1;
            }

        }
        else // only axes[1] is equal to 0
        {
            // seg-seg
            cfg.mMap = IntrConfiguration::m2_2;

            maxProjectedExtent = absAxes[0]*box.extent[0] + 
                absAxes[2]*box.extent[2];

            // axis 1 is perpendicular to axis
            if (axes[0] > (float)0)
            {
                if (axes[2] > (float)0) 
                {
                    cfg.mIndex[0] = 0;
                    cfg.mIndex[1] = 2;

                    cfg.mIndex[6] = 5;
                    cfg.mIndex[7] = 7;
                }
                else 
                {
                    cfg.mIndex[0] = 4;
                    cfg.mIndex[1] = 6;

                    cfg.mIndex[6] = 1;
                    cfg.mIndex[7] = 3;
                }
            }
            else // axes[0] < 0
            {
                if (axes[2] > (float)0)
                {
                    cfg.mIndex[0] = 1;
                    cfg.mIndex[1] = 3;

                    cfg.mIndex[6] = 4;
                    cfg.mIndex[7] = 6;
                }
                else
                {
                    cfg.mIndex[0] = 5;
                    cfg.mIndex[1] = 7;

                    cfg.mIndex[6] = 0;
                    cfg.mIndex[7] = 2;
                }
            }
        }
    }
    
    else if (absAxes[2] < EPSILON)
    {
        // only axis2 less than zero
        // seg-seg
        cfg.mMap = IntrConfiguration::m2_2;

        maxProjectedExtent = absAxes[0]*box.extent[0] +
            absAxes[1]*box.extent[1];

        // axis 2 is perpendicular to axis
        if (axes[0] > (float)0)
        {
            if (axes[1] > (float)0) 
            {
                cfg.mIndex[0] = 0;
                cfg.mIndex[1] = 4;

                cfg.mIndex[6] = 3;
                cfg.mIndex[7] = 7;
            }
            else 
            {
                cfg.mIndex[0] = 2;
                cfg.mIndex[1] = 6;

                cfg.mIndex[6] = 1;
                cfg.mIndex[7] = 5;
            }
        }
        else // axes[0] < 0
        {
            if (axes[1] > (float)0)
            {
                cfg.mIndex[0] = 1;
                cfg.mIndex[1] = 5;

                cfg.mIndex[6] = 2;
                cfg.mIndex[7] = 6;
            }
            else
            {
                cfg.mIndex[0] = 3;
                cfg.mIndex[1] = 7;

                cfg.mIndex[6] = 0;
                cfg.mIndex[7] = 4;
            }
        }
    }
  
    else // no axis is equal to zero
    {
        // point-point (unique maximal and minimal vertex)
        cfg.mMap = IntrConfiguration::m1_1;

        maxProjectedExtent = absAxes[0]*box.extent[0] +
            absAxes[1]*box.extent[1] + absAxes[2]*box.extent[2];

        // only these two vertices matter, the rest are irrelevant
        cfg.mIndex[0] = 
            (axes[0] > (float)0.0 ? 0 : 1) + 
            (axes[1] > (float)0.0 ? 0 : 2) + 
            (axes[2] > (float)0.0 ? 0 : 4);
        // by ordering the vertices this way, opposite corners add up to 7
        cfg.mIndex[7] = 7 - cfg.mIndex[0];
    }

    // Find projections onto line
    float origin = axis.Dot(box.center);
    cfg.mMin = origin - maxProjectedExtent;
    cfg.mMax = origin + maxProjectedExtent;
}


//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
FindContactSet::FindContactSet (const OBBox& box0,
    const OBBox& box1, int side, const IntrConfiguration& box0Cfg,
    const IntrConfiguration& box1Cfg, const Vector3& box0Velocity,
    const Vector3& box1Velocity, float tfirst, int& quantity,
    Vector3* P)
{
    // Move the boxes to their new positions.
    OBBox box0Final, box1Final;
    box0Final.center = box0.center + tfirst*box0Velocity;
    box1Final.center = box1.center + tfirst*box1Velocity;
    for (int i = 0; i < 3; ++i)
    {
        box0Final.axis[i] = box0.axis[i];
        box0Final.extent[i] = box0.extent[i];
        box1Final.axis[i] = box1.axis[i];
        box1Final.extent[i] = box1.extent[i];
    }

    const int* b0Index = box0Cfg.mIndex;
    const int* b1Index = box1Cfg.mIndex;

    if (side == IntrConfiguration::LEFT)
    {
        // box1 on left of box0
        if (box0Cfg.mMap == IntrConfiguration::m1_1)
        {
            quantity = 1;
            P[0] = GetPointFromIndex(b0Index[0], box0Final);
        }
        else if (box1Cfg.mMap == IntrConfiguration::m1_1)
        {
            quantity = 1;
            P[0] = GetPointFromIndex(b1Index[7], box1Final);
        }
        else if (box0Cfg.mMap == IntrConfiguration::m2_2)
        {
            if (box1Cfg.mMap == IntrConfiguration::m2_2)
            {
                // box0edge-box1edge intersection
                Vector3 edge0[2], edge1[2];
                edge0[0] = GetPointFromIndex(b0Index[0], box0Final);
                edge0[1] = GetPointFromIndex(b0Index[1], box0Final);
                edge1[0] = GetPointFromIndex(b1Index[6], box1Final);
                edge1[1] = GetPointFromIndex(b1Index[7], box1Final);
                SegmentSegment(edge0, edge1, quantity, P);
            }
            else // box1Cfg.mMap == IntrConfiguration::m44
            {
                // box0edge-box1face intersection
                Vector3 edge0[2], face1[4];
                edge0[0] = GetPointFromIndex(b0Index[0], box0Final);
                edge0[1] = GetPointFromIndex(b0Index[1], box0Final); 
                face1[0] = GetPointFromIndex(b1Index[4], box1Final); 
                face1[1] = GetPointFromIndex(b1Index[5], box1Final); 
                face1[2] = GetPointFromIndex(b1Index[6], box1Final); 
                face1[3] = GetPointFromIndex(b1Index[7], box1Final); 
                CoplanarSegmentRectangle(edge0, face1, quantity, P);
            }
        }
        else // box0Cfg.mMap == IntrConfiguration::m44
        {
            if (box1Cfg.mMap == IntrConfiguration::m2_2)
            {
                // box0face-box1edge intersection
                Vector3 face0[4], edge1[2];
                face0[0] = GetPointFromIndex(b0Index[0], box0Final);
                face0[1] = GetPointFromIndex(b0Index[1], box0Final);
                face0[2] = GetPointFromIndex(b0Index[2], box0Final);
                face0[3] = GetPointFromIndex(b0Index[3], box0Final);
                edge1[0] = GetPointFromIndex(b1Index[6], box1Final);
                edge1[1] = GetPointFromIndex(b1Index[7], box1Final);
                CoplanarSegmentRectangle(edge1, face0, quantity, P);
            }
            else
            {
                // box0face-box1face intersection
                Vector3 face0[4], face1[4];
                face0[0] = GetPointFromIndex(b0Index[0], box0Final);
                face0[1] = GetPointFromIndex(b0Index[1], box0Final);
                face0[2] = GetPointFromIndex(b0Index[2], box0Final);
                face0[3] = GetPointFromIndex(b0Index[3], box0Final);
                face1[0] = GetPointFromIndex(b1Index[4], box1Final);
                face1[1] = GetPointFromIndex(b1Index[5], box1Final);
                face1[2] = GetPointFromIndex(b1Index[6], box1Final);
                face1[3] = GetPointFromIndex(b1Index[7], box1Final);
                CoplanarRectangleRectangle(face0, face1, quantity, P);
            }
        }
    }
    else // side == RIGHT 
    {
        // box1 on right of box0
        if (box0Cfg.mMap == IntrConfiguration::m1_1)
        {
            quantity = 1;
            P[0] = GetPointFromIndex(b0Index[7], box0Final);
        }
        else if (box1Cfg.mMap == IntrConfiguration::m1_1)
        {
            quantity = 1;
            P[0] = GetPointFromIndex(b0Index[0], box1Final);
        }
        else if (box0Cfg.mMap == IntrConfiguration::m2_2)
        {
            if (box1Cfg.mMap == IntrConfiguration::m2_2)
            {
                // box0edge-box1edge intersection
                Vector3 edge0[2], edge1[2];
                edge0[0] = GetPointFromIndex(b0Index[6], box0Final);
                edge0[1] = GetPointFromIndex(b0Index[7], box0Final);
                edge1[0] = GetPointFromIndex(b1Index[0], box1Final);
                edge1[1] = GetPointFromIndex(b1Index[1], box1Final);
                SegmentSegment(edge0,edge1,quantity,P);
            }
            else // box1Cfg.mMap == IntrConfiguration::m44
            {
                // box0edge-box1face intersection
                Vector3 edge0[2], face1[4];
                edge0[0] = GetPointFromIndex(b0Index[6], box0Final);
                edge0[1] = GetPointFromIndex(b0Index[7], box0Final);
                face1[0] = GetPointFromIndex(b1Index[0], box1Final);
                face1[1] = GetPointFromIndex(b1Index[1], box1Final);
                face1[2] = GetPointFromIndex(b1Index[2], box1Final);
                face1[3] = GetPointFromIndex(b1Index[3], box1Final);
                CoplanarSegmentRectangle(edge0, face1, quantity, P);
            }
        }
        else // box0Cfg.mMap == IntrConfiguration::m44
        {
            if (box1Cfg.mMap == IntrConfiguration::m2_2)
            {
                // box0face-box1edge intersection
                Vector3 face0[4], edge1[2];
                face0[0] = GetPointFromIndex(b0Index[4], box0Final);
                face0[1] = GetPointFromIndex(b0Index[5], box0Final);
                face0[2] = GetPointFromIndex(b0Index[6], box0Final);
                face0[3] = GetPointFromIndex(b0Index[7], box0Final);
                edge1[0] = GetPointFromIndex(b1Index[0], box1Final);
                edge1[1] = GetPointFromIndex(b1Index[1], box1Final);
                CoplanarSegmentRectangle(edge1, face0, quantity, P);
            }
            else // box1Cfg.mMap == IntrConfiguration::m44
            {
                // box0face-box1face intersection
                Vector3 face0[4], face1[4];
                face0[0] = GetPointFromIndex(b0Index[4], box0Final);
                face0[1] = GetPointFromIndex(b0Index[5], box0Final);
                face0[2] = GetPointFromIndex(b0Index[6], box0Final);
                face0[3] = GetPointFromIndex(b0Index[7], box0Final);
                face1[0] = GetPointFromIndex(b1Index[0], box1Final);
                face1[1] = GetPointFromIndex(b1Index[1], box1Final);
                face1[2] = GetPointFromIndex(b1Index[2], box1Final);
                face1[3] = GetPointFromIndex(b1Index[3], box1Final);
                CoplanarRectangleRectangle(face0, face1, quantity, P);
            }
        }
    }
}
//----------------------------------------------------------------------------
void FindContactSet::ColinearSegments (const Vector3 segment0[2],
    const Vector3 segment1[2], int& quantity, Vector3* P)
{
    // The potential intersection is initialized to segment0 and clipped
    // against segment1.
    quantity = 2;
    for (int i = 0; i < 2; ++i)
    {
        P[i] = segment0[i];
    }

    // point 0
    Vector3 V = segment1[1] - segment1[0];
    float c = V.Dot(segment1[0]);
    ClipConvexPolygonAgainstPlane(V, c, quantity, P);

    // point 1
	//V = -V; //changed///
	V.Invert();
    c = V.Dot(segment1[1]);
    ClipConvexPolygonAgainstPlane(V, c, quantity, P);
}
//----------------------------------------------------------------------------
void FindContactSet::SegmentThroughPlane (
    const Vector3 segment[2], const Vector3& planeOrigin,
    const Vector3& planeNormal, int& quantity, Vector3* P)
{
    quantity = 1;

    float u = planeNormal.Dot(planeOrigin);
    float v0 = planeNormal.Dot(segment[0]);
    float v1 = planeNormal.Dot(segment[1]);

    // Now that there it has been reduced to a 1-dimensional problem via
    // projection, it becomes easy to find the ratio along V that V 
    // intersects with U.
    float ratio = (u - v0)/(v1 - v0);
    P[0] = segment[0] + ratio*(segment[1] - segment[0]);
}
//----------------------------------------------------------------------------
void FindContactSet::SegmentSegment (const Vector3 segment0[2], 
    const Vector3 segment1[2], int& quantity, Vector3* P)
{
    Vector3 dir0 = segment0[1] - segment0[0];
    Vector3 dir1 = segment1[1] - segment1[0];
    Vector3 normal = dir0.Cross(dir1);

    // The comparison is sin(kDir0,kDir1) < epsilon.
	float sqrLen0 = dir0.GetLengthSQ();
    float sqrLen1 = dir1.GetLengthSQ();
    float sqrLenN = normal.GetLengthSQ();
    if (sqrLenN < EPSILON*sqrLen0*sqrLen1)
    {
        ColinearSegments(segment0, segment1, quantity, P);
    }
    else
    {
        SegmentThroughPlane(segment1, segment0[0],
            normal.Cross(segment0[1]-segment0[0]), quantity, P);
    }
}
//----------------------------------------------------------------------------

void FindContactSet::ColinearSegmentTriangle (
    const Vector3 segment[2], const Vector3 triangle[3],
    int& quantity, Vector3* P)
{
    // The potential intersection is initialized to the line segment and then
    // clipped against the three sides of the tri
    quantity = 2;
    int i;
    for (i = 0; i < 2; ++i)
    {
        P[i] = segment[i];
    }

    Vector3 side[3] =
    {
        triangle[1] - triangle[0],
        triangle[2] - triangle[1],
        triangle[0] - triangle[2]
    };

    Vector3 normal = side[0].Cross(side[1]);
    for (i = 0; i < 3; ++i)
    {
        // Normal pointing inside the triangle.
        Vector3 sideN = normal.Cross(side[i]);
        float constant = sideN.Dot(triangle[i]);
        ClipConvexPolygonAgainstPlane(sideN, constant, quantity, P);
    }
}
//----------------------------------------------------------------------------

void FindContactSet::CoplanarSegmentRectangle (
    const Vector3 segment[2], const Vector3 rectangle[4],
    int& quantity, Vector3* P)
{
    // The potential intersection is initialized to the line segment and then
    // clipped against the four sides of the rect
    quantity = 2;
    for (int i = 0; i < 2; ++i)
    {
        P[i] = segment[i];
    }

    for (int i0 = 3, i1 = 0; i1 < 4; i0 = i1++)
    {
        Vector3 normal = rectangle[i1] - rectangle[i0];
        float constant = normal.Dot(rectangle[i0]);
        ClipConvexPolygonAgainstPlane(normal, constant, quantity, P);
    }
}
//----------------------------------------------------------------------------

void FindContactSet::CoplanarTriangleRectangle (
    const Vector3 triangle[3], const Vector3 rectangle[4],
    int& quantity, Vector3* P)
{
    // The potential intersection is initialized to the triangle, and then
    // clipped against the sides of the box
    quantity = 3;
    for (int i = 0; i < 3; ++i)
    {
        P[i] = triangle[i];
    }

    for (int i0 = 3, i1 = 0; i1 < 4; i0 = i1++)
    {
        Vector3 normal = rectangle[i1] - rectangle[i0];
        float constant = normal.Dot(rectangle[i0]);
        ClipConvexPolygonAgainstPlane(normal, constant, quantity, P);
    }
}
//----------------------------------------------------------------------------

void FindContactSet::CoplanarRectangleRectangle (
    const Vector3 rectangle0[4], const Vector3 rectangle1[4],
    int& quantity, Vector3* P)
{
    // The potential intersection is initialized to face 0, and then clipped
    // against the four sides of face 1.
    quantity = 4;
    for (int i = 0; i < 4; ++i)
    {
        P[i] = rectangle0[i];
    }

    for (int i0 = 3, i1 = 0; i1 < 4; i0 = i1++)
    {
        Vector3 normal = rectangle1[i1] - rectangle1[i0];
        float constant = normal.Dot(rectangle1[i0]);
        ClipConvexPolygonAgainstPlane(normal, constant, quantity, P);
    }
}

//----------------------------------------------------------------------------
void ClipConvexPolygonAgainstPlane (const Vector3& normal,
    float constant, int& quantity, Vector3* P)
{
    // The input vertices are assumed to be in counterclockwise order.  The
    // ordering is an invariant of this function.  The size of array P is
    // assumed to be large enough to store the clipped polygon vertices.

    // test on which side of line are the vertices
    int positive = 0, negative = 0, pIndex = -1;
    int currQuantity = quantity;

    float test[8];
    int i;
    for (i = 0; i < quantity; ++i)
    {

        // An epsilon is used here because it is possible for the dot product
        // and 'constant' to be exactly equal to each other (in theory), but
        // differ slightly because of floating point problems.  Thus, add a
        // little to the test number to push actually equal numbers over the
        // edge towards the positive.

        // TODO: This should probably be a relative tolerance.  Multiplying
        // by the constant is probably not the best way to do this.
        test[i] = normal.Dot(P[i]) - constant +
            fabsf(constant)*EPSILON;

        if (test[i] >= 0)
        {
            ++positive;
            if (pIndex < 0)
            {
                pIndex = i;
            }
        }
        else
        {
            ++negative;
        }
    }

    if (quantity == 2)
    {
        // Lines are a little different, in that clipping the segment
        // cannot create a new segment, as clipping a polygon would
        if (positive > 0)
        {
            if (negative > 0) 
            {
                int clip;

                if (pIndex == 0)
                {
                    // vertex0 positive, vertex1 is clipped
                    clip = 1;
                }
                else // pIndex == 1
                {
                    // vertex1 positive, vertex0 clipped
                    clip = 0;
                }

                float t = test[pIndex]/(test[pIndex] - test[clip]);
                P[clip] = P[pIndex] + t*(P[clip] - P[pIndex]);
            }
            // otherwise both positive, no clipping
        }
        else
        {
            // Assert:  The entire line is clipped, but we should not
            // get here.
            quantity = 0;
        }
    }
    else
    {
        if (positive > 0)
        {
            if (negative > 0)
            {
                // plane transversely intersects polygon
                Vector3 CV[8];
                int cQuantity = 0, cur, prv;
                float t;

                if (pIndex > 0)
                {
                    // first clip vertex on line
                    cur = pIndex;
                    prv = cur - 1;
                    t = test[cur]/(test[cur] - test[prv]);
                    CV[cQuantity++] = P[cur] + t*(P[prv] - P[cur]);

                    // vertices on positive side of line
                    while (cur < currQuantity && test[cur] >= 0)
                    {
                        CV[cQuantity++] = P[cur++];
                    }

                    // last clip vertex on line
                    if (cur < currQuantity)
                    {
                        prv = cur - 1;
                    }
                    else
                    {
                        cur = 0;
                        prv = currQuantity - 1;
                    }
                    t = test[cur]/(test[cur] - test[prv]);
                    CV[cQuantity++] = P[cur] + t*(P[prv] - P[cur]);
                }
                else  // pIndex is 0
                {
                    // vertices on positive side of line
                    cur = 0;
                    while (cur < currQuantity && test[cur] >= 0)
                    {
                        CV[cQuantity++] = P[cur++];
                    }

                    // last clip vertex on line
                    prv = cur - 1;
                    t = test[cur]/(test[cur] - test[prv]);
                    CV[cQuantity++] = P[cur] + t*(P[prv] - P[cur]);

                    // skip vertices on negative side
                    while (cur < currQuantity && test[cur] < 0)
                    {
                        cur++;
                    }

                    // first clip vertex on line
                    if (cur < currQuantity)
                    {
                        prv = cur - 1;
                        t = test[cur]/(test[cur] - test[prv]);
                        CV[cQuantity++] = P[cur] + t*(P[prv] - P[cur]);

                        // vertices on positive side of line
                        while (cur < currQuantity && test[cur] >= 0)
                        {
                            CV[cQuantity++] = P[cur++];
                        }
                    }
                    else
                    {
                        // cur = 0
                        prv = currQuantity - 1;
                        t = test[0]/(test[0] - test[prv]);
                        CV[cQuantity++] = P[0] + t*(P[prv] - P[0]);
                    }
                }

                currQuantity = cQuantity;
                memcpy(P, CV, cQuantity*sizeof(Vector3));
            }
            // else polygon fully on positive side of plane, nothing to do

            quantity = currQuantity;
        }
        else
        {
            // Polygon does not intersect positive side of plane, clip all.
            // This should not ever happen if called by the findintersect
            // routines after an intersection has been determined.
            quantity = 0;
        }    
    }
}
//----------------------------------------------------------------------------
Vector3 GetPointFromIndex (int index, const OBBox& box)
{
    Vector3 point = box.center;

    if (index & 4)
    {
        point += box.extent[2]*box.axis[2];
    }
    else
    {
        point -= box.extent[2]*box.axis[2];
    }

    if (index & 2)
    {
        point += box.extent[1]*box.axis[1];
    }
    else
    {
        point -= box.extent[1]*box.axis[1];
    }

    if (index & 1)
    {
        point += box.extent[0]*box.axis[0];
    }
    else
    {
        point -= box.extent[0]*box.axis[0];
    }

    return point;
}


}