#include <CGPrecompiled.h>
#include "GPED_pworld.h"
using namespace GPED;

ParticleWorld::ParticleWorld(unsigned maxContacts, unsigned iterations)
	: resolver(iterations), maxContacts(maxContacts)
{
	contacts = new ParticleContact[maxContacts];
	calculateIterations = (iterations == 0);
}

ParticleWorld::~ParticleWorld()
{
	delete[] contacts;
}

void ParticleWorld::startFrame()
{
	for (Particles::iterator p = particles.begin(); p != particles.end(); ++p)
	{
		// Remoe all forces from the accumulator
		(*p)->clearAccumulator();
	}
}

unsigned ParticleWorld::generateContacts()
{
	unsigned limit = maxContacts;
	ParticleContact* nextContact = contacts;
	for (ContactGenerators::iterator g = contactGenerators.begin();
		g != contactGenerators.end(); ++g)
	{
		unsigned used = (*g)->addContact(nextContact, limit);
		limit -= used;
		nextContact += used;

		// We've run out of contacts to fill. This means we're missing
		// contacts.
		if (limit <= 0) break;
	}

	// Return the number of contacts used.
	return maxContacts - limit;
}

void ParticleWorld::integrate(real duration)
{
	for (Particles::iterator p = particles.begin();
		p != particles.end();
		++p)
	{
		(*p)->integrate(duration);
	}
}

void ParticleWorld::runPhysics(real duration)
{
	// Frist apply the force generators
	registry.updateForces(duration);

	// Then integrate the objects
	integrate(duration);

	// Generate contacts
	unsigned usedContacts = generateContacts();

	if (usedContacts)
	{
		if (calculateIterations) resolver.setIterations(usedContacts * 2);
		resolver.resolveContacts(contacts, usedContacts, duration);
	}
}

ParticleWorld::Particles& ParticleWorld::getParticles() 
{
	return particles;
}

ParticleWorld::ContactGenerators & GPED::ParticleWorld::getContactGenerators()
{
	return contactGenerators;
}

ParticleForceRegistry & GPED::ParticleWorld::getForceRegistry()
{
	return registry;
}

void GPED::GroundContacts::init(ParticleWorld::Particles* particles)
{
	GroundContacts::particles = particles;
}

unsigned GPED::GroundContacts::addContact(ParticleContact* contact, unsigned limit) const
{
	unsigned count = 0;
	for (ParticleWorld::Particles::iterator p = particles->begin();
		p != particles->end();
		++p)
	{
		real y = (*p)->getPosition().y;

		if (y < 0.f)
		{
			contact->contactNormal = glm::vec3(0.f, 1.f, 0.f);
			contact->particle[0] = *p;
			contact->particle[1] = NULL;
			contact->penetration = -y;
			contact->restitution = 0.2f;
			++contact;
			++count;
		}

		if (count >= limit) return count;
	}

	return count;
}
