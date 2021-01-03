#ifndef __CG_RAY_H__
#define __CG_RAY_H__

#include <Geometry/CGGeometryConfig.h>
#include <Geometry/CGConvex.h>

namespace CG
{
	// referred to https://github.com/erincatto/Box2D/blob/master/Box2D/Collision/b2Collision.h
	// The ray extens from source to (source + maxFraction * (target - source))
	class CGRay : public CGConvex
	{
	public:
		CGRay() 
			: m_init(false)
		{ }

		CGRay(const CGVec3& source, const CGVec3& target)
			: m_init(true)
			, m_source(source)
			, m_target(target)
			, m_max_fraction(Length(m_target - m_source))
			, m_normalized_dir((m_target - m_source) * (CGScalar(1.0) / m_max_fraction))
		{}

		CGRay(const CGVec3& source, const CGVec3& target, const CGScalar max_fraction)
			: m_init(true)
			, m_source(source)
			, m_target(target)
			, m_max_fraction(max_fraction)
			, m_normalized_dir(Normalize(m_target - m_source))
		{}

		void SetRay(const CGVec3& source, const CGVec3& target, const CGScalar max_fraction)
		{
			m_init = true;
			m_source = source;
			m_target = target;
			m_normalized_dir = Normalize(m_target - m_source);
			m_max_fraction = max_fraction;
		}

		void SetRay(const CGVec3& source, const CGVec3& target)
		{
			m_init = true;
			m_source = source;
			m_target = target;
			m_max_fraction = Length(target - source);
			m_normalized_dir = (m_target - m_source) * (CGScalar(1.0) * m_max_fraction); 
		}

		const CGVec3& GetSource() const 
		{
			CG_DEBUG_ASSERT(m_init == true);
			return m_source; 
		}

		const CGVec3& GetTarget() const
		{
			CG_DEBUG_ASSERT(m_init == true);
			return m_target;
		}

		const CGScalar GetMaxFraction() const
		{
			CG_DEBUG_ASSERT(m_init == true);
			return m_max_fraction;
		}

		const CGVec3& GetDirection() const
		{
			CG_DEBUG_ASSERT(m_init == true);
			return m_normalized_dir;
		}

	private:
		bool m_init : 1;

		CGVec3 m_source;
		CGVec3 m_target;
		CGScalar m_max_fraction;
		CGVec3 m_normalized_dir;
	};
}

#endif
