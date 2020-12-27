#include <CGPrecompiled.h>
#include <Geometry/CGCollisionFunction.h>

#include <Geometry/CGSphere.h>
#include <Geometry/CGRay.h>
#include <Geometry/CGLineSegment.h>
#include <Geometry/CGPlane.h>
#include <Geometry/CGTriangle.h>

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
	CGRay tRay(ray.GetSource() - sphere.m_pos, ray.GetTarget() - sphere.m_pos, ray.GetMaxFraction());

	// Then Calculate the early exit for ray-sphere intersection
	CGScalar sourceSqLength = Dot(tRay.GetSource(), tRay.GetSource());
	CGVec3 r = tRay.GetTarget() - tRay.GetSource();	
	CGScalar rSqLength = Dot(r, r);

	CGScalar projectedLength = Dot(tRay.GetSource(), r);

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
	CGVec3 maxTarget = ray.GetSource() + rayDir * ray.GetMaxFraction();

	CGScalar t = (p.m_distance - Dot(p.m_normal, ray.GetSource())) / Dot(p.m_normal, maxTarget);

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
		CGScalar t = (p.m_distance - Dot(p.m_normal, ray.GetSource())) / rate;

		if (t >= CGScalar(0.f) && t <= ray.GetMaxFraction())
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

// Assume the triangle 0, 1, 2 is counter clockwise
bool CG::IntersectTruePlane(const CGTriangle& tri, const CGRay& ray)
{
	constexpr CGScalar triIntersectEps = CGScalar(1e-6);
	CGVec3 rayDir = ray.GetDirection();

	CGVec3 e1 = tri[1] - tri[0];
	CGVec3 e2 = tri[2] - tri[0];

	CGVec3 n = Normalize(Cross(e1, e2));

	CGVec3 q = Cross(rayDir, e2);
	CGScalar a = Dot(e1, q);

	// left condition : Ray direction from false plane
	// right condition : Ray is parallel for the plane
	if ((Dot(n, rayDir) >= 0) || (CGScalarUtil::Abs(a) <= triIntersectEps))
		return false;

	CGVec3 s = (ray.GetSource() - tri[0]) / a;
	CGVec3 r = Cross(s, e1);

	CGScalar b[3];
	b[0] = Dot(s, q);
	b[1] = Dot(r, rayDir);
	b[2] = CGScalar(1.0) - b[0] - b[1];

	if ((b[0] < CGScalar(0.0)) || (b[1] < CGScalar(0.0)) || (b[2] < CGScalar(0.0)))
		return false;

	CGScalar t = Dot(e2, r);
	return t >= CGScalar(0.0);
}