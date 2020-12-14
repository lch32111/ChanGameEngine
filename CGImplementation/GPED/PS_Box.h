#ifndef __PS_BOX_H__
#define __PS_BOX_H__

#include <GPED/CGCollisionOBB.h>

class Box : public CG::CGCollisionOBB
{
public:
	int proxyId;
	

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