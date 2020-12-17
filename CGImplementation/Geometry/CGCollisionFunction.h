#ifndef __CG_COLLISION_FUNCTION_H__
#define __CG_COLLISION_FUNCTION_H__

#include <Geometry/CGGeometryConfig.h>

namespace CG
{
	class CGSphere;
	class CGRay;
	class CGLineSegment;
	class CGPlane;
	struct CGContact;

	bool Intersect(const CGSphere& a, const CGSphere& b);
	bool Intersect(const CGSphere& a, const CGSphere& b, CGContact& c);
	bool Intersect(const CGSphere& sphere, const CGRay& ray);
	bool Intersect(const CGSphere& sphere, const CGLineSegment& segment);

	// Intersect plane with ray for both of the true/false plane
	bool Intersect(const CGPlane& p, const CGRay& ray);

	// only for the true plane
	bool IntersectTruePlane(const CGPlane& p, const CGRay& ray);
	bool IntersectTruePlane(CGVec3 planeA, CGVec3 planeB, CGVec3 planeC, const CGRay& ray);

	bool Intersect(const CGPlane& p, const CGLineSegment& segment);

	// Intersect LineSegment against plane of triangle ABC.
	bool Intersect(CGVec3 planeA, CGVec3 planeB, CGVec3 planeC, const CGLineSegment& segment);
}

#endif
