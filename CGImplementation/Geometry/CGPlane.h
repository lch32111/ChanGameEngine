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

		// Equation :  dot(m_normal, point) = m_distance
		// It's assumed that the m_normal is a unit vector
		CGVec3 m_normal;
		float m_distance;
	};
}

#endif