#pragma once
#ifndef __CG_PLANE_H__
#define __CG_PLANE_H__

#include <Geometry/CGGeometryConfig.h>
#include <Geometry/CGConvex.h>

namespace CG
{
	class CGPlane : public CGConvex
	{
	public:
		CGPlane() : CGConvex()
		{}

		CGVec3 normal;
		float distance;
	};
}

#endif