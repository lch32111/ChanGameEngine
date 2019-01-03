#include <GPED/CGCollisionMesh.h>

CGProj::CGCollisionMesh::CGCollisionMesh()
{
	CGCollisionPrimitive::m_primitiveType = COLLISION_PRIMITIVE_MESH;
}

void CGProj::CGCollisionMesh::getQuantizedTriangles(const GPED::c3AABB & aabbFromDynamicObjects) const
{
}
