
#pragma once

#include <CoreFramework/Core/CoreFramework.h>
#include <CoreFramework/Core/GDZArchive.h>
#include <math.h>


namespace GODZ
{
	struct WPlane;

	static const float BIG_REAL = 0x7fc00000;

	//3D Vector Structure
	struct GODZ_API Vector3
	{
		float x, y, z;

		Vector3()
#ifdef _DEBUG
			: x(BIG_REAL), y(BIG_REAL), z(BIG_REAL)
#endif
		{
		}

		Vector3(float _x, float _y, float _z)
			: x(_x)
			, y(_y)
			, z(_z)
		{
		}

		bool IsValid() const
		{
			return x != BIG_REAL && y != BIG_REAL && z != BIG_REAL;
		}

		//--------------------------------------------------------
		//These are 'point' operations used in relation to another
		//3d point in space
		//--------------------------------------------------------

		bool operator<(const Vector3& Other) const;
		bool operator>(const Vector3& Other) const;
		bool operator<=(const Vector3& Other) const;
		bool operator>=(const Vector3& Other) const;

		//--------------------------------------------------------
		//Vector operators
		//--------------------------------------------------------

		bool operator==(const Vector3& Other) const;
		bool operator!=(const Vector3& Other) const;
		Vector3 operator+(const Vector3& V) const;
		Vector3 operator-(const Vector3& V) const;
		Vector3 operator*(const Vector3& V) const;
		Vector3 operator/(const Vector3& V) const;
		Vector3 operator/(const float v) const;
		Vector3& operator/=(const float v);
		Vector3& operator=(const Vector3& V);
		Vector3& operator+=(const Vector3& V);
		Vector3& operator+=(const float v);
		Vector3& operator-=(const Vector3& V);
		Vector3& operator-=(const float v);
		Vector3& operator*=(const Vector3& V);
		Vector3& operator*=(const float v);
		Vector3 operator*(const float v) const;
		float operator()(int index) const;
		float& operator[](int index);
		const float& operator[](int index) const;

		Vector3 operator+(float v) const
		{
			return Vector3(x + v, y + v, z + v);
		}

		Vector3 operator-(float v) const
		{
			return Vector3(x - v, y - v, z - v);
		}

		operator float*();
		operator const float*() const;

		friend GDZArchive& operator<<(GDZArchive& ar, Vector3& v)
		{
			ar << v.x << v.y << v.z;
			return ar;
		}

		//--------------------------------------------------------
		//Vector functions
		//--------------------------------------------------------

		// Cross product
		void Cross(const Vector3& vec, Vector3& result) const
		{
			result.x = (y * vec.z) - (vec.y * z);
			result.y = (z * vec.x) - (vec.z * x);
			result.z = (x * vec.y) - (vec.x * y);
		}

		Vector3 Cross(const Vector3& vec) const
		{
			return Vector3((y * vec.z) - (vec.y * z),
						   (z * vec.x) - (vec.z * x),
						   (x * vec.y) - (vec.x * y));
		}

		//Returns the dot product
		float Dot(const Vector3& v) const
		{
			return (x*v.x) + (y*v.y) + (z*v.z);
		}

		void Get(float v[3]) const
		{
			v[0] = x;
			v[1] = y;
			v[2] = z;
		}

		float Get(int index) const;
		void Set(int index, float value);

		//! Returns squared length of the vector.
		/** This is useful because it is much faster then
		getLength(). */
		float GetLengthSQ() const { return x*x + y*y + z*z; }

		// Returns the magnitude (ie length or norm) of the Vector3
		float GetLength() const { return sqrtf( (x*x) + (y*y) + (z*z) ); }

		void Invert()
		{
			x*=-1;
			y*=-1;
			z*=-1;
		}

		// Normalizes the Vector3 == v \ ||v||
		void Normalize()
		{
			float mag = GetLength();

			x /= mag;
			y /= mag;
			z /= mag;
		}

		//Reflects this vector off a plane
		void Reflect(const Vector3& Normal)
		{
			//dot(N, L) gives the scalar product between two vectors
			float dist = Normal.Dot(*this);

			//so dot(N,L) * N, creates a vector in the direction of  N,
			// 2 * dot(N, L) * N, doubles this vector
			Vector3 vecProjAlongPlane;
			vecProjAlongPlane.x = 2 * dist * Normal.x;
			vecProjAlongPlane.y = 2 * dist * Normal.y;
			vecProjAlongPlane.z = 2 * dist * Normal.z;

			//when you subtract L from this, you're left with a vector in the correct direction
			x = x - vecProjAlongPlane.x;
			y = y - vecProjAlongPlane.y;
			z = z - vecProjAlongPlane.z;
		}

		// Scales the Vector3 by the amount passed in
		void Scale(float amt)
		{
			x *= amt;
			y *= amt;
			z *= amt;
		}

		//Transforms this point by a [4][4] transformation matrix
		void TransformBy(const float* matrix)
		{
			x = x * matrix[0] + y * matrix[4] + z * matrix[8] + matrix[12];
			y = x * matrix[1] + y * matrix[5] + z * matrix[9] + matrix[13];
			z = x * matrix[2] + y * matrix[6] + z * matrix[10] + matrix[14];
		}

		void MakeZero( void )
		{
			x = 0;
			y = 0;
			z = 0;
		}

		static Vector3 Zero( void )
		{
			Vector3 tmp;
			tmp.MakeZero();
			return tmp;
		}
	};

	GODZ_API inline Vector3 operator * (float k, const Vector3& a)
	{
		return Vector3((a.x * k),(a.y * k),(a.z * k));
	}

	static const Vector3 ZERO_VECTOR(0.0f,0.0f,0.0f);
	static const Vector3 RIGHT_VECTOR(1.0f, 0.0f, 0.0f); //X+
	static const Vector3 UP_VECTOR(0.0f, 1.0f, 0.0f); //Y+
	static const Vector3 FORWARD_VECTOR(0.0f, 0.0f, 1.0f); //Z+

	//Vector comparison function. Takes into account floating point imprecision.
	GODZ_API inline bool VectorsEqual(const Vector3& a, const Vector3& b);

	GODZ_API inline float Dot(const Vector3& v1, const Vector3& v2)
	{
		return (v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z);
	}

	// Scalar Projection
	GODZ_API inline float ProjectAtoBScalar( const Vector3& A, const Vector3& B )
	{
		return ( A.Dot(B) ) / B.GetLength();
	}

	GODZ_API inline Vector3 ProjectAtoB( const Vector3& A, const Vector3& B )
	{
		return B * ( ( A.Dot(B) )/ ( B.Dot(B) ) );
	}

	GODZ_API inline Vector3 RejectAfromB( const Vector3& A, const Vector3& B )
	{
		return A - (  ( ( A.Dot(B) ) / ( B.Dot(B) ) ) * B );
	}
}
