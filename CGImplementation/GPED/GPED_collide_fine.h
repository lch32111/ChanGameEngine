#ifndef __GPED_COLLIDE_FINE_H__
#define __GPED_COLLIDE_FINE_H__

#include <GPED/GPED_body.h>
#include <GPED/GPED_contacts.h>

namespace GPED
{
	// Forward declarations of primitive friends
	class IntersectionTests;
	class CollisionDetector;

	/**
	* Represents a primitive to detect collisions against.
	*/
	class CollisionPrimitive
	{
	public:
		enum primitiveType
		{
			UNDECLARED = 0,
			primitive_sphere,
			primitive_box
		};
		primitiveType m_primitiveType;

		/**
		 * This class exists to help the collision detector
		 * and intersection routines,so they should have
		 * access to its data.
		 */
		friend class IntersectionTests;
		friend class CollisionDetector;

		/**
		 * The rigid body that is represented bythis primitive.
		 */
		RigidBody* body;

		/**
		 * The offset of this primitive from the given rigid body.
		 */
		glm::mat4 offset = glm::mat4(1.0);

		/**
		 * Calculates the internals for the primitive.
		 */
		void calculateInternals();

		/**
		 * This is a convenience function to allow access to the
		 * axis vectors in the transform for this primitive.
		 */
		glm::vec3 getAxis(unsigned index) const
		{
			return transform[index];
		}

		/**
		 * Returns the resultant transform of the primitive, calculated from
		 * the combined offset of the primitiveand the transform
		 * (orientation + position) of the rigid body to which it is 
		 * attached.
		 */
		const glm::mat4& getTransform() const
		{
			return transform;
		}

		/** 18-11-14 Chanhaneg Lee
		 * this method is for the broadPhase.
		 * this virtual interface will make the code simple in the broadPhase class.
		 */
		virtual c3AABB makeAABB() const = 0;

	protected:
		/**
		 * The resultant transform of the primitive. This is
		 * calculated by combining the offset of the primitive
		 * with the transform of the rigid body.
		 */
		glm::mat4 transform;
	};

	/**
	 * Represents a rigid body that can be treated as a sphere
	 * for collision detection
	 */
	class CollisionSphere : public CollisionPrimitive
	{
	public:
		CollisionSphere()
		{
			CollisionPrimitive::m_primitiveType = primitive_sphere;
		}

		/**
		 * The radius of the sphere.
		 */
		real radius;

		virtual c3AABB makeAABB() const
		{
			c3AABB aabb;
			
			// Just ignore the orientation since it's sphere.
			glm::vec3 pos = getAxis(3);

			aabb.min = pos - glm::vec3(radius);
			aabb.max = pos + glm::vec3(radius);

			return aabb;
		}
	};

	/**
	 * The plane is not a primitive: it doesn't represent another
	 * rigid body. It is used for contacts with the immovable
	 * world geometry.
	 */
	class CollisionPlane
	{
	public:
		/**
		 * The plane normal
		 */
		glm::vec3 direction;

		/**
		 * The distance of the plane from the origin.
		 */
		real offset;
	};

	/**
	 * Represents a rigid body that can be treated as an aligned bounding
	 * box for collision detection
	 */
	class CollisionBox : public CollisionPrimitive
	{
	public:
		CollisionBox()
		{
			CollisionPrimitive::m_primitiveType = primitive_box;
		}

		/**
		 * Holds the half-sizes of the box along each of its local axes.
		 */
		glm::vec3 halfSize;

		virtual c3AABB makeAABB() const
		{
			c3AABB aabb;
			aabb.min = glm::vec3(REAL_MAX);
			aabb.max = glm::vec3(-REAL_MAX);

			glm::vec3 v[8] =
			{
				glm::vec3(-halfSize.x,-halfSize.y, -halfSize.z),
				glm::vec3(-halfSize.x,-halfSize.y, halfSize.z),
				glm::vec3(halfSize.x, -halfSize.y, halfSize.z),
				glm::vec3(halfSize.x,-halfSize.y, -halfSize.z),
				glm::vec3(-halfSize.x, halfSize.y, -halfSize.z),
				glm::vec3(-halfSize.x, halfSize.y, halfSize.z),
				glm::vec3(halfSize.x, halfSize.y, halfSize.z),
				glm::vec3(halfSize.x, halfSize.y, -halfSize.z)
			};

			for (int i = 0; i < 8; ++i)
			{
				v[i] = transform * glm::vec4(v[i], 1.0);

				aabb.min = GPED::rMin(aabb.min, v[i]);
				aabb.max = GPED::rMax(aabb.max, v[i]);
			}

			return aabb;
		}
	};

	/**
	* A wrapper class that holds fast intersection tests. These
	* can be used to drive the coarse collisiondetection system or
	* as an early out in the full collision tests below.
	*/
	class IntersectionTests
	{
	public:
		static bool sphereAndHalfSpace
		(
			const CollisionSphere& sphere,
			const CollisionPlane& plane
		);

		static bool sphereAndSphere
		(
			const CollisionSphere& one,
			const CollisionSphere& two
		);

		static bool boxAndBox
		(
			const CollisionBox& one,
			const CollisionBox& two
		);

		/**
		* Does an intersection test on an arbitrarily algined box and a half-space
		*
		* The box is given as a transformation matrix, including
		* position, and a vector of half-sizees for the extend of the
		* box along each local axis.
		*
		* The half-space is given as a direction (i.e. unit) vector and the
		* offset of the limiting plane from the origin, along the given
		* direction.
		*/
		static bool boxAndHalfSpace
		(
			const CollisionBox& box,
			const CollisionPlane& plane
		);
	};

	/**
	 * A helper structure that contains information for the detector to use
	 * in building its contact data.
	 */
	struct CollisionData
	{
		/**
		 * Holds the base of the collision data: the first contact
		 * in the array. This is used so that the contact pointer (below)
		 * can be incremented each time a contact is detected, while
		 * this pointer points to the first contact found.
		 */
		Contact* contactHead;

		/** Holds the contact array to write into */
		Contact* contacts;

		/** Holds the maximum number of contacts the array can take.*/
		int contactsLeft;

		/** Holds the number of contacts found so far. */
		unsigned contactCount;

		/** Holds the friction value to write into any collisions. */
		real friction;

		/** Holds the restitution value to write into any collisions. */
		real restitution;

		/** 
		 * Holds the collision tolerance, even uncolliding objects this
		 * close should have collisions generated.
		 */
		real tolerance;

		/**
		 * Checks if there are more contacts available in the contact
		 * data.
		 */
		bool hasMoreContacts()
		{
			return contactsLeft > 0;
		}

		/**
		 * Resets the data so that it has no used contacts recorded.
		 */
		void reset(unsigned maxContacts)
		{
			contactsLeft = maxContacts;
			contactCount = 0;
			contacts = contactHead;
		}

		/**
		 * Notifies the data that the given number of contacts have
		 * been added.
		 */
		void addContacts(unsigned count)
		{
			// Reduce the number of contacts remaining, add number used
			contactsLeft -= count;
			contactCount += count;

			// Move the array forward
			contacts += count;
		}

		void connectContactArray(Contact* c)
		{
			contactHead = c;
		}
	};
	
	/**
	 * A wrapper class that holds the fine grained collision detection
	 * routines.
	 *
	 * Each of the functions has the same format: it takes the details
	 * of two objects, and a pointer to a contact array to fill. It
	 * returns the number of contacts it wrote into the array.
	 */
	class CollisionDetector
	{
	public:
		static unsigned collision
		(
			const CollisionPrimitive* a, 
			const CollisionPrimitive* b, 
			CollisionData* data
		)
		{
			int aKey = 0;
			switch (a->m_primitiveType)
			{
			case GPED::CollisionPrimitive::primitiveType::primitive_sphere:
				aKey = 0;
				break;
			case GPED::CollisionPrimitive::primitiveType::primitive_box:
				aKey = 1;
				break;
			default:
				assert(0);
			}

			int bKey = 0;
			switch (b->m_primitiveType)
			{
			case GPED::CollisionPrimitive::primitiveType::primitive_sphere:
				bKey = 0;
				break;
			case GPED::CollisionPrimitive::primitiveType::primitive_box:
				bKey = 1;
				break;
			}

			if (aKey & bKey) return boxAndBox(*(CollisionBox*)a, *(CollisionBox*)b, data);
			if (!aKey & !bKey) return sphereAndSphere(*(CollisionSphere*)a, *(CollisionSphere*)b, data);
			if (aKey) return boxAndSphere(*(CollisionBox*)a, *(CollisionSphere*)b, data);
			else return boxAndSphere(*(CollisionBox*)b, *(CollisionSphere*)a, data);
		};


		static unsigned sphereAndHalfSpace
		(
			const CollisionSphere& sphere,
			const CollisionPlane& plane,
			CollisionData* data
		);

		static unsigned sphereAndTruePlane
		(
			const CollisionSphere& sphere,
			const CollisionPlane& plane,
			CollisionData* data
		);

		static unsigned sphereAndSphere
		(
			const CollisionSphere& one,
			const CollisionSphere& two,
			CollisionData* data
		);

		/**
		 * Does a collision test on a collision box and a plane representing
		 * a half-space (i.e. the normal of the plane
		 * points out of the half-space).
		 */
		static unsigned boxAndHalfSpace
		(
			const CollisionBox& box,
			const CollisionPlane& plane,
			CollisionData* data
		);

		static unsigned boxAndBox
		(
			const CollisionBox& one,
			const CollisionBox& two,
			CollisionData* data
		);

		static unsigned boxAndPoint
		(
			const CollisionBox& box,
			const glm::vec3& point,
			CollisionData* data
		);


		static unsigned boxAndSphere(
			const CollisionBox& box,
			const CollisionSphere& sphere,
			CollisionData* data
		);
	};


}


#endif
