#ifndef __CG_COLLISION_CONVEX_H__
#define __CG_COLLISION_CONVEX_H__

#include <Math/CGVector3.h>

#include <CollisionDetection/CGCollisionShape.h>

namespace CGProj
{
	namespace CollisionDetection
	{
		class CGCollisionConvex : public CGCollisionShape
		{
		public:
			CGCollisionConvex() : CGCollisionShape() { }
		};
	}
}

#endif
