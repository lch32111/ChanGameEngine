#ifndef __CG_COLLISION_FUNCTION_H__
#define __CG_COLLISION_FUNCTION_H__

#include <Geometry/CGSphere.h>
#include <Geometry/CGRay.h>
#include <Geometry/CGLineSegment.h>

namespace CG
{
	bool Intersect(const CGSphere& a, const CGSphere& b);
	bool Intersect(const CGSphere& a, const CGSphere& b, CGContact& c);
	bool Intersect(const CGSphere& sphere, const CGRay& ray);
	bool Intersect(const CGSphere& sphere, const CGLineSegment& segment);
}

#endif
