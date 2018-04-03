
#pragma once


#include "Vector3.h"
#include "WMatrix3.h"
#include "WMatrix.h"
#include "EulerAngle.h"

//----------------------------------------------------------------------------
namespace GODZ
{

class GODZ_API EulerAngleConverter
{
public:
   static EulerAngle FromMatrix3(Matrix3 const & matrix);
   static EulerAngle FromMatrix4(Matrix4 const & matrix);
};

}