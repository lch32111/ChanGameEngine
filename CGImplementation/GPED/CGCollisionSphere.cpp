#include <GPED/CGCollisionSphere.h>

CGProj::CGCollisionSphere::CGCollisionSphere()
{
	CGCollisionPrimitive::m_primitiveType = COLLISION_PRIMITIVE_SPHERE;
	radius = GPED::real(0.0);
}

void CGProj::CGCollisionSphere::getAABB(GPED::c3AABB& outAABB) const
{
	glm::vec3 pos = getAxis(3);
	outAABB.min = pos - glm::vec3(radius);
	outAABB.max = pos + glm::vec3(radius);
}