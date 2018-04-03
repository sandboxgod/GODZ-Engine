/* ===================================================================
	ViewFrustum

	Sources Cited:

	[1] http://www2.ravensoft.com/users/ggribb/plane%20extraction.pdf

	Created Sept 4, 2004 by Richard Osborne
	Copyright (c) 2010
	==================================================================
*/

#if !defined(__VIEWFRUSTUM__H__)
#define __VIEWFRUSTUM__H__

#include "WMatrix.h"
#include "WPlane.h"
#include "WBox.h"

namespace GODZ
{
	class GODZ_API ViewFrustum
	{
	public:
		enum FrustumResult
		{
			FrustumResult_IN,
			FrustumResult_INTERSECT,
			FrustumResult_OUT
		};

		ViewFrustum();

		// [in] View * Projection
		void ExtractPlanes(const WMatrix16f& combMat);
		FrustumResult SphereInFrustum(const WSphere& sphere);
		bool PointInFrustum(const Vector3& pt);
		FrustumResult BoxInFrustum(const WBox& Box);

		static void CalculateFrustumCorners(Vector3 pPoints[8],
							 const Vector3& vSource,
							 const Vector3& vTarget,
							 const Vector3& vUp,
							 float fNear, float fFar,
							 float fFOV, float fAspect, float fScale);

	private:
		WPlane m_plane[6];
	};
}

#endif //__VIEWFRUSTUM__H__