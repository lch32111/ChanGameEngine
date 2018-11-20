#include "GPED_world.h"
using namespace GPED;

GPED::World::World(unsigned maContacts, unsigned iterations)
	:firstBody(NULL),
	resolver(iterations),
	firstContactGen(NULL),
	maxContacts(NULL)
{
	contacts = new Contact[maxContacts];
	calculateIterations = (iterations == 0);
}

GPED::World::~World()
{
	delete[] contacts;
}

unsigned GPED::World::generateContacts()
{
	unsigned limit = maxContacts;
	Contact* nextContact = contacts;

	ContactGenRegistration* reg = firstContactGen;
	while (reg)
	{
		unsigned used = reg->gen->addContact(nextContact, limit);
		limit -= used;
		nextContact += used;

		// We've run out of contacts to fill. This means we'remissing
		// contacts
		if (limit <= 0) break;

		reg = reg->next;
	}

	// Return the number of contacts used.
	return maxContacts - limit;
}

void GPED::World::runPhysics(real duration)
{
	// First apply the force generators
	//registry.updateForces(duration);

	// Then integrate the objects
	BodyRegistration* reg = firstBody;
	while (reg)
	{
		// integrate
		reg->body->integrate(duration);

		// Get the next registration
		reg = reg->next;
	}

	// Generate contacts
	unsigned usedContacts = generateContacts();

	// And process them
	if (calculateIterations) resolver.setIterations(usedContacts * 4, usedContacts * 4);
	// resolver.resolveContacts(contacts, usedContacts, duration);
}

void GPED::World::startFrame()
{
	BodyRegistration* reg = firstBody;
	while (reg)
	{
		// Remove all forces from the accumulator
		reg->body->clearAccumulators();
		reg->body->calculateDerivedData();

		// Get the next registration
		reg = reg->next;
	}
}
