#include <CGPrecompiled.h>
#include "GPED_fgen.h"
using namespace GPED;

GPED::Gravity::Gravity(const glm::vec3 & gravity)
	: gravity(gravity)
{ }

void GPED::Gravity::updateForce(RigidBody * body, real duration)
{
	// Check that we do not have infinite mass
	if (!body->hasFiniteMass()) return;

	// Apply the mass-scaled force to the body
	body->addForce(gravity * body->getMass());
}

GPED::Spring::Spring(const glm::vec3 & localConnectionPt, RigidBody * other, const glm::vec3 & otherConnectionPt, real springConstant, real restLength)
	: connectionPoint(localConnectionPt), 
	other(other), otherConnectionPoint(otherConnectionPt),
	springConstant(springConstant), restLength(restLength)
{ }

void GPED::Spring::updateForce(RigidBody * body, real duration)
{
	// Calculate the two ends in world space
	glm::vec3 lws = body->getPointInWorldSpace(connectionPoint);
	glm::vec3 ows = other->getPointInWorldSpace(otherConnectionPoint);

	// Calculate the vector of the spring
	glm::vec3 force = lws - ows;

	// Calculate the magnitude of the force
	real magnitude = glm::length(force);
	magnitude = real_abs(magnitude - restLength);
	magnitude *= springConstant;

	// Calculate the final force and apply it
	force = glm::normalize(force);
	force *= -magnitude;
	body->addForceAtPoint(force, lws);
}

GPED::Aero::Aero(const glm::mat3 & tensor, const glm::vec3 & position, const glm::vec3 * windspeed)
	: tensor(tensor), position(position), windspeed(windspeed)
{ }

void GPED::Aero::updateForce(RigidBody * body, real duration)
{
	Aero::updateForceFromTensor(body, duration, tensor);
}

void GPED::Aero::updateForceFromTensor(RigidBody * body, real duration, const glm::mat3 & tensor)
{
	// Calculate total velocity (windspeed and body's velocity)
	glm::vec3 velocity = body->getVelocity();
	velocity += *windspeed;

	// Calculate the velocity in body coordinates
	// get Rotation 3x3 matrix and transpose(inverse) it to change the basis.
	glm::vec3 bodyVel = glm::transpose(glm::mat3(body->getTransform())) * velocity;

	// Calculate the force in body coordinates
	glm::vec3 bodyForce = tensor * bodyVel;

	// I think this code is returning the basis to the world coordinate again
	// so as to apply the force in world coordinates.
	glm::vec3 force = glm::mat3(body->getTransform()) * bodyForce;

	// Apply the force
	body->addForceAtBodyPoint(force, position);
}

// Helper Function for the matrix interpolation
static glm::mat3 linearInterpolate(const glm::mat3& a, const glm::mat3& b, real prop)
{
	glm::mat3 result;
	for (unsigned i = 0; i < 3; ++i)
		for (unsigned j = 0; j < 3; ++j)
			result[i][j] = a[i][j] * (1 - prop) + b[i][j] * prop;
	return result;
}

glm::mat3 GPED::AeroControl::getTensor()
{
	if (controlSetting <= real(-1.0)) return minTensor;
	else if (controlSetting >= real(1.0)) return maxTensor;
	else if (controlSetting < 0)
	{
		return linearInterpolate(minTensor, tensor, controlSetting + real(1.0));
	}
	else if (controlSetting > 0)
	{
		return linearInterpolate(tensor, maxTensor, controlSetting);
	}
	else return tensor;
}

GPED::AeroControl::AeroControl(const glm::mat3 & base,
	const glm::mat3 & min, const glm::mat3 & max, 
	const glm::vec3 & position, const glm::vec3 * windspeed)
	: Aero(base, position, windspeed),
	minTensor(min), maxTensor(max), controlSetting(0.0)
{ }

void GPED::AeroControl::setControl(real value)
{
	controlSetting = value;
}

void GPED::AeroControl::updateForce(RigidBody * body, real duration)
{
	glm::mat3 tensor = getTensor();
	Aero::updateForceFromTensor(body, duration, tensor);
}

GPED::AngledAero::AngledAero(const glm::mat3 & tensor, const glm::vec3 & position, const glm::vec3 * windspeed)
	:Aero(tensor, position, windspeed)
{ }

void GPED::AngledAero::setOrientation(const glm::quat & quat)
{
	orientation = quat;
}

void GPED::AngledAero::updateForce(RigidBody * body, real duration)
{

}


GPED::Buoyancy::Buoyancy(const glm::vec3 & cOfB, real maxDepth, real volume, real waterHeight, real liquidDensity)
	: centreOfBuoyancy(cOfB), maxDepth(maxDepth),
	volume(volume), waterHeight(waterHeight), liquidDensity(liquidDensity)
{ }

void GPED::Buoyancy::updateForce(RigidBody * body, real duration)
{
	// Calculate the submersion depth
	glm::vec3 pointInWorld = body->getPointInWorldSpace(centreOfBuoyancy);
	real depth = pointInWorld.y;

	// Check if we're out of the water
	if (depth >= waterHeight + maxDepth) return;
	glm::vec3 force(0);

	// Check if we're at maximum depth
	if (depth <= waterHeight - maxDepth)
	{
		force.y = liquidDensity * volume;
		body->addForceAtBodyPoint(force, centreOfBuoyancy);
		return;
	}

	// Otherwise we are partly submergered
	force.y = liquidDensity * volume * (depth - maxDepth - waterHeight) / 2 * maxDepth;
	body->addForceAtBodyPoint(force, centreOfBuoyancy);
}


void GPED::ForceRegistry::add(RigidBody * body, ForceGenerator * fg)
{
	ForceRegistry::ForceRegistration registration;
	registration.body = body;
	registration.fg = fg;
	registrations.push_back(registration);
}

void GPED::ForceRegistry::remove(RigidBody * body, ForceGenerator * fg)
{
	Registry::iterator i = registrations.begin();
	for (; i != registrations.end(); ++i)
		if (i->body == body && i->fg == fg)
		{
			registrations.erase(i);
			break;
		}
}

void GPED::ForceRegistry::clear()
{
	registrations.clear();
}

void GPED::ForceRegistry::updateForces(real duration)
{
	Registry::iterator i = registrations.begin();
	for (; i != registrations.end(); ++i)
		i->fg->updateForce(i->body, duration);
}