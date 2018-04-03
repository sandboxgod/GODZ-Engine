/* ===================================================================
	Defines Matrix classes

	Sources Cited:

	[1] Elementary Linear Algebra: Applications Version by Howard Anton
	and Chris Rorres

	[2] Introduction to 3D Game Programming with DirectX 9.0 
	by Frank D. Luna

	[3] http://mathworld.wolfram.com/EulerAngles.html

	[4] http://www.gamasutra.com/features/20000131/barad_01.htm

	[5] http://www.gamedev.net/reference/articles/article695.asp

	[6] Tips On Matrix Decomposition -
	http://www.flipcode.com/cgi-bin/msg.cgi?showThread=00008607&forum=3dtheory&id=-1

	Created May 18, 2003 by Richard Osborne
	Copyright (c) 2010
	==================================================================
*/

#if !defined(_WMATRIX_H_)
#define _WMATRIX_H_

#include <math.h>
#include "Vector3.h"
#include "Vector4.h"
#include "ProjectionMatrixInfo.h"
#include <memory.h>
#include <xmmintrin.h>
#include <fvec.h>

namespace GODZ
{
	struct WMatrix3;

	struct MatrixRow
	{
		float x,y,z,w;
	};

	//scalar matrix definition
	struct GODZ_API WMatrix
	{
		union
		{
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			float m[4][4];
			float m_afEntry[16];
			MatrixRow m_row[4];
		};

		WMatrix()
		{
		}

		//creates a uniform scale matrix (or Identity == 1)
		WMatrix(float scale)
			: _11(scale), _12(0), _13(0), _14(0)
			, _21(0), _22(scale), _23(0), _24(0)
			, _31(0), _32(0), _33(scale), _34(0)
			, _41(0), _42(0), _43(0), _44(1)
		{
		}

		WMatrix(float f11,float f12, float f13, float f14,
			float f21, float f22, float f23, float f24,
			float f31, float f32, float f33, float f34,
			float f41, float f42, float f43, float f44)
			: _11(f11), _12(f12), _13(f13), _14(f14), 
			_21(f21), _22(f22), _23(f23), _24(f24),
			_31(f31), _32(f32), _33(f33), _34(f34), 
			_41(f41), _42(f42), _43(f43), _44(f44)
		{
		}

		WMatrix(const WMatrix3& m);

		static WMatrix Identity( void )
		{
			WMatrix m;
			m.MakeIdentity();
			return m;
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

		//returns the translation of this matrix
		Vector3 GetTranslation() const
		{
			return Vector3(_41, _42, _43);
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

		//Based on David Eberly's implementation - http://www.magic-software.com/Math.html
		bool Inverse (WMatrix& kInv) const
		{
			float fA0 = m_afEntry[ 0]*m_afEntry[ 5] - m_afEntry[ 1]*m_afEntry[ 4];
		    float fA1 = m_afEntry[ 0]*m_afEntry[ 6] - m_afEntry[ 2]*m_afEntry[ 4];
		    float fA2 = m_afEntry[ 0]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 4];
		    float fA3 = m_afEntry[ 1]*m_afEntry[ 6] - m_afEntry[ 2]*m_afEntry[ 5];
		    float fA4 = m_afEntry[ 1]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 5];
		    float fA5 = m_afEntry[ 2]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 6];
		    float fB0 = m_afEntry[ 8]*m_afEntry[13] - m_afEntry[ 9]*m_afEntry[12];
		    float fB1 = m_afEntry[ 8]*m_afEntry[14] - m_afEntry[10]*m_afEntry[12];
		    float fB2 = m_afEntry[ 8]*m_afEntry[15] - m_afEntry[11]*m_afEntry[12];
		    float fB3 = m_afEntry[ 9]*m_afEntry[14] - m_afEntry[10]*m_afEntry[13];
		    float fB4 = m_afEntry[ 9]*m_afEntry[15] - m_afEntry[11]*m_afEntry[13];
		    float fB5 = m_afEntry[10]*m_afEntry[15] - m_afEntry[11]*m_afEntry[14];

			float fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
			if ( fabsf(fDet) <= EPSILON )
			{
				return false;
			}

		    kInv.m[0][0] = + m_afEntry[ 5]*fB5 - m_afEntry[ 6]*fB4 + m_afEntry[ 7]*fB3;
		    kInv.m[1][0] = - m_afEntry[ 4]*fB5 + m_afEntry[ 6]*fB2 - m_afEntry[ 7]*fB1;
		    kInv.m[2][0] = + m_afEntry[ 4]*fB4 - m_afEntry[ 5]*fB2 + m_afEntry[ 7]*fB0;
		    kInv.m[3][0] = - m_afEntry[ 4]*fB3 + m_afEntry[ 5]*fB1 - m_afEntry[ 6]*fB0;
		    kInv.m[0][1] = - m_afEntry[ 1]*fB5 + m_afEntry[ 2]*fB4 - m_afEntry[ 3]*fB3;
		    kInv.m[1][1] = + m_afEntry[ 0]*fB5 - m_afEntry[ 2]*fB2 + m_afEntry[ 3]*fB1;
		    kInv.m[2][1] = - m_afEntry[ 0]*fB4 + m_afEntry[ 1]*fB2 - m_afEntry[ 3]*fB0;
		    kInv.m[3][1] = + m_afEntry[ 0]*fB3 - m_afEntry[ 1]*fB1 + m_afEntry[ 2]*fB0;
		    kInv.m[0][2] = + m_afEntry[13]*fA5 - m_afEntry[14]*fA4 + m_afEntry[15]*fA3;
		    kInv.m[1][2] = - m_afEntry[12]*fA5 + m_afEntry[14]*fA2 - m_afEntry[15]*fA1;
		    kInv.m[2][2] = + m_afEntry[12]*fA4 - m_afEntry[13]*fA2 + m_afEntry[15]*fA0;
		    kInv.m[3][2] = - m_afEntry[12]*fA3 + m_afEntry[13]*fA1 - m_afEntry[14]*fA0;
		    kInv.m[0][3] = - m_afEntry[ 9]*fA5 + m_afEntry[10]*fA4 - m_afEntry[11]*fA3;
		    kInv.m[1][3] = + m_afEntry[ 8]*fA5 - m_afEntry[10]*fA2 + m_afEntry[11]*fA1;
		    kInv.m[2][3] = - m_afEntry[ 8]*fA4 + m_afEntry[ 9]*fA2 - m_afEntry[11]*fA0;
		    kInv.m[3][3] = + m_afEntry[ 8]*fA3 - m_afEntry[ 9]*fA1 + m_afEntry[10]*fA0;

		    float fInvDet = (1.0f)/fDet;
		    for (int iRow = 0; iRow < 4; iRow++)
			{
		        for (int iCol = 0; iCol < 4; iCol++)
				{
		            kInv(iRow,iCol) *= fInvDet;
				}
			}

			return true;
		}

		//just inverse translate (not rotate)
		inline void InverseTranslateVect( Vector3& pVect ) const
		{
			pVect[0] -=_41;
			pVect[1] -=_42;
			pVect[2] -=_43;
		}

		//just translate (not rotate)
		inline void TranslateVect( Vector3& pVect ) const
		{
			pVect[0] +=_41;
			pVect[1] +=_42;
			pVect[2] +=_43;
		}

		void MakeIdentity()
		{
			_11 = 1;
			_22 = 1;
			_33 = 1;
			_44 = 1;

			_12 = 0;
			_13 = 0;
			_14 = 0;

			_21 = 0;
			_23 = 0;
			_24 = 0;

			_31 = 0;
			_32 = 0;
			_34 = 0;

			_41 = 0;
			_42 = 0;
			_43 = 0;
		}

		//transforms the vector relative to this matrix's reference frame (4x4 mul)
		void Mul(Vector3& v) const
		{
			float x0 = v.x, y0 = v.y, z0 = v.z;

			//multiplies vec by the right, up, forward, and translation components of the matrix
	        v.x = x0 * m[0][0] + y0 * m[1][0] + z0 * m[2][0] + m[3][0];
		    v.y = x0 * m[0][1] + y0 * m[1][1] + z0 * m[2][1] + m[3][1];
			v.z = x0 * m[0][2] + y0 * m[1][2] + z0 * m[2][2] + m[3][2];
		}

		//transforms the vector relative to this matrix's reference frame (4x4 mul)
		void Mul(Vector4& v) const
		{
			float x0 = v.x, y0 = v.y, z0 = v.z, w0 = v.w;

			//multiplies vec by the right, up, forward, and translation components of the matrix
	        v.x = x0 * m[0][0] + y0 * m[1][0] + z0 * m[2][0] + w0 * m[3][0];
		    v.y = x0 * m[0][1] + y0 * m[1][1] + z0 * m[2][1] + w0 * m[3][1];
			v.z = x0 * m[0][2] + y0 * m[1][2] + z0 * m[2][2] + w0 * m[3][2];
			v.w = x0 * m[0][3] + y0 * m[1][3] + z0 * m[2][3] + w0 * m[3][3];
		}

		//Sets the translation parts of this matrix
		void PreTranslate(const Vector3& v)
		{
			_41 = v[0];
			_42 = v[1];
			_43 = v[2];
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

		void RotateX(float angle)
		{
			m_afEntry[5] =cosf(angle);
			m_afEntry[6] =sinf(angle);
			m_afEntry[9] = -sinf(angle);
			m_afEntry[10] = cosf(angle);
		}

		void RotateY(float angle)
		{
			m_afEntry[0] =cosf(angle);
			m_afEntry[2] =-sinf(angle);
			m_afEntry[8] = sinf(angle);
			m_afEntry[10] = cosf(angle);
		}

		void RotateZ(float angle)
		{
			m_afEntry[0] =cosf(angle);
			m_afEntry[1] =sinf(angle);
			m_afEntry[4] = -sinf(angle);
			m_afEntry[5] = cosf(angle);
		}
		
		//Rotates vertex using Right, Up, Forward parts of this matrix
		void RotateVect(Vector3& v) const
		{
			float vec[3];
			vec[0] = (_11 * v[0]) + (_12 * v[1]) + (_13 * v[2]);
			vec[1] = (_21 * v[0]) + (_22 * v[1]) + (_23 * v[2]);
			vec[2] = (_31 * v[0]) + (_32 * v[1]) + (_33 * v[2]);

			v[0] = vec[0];
			v[1] = vec[1];
			v[2] = vec[2];
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

		void SetInvMatrixTranslation(const Vector3& v)
		{
			_41 = -v.x;
			_42 = -v.y;
			_43 = -v.z;
		}

		void SetTranslation(const Vector3& vec)
		{
			_41 = vec.x;
			_42 = vec.y;
			_43 = vec.z;
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

		//sets the scale component of this matrix
		void SetScale(const Vector3& scale)
		{
			_11 = scale.x;
			_22 = scale.y;
			_33 = scale.z;
		}

		void Transpose()
		{
			WMatrix copy = *this;

			for(int i=0;i<4;i++)
			{
				for (int j=0;j<4;j++)
				{
					m[j][i] = copy.m[i][j];
				}
			}
		}

		WMatrix& operator*=(const WMatrix& other)
		{
			WMatrix newMatrix = (*this) * other;
			(*this) = newMatrix;
			return *this;
		}

		//matrix mul routine
		WMatrix WMatrix::operator*(const WMatrix& other) const
		{
			WMatrix newMatrix;

			for (int i = 0; i < 4; i++) // rows
			{
				for (int j=0;j<4;j++) //each column
				{
					float val = 0;
					for (int k=0;k<4;k++) //iterate through other matrix
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

		bool operator==(WMatrix& other) const
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j=0;j<4;j++)
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

		friend GDZArchive& operator<<(GDZArchive& ar, WMatrix& m) 
		{
			ar << m._11 << m._12 << m._13 << m._14;
			ar << m._21 << m._22 << m._23 << m._24;
			ar << m._31 << m._32 << m._33 << m._34;
			ar << m._41 << m._42 << m._43 << m._44;
			return ar;
		}
	};


	GODZ_API inline void WMatrixMul(const WMatrix& a, const WMatrix& b, WMatrix& result);

	//computes a left handed view matrix
	GODZ_API void ComputeViewMatrix(const Vector3& eye, const Vector3& lookAt, const Vector3& up, WMatrix& mat);

	//Builds a left-handed perspective projection matrix based on a field of view.
	GODZ_API void ComputePerspectiveFovLH(const ProjectionMatrixInfo& projMatInfo, WMatrix& mat);

	//Builds a customized, left-handed orthographic projection matrix. Note: Identity matrix expected to be passed in
	GODZ_API void ComputeMatrixOrthoOffCenterLH(float viewBoxMinX, float viewBoxMaxX, float viewBoxMinY, float viewBoxMaxY, float viewBoxMinZ, float viewBoxMaxZ, WMatrix& mat);


	typedef WMatrix WMatrix16f;
	typedef WMatrix Matrix4;
	static const WMatrix16f IDENTITY_MATRIX(1);
}

#endif
