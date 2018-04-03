/* ===================================================================

	[1] WildMagic

	==================================================================
*/

#pragma once

#include "Vector3.h"

namespace GODZ
{
	struct OBBox;

	//----------------------------------------------------------------------------
	class IntrConfiguration
	{
	public:
		// ContactSide (order of the intervals of projection).
		enum
		{
			LEFT,
			RIGHT,
			NONE
		};

		// VertexProjectionMap (how the vertices are projected to the minimum
		// and maximum points of the interval).
		enum
		{
			m2, m11,             // segments
			m3, m21, m12, m111,  // triangles
			m44, m2_2, m1_1      // boxes
		};

		// The VertexProjectionMap value for the configuration.
		int mMap;

		// The order of the vertices.
		int mIndex[8];

		// Projection interval.
		float mMin, mMax;
	};
	//----------------------------------------------------------------------------

	//----------------------------------------------------------------------------
	class FindContactSet
	{
	public:
		FindContactSet(const OBBox& box0, const OBBox& box1,
			int side, const IntrConfiguration& box0Cfg,
			const IntrConfiguration& box1Cfg,
			const Vector3& box0Velocity,
			const Vector3& box1Velocity, float tfirst, int& quantity,
			Vector3* P);

	private:
		// These functions are called when it is known that the features are
		// intersecting.  Consequently, they are specialized versions of the
		// object-object intersection algorithms.

		static void ColinearSegments (const Vector3 segment0[2],
			const Vector3 segment1[2], int& quantity, Vector3* P);

		static void SegmentThroughPlane (const Vector3 segment[2],
			const Vector3& planeOrigin, const Vector3& planeNormal,
			int& quantity, Vector3* P);

		static void SegmentSegment (const Vector3 segment0[2], 
			const Vector3 segment1[2], int& quantity, Vector3* P);

		static void ColinearSegmentTriangle (const Vector3 segment[2],
			const Vector3 triangle[3], int& quantity, Vector3* P);

		static void CoplanarSegmentRectangle (const Vector3 segment[2],
			const Vector3 rectangle[4], int& quantity, Vector3* P);

		static void CoplanarTriangleRectangle (const Vector3 triangle[3],
			const Vector3 rectangle[4], int& quantity, Vector3* P);

		static void CoplanarRectangleRectangle (
			const Vector3 rectangle0[4],
			const Vector3 rectangle1[4], int& quantity, Vector3* P);
	};

	//See IntrAxis<Real> in wildmagic5.1
	class GODZ_API IntersectionAxis
	{
	public:
		static void GetProjection (const Vector3& axis, const OBBox& box, float& imin, float& imax);
		static void GetProjection (const Vector3& axis, const Vector3 triangle[3], float& imin, float& imax);
		static bool Test (const Vector3& axis, const Vector3 triangle[3], const OBBox& box, const Vector3& velocity,
			float tmax, float& tfirst, float& tlast);

    // Configurations.
    static void GetConfiguration (const Vector3& axis, 
        const OBBox& box, IntrConfiguration& cfg);

    // Low-level test-query for projections.
    static bool Test (const Vector3& axis, const Vector3& velocity, float min0, float max0, float min1,
		float max1, float tmax, float& tfirst, float& tlast);

    // Find-query for intersection of projected intervals.  The velocity
    // input is the difference objectVelocity1 - objectVelocity0.  The
    // first and last times of contact are computed, as is information about
    // the contact configuration and the ordering of the projections (the
    // contact side).
    static bool Find (const Vector3& axis,
        const OBBox& box0, const OBBox& box1,
        const Vector3& velocity, float tmax, float& tfirst, float& tlast,
        int& side, IntrConfiguration& box0CfgFinal,
        IntrConfiguration& box1CfgFinal);

    // Low-level find-query for projections.
    static bool Find (const Vector3& axis,
        const Vector3& velocity,
        const IntrConfiguration& cfg0Start,
        const IntrConfiguration& cfg1Start, float tmax, int& side,
        IntrConfiguration& cfg0Final,
        IntrConfiguration& cfg1Final, float& tfirst, float& tlast);
	};


//----------------------------------------------------------------------------
// Miscellaneous support.
//----------------------------------------------------------------------------
// The input and output polygons are stored in P.  The size of P is
// assumed to be large enough to store the clipped convex polygon vertices.
// For now the maximum array size is 8 to support the current intersection
// algorithms.
void ClipConvexPolygonAgainstPlane (const Vector3& normal,
    float bonstant, int& quantity, Vector3* P);

// Translates an index into the box back into real coordinates.
Vector3 GetPointFromIndex (int index, const OBBox& box);
//----------------------------------------------------------------------------


}
