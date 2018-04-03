
#if !defined(__QUATERNION_H__)
#define __QUATERNION_H__

#include <math.h>
#include "vectormath.h" //for PI
#include "WMatrix.h"
#include "WMatrix3.h"
#include "Vector4.h"
#include "Vector3.h"

namespace GODZ
{
	struct GODZ_API Quaternion
	{
		//defines imaginary parts (xyz), real part is (w)
		Vector4 q;


		Quaternion()
		{}

		Quaternion(float x,float y,float z, float w)
		{
			q.x = x;
			q.y = y;
			q.z = z;
			q.w = w;
		}

		//Creates a Quat based on a Rotation matrix (better not have scale in it!)
		Quaternion(const WMatrix3& m);

		//Creates a Quat based on an angle around an axis
		Quaternion(const Vector3& axis, float angle)
		{
			float half_angle = angle / 2;
			float sincoeff = sinf(half_angle);

			q.x = axis.x * sincoeff;
			q.y = axis.y * sincoeff;
			q.z = axis.z * sincoeff;
			q.w = cosf(half_angle);
		}

		//Returns a quaternion multiplicative identity
		static inline Quaternion getIdentity()
		{
			return Quaternion(0,0,0,1);
		}

		//computes Quaternion based on the angle between two unit vectors
		void computeAngle(const Vector3& v1, const Vector3& v2)
		{
			q = v1.Cross(v2);
			q.w = 1.0f + (v1.Dot(v2));
			normalize();
		}

		void conjugate()
		{
			q.x = -q.x;
			q.y = -q.y;
			q.z = -q.z;
		}
		
		void fromRollPitchYaw(float roll, float pitch, float yaw)
		{
			q.x = sin( yaw / 2.0f) * cos(pitch / 2.0f) * sin(roll / 2.0f) + cos(yaw / 2.0f) * sin(pitch / 2.0f) * cos(roll / 2.0f);
			q.y = sin( yaw / 2.0f) * cos(pitch / 2.0f) * cos(roll / 2.0f) - cos(yaw / 2.0f) * sin(pitch / 2.0f) * sin(roll / 2.0f);
			q.z = cos(yaw / 2.0f) * cos(pitch / 2.0f) * sin(roll / 2.0f) - sin( yaw / 2.0f) * sin(pitch / 2.0f) * cos(roll / 2.0f);
			q.w = cos( yaw / 2.0f) * cos(pitch / 2.0f) * cos(roll / 2.0f) + sin(yaw / 2.0f) * sin(pitch / 2.0f) * sin(roll / 2.0f);
		}

		//http://willperone.net/Code/quaternion.php
		float dot(const Quaternion& quat) const
		{
			return q.x*quat.q.x + q.y*quat.q.y + q.z*quat.q.z + q.w*quat.q.w;
		}

		/*
		void fromAngles( const float* angles )
		{
			float angle;
			double sr, sp, sy, cr, cp, cy;

			angle = angles[2]*0.5f;
			sy = sin( angle );
			cy = cos( angle );
			angle = angles[1]*0.5f;
			sp = sin( angle );
			cp = cos( angle );
			angle = angles[0]*0.5f;
			sr = sin( angle );
			cr = cos( angle );

			double crcp = cr*cp;
			double srsp = sr*sp;

			x = ( float )( sr*cp*cy-cr*sp*sy );
			y = ( float )( cr*sp*cy+sr*cp*sy );
			z = ( float )( crcp*sy-srsp*cy );
			w = ( float )( crcp*cy+srsp*sy ); 
		}
		*/

		//! returns the axis and angle of this unit quaternion
		void to_axis_angle(Vector3& axis, float& angle) const
		{
			angle = acosf(q.w);

			// pre-compute to save time
			float sinf_theta_inv = 1.0/sinf(angle);

			// now the vector
			axis.x = q.x*sinf_theta_inv;
			axis.y = q.y*sinf_theta_inv;
			axis.z = q.z*sinf_theta_inv;

			// multiply by 2
			angle*=2;
		}

		void getEulerAngles(Vector3& euler) const;

		//more from http://willperone.net/Code/quaternion.php

		//! gets the length of this quaternion
		float length() const
		{ return sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w); }

		//! gets the squared length of this quaternion
		float length_squared() const
		{ return (q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w); }

		//! normalizes this quaternion
		void normalize()
		{
			float mag = length();
			q.x /= mag;
			q.y /= mag;
			q.z /= mag;
			q.w /= mag;
		}

		//! returns the normalized version of this quaternion
		Quaternion normalized() const
		{ return  *this/length(); }

		//! linear quaternion interpolation
		static Quaternion lerp(const Quaternion& q1, const Quaternion& q2, float t)
		{ return (q1*(1-t) + q2*t).normalized(); }

		//! spherical linear interpolation
		static Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t)
		{
			Quaternion q3;
			float dot = q1.dot(q2);

			/*	dot = cos(theta)
				if (dot < 0), q1 and q2 are more than 90 degrees apart,
				so we can invert one to reduce spinning	*/
			if (dot < 0)
			{
				dot = -dot;
				q3 = -q2;
			} else q3 = q2;
			
			if (dot < 0.95f)
			{
				float angle = acosf(dot);
				return (q1*sinf(angle*(1-t)) + q3*sinf(angle*t))/sinf(angle);
			} else // if the angle is small, use linear interpolation
				return lerp(q1,q3,t);
		}

		//! rotates v by this quaternion (quaternion must be unit)
		void rotate(Vector3& v) const;

		Quaternion	operator+( const Quaternion& quat ) const
		{
			return Quaternion( q.x + quat.q.x, q.y + quat.q.y, q.z + quat.q.z, q.w + quat.q.w );
		}

		Quaternion	operator-( const Quaternion& quat ) const
		{
			return Quaternion( q.x - quat.q.x, q.y - quat.q.y, q.z - quat.q.z, q.w - quat.q.w );
		}

		Quaternion	operator/( float scale ) const
		{
			return Quaternion( q.x / scale, q.y / scale, q.z / scale, q.w / scale );
		}

		//! Multiply a quaternion by another given one and return the result quaternion.
		Quaternion	operator*( const Quaternion& q2 ) const;

		Quaternion	operator*( float scale ) const
		{
			return Quaternion( q.x * scale, q.y * scale, q.z * scale, q.w * scale );
		}


		friend GDZArchive& operator<<(GDZArchive& ar, Quaternion& v)
		{
			ar << v.q.x << v.q.y << v.q.z << v.q.w;
			return ar;
		}

		const Quaternion operator -() const
		{ return Quaternion(-q.x,-q.y,-q.z, -q.w); }

		bool operator==(const Quaternion& other) const
		{
			double nx = fabsf(other.q.x - q.x);
			if (nx>EPSILON)
				return 0;
			double ny = fabsf(other.q.y - q.y);
			if (ny>EPSILON)
				return 0;
			double nz = fabsf(other.q.z - q.z);
			if (nz>EPSILON)
				return 0;
			double nw = fabsf(other.q.w - q.w);
			if (nw>EPSILON)
				return 0;

			return true;
		}
	};

	typedef Quaternion Quat4f;

	GODZ_API Quaternion* QuaternionFromMatrix(Quaternion* q, WMatrix16f& matrix);
	GODZ_API void QuaternionToMatrix(Quat4f& q, Vector3& right, Vector3& up, Vector3& forward);
	GODZ_API void QuaternionToMatrix(Quat4f& q, WMatrix16f& matrix);
	GODZ_API void QuaternionToMatrix(const Quat4f& q, WMatrix3& matrix);

}


#endif