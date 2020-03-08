#include <CollisionDetection/CGCollisionFunction.h>

using namespace CGProj;
using namespace CGProj::Math;
using namespace CGProj::CollisionDetection;

bool CGProj::CollisionDetection::intersect(const CGCollisionSphere& a, const CGCollisionSphere& b)
{
	CGVec3 ba = a.m_pos - b.m_pos;
	CGScalar radiSum = a.m_radius + b.m_radius;
	return Dot(ba, ba) <= (radiSum * radiSum);
}

bool CGProj::CollisionDetection::intersect(const CGCollisionSphere& a, const CGCollisionSphere& b, CGCollisionContact& c)
{
	CGVec3 ba = a.m_pos - b.m_pos;
	CGScalar radiSum = a.m_radius + b.m_radius;

	float sqDistance = (radiSum * radiSum) - Dot(ba, ba);
	if (sqDistance < CGScalar(0.0)) return false;
		
	c.penetration = CGScalarUtil::sqrt(sqDistance);
	c.normal = Math::Normalize(ba);
	c.position = b.m_pos + c.normal * c.penetration;
	return true;
}

// Collision Detection in Interactive 3D Environments by Gino van den Bergen
// From Section 3.1.2
// x = s + a * r
// norm(x) = radius
bool CGProj::CollisionDetection::intersect(const CGCollisionSphere& sphere, const CGCollisionRay& ray)
{
	CGVec3 r = ray.m_target - ray.m_source;
	CGScalar r_length2 = Dot(r, r);

	return true;
}

bool CGProj::CollisionDetection::intersect(const CGCollisionSphere& sphere, const CGCollisionLineSegment& segment)
{
	CGVec3 segmentDir = segment.GetDirection();

	return true;
}