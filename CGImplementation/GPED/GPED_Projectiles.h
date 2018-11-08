#ifndef __GPED_PROJECTILES_H__
#define __GPED_PROJECTILES_H__

#include <GPED/GPED_Particle.h>

namespace GPED
{
	enum SHOT_TYPE
	{
		UNUSED = 0,
		PROJECTILE_BULLET,
		PROJECTILE_ARTILLERY,
		PROJECTILE_FIREBALL,
		PROJECTILE_LASER
	};

	const static unsigned MAX_PROJECTILE_NUM = 50;
	class GPEDProjectile
	{
		struct AmmoRound
		{
			GPEDParticle particle;
			SHOT_TYPE type;
			unsigned startTime;
		};

	public:
		GPEDProjectile();
		
		AmmoRound ammo[MAX_PROJECTILE_NUM];
		unsigned int ammo_cursor = 0;
		SHOT_TYPE currentShotType;

		void fire();
		void update(float duration);

	};

}

#endif