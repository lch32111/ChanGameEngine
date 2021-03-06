#ifndef __CG_COLLISION_LINE_SEGMENT
#define __CG_COLLISION_LINE_SEGMENT

#include <Geometry/CGGeometryConfig.h>
#include <Geometry/CGConvex.h>

namespace CG
{
	class CGLineSegment : public CGConvex
	{
	public:
		CGLineSegment() { }

		CGVec3 GetDirection() const
		{
			return Normalize(m_target - m_source);
		}

		CGVec3 m_source;
		CGVec3 m_target;
	};
}

#endif
