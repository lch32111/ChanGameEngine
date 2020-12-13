#include <CGPrecompiled.h>
#include "GPED_ContactResolver.h"

GPED::ContactResolver::ContactResolver()
{
}

// Contact Resolver impelmentation
GPED::ContactResolver::ContactResolver(unsigned iterations,
	real velocityEpsilon, real positionEpsilon)
{
	setIterations(iterations, iterations);
	setEpsilon(velocityEpsilon, positionEpsilon);
}

GPED::ContactResolver::ContactResolver(
	unsigned velocityIterations, unsigned positionIterations,
	real velocityEpsilon, real positionEpsilon)
{
	setIterations(velocityIterations, positionIterations);
	setEpsilon(velocityEpsilon, positionEpsilon);
}

void GPED::ContactResolver::setIterations(unsigned velocityIterations, unsigned positionIterations)
{
	ContactResolver::velocityIterations = velocityIterations;
	ContactResolver::positionIterations = positionIterations;
}

void GPED::ContactResolver::setEpsilon(real velocityEpsilon, real positionEpsilon)
{
	ContactResolver::velocityEpsilon = velocityEpsilon;
	ContactResolver::positionEpsilon = positionEpsilon;
}

void GPED::ContactResolver::resolveContacts(CGProj::CGContactManager * CM, real duration)
{
	// Make sure we have something to do
	if (CM->GetNodeCount() == 0) return;
	if (!isValid()) return;

	// Prepare the contacts for processing
	prepareContacts(CM, duration);

	// Resolve the interpenetration problems with the contacts
	adjustPositions(CM, duration);

	// Resolve the velocity problems with the contacts.
	adjustVelocities(CM, duration);
}

void GPED::ContactResolver::prepareContacts(CGProj::CGContactManager * CM, real duration)
{
	CM->AllcalculateInternals(duration);
}

void GPED::ContactResolver::adjustPositions(CGProj::CGContactManager * CM, real duration)
{
	glm::vec3 linearChange[2], angularChange[2];

	for (unsigned i = 0; i < positionIterations; ++i)
	{
		Contact* MaxPenetrationOne = CM->GetMaxPenetration();

		if (MaxPenetrationOne->penetration < positionEpsilon) break;
		
		MaxPenetrationOne->matchAwakeState();
		MaxPenetrationOne->applyPositionChange(linearChange, angularChange);

		// Contacts Update of First Body
		int bodyContact = MaxPenetrationOne->body[0]->contacts;
		CM->updatePenetration(bodyContact, MaxPenetrationOne->myId, 0,  // 0 == means First Body
			linearChange[0], angularChange[0]);

		// Contacts Update of Second Body
		if (MaxPenetrationOne->body[1])
		{
			bodyContact = MaxPenetrationOne->body[1]->contacts;
			CM->updatePenetration(bodyContact, MaxPenetrationOne->myId, 1, // 1 == means Second Body
				linearChange[1], angularChange[1]);
		}
	}
}

void GPED::ContactResolver::adjustVelocities(CGProj::CGContactManager * CM, real duration)
{
	glm::vec3 velocityChange[2], rotationChange[2];
	
	for (unsigned i = 0; i < velocityIterations; ++i)
	{
		Contact* MaxVelocityOne = CM->GetMaxVelocity();
		if (MaxVelocityOne->desiredDeltaVelocity < velocityEpsilon) break;
		
		MaxVelocityOne->matchAwakeState();
		MaxVelocityOne->applyVelocityChange(velocityChange, rotationChange);

		// Contacts Update of First Body
		int bodyContact = MaxVelocityOne->body[0]->contacts;
		CM->updateDesiredVelocity(bodyContact, MaxVelocityOne->myId, 0, // 0 == means First Body
			velocityChange[0], rotationChange[0], duration);

		// Contacts Update of Second Body
		if (MaxVelocityOne->body[1])
		{
			bodyContact = MaxVelocityOne->body[1]->contacts;
			CM->updateDesiredVelocity(bodyContact, MaxVelocityOne->myId, 1, // 1 == means Second Body
				velocityChange[1], rotationChange[1], duration);
		}
	}
}