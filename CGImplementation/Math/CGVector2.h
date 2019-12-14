#ifndef __CG_VECTOR_2_H__
#define __CG_VECTOR_2_H__

#include <Math/CGMathConfig.h>

namespace CGProj
{
	namespace Math
	{
		template <typename Scalar>
		class CGVector2
		{
		public:
			CGVector2() { }
			explicit CGVector2(Scalar _x, Scalar _y) : m_value{ _x, _y} { }
			CGVector2(const CGVector2<Scalar>& v) : m_value{ v.m_value[0], v.m_value[1] } { }

			CGVector2<Scalar>& operator=(const CGVector2<Scalar>& v)
			{
				m_value[0] = v.m_value[0];
				m_value[1] = v.m_value[1];
				return *this;
			}

			CGVector2<Scalar> operator-()
			{
				return CGVector2<Scalar>(-m_value[0], -m_value[1]);
			}

			CGVector2<Scalar>& operator +=(const CGVector2<Scalar>& v)
			{
				m_value[0] += v.m_value[0];
				m_value[1] += v.m_value[1];
				return *this;
			}

			CGVector2<Scalar>& operator -=(const CGVector2<Scalar>& v)
			{
				m_value[0] -= v.m_value[0];
				m_value[1] -= v.m_value[1];
				return *this;
			}

			CGVector2<Scalar>& operator *=(const CGVector2<Scalar>& v)
			{
				m_value[0] *= v.m_value[0];
				m_value[1] *= v.m_value[1];
				return *this;
			}

			CGVector2<Scalar>& operator /=(const CGVector2<Scalar>& v)
			{
				m_value[0] /= v.m_value[0];
				m_value[1] /= v.m_value[1];
				return *this;
			}

			CGVector2<Scalar>& operator +=(Scalar s)
			{
				m_value[0] += s;
				m_value[1] += s;
				return *this;
			}

			CGVector2<Scalar>& operator -=(Scalar s)
			{
				m_value[0] -= s;
				m_value[1] -= s;
				return *this;
			}

			CGVector2<Scalar>& operator *=(Scalar s)
			{
				m_value[0] *= s;
				m_value[1] *= s;
				return *this;
			}

			CGVector2<Scalar>& operator /=(Scalar s)
			{
				m_value[0] /= s;
				m_value[1] /= s;
				return *this;
			}

			CGVector2<Scalar>& operator++()
			{
				++m_value[0]; ++m_value[1];
				return *this;
			}

			CGVector2<Scalar>& operator--()
			{
				--m_value[0]; --m_value[1];
				return *this;
			}

			CGVector2<Scalar> operator++(int) // postfix
			{
				CGVector2<Scalar> r(*this);
				++m_value[0]; ++m_value[1];
				return *this;
			}

			CGVector2<Scalar> operator--(int) // postfix
			{
				CGVector2<Scalar> r(*this);
				--m_value[0]; --m_value[1];
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

			Scalar m_value[2];
		};

		template <typename Scalar>
		inline CGVector2<Scalar> operator*(const CGVector2<Scalar>& v, const Scalar& scalar)
		{
			return CGVector2<Scalar>(v.m_value[0] * scalar, v.m_value[1] * scalar);
		}

		template <typename Scalar>
		inline CGVector2<Scalar> operator*(const Scalar& scalar, const CGVector2<Scalar>& v)
		{
			return CGVector2<Scalar>(scalar * v.m_value[0], scalar * v.m_value[1]);
		}

		template <typename Scalar>
		inline CGVector2<Scalar> operator+(const CGVector2<Scalar>& a, const CGVector2<Scalar>& b)
		{
			return CGVector2<Scalar>(a.m_value[0] + b.m_value[0], a.m_value[1] + b.m_value[1]);
		}

		template <typename Scalar>
		inline CGVector2<Scalar> operator-(const CGVector2<Scalar>& a, const CGVector2<Scalar>& b)
		{
			return CGVector2<Scalar>(a.m_value[0] - b.m_value[0], a.m_value[1] - b.m_value[1]);
		}

		template <typename Scalar>
		inline CGVector2<Scalar> operator*(const CGVector2<Scalar>& a, const CGVector2<Scalar>& b)
		{
			return CGVector2<Scalar>(a.m_value[0] * b.m_value[0], a.m_value[1] * b.m_value[1]);
		}

		template <typename Scalar>
		inline CGVector2<Scalar> operator/(const CGVector2<Scalar>& a, const CGVector2<Scalar>& b)
		{
			return CGVector2<Scalar>(a.m_value[0] / b.m_value[0], a.m_value[1] / b.m_value[1]);
		}

		template <typename Scalar>
		inline bool operator==(const CGVector2<Scalar> &a, const CGVector2<Scalar>& b)
		{
			return a.m_value[0] == b.m_value[0] &&
				a.m_value[1] == b.m_value[1];
		}

		template <typename Scalar>
		inline bool operator!=(const CGVector2<Scalar> &a, const CGVector2<Scalar>& b)
		{
			return a.m_value[0] != b.m_value[0] ||
				a.m_value[1] != b.m_value[1];
		}

		template <typename Scalar>
		inline Scalar Dot(const CGVector2<Scalar>& a, const CGVector2<Scalar>& b)
		{
			return a.m_value[0] * b.m_value[0] + a.m_value[1] * b.m_value[1];
		}

		template <typename Scalar>
		inline Scalar Cross(const CGVector2<Scalar>& a, const CGVector2<Scalar>& b)
		{
			return a.x * b.y - a.y * b.x;
		}

		template <typename Scalar>
		inline CGVector2<Scalar> Cross(const CGVector2<Scalar>& v, Scalar s)
		{
			return CGVector2<Scalar>(v.y, -s * v.x);
		}

		template <typename Scalar>
		inline CGVector2<Scalar> Cross(Scalar s, const CGVector2<Scalar> v)
		{
			return CGVector2<Scalar>(-s * v.y, v.x);
		}

		template <typename Scalar>
		inline Scalar Length(const CGVector2<Scalar>& v)
		{
			return Scalar_traits<Scalar>::sqrt(v.m_value[0] * v.m_value[0] + v.m_value[1] * v.m_value[1]);
		}

		template<typename Scalar>
		inline Scalar Distance(const CGVector2<Scalar>& a, const CGVector2<Scalar>& b)
		{
			return Length(a - b);
		}

		template <typename Scalar>
		inline CGVector2<Scalar> Normalize(const CGVector2<Scalar>& v)
		{
			Scalar length = Length(v);
			CG_DEBUG_BREAK(length);
			return v * (static_cast<Scalar>(1.0) / length);
		}
	}
}

#endif
