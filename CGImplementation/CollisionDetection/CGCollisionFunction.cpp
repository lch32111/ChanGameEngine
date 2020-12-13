#include <CGPrecompiled.h>
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
	if (sqDist < CGScalarUtil::Epsilon())
	{
		ba = CGVec3(CGScalar(1.0), CGScalar(0.0), CGScalar(0.0));
		sqDist = CGScalar(1.0);

		c.penetration = 0;
		c.normal = ba;
	}
	// for normal case
	else
	{
		sqDist = CGScalarUtil::Sqrt(sqDist);
		c.penetration = radiSum - sqDist;
		c.normal = ba * (CGScalar(1.0) / sqDist);
	}
	
	c.position = a.m_pos - c.normal * a.m_radius; // The witness point of Sphere A

	return true;
}

// Collision Detection in Interactive 3D Environments by Gino van den Bergen
// From Section 3.1.2
bool CGProj::CollisionDetection::intersect(const CGCollisionSphere& sphere, const CGCollisionRay& ray)
{
	// Translate Ray into the sphere local space;
	CGCollisionRay tRay;
	tRay.m_source = ray.m_source - sphere.m_pos;
	tRay.m_target = ray.m_target - sphere.m_pos;

	// Then Calculate the early exit for ray-sphere intersection
	CGScalar sourceSqLength = Dot(tRay.m_source, tRay.m_source);
	CGVec3 r = tRay.m_target - tRay.m_source;	
	CGScalar rSqLength = Dot(r, r);

	CGScalar projectedLength = Dot(tRay.m_source, r);

	CGScalar EarlyExit = projectedLength * projectedLength -
		rSqLength * (sourceSqLength - sphere.m_radius * sphere.m_radius);

	if (EarlyExit >= 0)
		return true;

	// entry point is max(lambdaEnter, 0)
	CGScalar lambdaEnter = (-projectedLength - CGScalarUtil::Sqrt(EarlyExit)) / (rSqLength * rSqLength);
	
	// exit point is min(lambdaExit, 1)
	CGScalar lambdaExit = (-projectedLength + CGScalarUtil::Sqrt(EarlyExit)) / (rSqLength * rSqLength);

	if (lambdaEnter <= CGScalar(1.0) && lambdaExit >= CGScalar(0.0))
	{
		return true;
	}

	return false;
}

bool CGProj::CollisionDetection::intersect(const CGCollisionSphere& sphere, const CGCollisionLineSegment& segment)
{
	CGVec3 segmentDir = segment.GetDirection();

	return true;
}