
#include "Vector3.h"
#include "WPlane.h"

namespace GODZ
{

float Vector3::operator()(int index) const
{
	godzassert(IsValid());
	godzassert(index < 3 && index > -1);

	const float* f = &x;
	return f[index];
}

bool Vector3::operator<(const Vector3& V) const
{
	godzassert(IsValid());
	godzassert(V.IsValid());
	if (x < V.x && y < V.y && z < V.z)
	return true;

	return false;
}

bool Vector3::operator<=(const Vector3& V) const
{
	godzassert(IsValid());
	godzassert(V.IsValid());
	if (x <= V.x && y <= V.y && z <= V.z)
		return true;

	return false;
}

bool Vector3::operator>(const Vector3& V) const
{
	godzassert(IsValid());
	godzassert(V.IsValid());
	if (x > V.x && y > V.y && z > V.z)
	return true;

	return false;
}

bool Vector3::operator>=(const Vector3& V) const
{
	godzassert(IsValid());
	godzassert(V.IsValid());
	if (x >= V.x && y >= V.y && z >= V.z)
		return true;

	return false;
}

bool Vector3::operator==(const Vector3& V) const
{
	godzassert(IsValid());
	godzassert(V.IsValid());
	return VectorsEqual(*this, V);
}

bool Vector3::operator!=(const Vector3& V) const
{
	godzassert(IsValid());
	godzassert(V.IsValid());
	return !VectorsEqual(*this,V);
}

Vector3 Vector3::operator+(const Vector3& V) const
{
	godzassert(IsValid());
	godzassert(V.IsValid());
	Vector3 A=*this;
	A.x += V.x;
	A.y += V.y;
	A.z += V.z;

	return A;
}

Vector3 Vector3::operator-(const Vector3& V) const
{
	godzassert(IsValid());
	godzassert(V.IsValid());
	Vector3 A=*this;
	A.x -= V.x;
	A.y -= V.y;
	A.z -= V.z;

	return A;
}

Vector3 Vector3::operator*(const Vector3& V) const
{
	godzassert(IsValid());
	godzassert(V.IsValid());
	Vector3 A=*this;
	A.x *= V.x;
	A.y *= V.y;
	A.z *= V.z;

	return A;
}

Vector3 Vector3::operator*(const float v) const
{
	godzassert(IsValid());
	Vector3 A=*this;
	A.x *= v;
	A.y *= v;
	A.z *= v;

	return A;
}

Vector3 Vector3::operator/(const Vector3& V) const
{
	godzassert(IsValid());
	godzassert(V.IsValid());
	Vector3 A=*this;
	A.x /= V.x;
	A.y /= V.y;
	A.z /= V.z;

	return A;
}

Vector3 Vector3::operator/(const float v) const
{
	godzassert(IsValid());

	Vector3 A=*this;
	float div = 1/v;
	A.x *= div;
	A.y *= div;
	A.z *= div;

	return A;
}

Vector3& Vector3::operator/=(const float scalar)
{
	godzassert(IsValid());

	this->x /= scalar;
	this->y /= scalar;
	this->z /= scalar;

	return *this;
}

Vector3& Vector3::operator=(const Vector3& V)
{
	godzassert(V.IsValid());
	this->x = V.x;
	this->y = V.y;
	this->z = V.z;

	return *this;
}

Vector3& Vector3::operator+=(const Vector3& V)
{
	godzassert(IsValid());
	godzassert(V.IsValid());
	this->x += V.x;
	this->y += V.y;
	this->z += V.z;

	return *this;
}

Vector3& Vector3::operator*=(const Vector3& V)
{
	godzassert(IsValid());
	godzassert(V.IsValid());
	this->x *= V.x;
	this->y *= V.y;
	this->z *= V.z;

	return *this;
}

Vector3& Vector3::operator-=(const Vector3& V)
{
	godzassert(IsValid());
	godzassert(V.IsValid());
	this->x -= V.x;
	this->y -= V.y;
	this->z -= V.z;

	return *this;
}

Vector3& Vector3::operator-=(const float v)
{
	godzassert(IsValid());
	
	this->x -= v;
	this->y -= v;
	this->z -= v;

	return *this;
}

Vector3& Vector3::operator*=(const float v)
{
	godzassert(IsValid());

	this->x *= v;
	this->y *= v;
	this->z *= v;

	return *this;
}

Vector3::operator float*()
{
	godzassert(IsValid());
	return &this->x;
}

Vector3::operator const float*() const
{
	godzassert(IsValid());
	return &this->x;
}

Vector3& Vector3::operator+=(const float v)
{
	godzassert(IsValid());

	this->x += v;
	this->y += v;
	this->z += v;

	return *this;
}


float Vector3::Get(int index) const
{
	godzassert(IsValid());
	godzassert(index < 3 && index > -1);

	const float* f = &x;
	return f[index];
}

void Vector3::Set(int index, float value)
{
	float* f = &x;
	f[index] = value;
}


float& Vector3::operator[] (int index)
{
	float* v = &x;
	return v[index];
}

const float& Vector3::operator[] (int index) const
{
	godzassert(IsValid());

	const float* v = &x;
	return v[index];
}

bool VectorsEqual(const Vector3& a, const Vector3& b)
{
	godzassert(a.IsValid());
	godzassert(b.IsValid());

	float x = fabsf(a.x - b.x);
	if (x>EPSILON)
		return false;
	float y = fabsf(a.y - b.y);
	if (y>EPSILON)
		return false;
	float z = fabsf(a.z - b.z);
	if (z>EPSILON)
		return false;

	return true;
}

}