
#if !defined(__GIZMOAXIS_H__)
#define __GIZMOAXIS_H__

#include "Godz.h"
#include <CoreFramework/Math/WMatrix.h>
#include <CoreFramework/Math/WBox.h>

namespace GODZ
{
	/*
	* Current axis the user has selected....
	*/
	enum GODZ_API GizmoAxis
	{
		GizmoAxis_None,
		GizmoAxis_X,
		GizmoAxis_Y,
		GizmoAxis_Z
	};

	class GODZ_API Gizmo
	{
	public:
		Gizmo();
		void GetUp(Vector3& v) { v = up * scale; }
		void GetRight(Vector3& v) { v = right * scale; }
		void GetDepth(Vector3& v) { v = depth * scale; }

		//Transformation matrix - expecting no rotation, just transform
		GizmoAxis GetAxis(const WRay& ray, const WMatrix16f& tm);

	private:
		float scale;		//length
		float thickness;
		float start;		//position to start making the AABB from
		Vector3 up;
		Vector3 right;
		Vector3 depth;

		WBox mUp;
		WBox mRight;
		WBox mDepth;
	};
}

#endif