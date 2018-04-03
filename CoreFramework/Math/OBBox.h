/* ===================================================================
	OBBox.h

	Sources Cited:

	[1] 3D Game Engine Design by David Eberly 
	http://www.geometrictools.com/Documentation/DynamicCollisionDetection.pdf

	[2] Real Time Collision Detection by Christer Ericson

	[3] WildMagic 5.1 source
	==================================================================
*/

#pragma once

#include "Vector3.h"
#include "WMatrix3.h"
#include "WMatrix.h"

namespace GODZ
{
	struct WBox;
	struct WSphere;
	struct WRay;

	//Oriented Bounding Box
	struct GODZ_API OBBox
	{
	public:
		Vector3 center;
		Vector3 axis[3];	//This is really a Matrix3
		Vector3 extent;

		OBBox() {}
		OBBox(const Vector3& center, const Vector3& extent);

		friend GDZArchive& operator<<(GDZArchive& ar, OBBox& box)
		{
			ar << box.center << box.extent;
			ar << box.axis[0] << box.axis[1] << box.axis[2];
			return ar;
		}

		static bool Test(const OBBox& box0,const OBBox& box1);
		static bool Intersects(const OBBox& box0, const Vector3& velocity0, const OBBox& box1, const Vector3& velocity1, float tmax, float& contactTime);
		static bool Find(float tmax, const Vector3& velocity0, const Vector3& velocity1, const OBBox& box0, const OBBox& box1, int& quantity, float& contactTime, Vector3 point[8]);

		//Returns the 8 points that composes this OBB
		void GetAABBPoints(Vector3 points[8]) const;

		//Static OBB vs Triangle
		bool Intersects(const Vector3 list[3]) const;

		//Moving OBB vs Triangle
		bool Intersects(float tmax, const Vector3 list[3], const Vector3& velocity0, const Vector3& velocity1, float& contactTime) const;

		bool Intersects(const WRay& ray, float& tmin, Vector3& q) const;

		void TransformBy(const WMatrix16f& m);
		void SetTransformUnscaled(const WMatrix16f& m);
		void MakeIdentity(void);
		void GetTransform(WMatrix& m) const;
		Vector3 GetNormalFromPoint(const Vector3& cpoint) const;
		void GetMinMax(Vector3& min, Vector3& max) const;

		operator WBox() const;
		operator WSphere() const;

	private:
		static bool IsSeparated (float min0, float max0, float min1, float max1, float speed,
			float tmax, float& tlast, float& contactTime);

	};
}
