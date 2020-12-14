#ifndef __CG_NEW_COLLISION_SPHERE_H__
#define __CG_NEW_COLLISION_SPHERE_H__

#include <CollisionDetection/CGConfig.h>
#include <CollisionDetection/CGConvex.h>

namespace CG
{
	class CGSphere : public CGConvex
	{
	public:
		CGSphere() : CGConvex()
		{ }

		CGScalar m_radius;
		CGVec3 m_pos;
	};
}

#endif
