
#pragma once

#include "WMatrix.h"
#include <math.h>
#include <memory.h>
#include <xmmintrin.h>
#include <fvec.h>

namespace GODZ
{

	//scalar matrix definition
	struct GODZ_API WMatrix3
	{
		union
		{
			struct
			{
				float _11, _12, _13;
				float _21, _22, _23;
				float _31, _32, _33;
			};
			float m[3][3];
			float m_afEntry[9];
		};

		WMatrix3()
		{
		}

		WMatrix3(const WMatrix16f& m)
			: _11(m._11), _12(m._12), _13(m._13)
			, _21(m._21), _22(m._22), _23(m._23)
			, _31(m._31), _32(m._32), _33(m._33)
		{
		}

		//creates an identity matrix by default. Otherwise, a uniform scale matrix is created.
		WMatrix3(float scale)
			: _11(scale), _12(0), _13(0),
			_21(0), _22(scale), _23(0),
			_31(0), _32(0), _33(scale)
		{
		}

		WMatrix3(const Vector3& X, const Vector3& Y, const Vector3& Z)
		{
			_11=X.x;
			_12=X.y;
			_13=X.z;

			_21=Y.x;
			_22=Y.y;
			_23=Y.z;

			_31 = Z.x;
			_32 = Z.y;
			_33 = Z.z;
		}

		static WMatrix3 Identity( void )
		{
			WMatrix3 m;
			m.MakeIdentity();
			return m;
		}

		friend GDZArchive& operator<<(GDZArchive& ar, WMatrix3& m) 
		{
			ar << m._11 << m._12 << m._13;
			ar << m._21 << m._22 << m._23;
			ar << m._31 << m._32 << m._33;
			return ar;
		}

		float Get( udword Row, udword Col ) const
		{
			return m[Row][Col];
		}

		//return the right components of the axis vectors
		Vector3 GetRight() const
		{
			return Vector3(_11, _12, _13);
		}

		//return the up components of the axis vectors
		Vector3 GetUp() const
		{
			return Vector3(_21, _22, _23);
		}

		//return the forward components of the axis vectors
		Vector3 GetForward() const
		{
			return Vector3(_31, _32, _33);
		}

		//returns the X, Y, and Z vectors of this matrix
		void GetAxes(Vector3& X, Vector3& Y, Vector3& Z) const
		{
			X.x=_11;
			X.y=_12;
			X.z=_13;

			Y.x=_21;
			Y.y=_22;
			Y.z=_23;

			Z.x=_31;
			Z.y=_32;
			Z.z=_33;
		}

		void SetX(const Vector3& v)
		{
			_11 = v.x;
			_12 = v.y;
			_13 = v.z;
		}

		void SetY(const Vector3& v)
		{
			_21 = v.x;
			_22 = v.y;
			_23 = v.z;
		}

		void SetZ(const Vector3& v)
		{
			_31 = v.x;
			_32 = v.y;
			_33 = v.z;
		}

		void MakeIdentity()
		{
			_11=1;
			_12=0;
			_13=0;

			_21=0;
			_22=1;
			_23=0;

			_31 = 0;
			_32 = 0;
			_33 = 1;
		}

		//transforms the vector relative to this matrix's reference frame (4x4 mul)
		void Mul(Vector3& v) const
		{
			float x0 = v.x, y0 = v.y, z0 = v.z;

			//multiplies vec by the right, up, forward, and translation components of the matrix
	        v.x = x0 * m[0][0] + y0 * m[1][0] + z0 * m[2][0];
		    v.y = x0 * m[0][1] + y0 * m[1][1] + z0 * m[2][1];
			v.z = x0 * m[0][2] + y0 * m[1][2] + z0 * m[2][2];
		}

		//creates a rotation matrix based on euler angles R * P * Y
		//in the same order as DirectX.
		void Rotate(const Vector3& v)
		{
			//yaw=v[2], pitch=v[1], roll=v[0]
			float cy = cosf(v[2]);
 			float cp = cosf(v[1]);
			float cr = cosf(v[0]);
			float sp = sinf(v[1]);
			float sr = sinf(v[0]);
			float sy = sinf(v[2]);
			
			_11  = cy * cr+ sr * sp * sy;
			_12 = sr * cp;
			_13 = cr * -sy + sr * sp * cy;
			_21 = -sr * cy + cr * sp * sy;
			_22 = cr * cp;
			_23 = -sr * -sy + cr * sp * cy;
			_31 = cp * sy;
			_32 = -sp;
			_33 = cy * cp;
		}

		/* ========================================================== */
		/* compute adjoint of matrix and scale
		 *
		 * Computes adjoint of matrix m, scales it by s, returning a
		 */
		 
		static void SCALE_ADJOINT_3X3(WMatrix3& a, float s, const WMatrix3& mat)
		{
		   a.m[0][0] = (s) * (mat.m[1][1] * mat.m[2][2] - mat.m[1][2] * mat.m[2][1]);
		   a.m[1][0] = (s) * (mat.m[1][2] * mat.m[2][0] - mat.m[1][0] * mat.m[2][2]);
		   a.m[2][0] = (s) * (mat.m[1][0] * mat.m[2][1] - mat.m[1][1] * mat.m[2][0]);

		   a.m[0][1] = (s) * (mat.m[0][2] * mat.m[2][1] - mat.m[0][1] * mat.m[2][2]);
		   a.m[1][1] = (s) * (mat.m[0][0] * mat.m[2][2] - mat.m[0][2] * mat.m[2][0]);
		   a.m[2][1] = (s) * (mat.m[0][1] * mat.m[2][0] - mat.m[0][0] * mat.m[2][1]);

		   a.m[0][2] = (s) * (mat.m[0][1] * mat.m[1][2] - mat.m[0][2] * mat.m[1][1]);
		   a.m[1][2] = (s) * (mat.m[0][2] * mat.m[1][0] - mat.m[0][0] * mat.m[1][2]);
		   a.m[2][2] = (s) * (mat.m[0][0] * mat.m[1][1] - mat.m[0][1] * mat.m[1][0]);
		}

		/* ========================================================== */
		/* adjoint of matrix
		 *
		 * Computes adjoint of matrix m, returning a
		 * (Note that adjoint is just the transpose of the cofactor matrix)
		 */
		 
		 
		static void ADJOINT_3X3(WMatrix3& a, const WMatrix3& mat)
		{
		   a.m[0][0] = mat.m[1][1]*mat.m[2][2] - mat.m[1][2]*mat.m[2][1];
		   a.m[1][0] = - (mat.m[1][0]*mat.m[2][2] - mat.m[2][0]*mat.m[1][2]);
		   a.m[2][0] = mat.m[1][0]*mat.m[2][1] - mat.m[1][1]*mat.m[2][0];
		   a.m[0][1] = - (mat.m[0][1]*mat.m[2][2] - mat.m[0][2]*mat.m[2][1]);
		   a.m[1][1] = mat.m[0][0]*mat.m[2][2] - mat.m[0][2]*mat.m[2][0];
		   a.m[2][1] = - (mat.m[0][0]*mat.m[2][1] - mat.m[0][1]*mat.m[2][0]);
		   a.m[0][2] = mat.m[0][1]*mat.m[1][2] - mat.m[0][2]*mat.m[1][1];
		   a.m[1][2] = - (mat.m[0][0]*mat.m[1][2] - mat.m[0][2]*mat.m[1][0]);
		   a.m[2][2] = (mat.m[0][0]*mat.m[1][1] - mat.m[0][1]*mat.m[1][0]);
		}



		/* ========================================================== */
		/* determinant of matrix
		 *
		 * Computes determinant of matrix m, returning d
		 */
		 
		static float DETERMINANT_3X3(const WMatrix3& mat)
		{
		   float d = mat.m[0][0] * (mat.m[1][1]*mat.m[2][2] - mat.m[1][2] * mat.m[2][1]);
		   d -= mat.m[0][1] * (mat.m[1][0]*mat.m[2][2] - mat.m[1][2] * mat.m[2][0]);
		   d += mat.m[0][2] * (mat.m[1][0]*mat.m[2][1] - mat.m[1][1] * mat.m[2][0]);
		   return d;
		}


		//computes a matrix that is rotated around an arbitrary axis
		void RotateAroundAxis(const Vector3& axis, float angle)
		{
			float ca = cosf(angle);
			float sa = sinf(angle);

			m[0][0] = (1.0f - ca) * axis.x * axis.x + ca;
			m[1][0] = (1.0f - ca) * axis.x * axis.y - sa * axis.z;
			m[2][0] = (1.0f - ca) * axis.x * axis.z + sa * axis.y;
			m[0][1] = (1.0f - ca) * axis.y * axis.x + sa * axis.z;
			m[1][1] = (1.0f - ca) * axis.y * axis.y + ca;
			m[2][1] = (1.0f - ca) * axis.y * axis.z - sa * axis.x;
			m[0][2] = (1.0f - ca) * axis.z * axis.x - sa * axis.y;
			m[1][2] = (1.0f - ca) * axis.z * axis.y + sa * axis.x;
			m[2][2] = (1.0f - ca) * axis.z * axis.z + ca;
		}

		//sets the scale component of this matrix
		void SetScale(const Vector3& scale)
		{
			_11 = scale.x;
			_22 = scale.y;
			_33 = scale.z;
		}

		void Transpose()
		{
			WMatrix3 copy = *this;

			for(int i=0;i<3;i++)
			{
				for (int j=0;j<3;j++)
				{
					m[j][i] = copy.m[i][j];
				}
			}
		}

		bool Inverse(WMatrix3& m)
		{
			float det;

			//Inverse equation from http://developer.apple.com/library/mac/#samplecode/glut/Listings/gle_vvector_h.html
			float tmp;
			det = DETERMINANT_3X3(*this);
			tmp = 1.0f / (det);
			SCALE_ADJOINT_3X3(m, tmp, *this);

			if ( fabsf(det) <= EPSILON )
			{
				return false;
			}

			return true;
		}

		WMatrix3& operator*=(const WMatrix3& other)
		{
			WMatrix3 newMatrix = (*this) * other;
			(*this) = newMatrix;
			return *this;
		}

		//matrix mul routine
		WMatrix3 WMatrix3::operator*(const WMatrix3& other) const
		{
			WMatrix3 newMatrix;

			for (int i = 0; i < 3; i++) // rows
			{
				for (int j=0;j<3;j++) //each column
				{
					float val = 0;
					for (int k=0;k<3;k++) //iterate through other matrix
					{
						val += m[i][k] * other.m[k][j];
					}

					newMatrix.m[i][j] = val;
				}
			}
			return newMatrix;
		}

		operator float*()
		{
			return &_11;
		}

		operator const float*() const
		{
			return &_11;
		}

		const WMatrix3& operator=(const WMatrix& tm)
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j=0;j<3;j++)
				{
					m[i][j] = tm.m[i][j];
				}
			}

			return *this;
		}

		bool operator==(WMatrix3& other) const
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j=0;j<3;j++)
				{
					float val =  fabsf(m[i][j] - other.m[i][j]);
					if (val>EPSILON)
						return 0;
				}
			}

			return true;
		}

		//direct access
		float& operator () ( UInt32 Row, UInt32 Col )
		{
			return m[Row][Col];
		}

		float operator () ( UInt32 Row, UInt32 Col ) const
		{
			return m[Row][Col];
		}
	};

	static void CopyMatrix(const WMatrix3& m, WMatrix16f& mat)
	{
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				mat(i,j) = m(i,j);
			}
		}
	}

	typedef WMatrix3 Matrix3;
}
