
/***********************************************************************************
	FileName: 	vec2f.h
	Author:		Igor Kravtchenko
	
	Info:		
************************************************************************************/

#ifndef OZMATH_VEC2F_H
#define OZMATH_VEC2F_H

#include "math.h"

namespace ozlib
{


class Vec2f {
public:

	inline  Vec2f()
	{
	}

	inline  Vec2f(float _x, float _y) : x(_x), y(_y)
	{
	}

	inline  Vec2f operator - () const
	{
		return Vec2f(-x, -y);
	}

	inline  void operator -= (const Vec2f &_v)
	{
		x -= _v.x;
		y -= _v.y;
	}

	inline  void operator += (const Vec2f &_v)
	{
		x += _v.x;
		y += _v.y;
	}

	inline  void operator *= (float _mul)
	{
		x *= _mul;
		y *= _mul;
	}

	inline  void operator *= (const Vec2f &_v)
	{
		x *= _v.x;
		y *= _v.y;
	}

	inline  void operator /= (float _div)
	{
		float mul = 1.0f / _div;
		x *= mul;
		y *= mul;
	}

	inline  Vec2f operator - (const Vec2f &_v) const
	{
		return Vec2f(x - _v.x, y - _v.y);
	}

	inline  Vec2f operator + (const Vec2f &_v) const
	{
		return Vec2f(x + _v.x, y + _v.y);
	}
	
	inline  Vec2f operator * (const Vec2f &_v) const
	{
		return Vec2f(x * _v.x, y * _v.y);
	}
	
	inline  Vec2f operator * (float _m) const
	{
		return Vec2f(x * _m, y * _m);
	}

	inline  Vec2f operator / (const Vec2f &_v) const
	{
		return Vec2f(x / _v.x, y / _v.y);
	}

	inline  Vec2f operator / (float _d) const
	{
		float m = 1.0f / _d;
		return Vec2f(x * m, y * m);
	}

	inline  Vec2f operator | (const Vec2f &_d) const
	{
		return Vec2f(y * _d.y - x * _d.y, // FIXME!
			y * _d.x - x * _d.y);
	}

	inline  bool operator == (const Vec2f &_v) const
	{
		if (x == _v.x && y == _v.y)
			return true;
		return false;
	}

	inline  bool operator != (const Vec2f &_v) const
	{
		if (x != _v.x || y != _v.y)
			return true;
		return false;
	}

	inline  float operator [] (int _i) const
	{
		const float *val = &x;
		return val[_i];
	}

	inline  float len() const
	{
		float len = x * x + y * y;
		return (float) sqrt(len);
	}

	inline  float lenSq() const
	{
		return x * x + y * y;
	}

	inline  float dot(const Vec2f &_v) const
	{
		return x * _v.x + y * _v.y;
	}

	inline  void normalize()
	{
		float ln = len();
		if (!ln)
			return;
		float div = 1.0f / ln;
		x *= div;
		y *= div;
	}

	inline  void positive()
	{
		if (x < 0) x = -x;
		if (y < 0) y = -y;
	}

	float x, y;
};

inline  Vec2f operator * (float _m, const Vec2f &_v)
{
	return Vec2f(_v.x * _m, _v.y * _m);
}

}
#endif
