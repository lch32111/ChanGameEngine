#include "GPED_plinks.h"
using namespace GPED;

real GPED::ParticleLink::currentLength() const
{
	glm::vec3 relativePos = particle[0]->getPosition() -
		particle[1]->getPosition();
	return glm::length(relativePos);
}

unsigned GPED::ParticleCable::addContact(ParticleContact* contact, unsigned limit) const
{
	// Find the length of the cable
	real length = currentLength();

	// Check if we're over extended
	if (length < maxLength) return 0;

	// Otherwise return the contact
	contact->particle[0] = particle[0];
	contact->particle[1] = particle[1];

	// Calculate the normal
	contact->contactNormal = glm::normalize(particle[1]->getPosition() - particle[0]->getPosition());;
	contact->penetration = length - maxLength;
	contact->restitution = restitution;

	return 1;
}

unsigned GPED::ParticleRod::addContact(ParticleContact * contact, unsigned limit) const
{
	// Find the length of the rod
	real currentLen = currentLength();

	// check if we're over-extended
	if (currentLen == length) return 0;

	// Otherwise return the contact
	contact->particle[0] = particle[0];
	contact->particle[1] = particle[1];

	// Calculate the normal
	glm::vec3 normal = glm::normalize(particle[1]->getPosition() - particle[0]->getPosition());
	
	// The contact normal depends on whether we're extending or compressing
	if (currentLen > length)
	{
		contact->contactNormal = normal;
		contact->penetration = currentLen - length;
	}
	else
	{
		contact->contactNormal = normal * real(-1.0);
		contact->penetration = length - currentLen;
	}

	// Always use zero restitution (no bounciness)
	contact->restitution = 0;

	return 1;
}

real GPED::ParticleConstraint::currentLength() const
{
	glm::vec3 relativePos = particle->getPosition() - anchor;
	return glm::length(relativePos);
}

unsigned GPED::ParticleCableConstraint::addContact(ParticleContact * contact, unsigned limit) const
{
	// Find the length of the cable
	real length = currentLength();

	// Check if we're over-extended
	if (length < maxLength)
	{
		return 0;
	}

	// Otherwise return the contact
	contact->particle[0] = particle;
	contact->particle[1] = NULL; // Immovable object

	// Calculate the normal
	glm::vec3 normal = anchor - particle->getPosition();
	normal = glm::normalize(normal);
	contact->contactNormal = normal;

	contact->penetration = length - maxLength;
	contact->restitution = restitution;

	return 1;
}

unsigned GPED::ParticleRodConstraint::addContact(ParticleContact * contact, unsigned limit) const
{
	// Find the length of the rod
	real currentLen = currentLength();

	// Check if we're over extended
	if (currentLen == length)
		return 0;


}
