#include "CGEditObject.h"

// =================================================================
/*** CG EDIT BOX    ***/
CGProj::CGEditBox::CGEditBox()
	: 
	m_BoxType(EDIT_PRIMITIVE_AABB),
	m_position(glm::vec3(0)),
	m_halfExtents(glm::vec3(1)),
	m_orientation(glm::quat(0, 0, 0, -1))
{
	updateAABB();
}

CGProj::CGEditBox::CGEditBox(
	const glm::vec3& pos, const glm::vec3& halfExtents, const glm::quat& m_orient)
	: m_position(pos), m_halfExtents(halfExtents), m_orientation(m_orient)
{
	// halfExtents should be positive
	assert(halfExtents.x > 0 || halfExtents.y > 0 || halfExtents.z > 0);
	m_BoxType = EditPrimitiveType::EDIT_PRIMITIVE_AABB;
	updateAABB();
}

void CGProj::CGEditBox::setPosition(const glm::vec3 & p)
{
	CGEditBox::m_position = p;
	updateAABB();
}

void CGProj::CGEditBox::setPosition(const GPED::real x, const GPED::real y, const GPED::real z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	updateAABB();
}

void CGProj::CGEditBox::setXposition(const GPED::real x)
{
	m_position.x = x;
	updateAABB();
}

void CGProj::CGEditBox::setYposition(const GPED::real y)
{
	m_position.y = y;
	updateAABB();
}

void CGProj::CGEditBox::setZposition(const GPED::real z)
{
	m_position.z = z;
	updateAABB();
}

glm::vec3 CGProj::CGEditBox::getPosition()
{
	return m_position;
}

void CGProj::CGEditBox::setHalfSize(const glm::vec3 & h)
{
	m_halfExtents = h;
	updateAABB();
}

void CGProj::CGEditBox::setHalfSize(const GPED::real x, GPED::real y, GPED::real z)
{
	m_halfExtents = glm::vec3(x, y, z);
	updateAABB();
}

void CGProj::CGEditBox::setXHalfSize(const GPED::real x)
{
	m_halfExtents.x = x;
	updateAABB();
}

void CGProj::CGEditBox::setYHalfSize(const GPED::real y)
{
	m_halfExtents.y = y;
	updateAABB();
}

void CGProj::CGEditBox::setZHalfSize(const GPED::real z)
{
	m_halfExtents.z = z;
	updateAABB();
}

glm::vec3 CGProj::CGEditBox::getHalfSize()
{
	return m_halfExtents;
}

GPED::c3AABB CGProj::CGEditBox::getFitAABB()
{
	return m_fitAABB;
}

void CGProj::CGEditBox::setPrimitiveType(EditPrimitiveType e)
{
	assert(e == EDIT_PRIMITIVE_AABB || e == EDIT_PRIMITIVE_OBB);
	m_BoxType = e;
	updateAABB();
}

CGProj::EditPrimitiveType CGProj::CGEditBox::getPrimitiveType()
{
	return m_BoxType;
}

void CGProj::CGEditBox::updateAABB()
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
CGProj::CGEditSpere::CGEditSpere()
	:
	m_position(glm::vec3(0)),
	m_radius(1)
{
	updateAABB();
}

CGProj::CGEditSpere::CGEditSpere(const glm::vec3 & pos, const GPED::real radius)
	: m_position(pos), m_radius(radius)
{
	assert(radius > 0);
	updateAABB();
}

void CGProj::CGEditSpere::setPosition(const glm::vec3 & p)
{
	CGEditSpere::m_position = p;
	updateAABB();
}

void CGProj::CGEditSpere::setPosition(const GPED::real x, const GPED::real y, const GPED::real z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	updateAABB();
}

void CGProj::CGEditSpere::setXposition(const GPED::real x)
{
	m_position.x = x;
	updateAABB();
}

void CGProj::CGEditSpere::setYposition(const GPED::real y)
{
	m_position.y = y;
	updateAABB();
}

void CGProj::CGEditSpere::setZposition(const GPED::real z)
{
	m_position.z = z;
	updateAABB();
}

glm::vec3 CGProj::CGEditSpere::getPosition()
{
	return m_position;
}

void CGProj::CGEditSpere::setRaidus(GPED::real r)
{
	CGEditSpere::m_radius = r;
	updateAABB();
}

GPED::real CGProj::CGEditSpere::getRadius()
{
	return m_radius;
}

GPED::c3AABB CGProj::CGEditSpere::getFitAABB()
{
	return m_fitAABB;
}

void CGProj::CGEditSpere::updateAABB()
{
	m_fitAABB = GPED::makeAABB(m_position, m_radius);
}
/*** CG EDIT Sphere  ***/
// =================================================================

// =================================================================
/*** CG EDIT Proxy Object  ***/

CGProj::CGEditProxyObject::CGEditProxyObject()
{
	CGEditProxyObject::setProxyType(EDIT_PROXY_DYNAMIC);
	CGEditProxyObject::setEditShape(EDIT_PRIMITIVE_AABB);
}

void CGProj::CGEditProxyObject::connectBroadPhase(CGBroadPhase * broad)
{
	assert(broad != nullptr);
	m_BroadPhase = broad;
}

void CGProj::CGEditProxyObject::setBroadPhaseId(int id)
{
	m_BroadPhaseId = id;
}

int CGProj::CGEditProxyObject::getBroadPhaseId()
{
	return m_BroadPhaseId;
}

void CGProj::CGEditProxyObject::updateBroadPhaseProxy()
{
	if (m_BroadPhaseId != Node_Null)
		m_BroadPhase->UpdateProxy(m_BroadPhaseId, CGEditProxyObject::getFitAABB());
}

void CGProj::CGEditProxyObject::setEditShape(EditPrimitiveType e)
{
	CGEditProxyObject::m_PrimitiveType = e;

	if (e <= EDIT_PRIMITIVE_OBB) // e may be AABBor OBB
	{
		// You need to set specific primitive type for the bounding volume.
		CGEditBox::setPrimitiveType(e);
	}

	updateBroadPhaseProxy();
}

CGProj::EditPrimitiveType CGProj::CGEditProxyObject::getEditShape()
{
	return m_PrimitiveType;
}

void CGProj::CGEditProxyObject::setProxyType(EditProxyType e)
{
	CGEditProxyObject::m_ProxyType = e;
}

CGProj::EditProxyType CGProj::CGEditProxyObject::getProxyType()
{
	return m_ProxyType;
}

void CGProj::CGEditProxyObject::setPosition(const glm::vec3 & p)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB: 
	case EDIT_PRIMITIVE_OBB:
		CGEditBox::setPosition(p);
		break;
	case EDIT_PRIMITIVE_SPHERE:
		CGEditSpere::setPosition(p);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditProxyObject::setPosition(const GPED::real x, const GPED::real y, const GPED::real z)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		CGEditBox::setPosition(x, y, z);
		break;
	case EDIT_PRIMITIVE_SPHERE:
		CGEditSpere::setPosition(x, y, z);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditProxyObject::setXposition(const GPED::real x)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		CGEditBox::setXposition(x);
		break;
	case EDIT_PRIMITIVE_SPHERE:
		CGEditSpere::setXposition(x);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditProxyObject::setYposition(const GPED::real y)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		CGEditBox::setYposition(y);
		break;
	case EDIT_PRIMITIVE_SPHERE:
		CGEditSpere::setYposition(y);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditProxyObject::setZposition(const GPED::real z)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		CGEditBox::setZposition(z);
		break;
	case EDIT_PRIMITIVE_SPHERE:
		CGEditSpere::setZposition(z);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

glm::vec3 CGProj::CGEditProxyObject::getPosition()
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		return CGEditBox::getPosition();
	case EDIT_PRIMITIVE_SPHERE:
		return CGEditSpere::getPosition();
	default:
		assert(0);
		break;
	}
}

GPED::c3AABB CGProj::CGEditProxyObject::getFitAABB()
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		return CGEditBox::getFitAABB();
	case EDIT_PRIMITIVE_SPHERE:
		return CGEditSpere::getFitAABB();
	default:
		assert(0);
		break;
	}
}

void CGProj::CGEditProxyObject::setHalfSize(const glm::vec3 & h)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		CGEditBox::setHalfSize(h);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditProxyObject::setHalfSize(const GPED::real x, GPED::real y, GPED::real z)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		CGEditBox::setHalfSize(x, y, z);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditProxyObject::setXHalfSize(const GPED::real x)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		CGEditBox::setXHalfSize(x);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditProxyObject::setYHalfSize(const GPED::real y)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		CGEditBox::setYHalfSize(y);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditProxyObject::setZHalfSize(const GPED::real z)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		CGEditBox::setZHalfSize(z);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

glm::vec3 CGProj::CGEditProxyObject::getHalfSize()
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		return CGEditBox::getHalfSize();
	default:
		assert(0);
		break;
	}
}

void CGProj::CGEditProxyObject::setRaidus(GPED::real r)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_SPHERE:
		CGEditSpere::setRaidus(r);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

GPED::real CGProj::CGEditProxyObject::getRadius()
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_SPHERE:
		return CGEditSpere::getRadius();
	default:
		assert(0);
		break;
	}
}
/*** CG EDIT Proxy Object  ***/
// =================================================================
