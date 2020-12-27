#pragma once
#ifndef __CG_TRIANGLE_H__
#define __CG_TRIANGLE_H__

#include <Geometry/CGGeometryConfig.h>
#include <Geometry/CGConvex.h>

namespace CG
{
	class CGTriangle : public CGConvex
	{
	public:
		CGTriangle() : CGConvex()
		{}

		CGVec3 m_vertices[3];

		CGVec3 operator[] (s32 i) const
		{
			return m_vertices[i];
		}

		CGVec3& operator[](s32 i)
		{
			return m_vertices[i];
		}
	};
}

#endif
