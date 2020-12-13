#include <CGPrecompiled.h>
#include "GPED_Pfgen.h"

using namespace GPED;

GPED::ParticleGravity::ParticleGravity(const glm::vec3 & gravity)
	: gravity(gravity)
{ }

void GPED::ParticleGravity::updateForce(GPEDParticle* particle, real duration)
{
	// Check that we do not have infinite mass.
	if (!particle->hasFiniteMass()) return;

	// Apply the mass-scaled for to the particle
	particle->addForce(gravity * particle->getMass());
}

GPED::ParticleDrag::ParticleDrag(real k1, real k2)
	: k1(k1), k2(k2)
{ }

void GPED::ParticleDrag::updateForce(GPEDParticle * particle, real duration)
{
	glm::vec3 force = particle->getVelocity();

	// Calculate the total drag coefficient.
	real dragCoeff = glm::length(force);
	dragCoeff = k1 * dragCoeff + k2 * dragCoeff * dragCoeff;

	// Calculate the final force and aplly it.
	force = glm::normalize(force);
	force *= -dragCoeff;
	particle->addForce(force);
}

GPED::ParticleSpring::ParticleSpring(GPEDParticle* other, real springConstant, real restLength)
	: other(other), springConstant(springConstant), restLength(restLength)
{ }

void GPED::ParticleSpring::updateForce(GPEDParticle* particle, real duration)
{
	// Calculate the vector of the spring
	glm::vec3 force = particle->getPosition();
	force -= other->getPosition();

	// Calculate the magnitude of the force.
	real magnitude = glm::length(force);
	magnitude = magnitude - restLength;
	magnitude *= springConstant;

	// Calculate the final force and apply it.
	force = glm::normalize(force);
	force *= -magnitude;
	particle->addForce(force);
}

GPED::ParticleAnchoredSpring::ParticleAnchoredSpring(glm::vec3* anchor, real springConstant, real restLength)
	: anchor(anchor), springConstant(springConstant), restLength(restLength)
{ }

void GPED::ParticleAnchoredSpring::updateForce(GPEDParticle* particle, real duration)
{
	// Calculate the vector of the spring.
	glm::vec3 force = particle->getPosition();
	force -= *anchor;

	// Calculate the magnitude of the force.
	real magnitude = glm::length(force);
	magnitude = magnitude - restLength;
	magnitude *= springConstant;

	// Calculate the final force and apply it
	force = glm::normalize(force);
	force *= -magnitude;
	particle->addForce(force);
}

GPED::ParticleBungee::ParticleBungee(GPEDParticle* other, real springConstant, real restLength)
	: other(other), springConstant(springConstant), restLength(restLength)
{ }

void GPED::ParticleBungee::updateForce(GPEDParticle* particle, real duration)
{
	// Calculate the vector of the spring
	glm::vec3 force = particle->getPosition();
	force -= other->getPosition();

	// Check if the bungee is compressed
	real magnitude = glm::length(force);
	if (magnitude <= restLength) return;

	// Calculate the magnitude of the force.
	magnitude = springConstant * (magnitude - restLength);

	// Calculate the final force and apply it
	force = glm::normalize(force);
	force *= -magnitude;
	particle->addForce(force);
}

void GPED::ParticleAnchoredBungee::updateForce(GPEDParticle * particle, real duration)
{
	// Calculate the vector of the spring.
	glm::vec3 force = particle->getPosition();
	force -= *anchor;

	// Calculate the magnitude of the force
	real magnitude = glm::length(force);
	if (magnitude <= restLength) return;

	magnitude = springConstant * (magnitude - restLength);

	// Calculate the final force and apply it
	force = glm::normalize(force);
	force *= -magnitude;
	particle->addForce(force);
}

GPED::ParticleBuoyancy::ParticleBuoyancy(real radius, real linearDrag, real waterHeight, real liquidDensity)
	:particleRadius(radius), linearDrag(linearDrag),
	waterHeight(waterHeight), liquidDensity(liquidDensity)
{ }

void GPED::ParticleBuoyancy::updateForce(GPEDParticle * particle, real duration)
{
	// Calculate the submersion depth.
	real depth = particle->getPosition().y;

	// Check if we're out of the water.
	if (depth - particleRadius >= waterHeight) return;

	glm::vec3 force(real(0.0));
	real partialMass = particle->getMass();

	// Fully Submerged
	if (depth + particleRadius <= waterHeight)
	{
		force.y = liquidDensity * (particleRadius * 2) * real(10.0); // gravity 10
	}
	else // Partially Submerged
	{
		// Volume approximated as y-axis submerged height
		real V = (waterHeight - (depth - particleRadius)); 
		force.y = liquidDensity * V * real(10.0); // gravity 10
		partialMass *= V / (particleRadius * 2);
	}
	
	// http://www.iforce2d.net/b2dtut/buoyancy 
	// Refer to the tutorial of buoyancy. the drag is approximation.
	// water velocity is assumed by (0, 0, 0)
	// https://box2d.org/downloads/
	// Refer to the buoyancy demo of Eric Catto
	// the water linear drag is normally 5.0, according to the demo
	float dragForce = (partialMass * linearDrag) * (-particle->getVelocity().y);
	force.y += dragForce;

	particle->addForce(force);
}

GPED::ParticleFakeSpring::ParticleFakeSpring(glm::vec3 * anchor, real springConstant, real damping)
	: anchor(anchor), springConstant(springConstant), damping(damping)
{ }


// It's not working. Don't use this now. If you want to use this,
// You have to know the simple and damped harmonic motion of the spring movement.
void GPED::ParticleFakeSpring::updateForce(GPEDParticle * particle, real duration)
{
	// Check that we do not have infinite mass
	if (!particle->hasFiniteMass()) return;

	// Calculate the relative position of the particle to the anchor
	glm::vec3 position = particle->getPosition();
	position -= *anchor;
	
	// Calculate the constants and check they are in bounds
	real gamma = real(0.5) * real_sqrt(4 * springConstant - damping * damping);
	if (gamma == real(0.0)) return;
	glm::vec3 c = position * (damping / (real(2.0) * gamma)) +
		particle->getVelocity() * (real(1.0) / gamma);

	// Calculate the target position
	glm::vec3 target = position * real_cos(gamma * duration) +
		c * real_sin(gamma * duration);
	target *= real_exp(real(-0.5) * damping * duration);

	// Calculate the resulting acceleration and therefore the force
	glm::vec3 accel = (target - position) * real(1.0) / (duration * duration) -
		particle->getVelocity() * duration;
	particle->addForce(accel * particle->getMass());
}


void GPED::ParticleForceRegistry::add(GPEDParticle* particle, ParticleForceGenerator* fg)
{
	registrations.push_back(ParticleForceRegistration{ particle, fg });
}

void GPED::ParticleForceRegistry::remove(GPEDParticle* particle, ParticleForceGenerator* fg)
{
	Registry::iterator i = registrations.begin();
	for (; i != registrations.end(); ++i)
		if (i->particle == particle && i->fg == fg)
		{
			registrations.erase(i);
			break;
		}
}

void GPED::ParticleForceRegistry::clear()
{
	registrations.clear();
}

void GPED::ParticleForceRegistry::updateForces(real duration)
{
	Registry::iterator i = registrations.begin();
	for (; i != registrations.end(); ++i)
		i->fg->updateForce(i->particle, duration);
}

