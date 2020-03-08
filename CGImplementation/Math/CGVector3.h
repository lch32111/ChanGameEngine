#ifndef __CG_VECTOR_3_H__
#define __CG_VECTOR_3_H__

#include <Math/CGMathConfig.h>

namespace CGProj
{
	namespace Math
	{
		template <typename Scalar>
		class CGVector3
		{
		public:
			CGVector3() { }
			explicit CGVector3(Scalar _x, Scalar _y, Scalar _z) : m_value{ _x, _y,_z } { }
			explicit CGVector3(Scalar fill) : m_value{ fill, fill, fill } { }
			CGVector3(const CGVector3<Scalar>& v) : m_value{v.m_value[0], v.m_value[1], v.m_value[2] } { }

			CGVector3<Scalar>& operator=(const CGVector3<Scalar>& v)
			{
				m_value[0] = v.m_value[0];
				m_value[1] = v.m_value[1];
				m_value[2] = v.m_value[2];
				return *this;
			}

			CGVector3<Scalar> operator-()
			{
				return CGVector3<Scalar>(-m_value[0], -m_value[1], -m_value[2]);
			}

			CGVector3<Scalar>& operator +=(const CGVector3<Scalar>& v)
			{
				m_value[0] += v.m_value[0];
				m_value[1] += v.m_value[1];
				m_value[2] += v.m_value[2];
				return *this;
			}

			CGVector3<Scalar>& operator -=(const CGVector3<Scalar>& v)
			{
				m_value[0] -= v.m_value[0];
				m_value[1] -= v.m_value[1];
				m_value[2] -= v.m_value[2];
				return *this;
			}

			CGVector3<Scalar>& operator *=(const CGVector3<Scalar>& v)
			{
				m_value[0] *= v.m_value[0];
				m_value[1] *= v.m_value[1];
				m_value[2] *= v.m_value[2];
				return *this;
			}

			CGVector3<Scalar>& operator /=(const CGVector3<Scalar>& v)
			{
				m_value[0] /= v.m_value[0];
				m_value[1] /= v.m_value[1];
				m_value[2] /= v.m_value[2];
				return *this;
			}

			CGVector3<Scalar>& operator +=(Scalar s)
			{
				m_value[0] += s;
				m_value[1] += s;
				m_value[2] += s;
				return *this;
			}

			CGVector3<Scalar>& operator -=(Scalar s)
			{
				m_value[0] -= s;
				m_value[1] -= s;
				m_value[2] -= s;
				return *this;
			}

			CGVector3<Scalar>& operator *=(Scalar s)
			{
				m_value[0] *= s;
				m_value[1] *= s;
				m_value[2] *= s;
				return *this;
			}

			CGVector3<Scalar>& operator /=(Scalar s)
			{
				m_value[0] /= s;
				m_value[1] /= s;
				m_value[2] /= s;
				return *this;
			}

			CGVector3<Scalar>& operator++()
			{
				++m_value[0]; ++m_value[1]; ++m_value[2];
				return *this;
			}

			CGVector3<Scalar>& operator--()
			{
				--m_value[0]; --m_value[1]; --m_value[2];
				return *this;
			}

			CGVector3<Scalar> operator++(int) // postfix
			{
				CGVector3<Scalar> r(*this);
				++m_value[0]; ++m_value[1]; ++m_value[2];
				return *this;
			}

			CGVector3<Scalar> operator--(int) // postfix
			{
				CGVector3<Scalar> r(*this);
				--m_value[0]; --m_value[1]; --m_value[2];
				return *this;
			}

			template<typename IndexType>
			Scalar operator[](IndexType i) const
			{
				return m_value[i];
			}

			template<typename IndexType>
			Scalar& operator[](IndexType i)
			{
				return m_value[i];
			}

			Scalar* data() { return m_value; }
			const Scalar* data() const { return m_value; }

			Scalar m_value[3];
		};

		template <typename Scalar>
		inline CGVector3<Scalar> operator*(const CGVector3<Scalar>& v, const Scalar& scalar)
		{
			return CGVector3<Scalar>(v.m_value[0] * scalar, v.m_value[1] * scalar, v.m_value[2] * scalar);
		}

		template <typename Scalar>
		inline CGVector3<Scalar> operator*(const Scalar& scalar, const CGVector3<Scalar>& v)
		{
			return CGVector3<Scalar>(scalar * v.m_value[0], scalar * v.m_value[1], scalar * v.m_value[2]);
		}

		template <typename Scalar>
		inline CGVector3<Scalar> operator+(const CGVector3<Scalar>& a, const CGVector3<Scalar>& b)
		{
			return CGVector3<Scalar>(a.m_value[0] + b.m_value[0], a.m_value[1] + b.m_value[1], a.m_value[2] + b.m_value[2]);
		}

		template <typename Scalar>
		inline CGVector3<Scalar> operator-(const CGVector3<Scalar>& a, const CGVector3<Scalar>& b)
		{
			return CGVector3<Scalar>(a.m_value[0] - b.m_value[0], a.m_value[1] - b.m_value[1], a.m_value[2] - b.m_value[2]);
		}

		template <typename Scalar>
		inline CGVector3<Scalar> operator*(const CGVector3<Scalar>& a, const CGVector3<Scalar>& b)
		{
			return CGVector3<Scalar>(a.m_value[0] * b.m_value[0], a.m_value[1] * b.m_value[1], a.m_value[2] * b.m_value[2]);
		}

		template <typename Scalar>
		inline CGVector3<Scalar> operator/(const CGVector3<Scalar>& a, const CGVector3<Scalar>& b)
		{
			return CGVector3<Scalar>(a.m_value[0] / b.m_value[0], a.m_value[1] / b.m_value[1], a.m_value[2] / b.m_value[2]);
		}

		template <typename Scalar>
		inline bool operator==(const CGVector3<Scalar> &a, const CGVector3<Scalar>& b)
		{
			return a.m_value[0] == b.m_value[0] &&
				a.m_value[1] == b.m_value[1] &&
				a.m_value[2] == b.m_value[2];
		}

		template <typename Scalar>
		inline bool operator!=(const CGVector3<Scalar> &a, const CGVector3<Scalar>& b)
		{
			return a.m_value[0] != b.m_value[0] ||
				a.m_value[1] != b.m_value[1] ||
				a.m_value[2] != b.m_value[2];
		}

		template <typename Scalar>
		inline Scalar Dot(const CGVector3<Scalar>& a, const CGVector3<Scalar>& b)
		{
			return a.m_value[0] * b.m_value[0] + a.m_value[1] * b.m_value[1] + a.m_value[2] * b.m_value[2];
		}

		template <typename Scalar>
		inline CGVector3<Scalar> Cross(const CGVector3<Scalar>& a, const CGVector3<Scalar>& b)
		{
			return CGVector3<Scalar>(a.m_value[1] * b.m_value[2] - a.m_value[2] * b.m_value[1],
				a.m_value[2] * b.m_value[0] - a.m_value[0] * b.m_value[2],
				a.m_value[0] * b.m_value[1] - a.m_value[1] * b.m_value[0]);
		}

		template <typename Scalar>
		inline Scalar Length(const CGVector3<Scalar>& v)
		{
			return Scalar_traits<Scalar>::sqrt(v.m_value[0] * v.m_value[0] + v.m_value[1] * v.m_value[1] + v.m_value[2] * v.m_value[2]);
		}

		template<typename Scalar>
		inline Scalar Distance(const CGVector3<Scalar>& a, const CGVector3<Scalar>& b)
		{
			return Length(a - b);
		}

		template <typename Scalar>
		inline CGVector3<Scalar> Normalize(const CGVector3<Scalar>& v)
		{
			Scalar length = Length(v);
			CG_DEBUG_BREAK(length);
			return v * (static_cast<Scalar>(1.0) / length);
		}
	}
}

#endif
