
#include "Quaternion.h"
#include "WMatrixUtil.h"
#include "vectormath.h"
#include <CoreFramework/Core/godzassert.h>
#include <stdio.h>
#include <stdlib.h>

namespace GODZ
{

Quaternion::Quaternion(const WMatrix3& pm)
{
	//SEE: http://www.irit.fr/~Loic.Barthe/Enseignements/BE_Modelisation/Robo/Doc/Quaternion_8cpp-source.html
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".

    float fTrace = pm(0,0) + pm(1,1) + pm(2,2);
    float fRoot;

    if ( fTrace > 0.f )
    {
        // |w| > 1/2, may as well choose w > 1/2
        fRoot = sqrt(fTrace + 1.f);  // 2w
        q.w = 0.5f * fRoot;
        fRoot = 0.5f / fRoot;  // 1/(4w)
        q.x = -(pm(2,1)-pm(1,2))*fRoot;
        q.y = -(pm(0,2)-pm(2,0))*fRoot;
        q.z = -(pm(1,0)-pm(0,1))*fRoot;
    }
    else
    {
		static int s_iNext[3] = { 1, 2, 0 };
        // |w| <= 1/2
        int i = 0;
        if ( pm(1,1) > pm(0,0) )
            i = 1;
        if ( pm(2,2) > pm(i,i) )
            i = 2;
        int j = s_iNext[i];
        int k = s_iNext[j];

        fRoot = sqrt(pm(i,i)-pm(j,j)-pm(k,k)+1.f);
        float* apfQuat[3] = { &q.x, &q.y, &q.z };
        *apfQuat[i] = -0.5f*fRoot;
        fRoot = 0.5f/fRoot;
        q.w = (pm(k,j)-pm(j,k))*fRoot;
        *apfQuat[j] = -(pm(j,i)+pm(i,j))*fRoot;
        *apfQuat[k] = -(pm(k,i)+pm(i,k))*fRoot;
    }
}

/*
For implementation:
http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
heading = atan2(2*qy*qw-2*qx*qz , 1 - 2*qy2 - 2*qz2)
attitude = asin(2*qx*qy + 2*qz*qw) 
bank = atan2(2*qx*qw-2*qy*qz , 1 - 2*qx2 - 2*qz2)
*/

//Good explanation but their implementation didn't work for me; still good info
//from http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
//NOTE: This implementation requires a normalized Quaternion
void Quaternion::getEulerAngles(Vector3& euler) const
{
	float test = q.x*q.y + q.z*q.w;
	if (test > 0.499f) { // singularity at north pole
		euler.z = 2 * atan2f(q.x,q.w);
		euler.y = PI/2;
		euler.x = 0;
	}
	else if (test < -0.499f) { // singularity at south pole
		euler.z = -2 * atan2f(q.x,q.w);
		euler.y = - PI/2;
		euler.x = 0;
	}
	else
	{
		float sqx = q.x*q.x;
		float sqy = q.y*q.y;
		float sqz = q.z*q.z;

		euler.z = atan2(2*q.y*q.w-2*q.x*q.z , 1 - 2*sqy - 2*sqz);
		euler.y = asin(2*q.x*q.y + 2*q.z*q.w);
		euler.x = atan2(2*q.x*q.w-2*q.y*q.z , 1 - 2*sqx - 2*sqz);
	}
}

void Quaternion::rotate(Vector3& v) const
{
	Quaternion point(v.x, v.y, v.z, 0);
	Quaternion conjugate = *this;
	conjugate.conjugate();
	Quaternion result(*this * point * conjugate);
	v.x = result.q.x;
	v.y = result.q.y;
	v.z = result.q.z;
}

//==================================================================
//QuaternionFromYawPitchRoll

//Takes quaternion q and generates a rotation matrix stored in the 
//three vectors right, up and forward
//==================================================================
void QuaternionToMatrix(Quat4f& quat, Vector3& right, Vector3& up, Vector3& forward)
{
	float xx, yy, zz, xy, wz, xz, wy, yz, wx;
	
	xx = quat.q.x * quat.q.x;  yy = quat.q.y * quat.q.y;  zz = quat.q.z * quat.q.z;
	xy = quat.q.x * quat.q.y;  wz = quat.q.w * quat.q.z;  xz = quat.q.x * quat.q.z;
	wy = quat.q.w * quat.q.y;  yz = quat.q.y * quat.q.z;  wx = quat.q.w * quat.q.x;

	right.x = 1 - 2 * yy - 2 * zz;
	right.y = 2 * xy - 2 * wz;
	right.z = 2 * xz + 2 * wy;
	
	up.x = 2 * xy + 2 * wz;
	up.y = 1 - 2 * xx - 2 * zz;
	up.z = 2 * yz - 2 * wx;

	forward.x = 2 * xz - 2 * wy;
	forward.y = 2 * yz + 2 * wx;
	forward.z = 1 - 2 * xx - 2 * yy;
}


void QuaternionToMatrix(Quat4f& quat, WMatrix16f& m)
{
	/*
	float xx, yy, zz, xy, wz, xz, wy, yz, wx;
	
	xx = quat.q.x * quat.q.x;  yy = quat.q.y * quat.q.y;  zz = quat.q.z * quat.q.z;
	xy = quat.q.x * quat.q.y;  wz = quat.q.w * quat.q.z;  xz = quat.q.x * quat.q.z;
	wy = quat.q.w * quat.q.y;  yz = quat.q.y * quat.q.z;  wx = quat.q.w * quat.q.x;

	m(0,0) = 1 - 2 * yy - 2 * zz;
	m(1,0) = 2 * xy - 2 * wz;
	m(2,0) = 2 * xz + 2 * wy;
	
	m(0,1) = 2 * xy + 2 * wz;
	m(1,1) = 1 - 2 * xx - 2 * zz;
	m(2,1) = 2 * yz - 2 * wx;
		{

	m(0,2) = 2 * xz - 2 * wy;
	m(1,2) = 2 * yz + 2 * wx;
	m(2,2) = 1 - 2 * xx - 2 * yy;


	D3DXMATRIX mat;
	D3DXQUATERNION q(quat.q.x,quat.q.y,quat.q.z,quat.q.w);
	D3DXMatrixRotationQuaternion(&mat, &q);

	for(int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			assert( m(i,j) == mat(i,j) );
		}
	}
	*/

	WMatrix3 mat(m);
	QuaternionToMatrix(quat, mat);
	CopyMatrix(mat, m);
}

void QuaternionToMatrix(const Quat4f& quat, WMatrix3& m)
{
	float xx, yy, zz, xy, wz, xz, wy, yz, wx;
	
	xx = quat.q.x * quat.q.x;  yy = quat.q.y * quat.q.y;  zz = quat.q.z * quat.q.z;
	xy = quat.q.x * quat.q.y;  wz = quat.q.w * quat.q.z;  xz = quat.q.x * quat.q.z;
	wy = quat.q.w * quat.q.y;  yz = quat.q.y * quat.q.z;  wx = quat.q.w * quat.q.x;

	m(0,0) = 1 - 2 * yy - 2 * zz;
	m(1,0) = 2 * xy - 2 * wz;
	m(2,0) = 2 * xz + 2 * wy;
	
	m(0,1) = 2 * xy + 2 * wz;
	m(1,1) = 1 - 2 * xx - 2 * zz;
	m(2,1) = 2 * yz - 2 * wx;

	m(0,2) = 2 * xz - 2 * wy;
	m(1,2) = 2 * yz + 2 * wx;
	m(2,2) = 1 - 2 * xx - 2 * yy;
}

//! Multiply a quaternion by another given one and return the result quaternion.
Quaternion	Quaternion::operator*( const Quaternion& q2 ) const
{
	Quaternion result;
	result.q.x =  q.x * q2.q.w + q.y * q2.q.z - q.z * q2.q.y + q.w * q2.q.x;
	result.q.y = -q.x * q2.q.z + q.y * q2.q.w + q.z * q2.q.x + q.w * q2.q.y;
	result.q.z =  q.x * q2.q.y - q.y * q2.q.x + q.z * q2.q.w + q.w * q2.q.z;
	result.q.w = -q.x * q2.q.x - q.y * q2.q.y - q.z * q2.q.z + q.w * q2.q.w;

	return result;
}
}
