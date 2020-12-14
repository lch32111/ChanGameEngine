#pragma once
#ifndef __CG_COLLISION_PLANE_H__
#define __CG_COLLISION_PLANE_H__

#include <GPED/GPED_Precision.h>

namespace CG
{
	/**
	* The plane is not a primitive: it doesn't represent another
	* rigid body. It is used for contacts with the immovable
	* world geometry.
	*/
	class CGCollisionPlane
	{
	public:
		/**
		 * The plane normal
		 */
		glm::vec3 direction;

		/**
		 * The distance of the plane from the origin.
		 */
		GPED::real offset;
	};
}

#endif