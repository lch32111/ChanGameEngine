#pragma once
#ifndef __CG_COLLISION_SPHERE_H__
#define __CG_COLLISION_SPHERE_H__

#include <GPED/CGCollisionPrimitive.h>

namespace CGProj
{
	/**
	* Represents a rigid body that can be treated as a sphere
	* for collision detection
	*/
	class CGCollisionSphere : public CGCollisionPrimitive
	{
	public:
		CGCollisionSphere();
		virtual void getAABB(GPED::c3AABB& outAABB) const;

		/**
		 * The radius of the sphere.
		 */
		GPED::real radius;
	};
}

#endif