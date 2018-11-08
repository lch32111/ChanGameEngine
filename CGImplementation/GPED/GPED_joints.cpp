#include "GPED_joints.h"
using namespace GPED;

void GPED::Joint::set(
	RigidBody * a, const glm::vec3 & a_pos, 
	RigidBody * b, const glm::vec3 & b_pos, real error)
{
	body[0] = a;
	body[1] = b;

	position[0] = a_pos;
	position[1] = b_pos;

	Joint::error = error;
}

unsigned GPED::Joint::addContact(Contact * contact, unsigned limit) const
{
	// Calculate the position of each connection point in world coordinates
	glm::vec3 a_pos_world = body[0]->getPointInWorldSpace(position[0]);
	glm::vec3 b_pos_world = body[1]->getPointInWorldSpace(position[1]);

	// Calculate the length of the joint
	glm::vec3 a_to_b = b_pos_world - a_pos_world;
	glm::vec3 normal = glm::normalize(a_to_b);
	real length = glm::length(a_to_b);

	// Check if it is violated
	if (real_abs(length) > error)
	{
		contact->body[0] = body[0];
		contact->body[1] = body[1];
		contact->contactNormal = normal;
		contact->contactPoint = (a_pos_world + b_pos_world) * 0.5f;
		contact->penetration = length - error;
		contact->friction = 1.f;
		contact->restitution = 0;
		return 1;
	}
	
	return 0;
}
