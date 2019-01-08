#include <GPED/CGCollisionPrimitive.h>

CGProj::CGCollisionPrimitive::CGCollisionPrimitive()
	: m_primitiveType(COLLISION_PRIMITIVE_UNDECLARED),
	body(nullptr),
	offset(glm::mat4(1.0))
{
}

void CGProj::CGCollisionPrimitive::calculateInternals()
{
	body->getTransform(&transform);
}

const glm::mat4& CGProj::CGCollisionPrimitive::getTransform() const
{
	return transform;
}

void CGProj::CGCollisionPrimitive::getAABB(GPED::c3AABB& OutAABB) const
{
}

glm::vec3 CGProj::CGCollisionPrimitive::getAxis(unsigned index) const
{
	return transform[index];
}