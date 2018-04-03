/* ===========================================================
	Converts data types from one form to another!

	Created Mar 12, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_DATA_CONVERSIONS_H_)
#define _DATA_CONVERSIONS_H_

#include <CoreFramework/Core/Godz.h>
#include <CoreFramework/Math/WBox.h>
#include <CoreFramework/Math/EulerAngle.h>
#include <CoreFramework/Math/EulerAngle.h>
#include <CoreFramework/Math/WMatrix3.h>

namespace GODZ
{
	//returns the string representation of this float
	rstring GetFloatAsString(float f);

	//returns the string representation of this floating point array
	rstring GetFloatArrayAsString(const float* v, int numFloats);
	rstring GetVectorAsString(const Vector3& v);

	//returns the string representation of this rotator
	rstring GetRotatorAsString(EulerAngle& v);

	//populates a box based on a string using Bounding Box format.
	//For ex., (min=2,2,3 max=4,5,6)
	void GetStringAsBox(const char* text, WBox& b);

	//populates a float array based on the string argument
	//maxElements - max # of floats we can write to
	void GetStringAsFloatArray(const char* text, float* v, size_t maxElements);
	void GetStringAsVector(const char* text, Vector3& v);

	//populates a rotator based on the string argument
	void GetStringAsRotator(const char* text, EulerAngle& v);

	//populates a matrix based on the string argument
	void GetStringAsMatrix(const char* text, WMatrix& mat);
	void GetStringAsMatrix3(const char* text, Matrix3& mat);
}

#endif