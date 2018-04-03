
#include "WMatrixUtil.h"


namespace GODZ
{

// O is your object's position
// P is the position of the object to face
// U is the nominal "up" vector (typically Vector3.Y)
// Note: this does not work when O is straight below or straight above P
//http://forums.xna.com/forums/t/10203.aspx
GODZ_API WMatrix3 RotateToFace(const Vector3& O, const Vector3& P, const Vector3& U)
{
	Vector3 D = (O - P);
	//D.Normalize(); -- i added
	Vector3 Right = U.Cross(D);
	Right.Normalize();
	Vector3 Backwards = Right.Cross(U);
	Backwards.Normalize();
	Vector3 Up = Backwards.Cross(Right);
	//Up.Normalize(); -- i added
	return WMatrix3(Right, Up, Backwards);
}

// http://www.euclideanspace.com/maths/geometry/affine/conversions/quaternionToMatrix/index.htm
GODZ_API void RotateAroundPoint(const Quaternion& quat, const Vector3& centre, WMatrix& m)
{
  double sqw = quat.q.w*quat.q.w;
  double sqx = quat.q.x*quat.q.x;
  double sqy = quat.q.y*quat.q.y;
  double sqz = quat.q.z*quat.q.z;

  /*
  m00 =  sqx - sqy - sqz + sqw; // since sqw + sqx + sqy + sqz =1
  m11 = -sqx + sqy - sqz + sqw;
  m22 = -sqx - sqy + sqz + sqw;
  */

  m.m[0][0] =  sqx - sqy - sqz + sqw; // since sqw + sqx + sqy + sqz =1
  m.m[1][1] = -sqx + sqy - sqz + sqw;
  m.m[2][2] = -sqx - sqy + sqz + sqw;

  
  double tmp1 = quat.q.x*quat.q.y;
  double tmp2 = quat.q.z*quat.q.w;

  /*
  m01 = 2.0 * (tmp1 + tmp2);
  m10 = 2.0 * (tmp1 - tmp2);
  */

  m.m[0][1] = 2.0 * (tmp1 + tmp2);
  m.m[1][0] = 2.0 * (tmp1 - tmp2);
  
  tmp1 = quat.q.x*quat.q.z;
  tmp2 = quat.q.y*quat.q.w;

  /*
  m02 = 2.0 * (tmp1 - tmp2);
  m20 = 2.0 * (tmp1 + tmp2);
  */

  m.m[0][2] = 2.0 * (tmp1 - tmp2);
  m.m[2][0] = 2.0 * (tmp1 + tmp2);

  tmp1 = quat.q.y*quat.q.z;
  tmp2 = quat.q.x*quat.q.w;

  /*
  m12 = 2.0 * (tmp1 + tmp2);
  m21 = 2.0 * (tmp1 - tmp2);
  */

  m.m[1][2] = 2.0 * (tmp1 + tmp2);
  m.m[2][1] = 2.0 * (tmp1 - tmp2);

  double a1,a2,a3;

  /*
  if (centre == 0)
  {
    a1=a2=a3=0;
  } else 
  */
  {
  a1 = centre.x; a2 = centre.y; a3 = centre.z;
  }

  /*
  m03 = a1 - a1 * m00 - a2 * m01 - a3 * m02;
  m13 = a2 - a1 * m10 - a2 * m11 - a3 * m12;
  m23 = a3 - a1 * m20 - a2 * m21 - a3 * m22;
  m30 = m31 = m32 = 0.0;
  m33 = 1.0;
  */

  m.m[0][3] = a1 - a1 * m.m[0][0] - a2 * m.m[0][1] - a3 * m.m[0][2];
  m.m[1][3] = a2 - a1 * m.m[1][0] - a2 * m.m[1][1] - a3 * m.m[1][2];
  m.m[2][3] = a3 - a1 * m.m[2][0] - a2 * m.m[2][1] - a3 * m.m[2][2];
}

}