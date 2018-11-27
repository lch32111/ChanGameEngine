#include "CGEditObject.h"

#include <Imgui/imgui.h>

#include <Graphics/GLPrimitiveUtil.h>
#include <Graphics/CGAssetManager.h>


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
	CGEditProxyObject::setProxyType(EDIT_PROXY_STATIC);
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

void CGProj::CGEditProxyObject::setFirstPassDefShader(Shader * shader)
{
	CGEditProxyObject::m_FirstPassDefShader = shader;
}

void CGProj::CGEditProxyObject::updateBroadPhaseProxy()
{
	if (m_BroadPhaseId != Node_Null)
		m_BroadPhase->UpdateProxy(m_BroadPhaseId, CGEditProxyObject::getFitAABB());
}

void CGProj::CGEditProxyObject::setEditShape(EditPrimitiveType e)
{
	CGEditProxyObject::m_PrimitiveType = e;

	if (e <= EDIT_PRIMITIVE_OBB) // e may be AABB or OBB
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

void CGProj::CGEditProxyObject::render(const glm::mat4 & view, const glm::mat4 & proj)
{
	m_FirstPassDefShader->use();
	
	// Material Setting
	m_FirstPassDefShader->setBool("material.CMorLM", m_CMorLM);
	m_FirstPassDefShader->setBool("material.isLMdiffuse", m_isLMdiffuse);
	m_FirstPassDefShader->setBool("material.isLMspecular", m_isLMspecular);
	m_FirstPassDefShader->setBool("material.isLMemissive", m_isLMemissive);

	if (m_CMorLM) // CM == false, LM == true
	{
		if (m_isLMdiffuse) glActiveTexture(GL_TEXTURE0), glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);
		if (m_isLMspecular) glActiveTexture(GL_TEXTURE1), glBindTexture(GL_TEXTURE_2D, m_specularTexture);
		if (m_isLMemissive) glActiveTexture(GL_TEXTURE2), glBindTexture(GL_TEXTURE_2D, m_emissiveTexture);
	}
	// Material Setting

	// Vertex Setting
	m_FirstPassDefShader->setMat4("projection", proj);
	
	glm::mat4 viewModel(1.0);
	
	// Model Calculation First
	viewModel = glm::translate(viewModel, this->getPosition());
	// model = glm::rotate(model, ) // TODO: add the rotation function later
	viewModel = glm::scale(viewModel, this->getScale());

	// View Model Calculation Second
	viewModel = view * viewModel;
	m_FirstPassDefShader->setMat4("viewModel", viewModel);
	m_FirstPassDefShader->setMat3("MVNormalMatrix", glm::mat3(glm::transpose(glm::inverse(viewModel))));
	
	// Now Ready to render. Go render according to the primitive
	renderPrimitive();
}

void CGProj::CGEditProxyObject::UIrender(CGAssetManager& am)
{
	ImGui::Begin("Edit Object");

	// Proxy Id
	ImGui::Text("Proxy Id : %d", m_BroadPhaseId);

	// Primitive Type
	switch (CGEditProxyObject::m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
		ImGui::Text("Primitive Type : AABB");
		break;
	case EDIT_PRIMITIVE_OBB:
		ImGui::Text("Primitive Type : OBB");
		break;
	case EDIT_PRIMITIVE_SPHERE:
		ImGui::Text("Primitive Type : Sphere");
		break;
	}

	// Position
	glm::vec3 pickedPos = this->getPosition();
	ImGui::Text("Position %.2f %.2f %.2f", pickedPos.x, pickedPos.y, pickedPos.z);

	// Primitive Dimension
	switch (CGEditProxyObject::m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		glm::vec3 halfExtents = this->getHalfSize();
		ImGui::Text("HalfSize : %.2f %.2f %.2f", halfExtents.x, halfExtents.y, halfExtents.z);
		break;
	case EDIT_PRIMITIVE_SPHERE:
		ImGui::Text("Radius : %.2f", this->getRadius());
		break;
	}

	int cmorlm = (int)m_CMorLM;
	ImGui::RadioButton("Color Material", &cmorlm, 0); ImGui::SameLine();
	ImGui::RadioButton("Light Map Material", &cmorlm, 1);
	m_CMorLM = bool(cmorlm);

	if (m_CMorLM) // Light Map Material
	{
		ImGui::Checkbox("Diffuse Texture", &m_isLMdiffuse);
		if (m_isLMdiffuse)
		{
			int selected = 0;
			if(ImGui::Combo("Set Diffuse", &selected, CG_TEXTURE_LIST, NUM_CG_TEXTURE_ENUM))
				setDiffuseTexture(am.getTexture(CG_TEXTURE_ENUM(selected), true));
		}

		ImGui::Checkbox("Specular Texture", &m_isLMspecular);
		if (m_isLMspecular)
		{
			int selected = 0;
			if (ImGui::Combo("Set Specular", &selected, CG_TEXTURE_LIST, NUM_CG_TEXTURE_ENUM))
				setSpecularTexture(am.getTexture(CG_TEXTURE_ENUM(selected), true));
		}

		ImGui::Checkbox("Emissive Texture", &m_isLMemissive);
		if (m_isLMemissive)
		{
			int selected = 0;
			if (ImGui::Combo("Set Emissive", &selected, CG_TEXTURE_LIST, NUM_CG_TEXTURE_ENUM))
				setEmissiveTexture(am.getTexture(CG_TEXTURE_ENUM(selected), true));
		}
	}
	else // Colro Material
	{

	}


	ImGui::End();
}

void CGProj::CGEditProxyObject::renderPrimitive()
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

bool CGProj::CGEditProxyObject::getCMorLM()
{
	return CGEditProxyObject::m_CMorLM;
}

void CGProj::CGEditProxyObject::setCMorLM(bool flag)
{
	CGEditProxyObject::m_CMorLM = flag;
}

bool CGProj::CGEditProxyObject::isDiffuseOn()
{
	return CGEditProxyObject::m_isLMdiffuse;
}

void CGProj::CGEditProxyObject::setDiffuseFlag(bool flag)
{
	CGEditProxyObject::m_isLMdiffuse = flag;
}

void CGProj::CGEditProxyObject::setDiffuseTexture(unsigned texId)
{
	CGEditProxyObject::m_diffuseTexture = texId;
}

bool CGProj::CGEditProxyObject::isSpecularOn()
{
	return CGEditProxyObject::m_isLMspecular;
}

void CGProj::CGEditProxyObject::setSpecularFlag(bool flag)
{
	CGEditProxyObject::m_isLMspecular = flag;
}

void CGProj::CGEditProxyObject::setSpecularTexture(unsigned texId)
{
	CGEditProxyObject::m_specularTexture = texId;
}

bool CGProj::CGEditProxyObject::isEmissiveOn()
{
	return CGEditProxyObject::m_isLMemissive;
}

void CGProj::CGEditProxyObject::setEmissiveFlag(bool flag)
{
	CGEditProxyObject::m_isLMemissive = flag;
}

void CGProj::CGEditProxyObject::setEmissiveTexture(unsigned texId)
{
	CGEditProxyObject::m_emissiveTexture = texId;
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

glm::vec3 CGProj::CGEditProxyObject::getScale()
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		return CGEditBox::getHalfSize();
	case EDIT_PRIMITIVE_SPHERE:
		return glm::vec3(CGEditSpere::getRadius());
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
