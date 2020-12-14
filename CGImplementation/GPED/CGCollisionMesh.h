#pragma once
#ifndef __CG_COLLISION_MESH_H__
#define __CG_COLLISION_MESH_H__

#include <GPED/CGCollisionPrimitive.h>
#include <GPED/CGCollisionTriangle.h>

namespace CG
{
	class CGCollisionMesh : public CGCollisionPrimitive
	{
	public:
		CGCollisionMesh();

		glm::vec3 m_meshOrigin;
		unsigned m_meshWidth;
		unsigned m_meshDepth;
		unsigned m_meshSubWidth;
		unsigned m_meshSubDepth;
		const float* m_heightData;
		unsigned m_heightDataNumb;

		struct Mesh_QuantizedGridRange
		{
			int startX, EndX;
			int startZ, EndZ;
		};

		void getQuantizedGridPoints(const GPED::c3AABB& aabbFromDynamicObjects, Mesh_QuantizedGridRange& outRange) const;
		void getVertex(unsigned gridX, unsigned gridZ, glm::vec3& outVertex) const;

	private:
		int getQuantized(float n) const;
	};
}

#endif
