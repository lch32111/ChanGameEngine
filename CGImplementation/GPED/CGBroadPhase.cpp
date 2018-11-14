#include "CGBroadPhase.h"

GPED::c3AABB CGProj::CGBroadPhase::convertFromCollisionPrimitive(const GPED::CollisionPrimitive& primitive)
{
	return primitive.makeAABB();
}
