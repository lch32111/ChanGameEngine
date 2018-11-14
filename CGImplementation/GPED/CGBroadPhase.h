#pragma once
#ifndef __CG_BROAD_PHASE_H__
#define __CG_BROAD_PHASE_H__

#include <GPED/DynamicAABBTree.h>
#include <GPED/GPED_collide_fine.h>

namespace CGProj
{
	struct cgPair
	{
		int proxyIdA;
		int proxyIdB;
	};

	class CGBroadPhase
	{
	public:
		GPED::c3AABB convertFromCollisionPrimitive(const GPED::CollisionPrimitive& primitive);


	private:
	};
}

#endif