#include <CGPrecompiled.h>
#include <Geometry/CGCollisionFunction.h>

#include <Geometry/CGSphere.h>
#include <Geometry/CGRay.h>
#include <Geometry/CGLineSegment.h>
#include <Geometry/CGPlane.h>

using namespace CG;

bool CG::Intersect(const CGSphere& a, const CGSphere& b)
{
	CGVec3 ba = a.m_pos - b.m_pos;
	CGScalar radiSum = a.m_radius + b.m_radius;
	return Dot(ba, ba) <= (radiSum * radiSum);
}

bool CG::Intersect(const CGSphere& a, const CGSphere& b, CGContact& c)
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
bool CG::Intersect(const CGSphere& sphere, const CGRay& ray)
{
	// Translate Ray into the sphere local space;
	CGRay tRay;
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

bool CG::Intersect(const CGSphere& sphere, const CGLineSegment& segment)
{
	// TODO
	CGVec3 segmentDir = segment.GetDirection();

	return true;
}

bool CG::Intersect(const CGPlane& p, const CGRay& ray)
{
	CGVec3 rayDir = ray.GetDirection();
	CGVec3 maxTarget = ray.m_source + rayDir * ray.m_maxFraction;

	CGScalar t = (p.m_distance - Dot(p.m_normal, ray.m_source)) / Dot(p.m_normal, maxTarget);

	if (t >= CGScalar(0.f) && t <= CGScalar(1.f))
	{
		return true;
	}

	return false;
}

// P : Dot(p.m_normal, X) = p.m_distance
// Ray X = A + t * rayDir
// t = (p.m_distance - Dot(p.m_normal, A)) / Dot(p.m_normal, rayDir)
bool CG::IntersectTruePlane(const CGPlane& p, const CGRay& ray)
{
	CGVec3 rayDir = ray.GetDirection();
	CGScalar rate = Dot(p.m_normal, rayDir);

	if (rate >= CGScalar(0.f))
	{
		return false;
	}
	else
	{
		CGScalar t = (p.m_distance - Dot(p.m_normal, ray.m_source)) / rate;

		if (t >= CGScalar(0.f) && t <= ray.m_maxFraction)
		{
			return true;
		}

		return false;
	}
}

bool CG::IntersectTruePlane(CGVec3 planeA, CGVec3 planeB, CGVec3 planeC, const CGRay& ray)
{
	CGPlane p;
	p.m_normal = Cross(planeB - planeA, planeC - planeA);
	p.m_distance = Dot(p.m_normal, planeA);
	return Intersect(p, ray);
}

// Real Time Collision Detection Chapter 5.3.1  Intersecting Segment Agains Plane
bool CG::Intersect(const CGPlane& p, const CGLineSegment& segment)
{
	CGScalar t = (p.m_distance - Dot(p.m_normal, segment.m_source)) / Dot(p.m_normal, (segment.m_target - segment.m_source));

	// If t in [0..1] compute and return intersection point
	if (t >= CGScalar(0.f) && t <= CGScalar(1.f)) 
	{
		// A + t * (B - A) is the intersection point
		return true;
	}
	// Else t is +INF, -INF, NaN, or not in[0..1] so no intersection
	return false;
}

bool CG::Intersect(CGVec3 planeA, CGVec3 planeB, CGVec3 planeC, const CGLineSegment& segment)
{
	CGPlane p;
	p.m_normal = Cross(planeB - planeA, planeC - planeA);
	p.m_distance = Dot(p.m_normal, planeA);
	return Intersect(p, segment);
}