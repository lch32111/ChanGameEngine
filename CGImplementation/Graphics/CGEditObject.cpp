#include <Graphics/CGEditObject.h>
#include <Graphics/GLPrimitiveUtil.h>
#include <Graphics/CGAssetManager.h>

// =================================================================
/*** CG EDIT Object  ***/
CGProj::CGEditObject::CGEditObject()
{
	m_EditPrimitiveDraw = false;

	m_BroadPhaseId = Node_Null;

	m_BroadPhase = nullptr;
	m_DefShader = nullptr;

	setObjectType(EDIT_OBJECT_PROXY);
	setEditShape(EDIT_PRIMITIVE_AABB);
}

void CGProj::CGEditObject::connectBroadPhase(CGBroadPhase * broad)
{
	assert(broad != nullptr);
	m_BroadPhase = broad;
}

void CGProj::CGEditObject::setBroadPhaseId(int id)
{
	m_BroadPhaseId = id;
}

int CGProj::CGEditObject::getBroadPhaseId()
{
	return m_BroadPhaseId;
}

void CGProj::CGEditObject::setDefShader(Shader * shader)
{
	m_DefShader = shader;
}

void CGProj::CGEditObject::setEditShape(EditPrimitiveType e)
{
	m_PrimitiveType = e;

	if (e <= EDIT_PRIMITIVE_OBB) // e may be AABB or OBB
	{
		// You need to set specific primitive type for the bounding volume.
		m_EditBox.setPrimitiveType(e);
	}

	updateBroadPhaseProxy();
}

CGProj::EditPrimitiveType CGProj::CGEditObject::getEditShape()
{
	return m_PrimitiveType;
}

void CGProj::CGEditObject::setPosition(const glm::vec3 & p)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		m_EditBox.setPosition(p);
		break;
	case EDIT_PRIMITIVE_SPHERE:
		m_EditSphere.setPosition(p);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditObject::setPosition(const GPED::real x, const GPED::real y, const GPED::real z)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		m_EditBox.setPosition(x, y, z);
		break;
	case EDIT_PRIMITIVE_SPHERE:
		m_EditSphere.setPosition(x, y, z);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditObject::setXposition(const GPED::real x)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		m_EditBox.setXposition(x);
		break;
	case EDIT_PRIMITIVE_SPHERE:
		m_EditSphere.setXposition(x);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditObject::setYposition(const GPED::real y)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		m_EditBox.setYposition(y);
		break;
	case EDIT_PRIMITIVE_SPHERE:
		m_EditSphere.setYposition(y);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditObject::setZposition(const GPED::real z)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		m_EditBox.setZposition(z);
		break;
	case EDIT_PRIMITIVE_SPHERE:
		m_EditSphere.setZposition(z);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

glm::vec3 CGProj::CGEditObject::getPosition()
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		return m_EditBox.getPosition();
	case EDIT_PRIMITIVE_SPHERE:
		return m_EditSphere.getPosition();
	default:
		assert(0);
		break;
	}

	return glm::vec3();
}

void CGProj::CGEditObject::setScale(float scale)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		m_EditBox.setHalfSize(scale, scale, scale);
		break;
	case EDIT_PRIMITIVE_SPHERE:
		m_EditSphere.setRaidus(scale);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

glm::vec3 CGProj::CGEditObject::getScale()
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		return m_EditBox.getHalfSize();
	case EDIT_PRIMITIVE_SPHERE:
		return glm::vec3(m_EditSphere.getRadius());
	default:
		assert(0);
		break;
	}

	return glm::vec3();
}

GPED::c3AABB CGProj::CGEditObject::getFitAABB()
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		return m_EditBox.getFitAABB();
	case EDIT_PRIMITIVE_SPHERE:
		return m_EditSphere.getFitAABB();
	default:
		assert(0);
		break;
	}

	return GPED::c3AABB();
}

void CGProj::CGEditObject::setHalfSize(const glm::vec3 & h)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		m_EditBox.setHalfSize(h);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditObject::setHalfSize(const GPED::real x, GPED::real y, GPED::real z)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		m_EditBox.setHalfSize(x, y, z);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditObject::setXHalfSize(const GPED::real x)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		m_EditBox.setXHalfSize(x);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditObject::setYHalfSize(const GPED::real y)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		m_EditBox.setYHalfSize(y);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

void CGProj::CGEditObject::setZHalfSize(const GPED::real z)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		m_EditBox.setZHalfSize(z);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

glm::vec3 CGProj::CGEditObject::getHalfSize()
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		return m_EditBox.getHalfSize();
	default:
		assert(0);
		break;
	}

	return glm::vec3();
}

void CGProj::CGEditObject::setRaidus(GPED::real r)
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_SPHERE:
		m_EditSphere.setRaidus(r);
		break;
	default:
		assert(0);
		break;
	}

	updateBroadPhaseProxy();
}

GPED::real CGProj::CGEditObject::getRadius()
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_SPHERE:
		return m_EditSphere.getRadius();
	default:
		assert(0);
		break;
	}

	return GPED::real(0);
}

void CGProj::CGEditObject::render(const glm::mat4 & view, const glm::mat4 & proj)
{

}

void CGProj::CGEditObject::UIrender(CGAssetManager & am)
{
}

CGProj::EditObjectType CGProj::CGEditObject::getObjectType()
{
	return m_ObjectType;
}

void CGProj::CGEditObject::setObjectType(EditObjectType e)
{
	m_ObjectType = e;
}

void CGProj::CGEditObject::updateBroadPhaseProxy()
{
	if (m_BroadPhaseId != Node_Null)
		m_BroadPhase->UpdateProxy(m_BroadPhaseId, getFitAABB());
}

void CGProj::CGEditObject::renderPrimitive()
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		renderCube();
		break;
	case EDIT_PRIMITIVE_SPHERE:
		renderSphere();
		break;
	default:
		assert(0);
		break;
	}
}
/*** CG EDIT Object  ***/
// =================================================================