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

void GPED::ContactResolver::resolveContacts(ContactManager * CM, real duration)
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

void GPED::ContactResolver::prepareContacts(ContactManager * CM, real duration)
{
	CM->AllcalculateInternals(duration);
}

void GPED::ContactResolver::adjustPositions(ContactManager * CM, real duration)
{
	unsigned i, index;
	glm::vec3 linearChange[2], angularChange[2];
	real max;
	glm::vec3 deltaPosition;

	// sorting contacts
	CM->sortByPenetration();

	for (unsigned i = 0; i < positionIterations; ++i)
	{
		Contact* MaxPenetrationOne = CM->GetFirstContact();

		if (MaxPenetrationOne->penetration < 0) break;

		MaxPenetrationOne->matchAwakeState();
		MaxPenetrationOne->applyPositionChange(linearChange, angularChange);


	}

	// iteratively resolve interpenetrations in order of severity.
	positionIterationsUsed = 0;
	while (positionIterationsUsed < positionIterations)
	{
		// Find biggest penetration
		max = positionEpsilon;
		index = numContacts;


		for (i = 0; i < numContacts; ++i)
		{
			if (c[i].penetration > max)
			{
				max = c[i].penetration;
				index = i;
			}
		}
		if (index == numContacts) break;

		// Match the awake state at the contact
		c[index].matchAwakeState();

		// Resolve the penetration.
		c[index].applyPositionChange(linearChange, angularChange);

		// Again this action may have changed the penetration of other
		// bodies, so we update contacts.
		for (i = 0; i < numContacts; ++i)
		{
			// Check each body in the contact
			for (unsigned b = 0; b < 2; ++b)
				if (c[i].body[b])
				{
					real sign = (b ? 1 : -1);
					// Check for a match with each body in the newly
					// resolved contact
					for (unsigned d = 0; d < 2; ++d)
					{
						if (c[i].body[b] == c[index].body[d])
						{
							deltaPosition = linearChange[d] +
								glm::cross(angularChange[d], c[i].relativeContactPosition[b]);

							// The sign of the change is positive if we're
							// dealing with the second body in a contaact
							// and negative otherwise (because we're
							// subtracting the resolution)..
							c[i].penetration += glm::dot(deltaPosition, c[i].contactNormal) * sign;
						}
					}

				}
		}

		++positionIterationsUsed;
	}
}

void GPED::ContactResolver::adjustVelocities(ContactManager * c, unsigned numContacts, real duration)
{
	glm::vec3 velocityChange[2], rotationChange[2];
	glm::vec3 deltaVel, tempVec;
	real max;
	int sign;

	// iteratively handle impacts in order of severity.
	velocityIterationsUsed = 0;
	while (velocityIterationsUsed < velocityIterations)
	{
		// Find contact with maximum magnitude of probabl velocity change.
		max = velocityEpsilon;
		unsigned index = numContacts;
		for (unsigned i = 0; i < numContacts; ++i)
		{
			if (c[i].desiredDeltaVelocity > max)
			{
				max = c[i].desiredDeltaVelocity;
				index = i;
			}
		}
		if (index == numContacts) break;

		// Match the awake state at the contact
		c[index].matchAwakeState();

		// Do the resolution on the contact that came out top.
		c[index].applyVelocityChange(velocityChange, rotationChange);

		// With the change in velocity of the two bodies, the update of
		// contact velocities means that some of the relative closing
		// velocities need recomputing.
		for (unsigned i = 0; i < numContacts; ++i)
		{
			// Check each body in the contact
			for (unsigned b = 0; b < 2; ++b)
				if (c[i].body[b])
				{
					sign = b ? real(-1) : real(1);
					// Check for a match with each body in the newly
					// resolved contact
					for (unsigned d = 0; d < 2; ++d)
					{
						if (c[i].body[b] == c[index].body[d])
						{
							deltaVel = velocityChange[d] +
								glm::cross(rotationChange[d], c[i].relativeContactPosition[b]);

							// The sign of the change is negative if we're dealing 
							// with the second body in a contact.
							for (int col = 0; col < 3; ++col)
								tempVec[col] = glm::dot(c[i].contactToWorld[col], deltaVel) * sign;

							c[i].contactVelocity += tempVec;
							c[i].calculateDesiredDeltaVelocity(duration);
						}
					}
				}
		}

		++velocityIterationsUsed;
	}
}