#ifndef __CG_COLLISION_SPHERE_H__
#define __CG_COLLISION_SPHERE_H__

#include <CollisionDetection/CGCollisionConfig.h>
#include <CollisionDetection/CGCollisionConvex.h>

namespace CGProj
{
	namespace CollisionDetection
	{
		class CGCollisionSphere : public CGCollisionConvex
		{
		public:
			CGScalar m_radius;
			CGVec3 m_pos;
		};
	}
}

#endif
