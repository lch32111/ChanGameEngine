#include <CollisionDetection/CGCollisionFunction.h>

using namespace CGProj;
using namespace CGProj::Math;
using namespace CGProj::CollisionDetection;

bool intersect(const CGCollisionSphere& a, const CGCollisionSphere& b)
{
	CGVec3 ba = a.m_pos - b.m_pos;
	CGScalar radiSum = a.m_radius + b.m_radius;
	return Dot(ba, ba) <= (radiSum * radiSum);
}

// Collision Detection in Interactive 3D Environments by Gino van den Bergen
// From Section 3.1.2
// x = s + a * r
// norm(x) = radius
bool intersect(const CGCollisionSphere& sphere, const CGCollisionRay& ray)
{
	CGVec3 r = ray.m_target - ray.m_source;
	CGScalar r_length2 = Dot(r, r);

	return true;
}

bool intersect(const CGCollisionSphere& sphere, const CGCollisionLineSegment& segment)
{
	CGVec3 segmentDir = segment.GetDirection();

	return true;
}