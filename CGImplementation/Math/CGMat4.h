#ifndef __CG_MAT3_H__
#define __CG_MAT3_H__

#include <Math/CGVector4.h>

namespace CGProj
{
	namespace Math
	{
		template<typename Scalar>
		class CGMat4
		{
		public:
			CGMat4() { }
			explicit CGMat4(Scalar s00, Scalar s10, Scalar s20, Scalar s30,
				Scalar s01, Scalar s11, Scalar s21, Scalar s31,
				Scalar s02, Scalar s12, Scalar s22, Scalar s32, 
				Scalar s03, Scalar s13, Scalar s23, Scalar s33)
				: m_col[0]{ s00, s10, s20, s30 },
				m_col[1]{ s01, s11, s21, s31 },
				m_col[2]{ s02, s12, s22, s32 },
				m_col[3]{ s03, s13, s23, s33 } { }

			explicit CGMat4(const CGVector4<Scalar>& col1,
				const CGVector4<Scalar>& col2,
				const CGVector4<Scalar>& col3,
				const CGVector4<Scalar>& col4)
				: m_col{ col1, col2, col3, col4 } { }

			explicit CGMat4(const CGMat4<Scalar>& m)
				: m_col{ m.m_col[0], m.m_col[1], m.m_col[2] } { }

			CGMat4<Scalar>& operator=(const CGMat4<Scalar>& m)
			{
				m_col[0] = m.m_col[0];
				m_col[1] = m.m_col[1];
				m_col[2] = m.m_col[2];
				m_col[3] = m.m_col[3];
				return *this;
			}

			CGMat4<Scalar> operator-()
			{
				return CGMat4(-m_col[0], -m_col[1], -m_col[2], -m_col[3]);
			}

			CGMat4<Scalar>& operator+=(const CGMat4<Scalar>& m)
			{
				m_col[0] += m.m_col[0];
				m_col[1] += m.m_col[1];
				m_col[2] += m.m_col[2];
				m_col[3] += m.m_col[3];
				return *this;
			}

			CGMat4<Scalar>& operator-=(const CGMat4<Scalar>& m)
			{
				m_col[0] -= m.m_col[0];
				m_col[1] -= m.m_col[1];
				m_col[2] -= m.m_col[2];
				m_col[3] -= m.m_col[3];
				return *this;
			}

			CGMat4<Scalar>& operator*=(const CGMat4<Scalar>& t)
			{
				const Scalar s00 = m_col[0][0] * t.m_col[0][0] +
					m_col[1][0] * t.m_col[0][1] +
					m_col[2][0] * t.m_col[0][2] +
					m_col[3][0] * t.m_col[0][3];
				const Scalar s10 = m_col[0][1] * t.m_col[0][0] +
					m_col[1][1] * t.m_col[0][1] +
					m_col[2][1] * t.m_col[0][2] + 
					m_col[3][1] * t.m_col[0][3];
				const Scalar s20 = m_col[0][2] * t.m_col[0][0] +
					m_col[1][2] * t.m_col[0][1] +
					m_col[2][2] * t.m_col[0][2] +
					m_col[3][2] * t.m_col[0][3];
				const Scalar s30 = m_col[0][3] * t.m_col[0][0] +
					m_col[1][3] * t.m_col[0][1] +
					m_col[2][3] * t.m_col[0][2] +
					m_col[3][3] * t.m_col[0][3];
				
				const Scalar s01 = m_col[0][0] * t.m_col[1][0] +
					m_col[1][0] * t.m_col[1][1] +
					m_col[2][0] * t.m_col[1][2] +
					m_col[3][0] * t.m_col[1][3];
				const Scalar s11 = m_col[0][1] * t.m_col[1][0] +
					m_col[1][1] * t.m_col[1][1] +
					m_col[2][1] * t.m_col[1][2] +
					m_col[3][1] * t.m_col[1][3];
				const Scalar s21 = m_col[0][2] * t.m_col[1][0] +
					m_col[1][2] * t.m_col[1][1] +
					m_col[2][2] * t.m_col[1][2] +
					m_col[3][2] * t.m_col[1][3];
				const Scalar s31 = m_col[0][3] * t.m_col[1][0] +
					m_col[1][3] * t.m_col[1][1] +
					m_col[2][3] * t.m_col[1][2] +
					m_col[3][3] * t.m_col[1][3];

				const Scalar s02 = m_col[0][0] * t.m_col[2][0] +
					m_col[1][0] * t.m_col[2][1] +
					m_col[2][0] * t.m_col[2][2] +
					m_col[3][0] * t.m_col[3][2];
				const Scalar s12 = m_col[0][1] * t.m_col[2][0] +
					m_col[1][1] * t.m_col[2][1] +
					m_col[2][1] * t.m_col[2][2] +
					m_col[3][1] * t.m_col[2][3];
				const Scalar s22 = m_col[0][2] * t.m_col[2][0] +
					m_col[1][2] * t.m_col[2][1] +
					m_col[2][2] * t.m_col[2][2] +
					m_col[3][2] * t.m_col[2][3];
				const Scalar s32 = m_col[0][3] * t.m_col[2][0] +
					m_col[1][3] * t.m_col[2][1] +
					m_col[2][3] * t.m_col[2][2] +
					m_col[3][3] * t.m_col[2][3];
				
				const Scalar s03 = m_col[0][0] * t.m_col[3][0] +
					m_col[1][0] * t.m_col[3][1] +
					m_col[2][0] * t.m_col[3][2] +
					m_col[3][0] * t.m_col[3][3];
				const Scalar s13 = m_col[0][1] * t.m_col[3][0] +
					m_col[1][1] * t.m_col[3][1] +
					m_col[2][1] * t.m_col[3][2] +
					m_col[3][1] * t.m_col[3][3];
				const Scalar s23 = m_col[0][2] * t.m_col[3][0] +
					m_col[1][2] * t.m_col[3][1] +
					m_col[2][2] * t.m_col[3][2] +
					m_col[3][2] * t.m_col[3][3];
				const Scalar s33 = m_col[0][3] * t.m_col[3][0] +
					m_col[1][3] * t.m_col[3][1] +
					m_col[2][3] * t.m_col[3][2] +
					m_col[3][3] * t.m_col[3][3];

				m_col[0] = { s00, s10, s20, s30 };
				m_col[1] = { s01, s11, s21, s31 };
				m_col[2] = { s02, s12, s22, s32 };
				m_col[3] = { s03, s13, s23, s33 };
				return *this;
			}

			CGMat4<Scalar>& operator/=(const CGMat4<Scalar>& m)
			{
				m_col[0] /= m.m_col[0];
				m_col[1] /= m.m_col[1];
				m_col[2] /= m.m_col[2];
				m_col[3] /= m.m_col[3];
				return *this;
			}

			CGMat4<Scalar>& operator+=(Scalar s)
			{
				m_col[0] += s;
				m_col[1] += s;
				m_col[2] += s;
				m_col[3] += s;
				return *this;
			}

			CGMat4<Scalar>& operator-=(Scalar s)
			{
				m_col[0] -= s;
				m_col[1] -= s;
				m_col[2] -= s;
				m_col[3] -= s;
				return *this;
			}

			CGMat4<Scalar>& operator*=(Scalar s)
			{
				m_col[0] *= s;
				m_col[1] *= s;
				m_col[2] *= s;
				m_col[3] *= s;
				return *this;
			}

			CGMat4<Scalar>& operator/=(Scalar s)
			{
				m_col[0] /= s;
				m_col[1] /= s;
				m_col[2] /= s;
				m_col[3] /= s;
				return *this;
			}

			CGMat4<Scalar>& operator++()
			{
				++m_col[0]; ++m_col[1]; ++m_col[2]; ++m_col[3];
				return *this;
			}

			CGMat4<Scalar>& operator--()
			{
				--m_col[0]; --m_col[1]; --m_col[2]; --m_col[3];
				return *this;
			}

			CGMat4<Scalar>& operator++(int)
			{
				CGMat4<Scalar> r(*this);
				++m_col[0]; ++m_col[1]; ++m_col[2]; ++m_col[3];
				return r;
			}

			CGMat4<Scalar>& operator--(int)
			{
				CGMat4<Scalar> r(*this);
				--m_col[0]; --m_col[1]; --m_col[2]; --m_col[3];
				return r;
			}

			CGVector4<Scalar>& operator[](unsigned i)
			{
				CG_DEBUG_ASSERT(i >= 0 && i <= 3);
				return m_col[i];
			}

			CGVector4<Scalar> operator[](unsigned i) const
			{
				CG_DEBUG_ASSERT(i >= 0 && i <= 3);
				return m_col[i];
			}

			Scalar* data() { return m_col; }
			const Scalar* data() const { return m_col; }

			CGVector4<Scalar> m_col[4];
		};

		template<typename Scalar>
		inline CGMat4<Scalar> operator*(const CGMat4<Scalar>& m, Scalar s)
		{
			return CGMat4<Scalar>(m.m_col[0] * s, m.m_col[1] * s, m.m_col[2] * s, m.m_col[3] * s);
		}

		template<typename Scalar>
		inline CGMat4<Scalar> operator*(Scalar s, const CGMat4<Scalar>& m)
		{
			return CGMat4<Scalar>(s * m.m_col[0], s * m.m_col[1], s * m.m_col[2], s * m.m_col[3]);
		}

		template<typename Scalar>
		inline CGMat4<Scalar> operator+(const CGMat4<Scalar>& a, const CGMat4<Scalar>& b)
		{
			return CGMat4<Scalar>(a.m_col[0] + b.m_col[0], a.m_col[1] + b.m_col[1], 
				a.m_col[2] + b.m_col[2], a.m_col[3] + b.m_col[3]);
		}

		template<typename Scalar>
		inline CGMat4<Scalar> operator-(const CGMat4<Scalar>& a, const CGMat4<Scalar>& b)
		{
			return CGMat4<Scalar>(a.m_col[0] - b.m_col[0], a.m_col[1] - b.m_col[1],
				a.m_col[2] - b.m_col[2], a.m_col[3] - b.m_col[3]);
		}

		template<typename Scalar>
		inline CGMat4<Scalar> operator*(const CGMat4<Scalar>& a, const CGMat4<Scalar>& b)
		{
			const Scalar s00 = a.m_col[0][0] * b.m_col[0][0] +
				a.m_col[1][0] * b.m_col[0][1] +
				a.m_col[2][0] * b.m_col[0][2] +
				a.m_col[3][0] * b.m_col[0][3];
			const Scalar s10 = a.m_col[0][1] * b.m_col[0][0] +
				a.m_col[1][1] * b.m_col[0][1] +
				a.m_col[2][1] * b.m_col[0][2] +
				a.m_col[3][1] * b.m_col[0][3];
			const Scalar s20 = a.m_col[0][2] * b.m_col[0][0] +
				a.m_col[1][2] * b.m_col[0][1] +
				a.m_col[2][2] * b.m_col[0][2] +
				a.m_col[3][2] * b.m_col[0][3];
			const Scalar s30 = a.m_col[0][3] * b.m_col[0][0] +
				a.m_col[1][3] * b.m_col[0][1] +
				a.m_col[2][3] * b.m_col[0][2] +
				a.m_col[3][3] * b.m_col[0][3];

			const Scalar s01 = a.m_col[0][0] * b.m_col[1][0] +
				a.m_col[1][0] * b.m_col[1][1] +
				a.m_col[2][0] * b.m_col[1][2] +
				a.m_col[3][0] * b.m_col[1][3];
			const Scalar s11 = a.m_col[0][1] * b.m_col[1][0] +
				a.m_col[1][1] * b.m_col[1][1] +
				a.m_col[2][1] * b.m_col[1][2] +
				a.m_col[3][1] * b.m_col[1][3];
			const Scalar s21 = a.m_col[0][2] * b.m_col[1][0] +
				a.m_col[1][2] * b.m_col[1][1] +
				a.m_col[2][2] * b.m_col[1][2] +
				a.m_col[3][2] * b.m_col[1][3];
			const Scalar s31 = a.m_col[0][3] * b.m_col[1][0] +
				a.m_col[1][3] * b.m_col[1][1] +
				a.m_col[2][3] * b.m_col[1][2] +
				a.m_col[3][3] * b.m_col[1][3];

			const Scalar s02 = a.m_col[0][0] * b.m_col[2][0] +
				a.m_col[1][0] * b.m_col[2][1] +
				a.m_col[2][0] * b.m_col[2][2] +
				a.m_col[3][0] * b.m_col[3][2];
			const Scalar s12 = a.m_col[0][1] * b.m_col[2][0] +
				a.m_col[1][1] * b.m_col[2][1] +
				a.m_col[2][1] * b.m_col[2][2] +
				a.m_col[3][1] * b.m_col[2][3];
			const Scalar s22 = a.m_col[0][2] * b.m_col[2][0] +
				a.m_col[1][2] * b.m_col[2][1] +
				a.m_col[2][2] * b.m_col[2][2] +
				a.m_col[3][2] * b.m_col[2][3];
			const Scalar s32 = a.m_col[0][3] * b.m_col[2][0] +
				a.m_col[1][3] * b.m_col[2][1] +
				a.m_col[2][3] * b.m_col[2][2] +
				a.m_col[3][3] * b.m_col[2][3];

			const Scalar s03 = a.m_col[0][0] * b.m_col[3][0] +
				a.m_col[1][0] * b.m_col[3][1] +
				a.m_col[2][0] * b.m_col[3][2] +
				a.m_col[3][0] * b.m_col[3][3];
			const Scalar s13 = a.m_col[0][1] * b.m_col[3][0] +
				a.m_col[1][1] * b.m_col[3][1] +
				a.m_col[2][1] * b.m_col[3][2] +
				a.m_col[3][1] * b.m_col[3][3];
			const Scalar s23 = a.m_col[0][2] * b.m_col[3][0] +
				a.m_col[1][2] * b.m_col[3][1] +
				a.m_col[2][2] * b.m_col[3][2] +
				a.m_col[3][2] * b.m_col[3][3];
			const Scalar s33 = a.m_col[0][3] * b.m_col[3][0] +
				a.m_col[1][3] * b.m_col[3][1] +
				a.m_col[2][3] * b.m_col[3][2] +
				a.m_col[3][3] * b.m_col[3][3];

			return CGMat4<Scalar>(s00, s10, s20, s30,
				s01, s11, s21, s31,
				s02, s12 ,s22, s32,
				s03, s13, s23, s33);
		}

		template<typename Scalar>
		inline CGMat4<Scalar> operator/(const CGMat4<Scalar>& a, const CGMat4<Scalar>& b)
		{
			return CGMat3<Scalar>(a.m_col[0] / b.m_col[0], a.m_col[1] / b.m_col[1], 
				a.m_col[2] / b.m_col[2], a.m_col[3] / b.m_col[3]);
		}

		template<typename Scalar>
		inline bool operator==(const CGMat4<Scalar>& a, const CGMat4<Scalar>& b)
		{
			return a.m_col[0] == b.m_col[0] &&
				a.m_col[1] == b.m_col[1] &&
				a.m_col[2] == b.m_col[2] &&
				a.m_col[3] == b.m_col[3];
		}

		template<typename Scalar>
		inline bool operator!=(const CGMat4<Scalar>& a, const CGMat4<Scalar>& b)
		{
			return a.m_col[0] != b.m_col[0] ||
				a.m_col[1] != b.m_col[1] ||
				a.m_col[2] != b.m_col[2] || 
				a.m_col[3] != b.m_col[3];
		}

		template<typename Scalar>
		inline CGMat4<Scalar> Transpose(const CGMat4<Scalar>& m)
		{
			return CGMat4<Scalar>(m.m_col[0][0], m.m_col[1][0], m.m_col[2][0], m.m_col[3][0],
				m.m_col[0][1], m.m_col[1][1], m.m_col[2][1], m.m_col[3][1],
				m.m_col[0][2], m.m_col[1][2], m.m_col[2][2], m.m_col[3][2],
				m.m_col[0][3], m.m_col[1][3], m.m_col[2][3], m.m_col[3][3]);
		}

		template<typename Scalar>
		inline Scalar Determinant(const CGMat4<Scalar>& m)
		{
			// https://github.com/willnode/N-Matrix-Programmer
			// I need optimized one. That's why i refer to the link above
			// My Matrix is Column Major, So I inverted the order of accessing the element.
			const Scalar A2323 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
			const Scalar A1323 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
			const Scalar A1223 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
			const Scalar A0323 = m[0][2] * m[3][3] - m[3][2] * m[0][3];
			const Scalar A0223 = m[0][2] * m[2][3] - m[2][2] * m[0][3];
			const Scalar A0123 = m[0][2] * m[1][3] - m[1][2] * m[0][3];

			return m[0][0] * (m.m[1][1] * A2323 - m.m[2][1] * A1323 + m.m[3][1] * A1223)
				- m[1][0] * (m.m[0][1] * A2323 - m.m[2][1] * A0323 + m.m[3][1] * A0223)
				+ m[2][0] * (m.m[0][1] * A1323 - m.m[1][1] * A0323 + m.m[3][1] * A0123)
				- m[3][0] * (m.m[0][1] * A1223 - m.m[1][1] * A0223 + m.m[2][1] * A0123);
		}

		template<typename Scalar>
		inline CGMat4<Scalar> Inverse(const CGMat4<Scalar>& m)
		{
			// https://github.com/willnode/N-Matrix-Programmer
			// I need optimized one. That's why i refer to the link above
			// My Matrix is Column Major, So I inverted the order of accessing the element.
			const Scalar A2323 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
			const Scalar A1323 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
			const Scalar A1223 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
			const Scalar A0323 = m[0][2] * m[3][3] - m[3][2] * m[0][3];
			const Scalar A0223 = m[0][2] * m[2][3] - m[2][2] * m[0][3];
			const Scalar A0123 = m[0][2] * m[1][3] - m[1][2] * m[0][3];
			const Scalar A2313 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
			const Scalar A1313 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
			const Scalar A1213 = m[1][1] * m[2][3] - m[2][1] * m[1][3];
			const Scalar A2312 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
			const Scalar A1312 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
			const Scalar A1212 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
			const Scalar A0313 = m[0][1] * m[3][3] - m[3][1] * m[0][3];
			const Scalar A0213 = m[0][1] * m[2][3] - m[2][1] * m[0][3];
			const Scalar A0312 = m[0][1] * m[3][2] - m[3][1] * m[0][2];
			const Scalar A0212 = m[0][1] * m[2][2] - m[2][1] * m[0][2];
			const Scalar A0113 = m[0][1] * m[1][3] - m[1][1] * m[0][3];
			const Scalar A0112 = m[0][1] * m[1][2] - m[1][1] * m[0][2];

			CGMat4<Scalar> r;
			const Scalar oneOverDeterminant = static_cast<Scalar>(1) /
				m[0][0] * (m.m[1][1] * A2323 - m.m[2][1] * A1323 + m.m[3][1] * A1223)
				- m[1][0] * (m.m[0][1] * A2323 - m.m[2][1] * A0323 + m.m[3][1] * A0223)
				+ m[2][0] * (m.m[0][1] * A1323 - m.m[1][1] * A0323 + m.m[3][1] * A0123)
				- m[3][0] * (m.m[0][1] * A1223 - m.m[1][1] * A0223 + m.m[2][1] * A0123);

			r[0][0] = (m[1][1] * A2323 - m[2][1] * A1323 + m[3][1] * A1223) * oneOverDeterminant;
			r[0][1] = -(m[0][1] * A2323 - m[2][1] * A0323 + m[3][1] * A0223) * oneOverDeterminant;
			r[0][2] = (m[0][1] * A1323 - m[1][1] * A0323 + m[3][1] * A0123) * oneOverDeterminant;
			r[0][3] = -(m[0][1] * A1223 - m[1][1] * A0223 + m[2][1] * A0123) * oneOverDeterminant;
			r[1][0] = -(m[1][0] * A2323 - m[2][0] * A1323 + m[3][0] * A1223) * oneOverDeterminant;
			r[1][1] = (m[0][0] * A2323 - m[2][0] * A0323 + m[3][0] * A0223) * oneOverDeterminant;
			r[1][2] = -(m[0][0] * A1323 - m[1][0] * A0323 + m[3][0] * A0123) * oneOverDeterminant;
			r[1][3] = (m[0][0] * A1223 - m[1][0] * A0223 + m[2][0] * A0123) * oneOverDeterminant;
			r[2][0] = (m[1][0] * A2313 - m[2][0] * A1313 + m[3][0] * A1213) * oneOverDeterminant;
			r[2][1] = -(m[0][0] * A2313 - m[2][0] * A0313 + m[3][0] * A0213) * oneOverDeterminant;
			r[2][2] = (m[0][0] * A1313 - m[1][0] * A0313 + m[3][0] * A0113) * oneOverDeterminant;
			r[2][3] = -(m[0][0] * A1213 - m[1][0] * A0213 + m[2][0] * A0113) * oneOverDeterminant;
			r[3][0] = -(m[1][0] * A2312 - m[2][0] * A1312 + m[3][0] * A1212) * oneOverDeterminant;
			r[3][1] = (m[0][0] * A2312 - m[2][0] * A0312 + m[3][0] * A0212) * oneOverDeterminant;
			r[3][2] = -(m[0][0] * A1312 - m[1][0] * A0312 + m[3][0] * A0112) * oneOverDeterminant;
			r[3][3] = (m[0][0] * A1212 - m[1][0] * A0212 + m[2][0] * A0112) * oneOverDeterminant;
			
			return r;
		}
	}
}

#endif