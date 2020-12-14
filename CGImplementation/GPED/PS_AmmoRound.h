#ifndef __PS_AMMO_ROUND_H__
#define __PS_AMMO_ROUND_H__

#include <GPED/CGCollisionSphere.h>
#include <Graphics/chanQuatCamera.h>

enum ShotType
{
	SHOT_UNUSED = 0,
	SHOT_PISTOL,
	SHOT_ARTILLERY,
	SHOT_FIREBALL,
	SHOT_LASER
};

class AmmoRound : public CG::CGCollisionSphere
{
public:
	int proxyId;

	ShotType m_shotType;
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
	void setState(ShotType shotType, const CG::chanQuatCamera& camera);
};

#endif