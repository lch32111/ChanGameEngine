#pragma once
#ifndef __CG_COLLISION_EARLY_EXIT_H__
#define __CG_COLLISION_EARLY_EXIT_H__

#include <GPED/CGCollisionSphere.h>
#include <GPED/CGCollisionOBB.h>
#include <GPED/CGCollisionMesh.h>
#include <GPED/CGCollisionPlane.h>

namespace CGProj
{
	/**
	* A wrapper class that holds fast intersection tests. These
	* can be used to drive the coarse collisiondetection system or
	* as an early out in the full collision tests below.
	*/
	class CGCollisionEarlyExit
	{
	public:
		static bool sphereAndHalfSpace
		(
			const CGCollisionSphere& sphere,
			const CGCollisionPlane& plane
		);

		static bool sphereAndSphere
		(
			const CGCollisionSphere& one,
			const CGCollisionSphere& two
		);

		static bool OBBAndOBB
		(
			const CGCollisionOBB& one,
			const CGCollisionOBB& two
		);

		/**
		* Does an intersection test on an arbitrarily aligned box and a half-space
		*
		* The box is given as a transformation matrix, including
		* position, and a vector of half-sizees for the extend of the
		* box along each local axis.
		*
		* The half-space is given as a direction (i.e. unit) vector and the
		* offset of the limiting plane from the origin, along the given
		* direction.
		*/
		static bool OBBAndHalfSpace
		(
			const CGCollisionOBB& box,
			const CGCollisionPlane& plane
		);

		static bool MeshAndAABB
		(
			const CGCollisionMesh& mesh,
			const GPED::c3AABB& aabb
		);
	};
}

#endif
