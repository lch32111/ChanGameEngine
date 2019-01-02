#pragma once
#ifndef __CG_COLLISION_OBB_H__
#define __CG_COLLISION_OBB_H__

#include <GPED/CGCollisionPrimitive.h>

namespace CGProj
{
	/**
	 * Represents a rigid body that can be treated as an aligned bounding
	 * box for collision detection
	 */
	class CGCollisionOBB : public CGCollisionPrimitive
	{
	public:
		CGCollisionOBB();
		virtual GPED::c3AABB makeAABB() const;

		/**
		 * Holds the half-sizes of the box along each of its local axes.
		 */
		glm::vec3 halfSize;
	};
}

#endif