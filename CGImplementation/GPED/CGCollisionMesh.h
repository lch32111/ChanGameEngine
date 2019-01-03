#pragma once
#ifndef __CG_COLLISION_MESH_H__
#define __CG_COLLISION_MESH_H__

#include <GPED/CGCollisionPrimitive.h>

namespace CGProj
{
	class CGCollisionMesh : public CGCollisionPrimitive
	{
	public:
		CGCollisionMesh();

		unsigned m_meshWidth;
		unsigned m_meshDepth;
		const float* m_heightData;
		unsigned m_heightDataNumb;

		void getQuantizedTriangles(const GPED::c3AABB& aabbFromDynamicObjects) const;
	};
}

#endif
