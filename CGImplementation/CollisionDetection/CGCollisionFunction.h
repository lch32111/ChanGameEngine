#ifndef __CG_COLLISION_FUNCTION_H__
#define __CG_COLLISION_FUNCTION_H__

#include <CollisionDetection/CGCollisionSphere.h>
#include <Math/CGRay.h>

namespace CGProj
{
	namespace CollisionDetection
	{
		bool intersect(const CGCollisionSphere& sphere, const Math::CGRay& ray);
	}
}

#endif
