#include <CGPrecompiled.h>
#include <GPED/CGCollisionPrimitive.h>

CG::CGCollisionPrimitive::CGCollisionPrimitive()
	: m_primitiveType(COLLISION_PRIMITIVE_UNDECLARED),
	body(nullptr),
	offset(glm::mat4(1.0))
{
}

void CG::CGCollisionPrimitive::calculateInternals()
{
	body->getTransform(&transform);
}

const glm::mat4& CG::CGCollisionPrimitive::getTransform() const
{
	return transform;
}

void CG::CGCollisionPrimitive::getAABB(GPED::c3AABB& OutAABB) const
{
}

glm::vec3 CG::CGCollisionPrimitive::getAxis(unsigned index) const
{
	return transform[index];
}