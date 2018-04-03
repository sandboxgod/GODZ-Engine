
#pragma once

#include "WMatrix3.h"
#include "WMatrix.h"
#include "Quaternion.h"

namespace GODZ
{
// O is your object's position
// P is the position of the object to face
// U is the nominal "up" vector (typically Vector3.Y)
// Note: this does not work when O is straight below or straight above P
//http://forums.xna.com/forums/t/10203.aspx
GODZ_API WMatrix3 RotateToFace(const Vector3& O, const Vector3& P, const Vector3& U);

// Builds a rotation matrix that will rotate an object around a point
GODZ_API void RotateAroundPoint(const Quaternion& q, const Vector3& centre, WMatrix& m);
}
