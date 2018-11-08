#ifndef __PS_BOX_H__
#define __PS_BOX_H__

#include <GPED/GPED_collide_fine.h>

class Box : public GPED::CollisionBox
{
public:
	Box()
	{
		body = new GPED::RigidBody;
	}

	~Box()
	{
		delete body;
	}

	/** Sets the box to a specific location. */
	void setState(GPED::real x, GPED::real y, GPED::real z);
};

#endif