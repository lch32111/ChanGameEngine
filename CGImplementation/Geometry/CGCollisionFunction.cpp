#include <CGPrecompiled.h>
#include <Geometry/CGCollisionFunction.h>

#include <Geometry/CGSphere.h>
#include <Geometry/CGRay.h>
#include <Geometry/CGLineSegment.h>
#include <Geometry/CGPlane.h>
#include <Geometry/CGTriangle.h>
#include <Geometry/CGAABB.h>

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

// Real Time Collision Detection 178p
// From Section 5.3.2 Intersecting Ray or Sgment Agains Sphere
bool CG::Intersect(const CGSphere& sphere, const CGRay& ray)
{
	CGVec3 m = ray.GetSource() - sphere.m_pos;
	CGScalar b = Dot(m, ray.GetDirection());
	CGScalar c = Dot(m, m) - sphere.m_radius * sphere.m_radius;

	if (c > CGScalar(0.0) && b > CGScalar(0.0)) 
		return false;

	CGScalar discr = b * b - c;
	if (discr < CGScalar(0.0))
		return false;

	return true;
}

bool CG::Intersect(const CGSphere& sphere, const CGRay& ray, CGScalar& enter)
{
	CGVec3 m = ray.GetSource() - sphere.m_pos;
	CGScalar b = Dot(m, ray.GetDirection());
	CGScalar c = Dot(m, m) - sphere.m_radius * sphere.m_radius;

	if (c > CGScalar(0.0) && b > CGScalar(0.0))
		return false;

	CGScalar discr = b * b - c;
	if (discr < CGScalar(0.0))
		return false;

	enter = -b - CGScalarUtil::Sqrt(discr);
	if (enter < CGScalar(0.0))
		enter = CGScalar(0.0);

	return true;
}

bool CG::Intersect(const CGSphere& sphere, const CGRay& ray, CGScalar& enter, CGScalar& exit)
{
	CGVec3 m = ray.GetSource() - sphere.m_pos;
	CGScalar b = Dot(m, ray.GetDirection());
	CGScalar c = Dot(m, m) - sphere.m_radius * sphere.m_radius;

	if (c > CGScalar(0.0) && b > CGScalar(0.0))
		return false;

	CGScalar discr = b * b - c;
	if (discr < CGScalar(0.0))
		return false;
	
	discr = CGScalarUtil::Sqrt(discr);
	enter = -b - discr;
	if (enter < CGScalar(0.0))
		enter = CGScalar(0.0);

	exit = -b + discr;
	if (exit > ray.GetMaxFraction())
		exit = ray.GetMaxFraction();

	return true;
}

bool CG::Intersect(const CGSphere& sphere, const CGLineSegment& segment)
{
	CGVec3 d = segment.m_target - segment.m_source;
	CGScalar d_len = Length(d);
	d *= (CGScalar(1.0) / d_len);

	CGVec3 m = segment.m_source - sphere.m_pos;
	CGScalar b = Dot(m, d);
	CGScalar c = Dot(m, m) - sphere.m_radius * sphere.m_radius;

	if (c > CGScalar(0.0) && b > CGScalar(0.0))
		return false;

	CGScalar discr = b * b - c;
	if (discr < CGScalar(0.0))
		return false;

	CGScalar enter = -b - CGScalarUtil::Sqrt(discr);
	if (enter > d_len)	// it's beyond the segment
		return false;

	return true;
}

bool CG::Intersect(const CGSphere& sphere, const CGLineSegment& segment, CGScalar& enter)
{
	CGVec3 d = segment.m_target - segment.m_source;
	CGScalar d_len = Length(d);
	d *= (CGScalar(1.0) / d_len);

	CGVec3 m = segment.m_source - sphere.m_pos;
	CGScalar b = Dot(m, d);
	CGScalar c = Dot(m, m) - sphere.m_radius * sphere.m_radius;

	if (c > CGScalar(0.0) && b > CGScalar(0.0))
		return false;

	CGScalar discr = b * b - c;
	if (discr < CGScalar(0.0))
		return false;

	enter = -b - CGScalarUtil::Sqrt(discr);
	if (enter > d_len)	// it's beyond the segment
		return false;

	if (enter < CGScalar(0.0))
		enter = CGScalar(0.0);

	enter /= d_len;

	return true;
}

// Real Time Collision Detection p183 ~ 184
bool CG::Intersect(const CGAABB& aabb, const CGLineSegment& segment)
{
	CGVec3 e = aabb.m_max - aabb.m_min;
	CGVec3 d = segment.m_target - segment.m_source;
	CGVec3 m = segment.m_source + segment.m_target - aabb.m_min - aabb.m_max;

	// Try world coordinate axes as separating axes
	CGScalar adx = CGScalarUtil::Abs(d[0]);
	if (CGScalarUtil::Abs(m[0]) > e[0] + adx) return false;
	CGScalar ady = CGScalarUtil::Abs(d[1]);
	if (CGScalarUtil::Abs(m[1]) > e[1] + ady) return false;
	CGScalar adz = CGScalarUtil::Abs(d[2]);
	if (CGScalarUtil::Abs(m[2]) > e[2] + adz) return false;

	// Add in an epsilon term to counteract arithmetic errors when segment is
	// (near) parallel to a coordinate axis (see text for detail)
	adx += CGScalarUtil::Epsilon(); ady += CGScalarUtil::Epsilon(); adz += CGScalarUtil::Epsilon();

	// Try cross products of segment direction vector with coordinate axes
	if (CGScalarUtil::Abs(m[1] * d[2] - m[2] * d[1]) > e[1] * adz + e[2] * ady) return false;
	if (CGScalarUtil::Abs(m[2] * d[0] - m[0] * d[2]) > e[0] * adz + e[2] * adx) return false;
	if (CGScalarUtil::Abs(m[0] * d[1] - m[1] * d[0]) > e[0] * ady + e[1] * adx) return false;

	return true;
}

// Real Time Collision Detection p180 ~ 181
bool CG::Intersect(const CGAABB& aabb, const CGRay& ray)
{
	CGScalar t_min = CGScalar(0.0);
	CGScalar t_max = CGScalarUtil::Max();

	const CGVec3 ray_dir = ray.GetDirection();
	const CGVec3 ray_origin = ray.GetSource();
	for (int i = 0; i < 3; ++i)
	{
		if (CGScalarUtil::Abs(ray_dir[i]) < CGScalarUtil::Epsilon())
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if (ray_origin[i] < aabb.m_min[i] || ray_origin[i] > aabb.m_max[i])
				return false;
		}
		else
		{
			// Compute Intersection t value of ray with near and far plane of slab
			CGScalar ood = CGScalar(1.0) / ray_dir[i];
			CGScalar t1 = (aabb.m_min[i] - ray_origin[i]) * ood;
			CGScalar t2 = (aabb.m_max[i] - ray_origin[i]) * ood;

			if (t1 > t2) Swap(t1, t2);
			if (t1 > t_min) t_min = t1;
			if (t2 > t_max) t_max = t2;
			if (t_min > t_max)
				return false;
		}
	}

	return true;
}

bool CG::Intersect(const CGPlane& p, const CGRay& ray)
{
	CGVec3 rayDir = ray.GetDirection();
	CGVec3 maxTarget = ray.GetSource() + rayDir * ray.GetMaxFraction();

	CGScalar t = (p.m_distance - Dot(p.m_normal, ray.GetSource())) / Dot(p.m_normal, maxTarget);

	if (t >= CGScalar(0.0) && t <= CGScalar(1.0))
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

bool CG::Intersect(const CGPlane& p, const CGLineSegment& segment, CGScalar& t)
{
	CGScalar k = (p.m_distance - Dot(p.m_normal, segment.m_source)) / Dot(p.m_normal, (segment.m_target - segment.m_source));

	// If t in [0..1] compute and return intersection point
	if (k >= CGScalar(0.f) && k <= CGScalar(1.f))
	{
		// A + t * (B - A) is the intersection point
		t = k;
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

bool CG::Intersect(const CGTriangle& tri, const CGLineSegment& segment)
{
	CGVec3 neg_ray_dir = (segment.m_source - segment.m_target);
	CGVec3 ab = tri[1] - tri[0];
	CGVec3 ac = tri[2] - tri[0];

	CGVec3 n = Cross(ab, ac);
	CGScalar d = Dot(neg_ray_dir, n);
	
	CGVec3 a_to_ray_source = segment.m_source - tri[0];

	CGScalar t = Dot(a_to_ray_source, n);

	if (t < CGScalar(0.0) || t > d)
		return false;

	CGVec3 e = Cross(neg_ray_dir, a_to_ray_source);
	CGScalar v = Dot(ac, e);
	if (v < CGScalar(0.0) || v > d) return false;

	CGScalar w = -Dot(ab, e);
	if (w < CGScalar(0.0) || v + w > d) return false;

	return true;
}

bool CG::Intersect(const CGTriangle& tri, const CGLineSegment& segment, CGScalar& t)
{
	CGVec3 neg_ray_dir = (segment.m_source - segment.m_target);
	CGVec3 ab = tri[1] - tri[0];
	CGVec3 ac = tri[2] - tri[0];

	CGVec3 n = Cross(ab, ac);
	CGScalar d = Dot(neg_ray_dir, n);

	CGVec3 a_to_ray_source = segment.m_source - tri[0];

	CGScalar k = Dot(a_to_ray_source, n);

	if (k < CGScalar(0.0) || k > d)
		return false;

	CGVec3 e = Cross(neg_ray_dir, a_to_ray_source);
	CGScalar v = Dot(ac, e);
	if (v < CGScalar(0.0) || v > d) return false;

	CGScalar w = -Dot(ab, e);
	if (w < CGScalar(0.0) || v + w > d) return false;

	CGScalar ood = CGScalar(1.0) / d;

	t = k * ood;

	return true;
}

bool CG::IntersectTruePlane(const CGPlane& p, const CGRay& ray, CGScalar& t)
{
	CGVec3 rayDir = ray.GetDirection();
	CGVec3 maxTarget = ray.GetSource() + rayDir * ray.GetMaxFraction();
	CGScalar s = (p.m_distance - Dot(p.m_normal, ray.GetSource())) / Dot(p.m_normal, maxTarget);

	if (s >= CGScalar(0.0) && s <= CGScalar(1.0))
	{
		t = s;
		return true;
	}

	return false;
}

/* Real Time Collision Detection Chapter 5.3.6 Intersecting Ray or Segment Against Triangle
   Assume the triangle 0, 1, 2 is counter clockwise
   T(v,w) = A + v(B - A) + w(C - A)
   R(t) = P + t(Q - P)
		= P + kZ. 0 <= k <= max_fraction -> 0 <= k / max_fraction == t <= 1

   A + v(B - A) + w(C - A) = P + kZ
   [ -Z   (B - A)  (C - A) ] [ k v w]^T = [ (P - A) ]

   n = (B - A) X (C - A)
   d = dot(-Z, n)
   e = -Z X (P - A)

   k = dot((P - A), n) / d
   v = dot((C - A), e) / d
   w = dot(-(B - A), e) / d
   u = 1.0 - v - w
*/
bool CG::IntersectTruePlane(const CGTriangle& tri, const CGRay& ray)
{
	CGVec3 neg_ray_dir = ray.GetDirection() * CGScalar(-1.0);

	CGVec3 ab = tri[1] - tri[0];
	CGVec3 ac = tri[2] - tri[0];

	CGVec3 n = Cross(ab, ac);
	CGScalar d = Dot(neg_ray_dir, n);
	// ray from false plane
	if (d <= 0)
		return false;

	CGVec3 a_to_ray_source = ray.GetSource() - tri[0];

	CGScalar k = Dot(a_to_ray_source, n);

	if (k < CGScalar(0.0) || k > ray.GetMaxFraction() * d)
		return false;

	CGVec3 e = Cross(neg_ray_dir, a_to_ray_source);
	CGScalar v = Dot(ac, e);
	if (v < CGScalar(0.0) || v > d) return false;

	CGScalar w = -Dot(ab, e);
	if (w < CGScalar(0.0) || v + w > d) return false;

	/*
	* Code for getting solutions
	CGScalar ood = CGSCalar(1.0) / d;
	k *= ood;
	v *= ood;
	w *= ood;
	CGScalar u = CGScalar(1.0) - v - w;
	*/
	return true;
}

bool CG::IntersectTruePlane(const CGTriangle& tri, const CGRay& ray, 
	CGScalar& u, CGScalar& v, CGScalar& w, CGScalar& t)
{
	CGVec3 neg_ray_dir = ray.GetDirection() * CGScalar(-1.0);

	CGVec3 ab = tri[1] - tri[0];
	CGVec3 ac = tri[2] - tri[0];

	CGVec3 n = Cross(ab, ac);
	CGScalar d = Dot(neg_ray_dir, n);
	// ray from false plane
	if (d <= 0)
		return false;

	CGVec3 a_to_ray_source = ray.GetSource() - tri[0];

	t = Dot(a_to_ray_source, n);

	if (t < CGScalar(0.0) || t > ray.GetMaxFraction() * d)
		return false;

	CGVec3 e = Cross(neg_ray_dir, a_to_ray_source);
	v = Dot(ac, e);
	if (v < CGScalar(0.0) || v > d) return false;

	w = -Dot(ab, e);
	if (w < CGScalar(0.0) || v + w > d) return false;

	CGScalar ood = CGScalar(1.0) / d;
	t *= ood;
	v *= ood;
	w *= ood;
	u = CGScalar(1.0) - v - w;

	return true;
}

bool CG::IntersectTruePlane(const CGTriangle& tri, const CGLineSegment& segment)
{
	CGVec3 neg_ray_dir = (segment.m_source - segment.m_target);
	CGVec3 ab = tri[1] - tri[0];
	CGVec3 ac = tri[2] - tri[0];

	CGVec3 n = Cross(ab, ac);
	CGScalar d = Dot(neg_ray_dir, n);
	// ray from false plane
	if (d <= 0)
		return false;

	CGVec3 a_to_ray_source = segment.m_source - tri[0];

	CGScalar t = Dot(a_to_ray_source, n);

	if (t < CGScalar(0.0) || t > d)
		return false;

	CGVec3 e = Cross(neg_ray_dir, a_to_ray_source);
	CGScalar v = Dot(ac, e);
	if (v < CGScalar(0.0) || v > d) return false;

	CGScalar w = -Dot(ab, e);
	if (w < CGScalar(0.0) || v + w > d) return false;

	return true;
}