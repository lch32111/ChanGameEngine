#include <CGPrecompiled.h>
#include <Graphics/CGEditPrimitive.h>

// =================================================================
/*** CG EDIT BOX    ***/
CG::CGEditBox::CGEditBox()
	:
	m_BoxType(EDIT_PRIMITIVE_AABB),
	m_position(glm::vec3(0)),
	m_halfExtents(glm::vec3(1)),
	m_orientation(glm::quat(0, 0, 0, -1))
{
	updateAABB();
}

CG::CGEditBox::CGEditBox(
	const glm::vec3& pos, const glm::vec3& halfExtents, const glm::quat& m_orient)
	: m_position(pos), m_halfExtents(halfExtents), m_orientation(m_orient)
{
	// halfExtents should be positive
	assert(halfExtents.x > 0 || halfExtents.y > 0 || halfExtents.z > 0);
	m_BoxType = EditPrimitiveType::EDIT_PRIMITIVE_AABB;
	updateAABB();
}

void CG::CGEditBox::setPosition(const glm::vec3 & p)
{
	CGEditBox::m_position = p;
	updateAABB();
}

void CG::CGEditBox::setPosition(const GPED::real x, const GPED::real y, const GPED::real z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	updateAABB();
}

void CG::CGEditBox::setXposition(const GPED::real x)
{
	m_position.x = x;
	updateAABB();
}

void CG::CGEditBox::setYposition(const GPED::real y)
{
	m_position.y = y;
	updateAABB();
}

void CG::CGEditBox::setZposition(const GPED::real z)
{
	m_position.z = z;
	updateAABB();
}

glm::vec3 CG::CGEditBox::getPosition()
{
	return m_position;
}

void CG::CGEditBox::setHalfSize(const glm::vec3 & h)
{
	m_halfExtents = h;
	updateAABB();
}

void CG::CGEditBox::setHalfSize(const GPED::real x, GPED::real y, GPED::real z)
{
	m_halfExtents = glm::vec3(x, y, z);
	updateAABB();
}

void CG::CGEditBox::setXHalfSize(const GPED::real x)
{
	m_halfExtents.x = x;
	updateAABB();
}

void CG::CGEditBox::setYHalfSize(const GPED::real y)
{
	m_halfExtents.y = y;
	updateAABB();
}

void CG::CGEditBox::setZHalfSize(const GPED::real z)
{
	m_halfExtents.z = z;
	updateAABB();
}

glm::vec3 CG::CGEditBox::getHalfSize()
{
	return m_halfExtents;
}

GPED::c3AABB CG::CGEditBox::getFitAABB()
{
	return m_fitAABB;
}

void CG::CGEditBox::setPrimitiveType(EditPrimitiveType e)
{
	assert(e == EDIT_PRIMITIVE_AABB || e == EDIT_PRIMITIVE_OBB);
	m_BoxType = e;
	updateAABB();
}

CG::EditPrimitiveType CG::CGEditBox::getPrimitiveType()
{
	return m_BoxType;
}

void CG::CGEditBox::updateAABB()
{
	switch (m_BoxType)
	{
	case EDIT_PRIMITIVE_AABB:
		m_fitAABB = GPED::makeAABB(m_position, m_halfExtents);
		break;
	case EDIT_PRIMITIVE_OBB:
		m_fitAABB = GPED::makeAABB(glm::mat3_cast(m_orientation), m_position, m_halfExtents);
		break;
	default:
		assert(0); // fail to set the correct primitive type for box
		break;
	}
}
/*** CG EDIT BOX  ***/
// =================================================================

// =================================================================
/*** CG EDIT Sphere  ***/
CG::CGEditSpere::CGEditSpere()
	:
	m_position(glm::vec3(0)),
	m_radius(1)
{
	updateAABB();
}

CG::CGEditSpere::CGEditSpere(const glm::vec3 & pos, const GPED::real radius)
	: m_position(pos), m_radius(radius)
{
	assert(radius > 0);
	updateAABB();
}

void CG::CGEditSpere::setPosition(const glm::vec3 & p)
{
	CGEditSpere::m_position = p;
	updateAABB();
}

void CG::CGEditSpere::setPosition(const GPED::real x, const GPED::real y, const GPED::real z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	updateAABB();
}

void CG::CGEditSpere::setXposition(const GPED::real x)
{
	m_position.x = x;
	updateAABB();
}

void CG::CGEditSpere::setYposition(const GPED::real y)
{
	m_position.y = y;
	updateAABB();
}

void CG::CGEditSpere::setZposition(const GPED::real z)
{
	m_position.z = z;
	updateAABB();
}

glm::vec3 CG::CGEditSpere::getPosition()
{
	return m_position;
}

void CG::CGEditSpere::setRaidus(GPED::real r)
{
	CGEditSpere::m_radius = r;
	updateAABB();
}

GPED::real CG::CGEditSpere::getRadius()
{
	return m_radius;
}

GPED::c3AABB CG::CGEditSpere::getFitAABB()
{
	return m_fitAABB;
}

void CG::CGEditSpere::updateAABB()
{
	m_fitAABB = GPED::makeAABB(m_position, m_radius);
}
/*** CG EDIT Sphere  ***/
// =================================================================