#include <GPED/CGCollisionOBB.h>

CGProj::CGCollisionOBB::CGCollisionOBB()
{
	CGCollisionPrimitive::m_primitiveType = primitive_box;
}

GPED::c3AABB CGProj::CGCollisionOBB::makeAABB() const
{
	GPED::c3AABB aabb;
	aabb.min = glm::vec3(REAL_MAX);
	aabb.max = glm::vec3(-REAL_MAX);

	glm::vec3 v[8] =
	{
		glm::vec3(-halfSize.x,-halfSize.y, -halfSize.z),
		glm::vec3(-halfSize.x,-halfSize.y, halfSize.z),
		glm::vec3(halfSize.x, -halfSize.y, halfSize.z),
		glm::vec3(halfSize.x,-halfSize.y, -halfSize.z),
		glm::vec3(-halfSize.x, halfSize.y, -halfSize.z),
		glm::vec3(-halfSize.x, halfSize.y, halfSize.z),
		glm::vec3(halfSize.x, halfSize.y, halfSize.z),
		glm::vec3(halfSize.x, halfSize.y, -halfSize.z)
	};

	for (int i = 0; i < 8; ++i)
	{
		v[i] = transform * glm::vec4(v[i], 1.0);

		aabb.min = GPED::rMin(aabb.min, v[i]);
		aabb.max = GPED::rMax(aabb.max, v[i]);
	}

	return aabb;
}