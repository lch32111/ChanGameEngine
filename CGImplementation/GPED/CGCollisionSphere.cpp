#include <GPED/CGCollisionSphere.h>

CGProj::CGCollisionSphere::CGCollisionSphere()
{
	CGCollisionPrimitive::m_primitiveType = COLLISION_PRIMITIVE_SPHERE;
	radius = GPED::real(0.0);
}

GPED::c3AABB CGProj::CGCollisionSphere::makeAABB() const
{
	GPED::c3AABB aabb;

	// Just ignore the orientation since it's sphere.
	glm::vec3 pos = getAxis(3);

	aabb.min = pos - glm::vec3(radius);
	aabb.max = pos + glm::vec3(radius);

	return aabb;
}