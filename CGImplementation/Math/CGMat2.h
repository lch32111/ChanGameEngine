#ifndef __CG_MAT2_H__
#define __CG_MAT2_H__

#include <Math/CGVector2.h>

namespace CG
{
	template<typename Scalar>
	class CGMat2
	{
	public:
		CGMat2() { }
			
		explicit CGMat2(Scalar s)
			: m_col{ s, 0, 0, s } { }

		explicit CGMat2(Scalar s00, Scalar s10, Scalar s01, Scalar s11)
			: m_col{ s00, s10, s01, s11} { }
				
		explicit CGMat2(const CGVector2<Scalar>& col1,
			const CGVector2<Scalar>& col2)
			: m_col{ col1, col2} { }

		CGMat2(const CGMat2<Scalar>& m)
			: m_col{ m.m_col[0], m.m_col[1] } { }

		CGMat2<Scalar>& operator=(const CGMat2<Scalar>& m)
		{
			m_col[0] = m.m_col[0];
			m_col[1] = m.m_col[1];
			return *this;
		}

		CGMat2<Scalar> operator-()
		{
			return CGMat2<Scalar>(-m_col[0], -m_col[1]);
		}

		CGMat2<Scalar>& operator+=(const CGMat2<Scalar>& m)
		{
			m_col[0] += m.m_col[0];
			m_col[1] += m.m_col[1];
			return *this;
		}

		CGMat2<Scalar>& operator-=(const CGMat2<Scalar>& m)
		{
			m_col[0] -= m.m_col[0];
			m_col[1] -= m.m_col[1];
			return *this;
		}

		CGMat2<Scalar>& operator*=(const CGMat2<Scalar>& t)
		{
			const Scalar s00 = m_col[0][0] * t.m_col[0][0] +
				m_col[1][0] * t.m_col[0][1];
			const Scalar s10 = m_col[0][1] * t.m_col[0][0] +
				m_col[1][1] * t.m_col[0][1];

			const Scalar s01 = m_col[0][0] * t.m_col[1][0] +
				m_col[1][0] * t.m_col[1][1];
			const Scalar s11 = m_col[0][1] * t.m_col[1][0] +
				m_col[1][1] * t.m_col[1][1];
			
			m_col[0] = { s00, s10};
			m_col[1] = { s01, s11};
			return *this;
		}

		CGMat2<Scalar>& operator/=(const CGMat2<Scalar>& m)
		{
			m_col[0] /= m.m_col[0];
			m_col[1] /= m.m_col[1];
			return *this;
		}

		CGMat2<Scalar>& operator+=(Scalar s)
		{
			m_col[0] += s;
			m_col[1] += s;
			return *this;
		}

		CGMat2<Scalar>& operator-=(Scalar s)
		{
			m_col[0] -= s;
			m_col[1] -= s;
			return *this;
		}

		CGMat2<Scalar>& operator*=(Scalar s)
		{
			m_col[0] *= s;
			m_col[1] *= s;
			return *this;
		}

		CGMat2<Scalar>& operator/=(Scalar s)
		{
			m_col[0] /= s;
			m_col[1] /= s;
			return *this;
		}

		CGMat2<Scalar>& operator++()
		{
			++m_col[0]; ++m_col[1];
			return *this;
		}

		CGMat2<Scalar>& operator--()
		{
			--m_col[0]; --m_col[1];
			return *this;
		}

		CGMat2<Scalar>& operator++(int)
		{
			CGMat2<Scalar> r(*this);
			++m_col[0]; ++m_col[1];
			return r;
		}

		CGMat2<Scalar>& operator--(int)
		{
			CGMat2<Scalar> r(*this);
			--m_col[0]; --m_col[1];
			return r;
		}

		CGVector2<Scalar>& operator[](unsigned i)
		{
			CG_DEBUG_ASSERT(i >= 0 && i <= 1);
			return m_col[i];
		}

		CGVector2<Scalar> operator[](unsigned i) const
		{
			CG_DEBUG_ASSERT(i >= 0 && i <= 1);
			return m_col[i];
		}

		Scalar* data() { return m_col; }
		const Scalar* data() const { return m_col; }

		CGVector2<Scalar> m_col[2];
	};

	template<typename Scalar>
	inline CGMat2<Scalar> operator*(const CGMat2<Scalar>& m, Scalar s)
	{
		return CGMat2<Scalar>(m.m_col[0] * s, m.m_col[1] * s);
	}

	template<typename Scalar>
	inline CGMat2<Scalar> operator*(Scalar s, const CGMat2<Scalar>& m)
	{
		return CGMat2<Scalar>(s * m.m_col[0], s * m.m_col[1]);
	}

	template<typename Scalar>
	inline CGMat2<Scalar> operator+(const CGMat2<Scalar>& a, const CGMat2<Scalar>& b)
	{
		return CGMat2<Scalar>(a.m_col[0] + b.m_col[0], a.m_col[1] + b.m_col[1]);
	}

	template<typename Scalar>
	inline CGMat2<Scalar> operator-(const CGMat2<Scalar>& a, const CGMat2<Scalar>& b)
	{
		return CGMat2<Scalar>(a.m_col[0] - b.m_col[0], a.m_col[1] - b.m_col[1]);
	}

	template<typename Scalar>
	inline CGMat2<Scalar> operator*(const CGMat2<Scalar>& a, const CGMat2<Scalar>& b)
	{
		const Scalar s00 = a.m_col[0][0] * b.m_col[0][0] +
			a.m_col[1][0] * b.m_col[0][1];
		const Scalar s10 = a.m_col[0][1] * b.m_col[0][0] +
			a.m_col[1][1] * b.m_col[0][1];

		const Scalar s01 = a.m_col[0][0] * b.m_col[1][0] +
			a.m_col[1][0] * b.m_col[1][1];
		const Scalar s11 = a.m_col[0][1] * b.m_col[1][0] +
			a.m_col[1][1] * b.m_col[1][1];

		return CGMat2<Scalar>(s00, s10,
			s01, s11);
	}

	template<typename Scalar>
	inline CGMat2<Scalar> operator/(const CGMat2<Scalar>& a, const CGMat2<Scalar>& b)
	{
		return CGMat2<Scalar>(a.m_col[0] / b.m_col[0], a.m_col[1] / b.m_col[1]);
	}

	template<typename Scalar>
	inline bool operator==(const CGMat2<Scalar>& a, const CGMat2<Scalar>& b)
	{
		return a.m_col[0] == b.m_col[0] &&
			a.m_col[1] == b.m_col[1];
	}

	template<typename Scalar>
	inline bool operator!=(const CGMat2<Scalar>& a, const CGMat2<Scalar>& b)
	{
		return a.m_col[0] != b.m_col[0] ||
			a.m_col[1] != b.m_col[1];
	}

	template<typename Scalar>
	inline CGMat2<Scalar> Transpose(const CGMat2<Scalar>& m)
	{
		return CGMat2<Scalar>(m.m_col[0][0], m.m_col[1][0],
			m.m_col[0][1], m.m_col[1][1]);
	}

	template<typename Scalar>
	inline Scalar Determinant(const CGMat2<Scalar>& m)
	{
		return m.m_col[0][0] * m.m_col[1][1] - m.m_col[1][0] * m.m_col[0][1];
	}

	template<typename Scalar>
	inline CGMat2<Scalar> Inverse(const CGMat2<Scalar>& m)
	{
		CGMat2<Scalar> r;
		const Scalar oneOverDeterminant = static_cast<Scalar>(1) /
			m.m_col[0][0] * m.m_col[1][1] - m.m_col[1][0] * m.m_col[0][1];

		r.m_col[0][0] = m.m_col[1][1] * oneOverDeterminant;
		r.m_col[0][1] = -m.m_col[0][1] * oneOverDeterminant;
		r.m_col[1][0] = -m.m_col[1][0] * oneOverDeterminant;
		r.m_col[1][1] = m.m_col[0][0] * oneOverDeterminant;
			
		return r;
	}
}

#endif