#include <CGPrecompiled.h>
#include "GPED_Pcontacts.h"
using namespace GPED;

void GPED::ParticleContact::resolve(real duration)
{
	resolveVelocity(duration);
	resolveInterpenetration(duration);
}

real GPED::ParticleContact::calculateSeparatingVelocity() const
{
	glm::vec3 relativeVelocity = particle[0]->getVelocity();
	if (particle[1]) relativeVelocity -= particle[1]->getVelocity();
	return glm::dot(relativeVelocity, contactNormal);
}

void GPED::ParticleContact::resolveVelocity(real duration)
{
	// Find the velocity in the direction of the contact
	real separatingVelocity = calculateSeparatingVelocity();

	// Check whether it needs to be resolved
	if (separatingVelocity > 0)
	{
		// The contact is either separating or stationary - there's
		// no implies required.
		return;
	}

	// Calculate the new separating velocity.
	real newSepVelocity = -separatingVelocity * restitution;

	// Check the velocity build-up due to acceleration only
	glm::vec3 accCausedVelocity = particle[0]->getAcceleration();
	if (particle[1]) accCausedVelocity -= particle[1]->getAcceleration();
	real accCausedSepVelocity = glm::dot(accCausedVelocity, contactNormal) * duration;

	// If we've got a closing velocity due to acceleration build-up,
	// remove it from the new separating velocity,
	if (accCausedSepVelocity < 0)
	{
		newSepVelocity += restitution * accCausedSepVelocity;

		// Make sure we haven't removed more than was
		// there to remove.
		if (newSepVelocity < 0) newSepVelocity = 0;
	}

	real deltaVelocity = newSepVelocity - separatingVelocity;

	// We apply the change in velocity to each object in proportion to
	// its inverse mass (i.e., those with lower inverse mass [higher
	// actual mass] get less change in velocity).
	real totalInverseMass = particle[0]->getInverseMass();
	if (particle[1]) totalInverseMass += particle[1]->getInverseMass();

	// If all particles have infinite mass, then impulses have no effect.
	if (totalInverseMass <= 0) return;

	// Calculate the impulses to apply
	// Find the amount of impulse per unit of inverse mass.
	real impulse = deltaVelocity / totalInverseMass;

	glm::vec3 impulsePerIMass = contactNormal * impulse;

	// Apply impulses: they are applied in the direction of the contact,
	// and are proportional to the inverse mass.
	particle[0]->setVelocity
	(
		particle[0]->getVelocity() + impulsePerIMass * particle[0]->getInverseMass()
	);
	if (particle[1])
	{
		// Particle 1 goes in the opposite direction
		particle[1]->setVelocity
		(
			particle[1]->getVelocity() + impulsePerIMass * -particle[0]->getInverseMass()
		);
	}
}

void GPED::ParticleContact::resolveInterpenetration(real duration)
{
	// If we don't have any penetration, skip this step.
	if (penetration <= 0) return;

	// The movement of each object is based on its inverse mass, so
	// total that.
	real totalInverseMass = particle[0]->getInverseMass();
	if (particle[1]) totalInverseMass += particle[1]->getInverseMass();

	// If all particles have infinite mass, thenwe do nothing
	if (totalInverseMass <= 0) return;

	// FInd the amount of penetration resolution per unit of inverse mass.
	glm::vec3 movePerIMass = contactNormal * (penetration / totalInverseMass);

	// Calculate the movement amounts
	particleMovement[0] = movePerIMass * particle[0]->getInverseMass();
	if (particle[1])
		particleMovement[1] = movePerIMass * -particle[1]->getInverseMass();
	else
		particleMovement[1] = glm::vec3(0.0);

	// Apply the penetration resolution
	particle[0]->setPosition
	(
		particle[0]->getPosition() + particleMovement[0]
	);
	if (particle[1])
	{
		particle[1]->setPosition
		(
			particle[1]->getPosition() + particleMovement[1]
		);
	}
}

GPED::ParticleContactResolver::ParticleContactResolver(unsigned iterations)
	:iterations(iterations), iterationUsed(0)
{ }

void GPED::ParticleContactResolver::setIterations(unsigned iterations)
{
	ParticleContactResolver::iterations = iterations;
}

void GPED::ParticleContactResolver::resolveContacts(ParticleContact* contactArray, unsigned numContacts, real duration)
{
	unsigned i;

	iterationUsed = 0;
	while (iterationUsed < iterations)
	{
		// Find the contact with the largest closing velocity;
		real max = REAL_MAX;
		unsigned maxIndex = numContacts;
		for (i = 0; i < numContacts; ++i)
		{
			real sepVel = contactArray[i].calculateSeparatingVelocity();
			if (sepVel < max && (sepVel < 0 || contactArray[i].penetration > 0))
			{
				max = sepVel;
				maxIndex = i;
			}
		}

		// Do we have anything worth resolving?
		if (maxIndex == numContacts) break;

		// Resolve this contact
		contactArray[maxIndex].resolve(duration);
		
		//Update the interpenetrations for all particles
		glm::vec3* move = contactArray[maxIndex].particleMovement;
		for (i = 0; i < numContacts; ++i)
		{
			if (contactArray[i].particle[0] == contactArray[maxIndex].particle[0])
			{
				// Project the move[0] vector on contact Normal
				contactArray[i].penetration -= glm::dot(move[0], contactArray[i].contactNormal);
			}
			else if (contactArray[i].particle[0] == contactArray[maxIndex].particle[1])
			{
				contactArray[i].penetration -= glm::dot(move[1], contactArray[i].contactNormal);
			}

			if (contactArray[i].particle[1])
			{
				if (contactArray[i].particle[1] == contactArray[maxIndex].particle[0])
				{
					contactArray[i].penetration += glm::dot(move[0], contactArray[i].contactNormal);
				}
				else if (contactArray[i].particle[1] == contactArray[maxIndex].particle[1])
				{
					contactArray[i].penetration += glm::dot(move[1], contactArray[i].contactNormal);
				}
			}
		}

		++iterationUsed;
	}
}
