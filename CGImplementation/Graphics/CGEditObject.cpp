#include "CGEditObject.h"

void CGProj::CGEditObject::setObjectType(EditObjectType t)
{
	CGEditObject::m_ObjectType = t;
}

CGProj::EditObjectType CGProj::CGEditObject::getObjectType()
{
	return m_ObjectType;
}

void CGProj::CGEditObject::setPrimitiveType(EditPrimitiveType t)
{
	CGEditObject::m_PrimitiveType = t;
}

CGProj::EditPrimitiveType CGProj::CGEditObject::getPrimitiveType()
{
	return m_PrimitiveType;
}

void CGProj::CGEditObject::setProxyType(EditProxyType t)
{
	CGEditObject::m_proxyType = t;
}

CGProj::EditProxyType CGProj::CGEditObject::getProxyType()
{
	return m_proxyType;
}

void CGProj::CGEditObject::setLightType(EditLightType t)
{
	CGEditObject::m_LightType = t;
}

CGProj::EditLightType CGProj::CGEditObject::getLightType()
{
	return m_LightType;
}

CGProj::CGEditBox::CGEditBox(
	EditObjectType oT, EditPrimitiveType pT, EditProxyType proxyT,
	const glm::vec3& pos, const glm::vec3& halfExtents, const glm::quat& m_orient)
	: m_position(pos), m_halfExtents(halfExtents), m_orientation(m_orient)
{
	assert(oT == EDIT_PROXY_OBJECT);
	assert(pT == EDIT_PRIMITIVE_AABB || pT == EDIT_PRIMITIVE_OBB);

	m_ObjectType = oT;
	m_PrimitiveType = pT;
	m_proxyType = proxyT;

	updateAABB();
}

CGProj::CGEditBox::CGEditBox(
	EditObjectType oT, EditPrimitiveType pT, EditLightType lT,
	const glm::vec3& pos, const glm::vec3& halfExtents, const glm::quat& m_orient)
{
	assert(oT == EDIT_LIGHT_OBJECT);
	assert(pT == EDIT_PRIMITIVE_AABB || pT == EDIT_PRIMITIVE_OBB);

	m_ObjectType = oT;
	m_PrimitiveType = pT;
	m_LightType = lT;

	updateAABB();
}

void CGProj::CGEditBox::setPosition(const glm::vec3 & p)
{
	CGEditBox::m_position = p;
	updateAABB();
}

void CGProj::CGEditBox::setPosition(const GPED::real x, const GPED::real y, const GPED::real z)
{
	CGEditBox::m_position = glm::vec3(x, y, z);
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

void CGProj::CGEditBox::updateAABB()
{
	if (m_PrimitiveType == EDIT_PRIMITIVE_AABB)
		m_fitAABB = GPED::makeAABB(m_position, m_halfExtents);
	else if (m_PrimitiveType == EDIT_PRIMITIVE_OBB)
		m_fitAABB = GPED::makeAABB(glm::mat3_cast(m_orientation), m_position, m_halfExtents);
}

CGProj::CGEditSpere::CGEditSpere(
	EditObjectType oT, EditPrimitiveType pT, EditProxyType proxyT,
	const glm::vec3 & pos, const GPED::real radius)
	: m_position(pos), m_radius(radius)
{
	assert(oT == EDIT_PROXY_OBJECT);
	assert(pT == EDIT_PRIMITIVE_SPHERE);

	m_ObjectType = oT;
	m_PrimitiveType = pT;
	m_proxyType = proxyT;

	updateAABB();
}

CGProj::CGEditSpere::CGEditSpere(
	EditObjectType oT, EditPrimitiveType pT, EditLightType lT, 
	const glm::vec3 & pos, const GPED::real radius)
	: m_position(pos), m_radius(radius)
{
	assert(oT == EDIT_LIGHT_OBJECT);
	assert(pT == EDIT_PRIMITIVE_SPHERE);

	m_ObjectType = oT;
	m_PrimitiveType = pT;
	m_LightType = lT;

	updateAABB();
}

void CGProj::CGEditSpere::setPosition(const glm::vec3 & p)
{
	CGEditSpere::m_position = p;
	updateAABB();
}

void CGProj::CGEditSpere::setPosition(const GPED::real x, const GPED::real y, const GPED::real z)
{
	CGEditSpere::m_position = glm::vec3(x, y, z);
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
