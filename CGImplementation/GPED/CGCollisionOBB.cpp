#include <CGPrecompiled.h>
#include <GPED/CGCollisionOBB.h>

CGProj::CGCollisionOBB::CGCollisionOBB()
{
	CGCollisionPrimitive::m_primitiveType = COLLISION_PRIMITIVE_OBB;
}

void CGProj::CGCollisionOBB::getAABB(GPED::c3AABB& outAABB) const
{
	glm::mat3 AbsRotate;
	for (unsigned i = 0; i < 3; ++i)
		for (unsigned j = 0; j < 3; ++j)
			AbsRotate[i][j] = real_abs(transform[j][i]);

	glm::vec3 extent;
	for (unsigned i = 0; i < 3; ++i)
		extent[i] = glm::dot(AbsRotate[i], halfSize);
	
	glm::vec3 origin = transform[3];

	outAABB.min = origin - extent;
	outAABB.max = origin + extent;
}