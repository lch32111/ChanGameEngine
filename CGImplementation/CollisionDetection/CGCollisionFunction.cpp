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

	CGScalar sqDist = Dot(ba, ba);
	CGScalar sqRadisum = radiSum * radiSum;
	if (sqDist > sqRadisum) return false;
	
	// for concentric spheres
	if (sqDist < CGScalarUtil::epsilon())
	{
		ba = CGVec3(CGScalar(1.0), CGScalar(0.0), CGScalar(0.0));
		sqDist = CGScalar(1.0);

		c.penetration = 0;
		c.normal = ba;
	}
	// for normal case
	else
	{
		sqDist = CGScalarUtil::sqrt(sqDist);
		c.penetration = radiSum - sqDist;
		c.normal = ba * (CGScalar(1.0) / sqDist);
	}
	
	c.position = a.m_pos - c.normal * a.m_radius; // The witness point of Sphere A

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