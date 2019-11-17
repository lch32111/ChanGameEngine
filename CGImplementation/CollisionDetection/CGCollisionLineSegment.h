#ifndef __CG_COLLISION_LINE_SEGMENT
#define __CG_COLLISION_LINE_SEGMENT

#include <CollisionDetection/CGCollisionConfig.h>
#include <CollisionDetection/CGCollisionConvex.h>

namespace CGProj
{
	namespace CollisionDetection
	{
		class CGCollisionLineSegment : public CGCollisionConvex
		{
		public:
			CGCollisionLineSegment() { }

			CGVec3 GetDirection() const
			{
				return Math::Normalize(m_target - m_source);
			}

			CGVec3 m_source;
			CGVec3 m_target;
		};
	}
}

#endif
