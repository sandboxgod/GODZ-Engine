/* ===================================================================
	WBox.h

	Sources Cited:

	[1] 3D Game Engine Design by David Eberly
	[2] Real Time Collision Detection by Christer Ericson

	Created July 19, 2003 by Richard Osborne
	Copyright (c) 2003, Richard Osborne
	==================================================================
*/

#if !defined(_WBOX_H_)
#define _WBOX_H_

#include "WMatrix.h"
#include <CoreFramework/Core/Godz.h>
#include <CoreFramework/Core/GDZArchive.h>
#include <CoreFramework/Core/VertexTypes.h>

namespace GODZ
{
	struct WSphere;
	struct OBBox;
	struct WRay;
	struct WPlane;

	//Axis Aligned Bounding Box (AABB). All containment tests return a
	//positive result when a point/volume intersects this box.
	struct GODZ_API WBox
	{
	public:


		WBox();

		// Creates an AABB based on a mesh definition.
		WBox(Vertex* list, size_t count);
		WBox(Vector3* list, size_t count);
		WBox(BaseVertex* list, size_t count);

		// Creates a Box based on the min/max extents
		WBox(const Vector3& min, const Vector3& max);

		void SetDimensions(const Vector3& center, const Vector3& extent);

		//Test to determine if the Vector argument is contained within this volume
		bool ContainsPoint(const Vector3& point) const;

		//Test to determine if the Box argument is contained within this volume
		bool ContainsBox(const WBox& box) const;

		// Returns true if the triable is *completely* encapsulated within this volume.
		bool ContainsTriangle(const Vector3 list[3]) const;

		// Returns true if the vertices are completely encapsulated within this volume.
		bool ContainsTriangle(const BaseVertex list[3]) const;

		//AABB vs Ray
		bool Intersects(const WRay& ray, float& tmin, Vector3& q) const;

		//Static AABB vs Triangle
		bool Intersects(const Vector3 list[3]) const;

		bool Intersects(const WPlane& p) const;
		bool Intersects(const WSphere& s) const;
		bool Intersects(const Vector3& c, float r) const;

		//Test for Intersection between 2 boxes
		bool IntersectsBox(const WBox& box) const;

		//Intersection test for two AABBs with constant velocities.
		//Grabbed from Real Time Collision Detection book - Very Fast
		static bool IntersectsMovingBox(const WBox& a, const Vector3& va, const WBox& b, const Vector3& bv, float& tfirst, float& tlast);
		static bool AABBSweep(const WBox& a, const Vector3& va, const WBox& b, const Vector3& bv, float& tfirst, float& tlast);

		//Returns true if this is a valid 3D box
		bool IsValid() const;

		//Returns the bottom-center of this box
		Vector3 GetBottom() const;

		//Returns the min bound for the req axis
		float GetMin(int iAxis) const;

		//Returns the max bound for the req axis
		float GetMax(int iAxis) const;

		const Vector3& GetMin() const { return min; }
		const Vector3& GetMax() const { return max; }
		void SetMinMax(const Vector3& min, const Vector3& max);

		//Returns the center of this box
		const Vector3& GetCenter() const { return center; }
		void SetCenter(const Vector3& pos);

		//Returns the extent of this box
		const Vector3& GetExtent() const { return extent; }

		//Returns the index of the longest axis {x=0,y=1,z=2}
		int GetLongestAxis() const;

		//Returns the size of this box.
		float GetSize() const;

		//Returns the 8 points that composes this AABB
		void GetAABBPoints(Vector3 points[8]) const;
		Vector3 GetNormalFromPoint(const Vector3& point) const;
		
		// Creates an AABB based on a mesh definition.
		inline void SetBounds(Vertex* list, int count);

		// Creates an AABB based on a list of vectors.
		inline void SetBounds(Vector3* list, int count);

		void Serialize(GDZArchive& ar);

		//Transforms using the Matrix (rotation / scale / translation)
		void Transform(const WMatrix& m);
		void Transform(const Vector3& p);

		//Performs a separating axis test to determine if two boxes overlap.
		inline bool Overlaps(const WBox& box) const;

		//Returns a normalized plane normal that is closest to the argument
		static Vector3 GetNearPlane(const Vector3& v);

		static WBox Zero()
		{
			return WBox( Vector3::Zero(), Vector3::Zero() );
		}

		//--------------------------------------------------------
		//Box operators
		//--------------------------------------------------------

		bool operator==(const WBox& Box) const;
		bool operator!=(const WBox& Box) const;

		//Returns a sphere
		operator WSphere() const;
		operator OBBox() const;

		private:
			void updateCenterExtent( void );

			//Center & Extent are useful for when users need to translate & scale
			//the AABB. Otherwise, using just min/max one can run into trouble doing a 
			//scale + translate + scale (depending on the order of operation you get
			//a different box).
			Vector3 extent;
			Vector3 center;

			//Some equations prefer min/max. So we also define this for speed at the cost of
			//a little extra memory consumption
			Vector3 min;
			Vector3 max;
	};

	static const WBox EMPTY_BOX = WBox(ZERO_VECTOR, ZERO_VECTOR);
}

#endif