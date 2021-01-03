#pragma once
#ifndef __CG_AABB_H__
#define __CG_AABB_H__

#include <Geometry/CGGeometryConfig.h>
#include <Geometry/CGConvex.h>

namespace CG
{
	class CGAABB : public CGConvex
	{
	public:
		CGAABB() { }

		CGVec3 m_min;
		CGVec3 m_max;
	};
}


#endif
