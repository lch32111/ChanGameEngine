#ifndef __CG_COLLISION_FUNCTION_H__
#define __CG_COLLISION_FUNCTION_H__

#include <Geometry/CGGeometryConfig.h>

namespace CG
{
	class CGSphere;
	class CGRay;
	class CGLineSegment;
	class CGPlane;
	class CGTriangle;
	class CGAABB;
	struct CGContact;

	bool Intersect(const CGSphere& a, const CGSphere& b);
	bool Intersect(const CGSphere& a, const CGSphere& b, CGContact& c);
	bool Intersect(const CGSphere& sphere, const CGRay& ray);
	bool Intersect(const CGSphere& sphere, const CGRay& ray, CGScalar& enter);
	bool Intersect(const CGSphere& sphere, const CGRay& ray, CGScalar& enter, CGScalar& exit);
	bool Intersect(const CGSphere& sphere, const CGLineSegment& segment);
	bool Intersect(const CGSphere& sphere, const CGLineSegment& segment, CGScalar& enter);
	
	bool Intersect(const CGAABB& aabb, const CGLineSegment& segment);
	bool Intersect(const CGAABB& aabb, const CGRay& ray);


	// Intersect plane with ray for both of the true/false plane
	bool Intersect(const CGPlane& p, const CGRay& ray);
	bool Intersect(const CGPlane& p, const CGLineSegment& segment);
	bool Intersect(const CGPlane& p, const CGLineSegment& segment, CGScalar& t);
	bool Intersect(CGVec3 planeA, CGVec3 planeB, CGVec3 planeC, const CGLineSegment& segment); // Intersect LineSegment against plane of triangle ABC.

	bool Intersect(const CGTriangle& tri, const CGLineSegment& segment);
	bool Intersect(const CGTriangle& tri, const CGLineSegment& segment, CGScalar& t);
	
	bool IntersectTruePlane(const CGPlane& p, const CGRay& ray);
	bool IntersectTruePlane(CGVec3 planeA, CGVec3 planeB, CGVec3 planeC, const CGRay& ray);
	bool IntersectTruePlane(const CGPlane& p, const CGRay& ray, CGScalar& t);

	bool IntersectTruePlane(const CGTriangle& t, const CGRay& ray);
	bool IntersectTruePlane(const CGTriangle& tri, const CGRay& ray, CGScalar& u, CGScalar& v, CGScalar& w, CGScalar& t);
	bool IntersectTruePlane(const CGTriangle& tri, const CGLineSegment& segment);
}

#endif
