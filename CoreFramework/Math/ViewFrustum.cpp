
#include "ViewFrustum.h"
#include "WSphere.h"

using namespace GODZ;


//code: http://www.flipcode.com/archives/Frustum_Culling.shtml


ViewFrustum::ViewFrustum()
{
}

void ViewFrustum::ExtractPlanes(const WMatrix& combMat)
{
	//left clipping plane
	m_plane[0].N.x = combMat._14 + combMat._11;
	m_plane[0].N.y = combMat._24 + combMat._21;
	m_plane[0].N.z = combMat._34 + combMat._31;
	m_plane[0].D   = combMat._44 + combMat._41;

	//right clipping plane
	m_plane[1].N.x = combMat._14 - combMat._11;
	m_plane[1].N.y = combMat._24 - combMat._21;
	m_plane[1].N.z = combMat._34 - combMat._31;
	m_plane[1].D   = combMat._44 - combMat._41;

	//top clipping plane
	m_plane[2].N.x = combMat._14 - combMat._12;
	m_plane[2].N.y = combMat._24 - combMat._22;
	m_plane[2].N.z = combMat._34 - combMat._32;
	m_plane[2].D   = combMat._44 - combMat._42;

	//bottom clipping plane
	m_plane[3].N.x = combMat._14 + combMat._12;
	m_plane[3].N.y = combMat._24 + combMat._22;
	m_plane[3].N.z = combMat._34 + combMat._32;
	m_plane[3].D   = combMat._44 + combMat._42;

	//near clipping plane
	m_plane[4].N.x = combMat._13;
	m_plane[4].N.y = combMat._23;
	m_plane[4].N.z = combMat._33;
	m_plane[4].D   = combMat._43;

	//far clipping plane
	m_plane[5].N.x = combMat._14 - combMat._13;
	m_plane[5].N.y = combMat._24 - combMat._23;
	m_plane[5].N.z = combMat._34 - combMat._33;
	m_plane[5].D   = combMat._44 - combMat._43;

	for (int i=0; i<6; i++)
	{
		m_plane[i].Normalize();
	}
}

bool ViewFrustum::PointInFrustum(const Vector3& pt)
{
	////////////////////////////////////////////////////////////////////////
	// Return true when the given point is inside the view frustum. This is
	// the case when its distance is positive to all the frustum planes
	////////////////////////////////////////////////////////////////////////

	unsigned int iCurPlane;

	for (iCurPlane = 0; iCurPlane<6; iCurPlane++)
	{
		if (m_plane[iCurPlane].N.x * pt.x + m_plane[iCurPlane].N.y * 
			pt.y + m_plane[iCurPlane].N.z * pt.z + m_plane[iCurPlane].D <= 0)
		{
			return false;
		}
	}

   return true;
}

ViewFrustum::FrustumResult ViewFrustum::SphereInFrustum(const WSphere& sphere)
{
	////////////////////////////////////////////////////////////////////////
	// Return true when the sphere is inside the view frustum. This is
	// the case when it is never further behind a any plane than its radius
	////////////////////////////////////////////////////////////////////////

	unsigned int i;
	float dist;

	for (i = 0; i<6; i++)
	{
		// find the distance to this plane
		dist = m_plane[i].N.Dot(sphere.center) + m_plane[i].D;
		if (dist <= -sphere.radius)
		{
			return FrustumResult_OUT;
		}

		// else if the distance is between +- radius, then we intersect
		if(fabsf(dist) < sphere.radius)
			return(FrustumResult_INTERSECT);
	}

   return FrustumResult_IN;
}

//Should call SphereInFrustum instead....
ViewFrustum::FrustumResult ViewFrustum::BoxInFrustum(const WBox& refBox)
{
	Vector3 vCorner[8];
	int iTotalIn = 0;

	// get the corners of the box into the vCorner array
	refBox.GetAABBPoints(vCorner);

	// test all 8 corners against the 6 sides 
	// if all points are behind 1 specific plane, we are out
	// if we are in with all points, then we are fully in
	for(int p = 0; p < 6; ++p)
	{
	
		int iInCount = 8;
		int iPtIn = 1;

		for(int i = 0; i < 8; ++i)
		{

			// test this point against the planes
			if(m_plane[p].ClassifyPoint(vCorner[i]) == Halfspace_BEHIND)
			{
				iPtIn = 0;
				--iInCount;
			}
		}

		// were all the points outside of plane p?
		if(iInCount == 0)
			return(FrustumResult_OUT);

		// check if they were all on the right side of the plane
		iTotalIn += iPtIn;
	}

	// so if iTotalIn is 6, then all are inside the view
	if(iTotalIn == 6)
		return(FrustumResult_IN);

	// we must be partly in then otherwise
	return(FrustumResult_INTERSECT);
}


// Computes corner points of a frustum
//
//
void ViewFrustum::CalculateFrustumCorners(Vector3 pPoints[8],
							 const Vector3& vSource,
							 const Vector3& vTarget,
							 const Vector3& vUp,
							 float fNear, float fFar,
							 float fFOV, float fAspect, float fScale)
{

	//rno - looks like we are computing the X,Y,Z axis of a left handed view matrix here
	Vector3 vZ=vTarget-vSource;
	vZ.Normalize();

	Vector3 vX;
	vX = vUp.Cross(vZ);
	vX.Normalize();

	Vector3 vY;
	vY = vZ.Cross(vX);


	//our FOV is already in radians here
	float fNearPlaneHeight = tanf((fFOV) * 0.5f) * fNear;
	float fNearPlaneWidth = fNearPlaneHeight * fAspect;

	float fFarPlaneHeight = tanf((fFOV) * 0.5f) * fFar;
	float fFarPlaneWidth = fFarPlaneHeight * fAspect;

	Vector3 vNearPlaneCenter = vSource + vZ * fNear;
	Vector3 vFarPlaneCenter = vSource + vZ * fFar;

	pPoints[0]=Vector3(vNearPlaneCenter - vX*fNearPlaneWidth - vY*fNearPlaneHeight);
	pPoints[1]=Vector3(vNearPlaneCenter - vX*fNearPlaneWidth + vY*fNearPlaneHeight);
	pPoints[2]=Vector3(vNearPlaneCenter + vX*fNearPlaneWidth + vY*fNearPlaneHeight);
	pPoints[3]=Vector3(vNearPlaneCenter + vX*fNearPlaneWidth - vY*fNearPlaneHeight);

	pPoints[4]=Vector3(vFarPlaneCenter - vX*fFarPlaneWidth - vY*fFarPlaneHeight); //0,0
	pPoints[5]=Vector3(vFarPlaneCenter - vX*fFarPlaneWidth + vY*fFarPlaneHeight); //0,1
	pPoints[6]=Vector3(vFarPlaneCenter + vX*fFarPlaneWidth + vY*fFarPlaneHeight); //1,1
	pPoints[7]=Vector3(vFarPlaneCenter + vX*fFarPlaneWidth - vY*fFarPlaneHeight); //1,0

	if (fScale > 0.0f)
	{
		// calculate center of points
		Vector3 vCenter(0,0,0);
		for(int i=0;i<8;i++) vCenter+=pPoints[i];
		vCenter/=8;

		// for each point
		for(int i=0;i<8;i++)
		{
			// scale by adding offset from center
			pPoints[i]+=(pPoints[i]-vCenter)*(fScale-1);
		}
	}
}

