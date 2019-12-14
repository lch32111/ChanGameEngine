#ifndef __CG_RAY_H__
#define __CG_RAY_H__

#include <CollisionDetection/CGCollisionConfig.h>
#include <CollisionDetection/CGCollisionConvex.h>

namespace CGProj
{
	namespace CollisionDetection
	{
		// referred to https://github.com/erincatto/Box2D/blob/master/Box2D/Collision/b2Collision.h
		// The ray extens from source to (source + maxFraction * (target - source))
		class CGCollisionRay : public CGCollisionConvex
		{
		public:
			CGCollisionRay() { }

			CGVec3 GetDirection()
			{
				return Normalize(m_target - m_source);
			}

			CGVec3 m_source;
			CGVec3 m_target;
			CGScalar m_maxFraction;
		};
	}
}

#endif