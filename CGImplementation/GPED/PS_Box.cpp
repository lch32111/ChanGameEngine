#include "PS_Box.h"

void Box::setState(GPED::real x, GPED::real y, GPED::real z)
{
	body->setPosition(x, y, z);
	body->setOrientation(1, 0, 0, 0);
	body->setVelocity(0, 0, 0);
	body->setRotation(0, 0, 0);
	halfSize = glm::vec3(1, 1, 1);

	GPED::real mass = halfSize.x * halfSize.y * halfSize.z * 15.0f;
	body->setMass(mass);

	glm::mat3 tensor = GPED::getBlockInertiaTensor(halfSize, mass);
	body->setInertiaTensor(tensor);

	body->setLinearDamping(0.95f);
	body->setAngularDamping(0.8f);
	body->clearAccumulators();
	body->setAcceleration(0, -10.0f, 0);

	body->setCanSleep(true);
	body->setAwake();

	body->calculateDerivedData();
	calculateInternals();
}