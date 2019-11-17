#ifndef __CG_COLLISION_FUNCTION_H__
#define __CG_COLLISION_FUNCTION_H__

#include <CollisionDetection/CGCollisionSphere.h>
#include <CollisionDetection/CGCollisionRay.h>
#include <CollisionDetection/CGCollisionLineSegment.h>

namespace CGProj
{
	namespace CollisionDetection
	{
		bool intersect(const CGCollisionSphere& a, const CGCollisionSphere& b);

		bool intersect(const CGCollisionSphere& sphere, const CGCollisionRay& ray);

		bool intersect(const CGCollisionSphere& sphere, const CGCollisionLineSegment& segment);
	}
}

#endif
