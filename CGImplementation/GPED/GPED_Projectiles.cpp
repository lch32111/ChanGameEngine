#include "GPED_Projectiles.h"
#include <GLFW/glfw3.h>

using namespace GPED;

GPEDProjectile::GPEDProjectile()
{
	for (unsigned int i = 0; i < MAX_PROJECTILE_NUM; ++i)
		ammo[i].type = UNUSED;

	currentShotType = PROJECTILE_BULLET;
}

void GPEDProjectile::fire()
{
	if (ammo_cursor >= MAX_PROJECTILE_NUM)
		ammo_cursor = 0;


	AmmoRound& proj = ammo[ammo_cursor];
	switch (currentShotType)
	{
	case PROJECTILE_BULLET:
		proj.particle.setMass(2.f); // 2.0kg
		proj.particle.setVelocity(glm::vec3(0.f, 0.f, 35.f)); // 35m/s
		proj.particle.setAcceleration(glm::vec3(0.f, -1.f, 0.0f));
		proj.particle.setDamping(0.99f);
		break;
	case PROJECTILE_ARTILLERY:
		proj.particle.setMass(200.0f); // 200.0kg
		proj.particle.setVelocity(0.0f, 30.0f, 40.0f); // 50m/s
		proj.particle.setAcceleration(0.0f, -20.0f, 0.0f);
		proj.particle.setDamping(0.99f);
		break;
	case PROJECTILE_FIREBALL:
		proj.particle.setMass(1.0f); // 1.0kg - mostly blast damage
		proj.particle.setVelocity(0.0f, 0.0f, 10.0f); // 5m/s
		proj.particle.setAcceleration(0.0f, 0.6f, 0.0f); // Floats up
		proj.particle.setDamping(0.9f);
		break;
	case PROJECTILE_LASER:
		// Note that this is the kind of laser bolt seen in films,
		// not a realistic laser beam!
		proj.particle.setMass(0.1f); // 0.1kg - almost no weight
		proj.particle.setVelocity(0.0f, 0.0f, 100.0f); // 100m/s
		proj.particle.setAcceleration(0.0f, 0.0f, 0.0f); // No gravity
		proj.particle.setDamping(0.99f);
		break;
	}

	proj.particle.setPosition(0.f, 1.5f, 0.f);
	proj.type = currentShotType;
	proj.startTime = glfwGetTime();

	// Clear the force accumulators
	proj.particle.clearAccumulator();

	++ammo_cursor;
}

void GPEDProjectile::update(float duration)
{
	for (unsigned int i = 0; i < MAX_PROJECTILE_NUM; ++i)
	{
		if (ammo[i].type == UNUSED) continue;

		ammo[i].particle.integrate(duration);

		// manual check for world collision 
		glm::vec3 position = ammo[i].particle.getPosition();
		position = glm::min(glm::max(position, glm::vec3(-50.f, 0.f, -50.f)), glm::vec3(50.f, 50.f, 50.f));
		ammo[i].particle.setPosition(position);

		if (ammo[i].startTime + 5.f < glfwGetTime())
			ammo[i].type = UNUSED;
	}
}
