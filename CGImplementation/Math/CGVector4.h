#ifndef __CG_VECTOR_4_H__
#define __CG_VECTOR_4_H__

#include <Math/CGMathConfig.h>

namespace CGProj
{
	namespace Math
	{
		template <typename Scalar>
		class CGVector4
		{
		public:
			CGVector4() { }
			explicit CGVector4(Scalar _x, Scalar _y, Scalar _z, Scalar _w) : m_value{ _x, _y,_z , _w} { }
			explicit CGVector4(Scalar fill) : m_value { fill, fill, fill, fill} { }
			CGVector4(const CGVector4<Scalar>& v) : m_value{ v.m_value[0], v.m_value[1], v.m_value[2], v.m_value[3] } { }

			CGVector4<Scalar>& operator=(const CGVector4<Scalar>& v)
			{
				m_value[0] = v.m_value[0];
				m_value[1] = v.m_value[1];
				m_value[2] = v.m_value[2];
				m_value[3] = v.m_value[3];
				return *this;
			}

			CGVector4<Scalar> operator-()
			{
				return CGVector4<Scalar>(-m_value[0], -m_value[1], -m_value[2], -m_value[3]);
			}

			CGVector4<Scalar>& operator +=(const CGVector4<Scalar>& v)
			{
				m_value[0] += v.m_value[0];
				m_value[1] += v.m_value[1];
				m_value[2] += v.m_value[2];
				m_value[3] += v.m_value[3];
				return *this;
			}

			CGVector4<Scalar>& operator -=(const CGVector4<Scalar>& v)
			{
				m_value[0] -= v.m_value[0];
				m_value[1] -= v.m_value[1];
				m_value[2] -= v.m_value[2];
				m_value[3] -= v.m_value[3];
				return *this;
			}

			CGVector4<Scalar>& operator *=(const CGVector4<Scalar>& v)
			{
				m_value[0] *= v.m_value[0];
				m_value[1] *= v.m_value[1];
				m_value[2] *= v.m_value[2];
				m_value[3] *= v.m_value[3];
				return *this;
			}

			CGVector4<Scalar>& operator /=(const CGVector4<Scalar>& v)
			{
				m_value[0] /= v.m_value[0];
				m_value[1] /= v.m_value[1];
				m_value[2] /= v.m_value[2];
				m_value[3] /= v.m_value[3];
				return *this;
			}

			CGVector4<Scalar>& operator +=(Scalar s)
			{
				m_value[0] += s;
				m_value[1] += s;
				m_value[2] += s;
				m_value[3] += s;
				return *this;
			}

			CGVector4<Scalar>& operator -=(Scalar s)
			{
				m_value[0] -= s;
				m_value[1] -= s;
				m_value[2] -= s;
				m_value[3] -= s;
				return *this;
			}

			CGVector4<Scalar>& operator *=(Scalar s)
			{
				m_value[0] *= s;
				m_value[1] *= s;
				m_value[2] *= s;
				m_value[3] *= s;
				return *this;
			}

			CGVector4<Scalar>& operator /=(Scalar s)
			{
				m_value[0] /= s;
				m_value[1] /= s;
				m_value[2] /= s;
				m_value[3] /= s;
				return *this;
			}

			CGVector4<Scalar>& operator++()
			{
				++m_value[0]; ++m_value[1]; ++m_value[2]; ++m_value[3];
				return *this;
			}

			CGVector4<Scalar>& operator--()
			{
				--m_value[0]; --m_value[1]; --m_value[2]; --m_value[3];
				return *this;
			}

			CGVector4<Scalar> operator++(int) // postfix
			{
				CGVector3<Scalar> r(*this);
				++m_value[0]; ++m_value[1]; ++m_value[2]; ++m_value[3];
				return *this;
			}

			CGVector4<Scalar> operator--(int) // postfix
			{
				CGVector3<Scalar> r(*this);
				--m_value[0]; --m_value[1]; --m_value[2]; --m_value[3];
				return *this;
			}

			template<typename indexType>
			Scalar operator[](indexType i) const
			{
				return m_value[i];
			}

			template<typename indexType>
			Scalar& operator[](indexType i)
			{
				return m_value[i];
			}

			Scalar* data() { return m_value; }
			const Scalar* data() const { return m_value; }

			Scalar m_value[4];
		};

		template <typename Scalar>
		inline CGVector4<Scalar> operator*(const CGVector4<Scalar>& v, const Scalar& scalar)
		{
			return CGVector4<Scalar>(v.m_value[0] * scalar, v.m_value[1] * scalar, v.m_value[2] * scalar, v.m_value[3] * scalar);
		}

		template <typename Scalar>
		inline CGVector4<Scalar> operator*(const Scalar& scalar, const CGVector4<Scalar>& v)
		{
			return CGVector4<Scalar>(scalar * v.m_value[0], scalar * v.m_value[1], scalar * v.m_value[2], scalar * v.m_value[3]);
		}

		template <typename Scalar>
		inline CGVector4<Scalar> operator+(const CGVector4<Scalar>& a, const CGVector4<Scalar>& b)
		{
			return CGVector3<Scalar>(a.m_value[0] + b.m_value[0], a.m_value[1] + b.m_value[1], a.m_value[2] + b.m_value[2], a.m_value[3] + b.m_value[3]);
		}

		template <typename Scalar>
		inline CGVector4<Scalar> operator-(const CGVector4<Scalar>& a, const CGVector4<Scalar>& b)
		{
			return CGVector3<Scalar>(a.m_value[0] - b.m_value[0], a.m_value[1] - b.m_value[1], a.m_value[2] - b.m_value[2], a.m_value[3] - b.m_value[3]);
		}

		template <typename Scalar>
		inline CGVector4<Scalar> operator*(const CGVector4<Scalar>& a, const CGVector4<Scalar>& b)
		{
			return CGVector3<Scalar>(a.m_value[0] * b.m_value[0], a.m_value[1] * b.m_value[1], a.m_value[2] * b.m_value[2], a.m_value[3] * b.m_value[3]);
		}

		template <typename Scalar>
		inline CGVector4<Scalar> operator/(const CGVector4<Scalar>& a, const CGVector4<Scalar>& b)
		{
			return CGVector3<Scalar>(a.m_value[0] / b.m_value[0], a.m_value[1] / b.m_value[1], a.m_value[2] / b.m_value[2], a.m_value[3] / b.m_value[3]);
		}

		template <typename Scalar>
		inline bool operator==(const CGVector4<Scalar> &a, const CGVector4<Scalar>& b)
		{
			return a.m_value[0] == b.m_value[0] &&
				a.m_value[1] == b.m_value[1] &&
				a.m_value[2] == b.m_value[2] &&
				a.m_value[3] == b.m_value[3];
		}

		template <typename Scalar>
		inline bool operator!=(const CGVector4<Scalar> &a, const CGVector4<Scalar>& b)
		{
			return a.m_value[0] != b.m_value[0] ||
				a.m_value[1] != b.m_value[1] ||
				a.m_value[2] != b.m_value[2] ||
				a.m_value[3] != b.m_value[3];
		}

		template <typename Scalar>
		inline Scalar Dot(const CGVector4<Scalar>& a, const CGVector4<Scalar>& b)
		{
			return a.m_value[0] * b.m_value[0] + a.m_value[1] * b.m_value[1] + a.m_value[2] * b.m_value[2] + a.m_value[3] * b.m_value[3];
		}

		template <typename Scalar>
		inline Scalar Length(const CGVector4<Scalar>& v)
		{
			return ScalarOp<Scalar>::sqrt(v.m_value[0] * v.m_value[0] + 
				v.m_value[1] * v.m_value[1] + 
				v.m_value[2] * v.m_value[2] + 
				v.m_value[3] * v.m_value[3]);
		}

		template<typename Scalar>
		inline Scalar Distance(const CGVector4<Scalar>& a, const CGVector4<Scalar>& b)
		{
			return Length(a - b);
		}

		template <typename Scalar>
		inline CGVector4<Scalar> Normalize(const CGVector4<Scalar>& v)
		{
			Scalar length = Length(v);
			CG_DEBUG_BREAK(length);
			return v * (static_cast<Scalar>(1.0) / length);
		}
	}
}

#endif
