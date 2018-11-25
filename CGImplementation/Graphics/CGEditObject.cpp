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
	EditObjectType oT, EditPrimitiveType pT, EditProxyType proxyT)
{
	assert(oT == EDIT_PROXY_OBJECT);
	assert(pT == EDIT_PRIMITIVE_AABB || pT == EDIT_PRIMITIVE_OBB);

	m_ObjectType = oT;
	m_PrimitiveType = pT;
	m_proxyType = proxyT;
}

CGProj::CGEditBox::CGEditBox(EditObjectType oT, EditPrimitiveType pT, EditLightType lT)
{
	assert(oT == EDIT_LIGHT_OBJECT);
	assert(pT == EDIT_PRIMITIVE_AABB || pT == EDIT_PRIMITIVE_OBB);

	m_ObjectType = oT;
	m_PrimitiveType = pT;
	m_LightType = lT;
}

void CGProj::CGEditBox::setPosition(const glm::vec3 & p)
{
	CGEditBox::m_position = p;
}

void CGProj::CGEditBox::setPosition(const GPED::real x, const GPED::real y, const GPED::real z)
{
	CGEditBox::m_position = glm::vec3(x, y, z);
}

void CGProj::CGEditBox::setXposition(const GPED::real x)
{
	m_position.x = x;
}

void CGProj::CGEditBox::setYposition(const GPED::real y)
{
	m_position.y = y;
}

void CGProj::CGEditBox::setZposition(const GPED::real z)
{
	m_position.z = z;
}

glm::vec3 CGProj::CGEditBox::getPosition()
{
	return m_position;
}

void CGProj::CGEditBox::setHalfSize(const glm::vec3 & h)
{
	m_halfExtents = h;
}

void CGProj::CGEditBox::setHalfSize(const GPED::real x, GPED::real y, GPED::real z)
{
	m_halfExtents = glm::vec3(x, y, z);
}

void CGProj::CGEditBox::setXHalfSize(const GPED::real x)
{
	m_halfExtents.x = x;
}

void CGProj::CGEditBox::setYHalfSize(const GPED::real y)
{
	m_halfExtents.y = y;
}

void CGProj::CGEditBox::setZHalfSize(const GPED::real z)
{
	m_halfExtents.z = z;
}

glm::vec3 CGProj::CGEditBox::getHalfSize()
{
	return m_halfExtents;
}
