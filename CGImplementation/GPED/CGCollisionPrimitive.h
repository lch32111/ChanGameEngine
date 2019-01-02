#pragma once
#ifndef __CG_COLLISION_PRIMITIVE_H__
#define __CG_COLLISION_PRIMITIVE_H__

#include <GPED/GPED_body.h>

namespace CGProj
{
	// Forward declarations of primitive friends
	class CGCollisionEarlyExit;
	class CGCollisionNarrow;

	/**
	* Represents a primitive to detect collisions against.
	*/
	class CGCollisionPrimitive
	{
		/**
		* This class exists to help the collision detector
		* and intersection routines,so they should have
		* access to its data.
		*/
		friend class CGCollisionEarlyExit;
		friend class CGCollisionNarrow;

	public:
		CGCollisionPrimitive();

		enum primitiveType
		{
			UNDECLARED = 0,
			primitive_sphere,
			primitive_box,
			primitive_mesh
		};
		primitiveType m_primitiveType;

		/**
		 * The rigid body that is represented by this primitive.
		 */
		GPED::RigidBody* body;

		/**
		 * The offset of this primitive from the given rigid body.
		 */
		glm::mat4 offset;

		/**
		 * Calculates the internals for the primitive.
		 */
		void calculateInternals();

		/**
		 * This is a convenience function to allow access to the
		 * axis vectors in the transform for this primitive.
		 */
		glm::vec3 getAxis(unsigned index) const;

		/**
		 * Returns the resultant transform of the primitive, calculated from
		 * the combined offset of the primitiveand the transform
		 * (orientation + position) of the rigid body to which it is
		 * attached.
		 */
		const glm::mat4& getTransform() const;

		/** 18-11-14 Chanhaneg Lee
		 * this method is for the broadPhase.
		 * this virtual interface will make the code simple in the broadPhase class.
		 */
		virtual GPED::c3AABB makeAABB() const = 0;

	protected:
		/**
		 * The resultant transform of the primitive. This is
		 * calculated by combining the offset of the primitive
		 * with the transform of the rigid body.
		 */
		glm::mat4 transform;
	};
}

#endif