#ifndef __GPED_CONTACTS_H__
#define __GPED_CONTACTS_H__

#include <GPED/GPED_body.h>

// Forward Declaration for accessing the Contact
namespace CGProj { class CGContactManager; }

namespace GPED
{
	/**
	 * Forward declaration, see full declaration below for complete
	 * documentation.
	 */
	class ContactResolver;

	/**
	* A contact represents two bodies in contact. Resolving a
	* contact removes their interpenetration, and applies sufficient
	* impulse to keep them apart. Colliding bodies may also rebound.
	* Contacts can be used to represent positional joints, by making
	* the contact constraint keep the bodies in their correct
	* orientation.
	*
	* It can be a good idea to create a contact object even when the
	* contact isn't violated. Because resolving one contact can violate
	* another, contacts that are close to being violated should be
	* sent to the resolver; that way if one resolution moves the body,
	* the contact may be violated, and can be resolved. If the contact
	* is not violated, it will not be resolved, so you only loose a
	* small amount of execution time.
	*
	* The contact has no callable functions, it just holds the contact
	* details. To resolve a set of contacts, use the contact resolver
	* class.
	*/
	class Contact
	{
		/**
		 * The contact resolver object needs access into the contacts to
		 * set and effect the contact.
		 */
		friend class ContactResolver;
		friend class CGProj::CGContactManager;

	public:
		/**
		 * Holds the bodies that are involved in the contact. The
		 * second of these can be NULL, for contacts with the scenery.
		 */
		RigidBody* body[2];

		/**
		 * Holds the lateral friction coefficient at the contact.
		 */
		real friction;

		/**
		 * Holds the normal restitution coefficient at the contact.
		 */
		real restitution;

		/**
		* Holds the position of the contact in world coordinates.
		*/
		glm::vec3 contactPoint;

		/**
		* Holds the direction of the contact in world coordinates.
		*/
		glm::vec3 contactNormal;

		/**
		* Holds the depth of penetration at the contact point. If both
		* bodies are specified then the contact point should be midway
		* between the inter-penetrating points.
		*/
		real penetration;

		/**
		 * Sets the data that doesn't normally depend on the position
		 * of the contact (i.e. the bodies,and their material properties).
		 * 
		 * Chanhaeng Lee 181120
		 * This method is substituted by ContactManager::SetBodyData
		 * The reson is that Contact Manager should manage all of contacts info
		 */
		// void setBodyData(RigidBody* one, RigidBody* two, 
		//	real friction, real restitution);

		Contact();
	protected:
		/**
		 * A transform matrix that converts coordinates in the contact's
		 * frame of reference to world coordinates. The columns of this
		 * matrix form an orthonormal set of vectors.
		 */
		glm::mat3 contactToWorld;

		/**
		 * Holds the closing velocity at the point of contact. This is set
		 * when the calculateInternals function is run.
		 */
		glm::vec3 contactVelocity;

		/**
		 * Holds the required change in velocity for this contact to be 
		 * resolved.
		 */
		real desiredDeltaVelocity;

		/**
		 * Holds the world space position of the contact point relative to
		 * centre of each body. This is set when the calculateInternals
		 * function is run.
		 */
		glm::vec3 relativeContactPosition[2];

	protected:
		/**
		 * Reverses the contact. This involves swapping the two rigid bodies
		 * and reversing the contact normal. The internal values should then
		 * be recalculated using calculateInternals (this is not dont
		 * automatically).
		 */
		void swapBodies();

		/**
		 * Updates the awake state of rigid bodies that are taking
		 * place in given contact. A body will be made awake if it
		 * is in contact with a body that is awake.
		 */
		void matchAwakeState();

		/** 
		 * Calculates and sets the internal value for 
		 * the desired delta velocity.
		 */
		void calculateDesiredDeltaVelocity(real duration);

		/**
		 * Calculates and returns the velocity of the contact
		 * point on the given body.
		 */
		glm::vec3 calculateLocalVelocity(unsigned bodyIndex, real duration);

		/**
		 * Calculates an orthonormal basis for the contact point, based on
		 * primary friction direction (for anisotropic friction) or
		 * a random orientation (for isotropic friction).
		 */
		void calculateContactBasis();

		/**
		* Calculates internal data from state data. This is called before
		* the resolution algorithm tries to do any resolution. It should
		* never need to be called manually.
		*/
		void calculateInternals(real duration);

		/**
		 * Applies an impulse to the given body, returning the
		 * change in velocities.
		 */
		void applyImpulse(const glm::vec3& impulse, RigidBody* body,
			glm::vec3* velocityChange, glm::vec3* rotationChange);

		/**
		 * Performs an inertia-weighted impulse based resolution of this
		 * contact alone.
		 */
		void applyVelocityChange(glm::vec3 velocityChange[2], glm::vec3 rotationChange[2]);

		/**
		 * Performs an inertia weighted penetration resolution of this
		 * contact alone.
		 */
		void applyPositionChange(glm::vec3 linearChange[2], 
			glm::vec3 angularChange[2]);

		/**
		 * Calculates the impulse needed to resolve the contact
		 * given that the contact has no friction. A pair of inertia
		 * tensors - one for each contact object - is specified to
		 * save calculation time: the calling function has access to
		 * these anyway.
		 */
		glm::vec3 calculateFrictionlessImpulse(glm::mat3* inverseInertiaTensor);

		/**
		 * Calculates the impulse needed to resolve this contact,
		 * given that the contact has a non-zero coefficient of
		 * friction. A pair of inertia tensors - one for each contact
		 * object - is specified to save calculation time: the calling
		 * function has access to these anyway.
		 */
		glm::vec3 calculateFrictionImpulse(glm::mat3* inverseInertiaTensor);

		void updatePenetration(
			const glm::vec3& linearChange, 
			const glm::vec3& angularChange, 
			unsigned index);

		void updateDesiredVelocity(
			const glm::vec3& velocityChange,
			const glm::vec3& rotationChange,
			unsigned index,
			real duration
		);
	protected:
		// Holds its own Id
		int myId;

		// Holds the doubly linked list pointers for the ordered list.
		int prev;
		int next;

		// Holds pointers to the next contact that involves each rigid body
		int nextObjects[2];
	};



	/**
	 * This is the basic polymorphic interface for contact generators
	 * applying to rigid bodies
	 */
	class ContactGenerator
	{
	public:
		/**
		 * Fills the given contact structure with the generated
		 * contact. The contact pointer should point to the first
		 * available contact in a contact array, where limit is the
		 * maximum number of contacts in the array that can be written
		 * to. The method returns the number of contacts that have 
		 * been written.
		 */
		virtual unsigned addContact(Contact* contact, unsigned limit) const = 0;
	};

}

#endif