#pragma once
#ifndef __CG_COLLISION_NARROW_H__
#define __CG_COLLISION_NARROW_H__

#include <GPED/CGContactManager.h>
#include <GPED/CGCollisionPrimitive.h>
#include <GPED/CGCollisionSphere.h>
#include <GPED/CGCollisionOBB.h>
#include <GPED/CGCollisionPlane.h>


namespace CGProj
{
	/**
	* A wrapper class that holds the fine grained collision detection
	* routines.
	*
	* Each of the functions has the same format: it takes the details
	* of two objects, and a pointer to a contact array to fill. It
	* returns the number of contacts it wrote into the array.
	*/
	class CGCollisionNarrow
	{
	public:
		static unsigned RayCollisionCallback
		(
			GPED::c3RayOutput& output,
			const GPED::c3RayInput& input,
			const CGCollisionPrimitive* primitive
		);

		static unsigned sphereAndHalfSpace
		(
			const CGCollisionSphere& sphere,
			const CGCollisionPlane& plane,
			CGContactManager* data
		);

		static unsigned sphereAndTruePlane
		(
			const CGCollisionSphere& sphere,
			const CGCollisionPlane& plane,
			CGContactManager* data
		);

		static unsigned sphereAndSphere
		(
			const CGCollisionSphere& one,
			const CGCollisionSphere& two,
			CGContactManager* data
		);

		/**
		 * Does a collision test on a collision obb and a plane representing
		 * a half-space (i.e. the normal of the plane
		 * points out of the half-space).
		 */
		static unsigned OBBAndHalfSpace
		(
			const CGCollisionOBB& box,
			const CGCollisionPlane& plane,
			CGContactManager* data
		);

		static unsigned OBBAndOBB
		(
			const CGCollisionOBB& one,
			const CGCollisionOBB& two,
			CGContactManager* data
		);

		static unsigned OBBAndPoint
		(
			const CGCollisionOBB& box,
			const glm::vec3& point,
			CGContactManager* data
		);

		static void fillPointFaceBoxBox
		(
			const CGCollisionOBB& one,
			const CGCollisionOBB& two,
			const glm::vec3& toCentre,
			CGContactManager* data,
			unsigned best,
			GPED::real pen
		);


		static unsigned OBBAndSphere(
			const CGCollisionOBB& box,
			const CGCollisionSphere& sphere,
			CGContactManager* data
		);

		static unsigned rayAndOBB
		(
			GPED::c3RayOutput& output,
			const GPED::c3RayInput& input,
			const CGCollisionOBB& box
		);

		static unsigned rayAndSphere
		(
			GPED::c3RayOutput& output,
			const GPED::c3RayInput& input,
			const CGCollisionSphere& sphere
		);
	};
}

#endif