#ifndef __PS_AMMO_ROUND_H__
#define __PS_AMMO_ROUND_H__

#include <GPED/GPED_collide_fine.h>
#include <Graphics/chanQuatCamera.h>

enum ShotType
{
	SHOT_UNUSED = 0,
	SHOT_PISTOL,
	SHOT_ARTILLERY,
	SHOT_FIREBALL,
	SHOT_LASER
};

class AmmoRound : public GPED::CollisionSphere
{
public:
	ShotType type;
	unsigned startTime;

	AmmoRound()
	{
		body = new GPED::RigidBody;
	}

	~AmmoRound()
	{
		delete body;
	}

	/** Sets the box to a specific location. */
	void setState(ShotType shotType);
	void setState(ShotType shotType, glm::vec3 Position, glm::vec3 Velocity);
	void setState(ShotType shotType, const chanQuatCamera& camera);
};

#endif