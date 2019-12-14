#include "GPED_contacts.h"
#include <iostream>

GPED::Contact::Contact()
	: friction(0), restitution(0),
	contactPoint(glm::vec3(0)), contactNormal(glm::vec3(0)),
	penetration(0), contactToWorld(glm::mat3(0)), 
	contactVelocity(glm::vec3(0)), desiredDeltaVelocity(0)
{
	body[0] = body[1] = NULL;
	relativeContactPosition[0] = relativeContactPosition[1] = glm::vec3(0);
}

/*
void GPED::Contact::setBodyData(RigidBody * one, RigidBody * two, 
	real friction, real restitution)
{
	Contact::body[0] = one;
	Contact::body[1] = two;
	Contact::friction = friction;
	Contact::restitution = restitution;

	int index = 0;
	if (one)
	{
		Contact* move = one->contacts;

		if (move)
		{
			index = (move->body[0] == one) ? 0 : 1;
			while (move->nextObjects[index] != nullptr)
			{
				move = move->nextObjects[index];
				index = (move->body[0] == one) ? 0 : 1;
			}

			move->nextObjects[index] = this;
		}
		else
		{
			one->contacts = this;
		}
	}
	
	if (two)
	{
		Contact* move = two->contacts;

		if(move)
		{
			index = (move->body[0] == two) ? 0 : 1;
			while (move)
			{
				move = move->nextObjects[index];
				index = (move->body[0] == two) ? 0 : 1;
			}

			move->nextObjects[index] = this;
		}
		else
		{
			two->contacts = this;
		}
	}
}
*/

/**
 * Swaps the bodies in the current contact. so body 0 is at body 1 and
 * vice versa. This also changes the direction of the contact normal,
 * but doesn't update any calculated internal data. If you are calling
 * this method manually, then call calculateInternals afterwards to
 * make sure the internal data is up to date.
 */
void GPED::Contact::swapBodies()
{
	contactNormal *= -1;

	RigidBody* temp = body[0];
	body[0] = body[1];
	body[1] = temp;
}

void GPED::Contact::matchAwakeState()
{
	// Collisions with the world never cause a body to wake up.
	if (!body[1]) return;

	bool body0awake = body[0]->getAwake();
	bool body1awake = body[1]->getAwake();

	// Wake up only the sleepingone
	if (body0awake ^ body1awake)
	{
		if (body0awake) body[1]->setAwake();
		else body[0]->setAwake();
	}
}

/**
* Constructs an arbitrary orthonormal basis for the contact. This is
* stored as a 3x3 matrix, where each vector is a column (in other
* words the matrix transforms contact space into world space). The x
* direction is generated from the contact normal, and the y and z
* directions are set so they are right angles to it.
*/
/**
 * The slightly modifed Version by ChanHaneg
 * Refer to the issue of github which I uploaded
 * https://github.com/idmillington/cyclone-physics/issues/49
 */
inline void GPED::Contact::calculateContactBasis()
{
	glm::vec3 contactTangent[2];

	// Check whether the Z-axis is nearer to the X or Y axis
	if (real_abs(contactNormal.x) > real_abs(contactNormal.y))
	{
		// Scaling factor to ensure the results are normalised
		const real s = (real)1.0f / real_sqrt(contactNormal.z*contactNormal.z +
			contactNormal.x*contactNormal.x);
	
		/*
		18-11-05 Chanhaeng Lee
		I changed the original GPED code into this.
		The reason is that cross(X(contactNormal), World-Y(0,1,0)) should be below
		I mean contactTangent[0] = cross(contactNormal, World-Y); -> you got Z-axis now
			   contactTangent[1] = cross(contactTangent[0], contactNormal); -> you got Y-axis now
		I guess the original code is wrong in the calculation of the cross product.
		you don't need to set the order of contactToWorld Matrix Columns. 
		Because the matrix is rotation matrix, It's okay for the basis to change y and z axis except the x axis, contactNormal.

		After a few experiments for simulation between original and my own codes,
		I observed that when a box is on another box, my code works better than the original.
		*/
		contactTangent[0].x = -contactNormal.z*s;
		contactTangent[0].y = 0;
		contactTangent[0].z = contactNormal.x*s;

		contactTangent[1].x = -contactNormal.y * contactTangent[0].z;
		contactTangent[1].y = -contactTangent[0].x * contactNormal.z + contactTangent[0].z * contactNormal.x;
		contactTangent[1].z = contactTangent[0].x *contactNormal.y;
	}
	else
	{
		// Scaling factor to ensure the results are normalised
		const real s = (real)1.0 / real_sqrt(contactNormal.z*contactNormal.z +
			contactNormal.y*contactNormal.y);
		/*
		18-11-05 Chanhaeng Lee
		For understandings of this 'else' statement, I added the comment here.
		To prevent the case that the contact normal points to World Y-axis, meaning contactNormal = (0, 1, 0),
		we use this kind of if statement, if (real_abs(contactNormal.x) > real_abs(contactNormal.y)).

		if the contactNormal is (0, 1, 0), the if statement body above will generate Zero Vector Z-axis and Y-axis.
		so, with the if statement, we can avoid this situation. in the case of (0, 1, 0), the x component 0 < y component 1.
		It means that we are guessing if(real_abs(contactNormal.x) < real_abs(contactNormal.y)), 
		the contact normal may point the y-axis. So, in this case, we will assume the contact Normal is Y-axis.
		and we use the World X-axis (1, 0, 0) to generate Z-axis and Y-axis 
		So Actually, In the case, contactNormal == Y-axis, contactTangent[0] == Z-axis, contactTangent[1] = X-axis.
		*/

		// The new X-axis is at right angles to the world X-axis
		contactTangent[0].x = 0;
		contactTangent[0].y = -contactNormal.z*s;
		contactTangent[0].z = contactNormal.y*s;

		// The new Y-axis is at right angles to the new X- and Z- axes
		contactTangent[1].x = contactNormal.y*contactTangent[0].z -
			contactNormal.z*contactTangent[0].y;
		contactTangent[1].y = -contactNormal.x*contactTangent[0].z;
		contactTangent[1].z = contactNormal.x*contactTangent[0].y;
		
	}
	
	// Make a matrix from the three vectors
	contactToWorld[0] = contactNormal;
	contactToWorld[1] = contactTangent[0];
	contactToWorld[2] = contactTangent[1];
}

glm::vec3 GPED::Contact::calculateLocalVelocity(unsigned bodyIndex, real duration)
{
	RigidBody* thisBody = body[bodyIndex];

	// Work out the velocity of the contact point
	glm::vec3 velocity = glm::cross(thisBody->getRotation(), relativeContactPosition[bodyIndex]);
	velocity += thisBody->getVelocity();

	// Turn the velocity into contact-coordinates.
	glm::mat3 WorldToContact = glm::transpose(contactToWorld);
	glm::vec3 contactVelocity = WorldToContact * velocity;

	// Calculate the amount of velocity that is due to forces without reactions.
	glm::vec3 accVelocity = thisBody->getLastFrameAcceleration() * duration;

	// Calculate the velocity in contact-coordinates.
	accVelocity = WorldToContact * accVelocity;

	// We ignore any component of acceleration in the contact normal
	// direction, we are only interested in planar acceleration
	accVelocity.x = 0;

	// Add the planar velcities - if there's enough friction they will
	// be removed during velocity resolution
	contactVelocity += accVelocity;

	// And return it.
	return contactVelocity;
}

void GPED::Contact::calculateDesiredDeltaVelocity(real duration)
{
	const static real velocityLimit = (real)0.25f;

	// Calculate the acceleration induced velocity accumulated this frame
	real velocityFromAcc = 0;

	if (body[0]->getAwake())
	{
		velocityFromAcc += glm::dot(body[0]->getLastFrameAcceleration() * duration, contactNormal);
	}

	if (body[1] && body[1]->getAwake())
	{
		velocityFromAcc -= glm::dot(body[1]->getLastFrameAcceleration() * duration, contactNormal);
	}

	// If the velocity is very slow, limit the restitution
	real thisRestitution = restitution;
	if (real_abs(contactVelocity.x) < velocityLimit)
	{
		thisRestitution = (real)0.0;
	}

	// Combine the bounce velocity with the removed acceleration velocity
	/*
	18-11-05 Chanhaeng Lee
	// desiredDeltaVelocity = -contactVelocity.x - thisRestitution * (contactVelocity.x - velocityFromAcc);
	I changed the above line into the bottom one to help understand the collision resolution process.
	the equation of solving the impulse j is, j = -(1 + Restitution) *  dot(V^(AB), contactNormal)
	But you don't need to do the dot product, because the contactVelocity is in the contact coordinates.
	*/
	desiredDeltaVelocity = -(1 + thisRestitution) * contactVelocity.x + thisRestitution * velocityFromAcc;
}

void GPED::Contact::calculateInternals(real duration)
{
	// Check if the first object is NULL, and swap if it is.
	if (!body[0]) swapBodies();
	assert(body[0]);

	// Calculate an set of axis at the contact point.
	calculateContactBasis();

	// Store te relative position of the contact relative to each body
	relativeContactPosition[0] = contactPoint - body[0]->getPosition();
	if (body[1])
	{
		relativeContactPosition[1] = contactPoint - body[1]->getPosition();
	}

	// Find the relative velocity ofthe bodiesat the contact point.
	contactVelocity = calculateLocalVelocity(0, duration);
	if (body[1])
	{
		contactVelocity -= calculateLocalVelocity(1, duration);
	}

	// Caculate the desired change in velocity for resolution
	calculateDesiredDeltaVelocity(duration);
}

void GPED::Contact::applyImpulse(const glm::vec3 & impulse, RigidBody * body, glm::vec3 * velocityChange, glm::vec3 * rotationChange)
{
}

void GPED::Contact::applyVelocityChange(
	glm::vec3 velocityChange[2], glm::vec3 rotationChange[2])
{
	// Get hold of the inverse mass and inverse inertia tensor, both in
	// world coordintates.
	glm::mat3 inverseInertiaTensor[2];
	body[0]->getInverseInertiaTensorWorld(&inverseInertiaTensor[0]);
	if(body[1]) body[1]->getInverseInertiaTensorWorld(&inverseInertiaTensor[1]);

	// We will calculate the impulse for each contact axis
	glm::vec3 impulseContact;

	if (friction == (real)0.0)
	{
		// use the short format for frictionless contacts
		impulseContact = calculateFrictionlessImpulse(inverseInertiaTensor);
	}
	else
	{
		// Otherwise we may have impulses that aren't in the direction of the
		// contact, so we need the more complex version.
		impulseContact = calculateFrictionImpulse(inverseInertiaTensor);
	}

	// Convert impulse to world coordinates
	glm::vec3 impulse = contactToWorld * impulseContact;

	// Split in the impulse into linear and rotation components
	glm::vec3 impulsiveTorque = glm::cross(relativeContactPosition[0], impulse);
	rotationChange[0] = inverseInertiaTensor[0] * impulsiveTorque;
	velocityChange[0] = impulse * body[0]->getInverseMass();

	// Apply the changes
	body[0]->addVelocity(velocityChange[0]);
	body[0]->addRotation(rotationChange[0]);

	if (body[1])
	{
		// Work out body one's linear and angular changes
		glm::vec3 impulsiveTorque = glm::cross(impulse, relativeContactPosition[1]);
		rotationChange[1] = inverseInertiaTensor[1] * impulsiveTorque;
		velocityChange[1] = -impulse * body[1]->getInverseMass();

		// And apply them.
		body[1]->addVelocity(velocityChange[1]);
		body[1]->addRotation(rotationChange[1]);
	}
}

/* 181107 Chanhaeng Lee
I changed the parameters of this method, applyPositionChange.
The change is just deleting the third parameter, penetration.
The reason is that you can just access the penetration value on the Contact Class.
Because this method is in the Contact Class, you don't need to get the parameter penetration.
*/
void GPED::Contact::applyPositionChange(
	glm::vec3 linearChange[2], glm::vec3 angularChange[2])
{
	real linearInertia[2];
	real angularInertia[2];

	real totalInertia = 0;
	real inverseTotalInertia;

	const real angularLimit = (real)0.2;
	real angularMove[2];
	real linearMove[2];

	// We need to work out the inertia of each object in the direction
	// of the ocntact normal, due to angular inertia only.
	for(unsigned i = 0; i < 2; ++i)
		if (body[i])
		{
			glm::mat3 inverseInertiaTensor;
			body[i]->getInverseInertiaTensorWorld(&inverseInertiaTensor);

			// Use the same procedure as for calculating frictionless
			// velocity change to work out the angular inertia.
			glm::vec3 angularInertiaWorld = glm::cross(relativeContactPosition[i], contactNormal);
			angularInertiaWorld = inverseInertiaTensor * angularInertiaWorld;
			angularInertiaWorld = glm::cross(angularInertiaWorld, relativeContactPosition[i]);
			angularInertia[i] = glm::dot(angularInertiaWorld, contactNormal);

			// The linear component is simply the inverse mass
			linearInertia[i] = body[i]->getInverseMass();

			// keep track of the total inertia from all components
			totalInertia += linearInertia[i] + angularInertia[i];

			// We break the loop here so that the totalInertia value is
			// completely calculated (by both iterations) before continuing.
		}

	// Loop through again calculating and applying the changes
	inverseTotalInertia = real(1) / totalInertia;
	for(unsigned i = 0; i < 2; ++i)
		if (body[i])
		{
			// The linear and angular movements required are in proportion to
			// the two inverse inertias.
			real sign = real((i == 0) ? 1 : -1);
			angularMove[i] = sign * penetration * (angularInertia[i] * inverseTotalInertia);
			linearMove[i] = sign * penetration * (linearInertia[i] * inverseTotalInertia);

			/** Chan Comment
			 *  Refer to the section "Avoiding the large rotation"
			 */
			// To avoid angular projections that are too great (when mass is large
			// but inertia tensor is small) limit the angular move.
			glm::vec3 projection = relativeContactPosition[i];
			projection += contactNormal * -glm::dot(relativeContactPosition[i], contactNormal);

			// use the small angle approximation for the sine of the angle (i.e.
			// the magnitude would be sine(angularLimit) * projection.magnitude
			// but we approximate sign(angularLimit) to angular Limit).
			real  maxMagnitude = angularLimit * glm::length(projection);
			if (angularMove[i] < -maxMagnitude) 
			{
				real totalmove = angularMove[i] + linearMove[i];
				angularMove[i] = -maxMagnitude;
				linearMove[i] = totalmove - angularMove[i];
			}
			else if (angularMove[i] > maxMagnitude) 
			{
				real totalmove = angularMove[i] + linearMove[i];
				angularMove[i] = maxMagnitude;
				linearMove[i] = totalmove - angularMove[i];
			}
			

			// We have the linear amount of movement required by turning
			// the rigid body (in angularMove[i]). We now need to
			// calculate the desired rotation to achieve that
			if (angularMove[i] == 0)
			{
				// Easy case - no angular movement means no rotation.
				angularChange[i] = glm::vec3(real(0));
			}
			else
			{
				// Work out the direction we'd like to rotate in
				glm::vec3 targetAngularDirection = glm::cross(relativeContactPosition[i], contactNormal);

				glm::mat3 inverseInertiaTensor;
				body[i]->getInverseInertiaTensorWorld(&inverseInertiaTensor);

				// Work out the direction we'd need to rotate to achieve that
				angularChange[i] = inverseInertiaTensor * targetAngularDirection * (angularMove[i] / angularInertia[i]);
			}

			// Velocity change is easier - it isjsut the linear movement
			// along the contact normal.
			linearChange[i] = contactNormal * linearMove[i];

			// Now we can start to apply the values we've calculated
			// Apply the linear movement
			glm::vec3 pos;
			body[i]->getPosition(&pos);
			pos += linearChange[i];
			body[i]->setPosition(pos);

			// And the change in orientation
			glm::quat q;
			body[i]->getOrientation(&q);
			glm::quat deltaOrien(0, angularChange[i]);
			deltaOrien *= q;
			q.w += deltaOrien.w * ((real)0.5);
			q.x += deltaOrien.x * ((real)0.5);
			q.y += deltaOrien.y * ((real)0.5);
			q.z += deltaOrien.z * ((real)0.5);
			body[i]->setOrientation(q);

			// We need to calculate the derived data for any body that is
			// asleep, so that the changes are reflected in the object's 
			// data. Otherwise the resolution will not change the position
			// of the object, and the next collision detection round will
			// have the same penetration.
			if (!body[i]->getAwake()) body[i]->calculateDerivedData();
		}

}

inline
glm::vec3 GPED::Contact::calculateFrictionlessImpulse(glm::mat3 * inverseInertiaTensor)
{
	glm::vec3 impulseContact;

	// Build a vector that shows the change in velocity in world space
	// for a unit impulse in the direction of the contact normal.
	glm::vec3 deltaVelWorld = glm::cross(relativeContactPosition[0], contactNormal);
	deltaVelWorld = inverseInertiaTensor[0] * deltaVelWorld;
	deltaVelWorld = glm::cross(deltaVelWorld, relativeContactPosition[0]);

	// Work out the change in velocity in contact coordinates.
	real deltaVelocity = glm::dot(deltaVelWorld, contactNormal);

	// Add the linear component of velocity change
	deltaVelocity += body[0]->getInverseMass();

	// Check if we need to the second body's data
	if (body[1])
	{
		deltaVelWorld = glm::cross(relativeContactPosition[1], contactNormal);
		deltaVelWorld = inverseInertiaTensor[1] * deltaVelWorld;
		deltaVelWorld = glm::cross(deltaVelWorld, relativeContactPosition[1]);

		// Add the change in velocity due to rotation
		deltaVelocity += glm::dot(deltaVelWorld, contactNormal);

		// Add the change in velocity due to linear motion
		deltaVelocity += body[1]->getInverseMass();
	}

	// Calculate the required size of the impulse
	impulseContact.x = desiredDeltaVelocity / deltaVelocity;
	impulseContact.y = 0;
	impulseContact.z = 0;
	return impulseContact;
}


inline
glm::vec3 GPED::Contact::calculateFrictionImpulse(glm::mat3 * inverseInertiaTensor)
{
	glm::vec3 impulseContact;
	real inverseMass = body[0]->getInverseMass();

	// The equivalent of a cross product in matrices is multiplication
	// by a skew symmetric matrix - we build the matrix for converting
	// between linear and angular quantities.
	glm::mat3 impulseToTorque;
	getSkewSymmetric(relativeContactPosition[0], impulseToTorque);

	// Build the matrix to convert contact impulse to change in velocity in world coordinates
	glm::mat3 deltaVelWorld = impulseToTorque;
	deltaVelWorld *= inverseInertiaTensor[0] * impulseToTorque;
	deltaVelWorld *= -1;

	// Check if we need to add body two's data
	if (body[1])
	{
		// SEt the cross product matrix
		getSkewSymmetric(relativeContactPosition[1], impulseToTorque);

		// Calculate the velocity change matrix
		glm::mat3 deltaVelWorld2 = impulseToTorque;
		deltaVelWorld2 *= inverseInertiaTensor[1] * impulseToTorque;
		deltaVelWorld2 *= -1;

		// Add to the total delta velocity
		deltaVelWorld += deltaVelWorld2;

		// Add to the inverse mass
		inverseMass += body[1]->getInverseMass();
	}

	// Do a change of basis to convert into contact coordinates.
	glm::mat3 deltaVelocity = glm::transpose(contactToWorld) * deltaVelWorld * contactToWorld;

	// Add in the linear velocity change
	deltaVelocity[0][0] += inverseMass;
	deltaVelocity[1][1] += inverseMass;
	deltaVelocity[2][2] += inverseMass;

	// Invert to get the impulse needed per unit velocity
	glm::mat3 impulseMatrix = glm::inverse(deltaVelocity);

	// Find the target velocities to kill
	glm::vec3 velKill(desiredDeltaVelocity, -contactVelocity.y, -contactVelocity.z);

	// Find the impulse to kill target velocities
	impulseContact = impulseMatrix * velKill;

	// Check for exceeding friction
	real planarImpulse = real_sqrt(
		impulseContact.y * impulseContact.y
		+ impulseContact.z * impulseContact.z);
	if (planarImpulse > impulseContact.x * friction)
	{
		// We need to use dynamic friction
		impulseContact.y /= planarImpulse;
		impulseContact.z /= planarImpulse;

		impulseContact.x = deltaVelocity[0][0] +
			deltaVelocity[1][0] * friction * impulseContact.y +
			deltaVelocity[2][0] * friction * impulseContact.z;
		impulseContact.x = desiredDeltaVelocity / impulseContact.x;
		impulseContact.y *= friction * impulseContact.x;
		impulseContact.z *= friction * impulseContact.x;
	}

	return impulseContact;
}

void GPED::Contact::updatePenetration(
	const glm::vec3& linearChange, 
	const glm::vec3& angularChange, 
	unsigned index)
{
	real sign = (index ? (real)1 : (real)-1);
	glm::vec3 deltaPosition = linearChange + 
		glm::cross(angularChange, relativeContactPosition[index]);
	penetration += glm::dot(deltaPosition, contactNormal) * sign;
}

void GPED::Contact::updateDesiredVelocity(
	const glm::vec3 & velocityChange, 
	const glm::vec3 & rotationChange, 
	unsigned index,
	real duration
)
{
	real sign = (index ? (real)-1 : (real)1);
	glm::vec3 deltaVel = velocityChange + 
		glm::cross(rotationChange, relativeContactPosition[index]);
	
	glm::vec3 tempVec;
	for (unsigned col = 0; col < 3; ++col)
		tempVec[col] = glm::dot(contactToWorld[col], deltaVel) * sign;

	contactVelocity += tempVec;
	calculateDesiredDeltaVelocity(duration);
}
