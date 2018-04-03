
#include "WMatrix.h"
#include "WMatrix3.h"

using namespace GODZ;

WMatrix::WMatrix(const WMatrix3& tm)
: _11(1), _12(0), _13(0), _14(0)
, _21(0), _22(1), _23(0), _24(0)
, _31(0), _32(0), _33(1), _34(0)
, _41(0), _42(0), _43(0), _44(1)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j=0;j<3;j++)
		{
			m[i][j] = tm.m[i][j];
		}
	}
}

void GODZ::WMatrixMul(const WMatrix& a, const WMatrix& b, WMatrix& result)
{
	int column = 0;
	for (int i = 0; i < 4; i++) // rows
	{
		for (int j=0;j<4;j++) //each column
		{
			float val = 0;
			for (int k=0;k<4;k++) //iterate through other matrix
			{
				val += a.m[i][k] * b.m[k][j];
			}

			result.m[i][j] = val;
		}
	}
}

//Computes left handed view matrix 
void GODZ::ComputeViewMatrix(const Vector3& eye, const Vector3& lookAt, const Vector3& up, WMatrix& mat) 
{
	Vector3 zaxis = (lookAt - eye);
	zaxis.Normalize();
	Vector3 xaxis = (up.Cross(zaxis));
	xaxis.Normalize();
	Vector3 yaxis = (zaxis.Cross(xaxis));

	mat._11 = xaxis.x;
	mat._21 = xaxis.y;
	mat._31 = xaxis.z;

	mat._12 = yaxis.x;
	mat._22 = yaxis.y;
	mat._32 = yaxis.z;

	mat._13 = zaxis.x;
	mat._23 = zaxis.y;
	mat._33 = zaxis.z;

	mat._41 = -xaxis.Dot(eye);
	mat._42 = -yaxis.Dot(eye);
	mat._43 = -zaxis.Dot(eye);
}

void GODZ::ComputePerspectiveFovLH(const ProjectionMatrixInfo& projMatInfo, WMatrix& mat) 
{

	float yscale = 1/tan(projMatInfo.fov/2);
	float xScale = yscale/projMatInfo.aspectRatio;	
	
	mat._11 = xScale;
	mat._22 = yscale;
	mat._33 = projMatInfo.far_plane/(projMatInfo.far_plane-projMatInfo.near_plane) ;
	mat._34 = 1.0f;
	mat._43 = -projMatInfo.near_plane*projMatInfo.far_plane/(projMatInfo.far_plane-projMatInfo.near_plane);
	mat._44 = 0;
}

void GODZ::ComputeMatrixOrthoOffCenterLH(float l, float r, float b, float t, float zn, float zf, WMatrix& mat)
{
	/*
	2/(r-l)      0            0           0
	0            2/(t-b)      0           0
	0            0            1/(zf-zn)   0
	(l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  l
	*/

	mat._11 = 2/(r-1);
	mat._22 = 2/(t-b);
	mat._33 = 1/(zf-zn);
	mat._41 = (l+r)/(l-r);
	mat._42 = (t+b)/(b-t);
	mat._43 = zn/(zn-zf);
}
