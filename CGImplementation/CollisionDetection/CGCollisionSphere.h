#ifndef __CG_NEW_COLLISION_SPHERE_H__
#define __CG_NEW_COLLISION_SPHERE_H__

#include <CollisionDetection/CGCollisionConfig.h>
#include <CollisionDetection/CGCollisionConvex.h>

namespace CGProj
{
	namespace CollisionDetection
	{
		class CGCollisionSphere : public CGCollisionConvex
		{
		public:
			CGCollisionSphere() : CGCollisionConvex()
			{ }

			CGScalar m_radius;
			CGVec3 m_pos;
		};
	}
}

#endif
