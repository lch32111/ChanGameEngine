#include <CGPrecompiled.h>
#include "PS_AmmoRound.h"
#include <GLFW/glfw3.h>
#include <vector>

void AmmoRound::setState(ShotType shotType)
{
	m_shotType = shotType;

	// Set the properties of the particle
	switch (m_shotType)
	{
	case SHOT_PISTOL:
		body->setMass(1.5f);
		body->setVelocity(0.f, 0.f, 20.f);
		body->setAcceleration(0.f, -0.5f, 0.f);
		body->setDamping(0.99f, 0.8f);
		radius = 0.2f;
		break;
	case SHOT_ARTILLERY:
		body->setMass(200.f); // 200.0kg
		body->setVelocity(0.f, 30.f, 40.f); // 50m/s
		body->setAcceleration(0.f, -21.0f, 0.f);
		body->setDamping(0.99f, 0.8f);
		radius = 0.4f;
		break;
	case SHOT_FIREBALL:
		body->setMass(4.f); // 4.0kg - mostly blast damage
		body->setVelocity(0.f, -0.5f, 10.f); // 10m/s
		body->setAcceleration(0.f, 0.3f, 0.f); // Floats up
		body->setDamping(0.9f, 0.8f);
		radius = 0.6f;
		break;
	case SHOT_LASER:
		// Note that this is the kind of laser bolt seen in films,
		// not a realisti laser beam!
		body->setMass(0.1f); // 0.1kg - almost no weight
		body->setVelocity(0.f, 0.f, 100.f); // 100m/s
		body->setAcceleration(0.f, 0.f, 0.f); // No gravity
		body->setDamping(0.99f, 0.8f);
		radius = 0.2f;
		break;
	}

	body->setCanSleep(false);
	body->setAwake();

	glm::mat3 tensor;
	GPED::real coeff = GPED::real(0.4) * body->getMass() * radius * radius;
	GPED::getInertiaTensorCoeffs(tensor, coeff, coeff, coeff);
	body->setInertiaTensor(tensor);

	// Set the data common to all particle types
	body->setPosition(0.f, 1.5f, 0.f);
	startTime = (unsigned)glfwGetTime();
	body->setOrientation(1, 0, 0, 0);
	body->setRotation(0, 0, 0);
	body->clearAccumulators();

	// Clear the force accumulators
	body->calculateDerivedData();
	calculateInternals();
}

void AmmoRound::setState(ShotType shotType, glm::vec3 Position, glm::vec3 Velocity)
{
	m_shotType = shotType;

	// Set the properties of the particle
	switch (m_shotType)
	{
	case SHOT_PISTOL:
		body->setMass(1.5f);
		body->setVelocity(0.f, 0.f, -20.f);
		body->setAcceleration(0.f, -0.5f, 0.f);
		body->setDamping(0.99f, 0.8f);
		radius = 0.2f;
		break;
	case SHOT_ARTILLERY:
		body->setMass(200.f); // 200.0kg
		body->setVelocity(0.f, 30.f, -40.f); // 50m/s
		body->setAcceleration(0.f, -21.0f, 0.f);
		body->setDamping(0.99f, 0.8f);
		radius = 0.4f;
		break;
	case SHOT_FIREBALL:
		body->setMass(4.f); // 4.0kg - mostly blast damage
		body->setVelocity(0.f, -0.5f, -10.f); // 10m/s
		body->setAcceleration(0.f, 0.3f, 0.f); // Floats up
		body->setDamping(0.9f, 0.8f);
		radius = 0.6f;
		break;
	case SHOT_LASER:
		// Note that this is the kind of laser bolt seen in films,
		// not a realisti laser beam!
		body->setMass(0.1f); // 0.1kg - almost no weight
		body->setVelocity(0.f, 0.f, -100.f); // 100m/s
		body->setAcceleration(0.f, 0.f, 0.f); // No gravity
		body->setDamping(0.99f, 0.8f);
		radius = 0.2f;
		break;
	}

	body->setCanSleep(false);
	body->setAwake();

	glm::mat3 tensor;
	GPED::real coeff = GPED::real(0.4) * body->getMass() * radius * radius;
	GPED::getInertiaTensorCoeffs(tensor, coeff, coeff, coeff);
	body->setInertiaTensor(tensor);

	// Set the data common to all particle types
	body->setPosition(Position);
	startTime = (unsigned)glfwGetTime();
	body->setOrientation(1,0,0,0);

	// Camera Direction Shooting
	body->setVelocity(Velocity);

	body->setRotation(0, 0, 0);
	body->clearAccumulators();

	// Clear the force accumulators
	body->calculateDerivedData();
	calculateInternals();
}

void AmmoRound::setState(ShotType shotType, const CGProj::chanQuatCamera& camera)
{
	m_shotType = shotType;

	// Set the properties of the particle
	switch (m_shotType)
	{
	case SHOT_PISTOL:
		body->setMass(1.5f);
		body->setVelocity(0.f, 0.f, -20.f);
		body->setAcceleration(0.f, -0.5f, 0.f);
		body->setDamping(0.99f, 0.8f);
		radius = 0.2f;
		break;
	case SHOT_ARTILLERY:
		body->setMass(200.f); // 200.0kg
		body->setVelocity(0.f, 30.f, -40.f); // 50m/s
		body->setAcceleration(0.f, -21.0f, 0.f);
		body->setDamping(0.99f, 0.8f);
		radius = 0.4f;
		break;
	case SHOT_FIREBALL:
		body->setMass(4.f); // 4.0kg - mostly blast damage
		body->setVelocity(0.f, -0.5f, -10.f); // 10m/s
		body->setAcceleration(0.f, 0.3f, 0.f); // Floats up
		body->setDamping(0.9f, 0.8f);
		radius = 0.6f;
		break;
	case SHOT_LASER:
		// Note that this is the kind of laser bolt seen in films,
		// not a realisti laser beam!
		body->setMass(0.1f); // 0.1kg - almost no weight
		body->setVelocity(0.f, 0.f, -100.f); // 100m/s
		body->setAcceleration(0.f, 0.f, 0.f); // No gravity
		body->setDamping(0.99f, 0.8f);
		radius = 0.2f;
		break;
	}

	body->setCanSleep(false);
	body->setAwake();

	glm::mat3 tensor;
	GPED::real coeff = GPED::real(0.4) * body->getMass() * radius * radius;
	GPED::getInertiaTensorCoeffs(tensor, coeff, coeff, coeff);
	body->setInertiaTensor(tensor);

	// Set the data common to all particle types
	glm::vec3 MinusZaxis(0, 0, -1);
	glm::quat mzQ = camera.Orientation *  glm::quat(0, MinusZaxis)  * glm::conjugate(camera.Orientation);
	glm::vec3 forwardSetting(mzQ.x, mzQ.y, mzQ.z);
	body->setPosition(camera.Position + forwardSetting * 5.f);
	startTime = (unsigned)glfwGetTime();
	body->setOrientation(camera.Orientation);

	// Camera Direction Shooting
	glm::vec3 OriginVel = body->getVelocity();
	glm::quat q = camera.Orientation *  glm::quat(0, OriginVel)  * glm::conjugate(camera.Orientation);
	body->setVelocity(q.x, q.y, q.z);

	body->setRotation(0, 0, 0);
	body->clearAccumulators();

	// Clear the force accumulators
	body->calculateDerivedData();
	calculateInternals();
}

