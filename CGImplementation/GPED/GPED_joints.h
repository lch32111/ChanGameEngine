#ifndef __GPED_JOINTS_H__
#define __GPED_JOINTS_H__

#include <GPED/GPED_contacts.h>

namespace GPED
{
	/**
	 * Joints link together two rigid bodies and make sure they do not
	 * separate. In a general physics engine there may be many
	 * different types of joint, that reduce the number of relative
	 * degrees of freedom between two objects. This joint is a common
	 * position joint: each object has a location (given in
	 * body-coordinates) that will be kept at the same point in the
	 * simulation.
	 */
	class Joint : public ContactGenerator
	{
	public:
		/**
		 * Holds the two rigid bodies that are connected by this joint.
		 */
		RigidBody* body[2];

		/**
		 * Holds the relative location of the connection for each
		 * body, given in local coordinates.
		 */
		glm::vec3 position[2];

		/**
		 * Holds the maximum displacement at the joint before the 
		 * join is considered to be violated. This is normally a
		 * small, epsilon value. It can be larger, however, in which
		 * case the joint will behave as if an inelastic cable joined
		 * the bodies at their joint locations.
		 */
		real error;

		/**
		 * Configures the joint in one go.
		 */
		void set(
			RigidBody* a, const glm::vec3& a_pos,
			RigidBody* b, const glm::vec3& b_pos,
			real error
		);

		/**
		 * Generates the contacts required to restore the joint if it
		 * has been violated.
		 */
		unsigned addContact(Contact* contact, unsigned limit) const;
	};

}

#endif // !__GPED_JOINTS_H__
