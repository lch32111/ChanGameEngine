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
/*** CG EDIT Object  ***/
CGProj::CGEditObject::CGEditObject()
{
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

void CGProj::CGEditObject::setFirstPassDefShader(Shader * shader)
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


// =================================================================
/*** CG EDIT Proxy Object  ***/

CGProj::CGEditProxyObject::CGEditProxyObject()
{
	setProxyType(EDIT_PROXY_STATIC);
}

void CGProj::CGEditProxyObject::render(const glm::mat4 & view, const glm::mat4 & proj)
{
	// CGEditObject::render(view, proj);

	m_DefShader->use();
	
	// 1. Material Setting
	m_DefShader->setBool("material.CMorLM", m_CMorLM);
	m_DefShader->setBool("material.isLMdiffuse", m_isLMdiffuse);
	m_DefShader->setBool("material.isLMspecular", m_isLMspecular);
	m_DefShader->setBool("material.isLMemissive", m_isLMemissive);
	if (m_CMorLM) // CM == false, LM == true
	{
		if (m_isLMdiffuse) glActiveTexture(GL_TEXTURE0), glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);
		if (m_isLMspecular) glActiveTexture(GL_TEXTURE1), glBindTexture(GL_TEXTURE_2D, m_specularTexture);
		if (m_isLMemissive) glActiveTexture(GL_TEXTURE2), glBindTexture(GL_TEXTURE_2D, m_emissiveTexture);
	}
	else
	{
		m_DefShader->setVec3("material.CMambient", m_CMambient);
		m_DefShader->setVec3("material.CMdiffuse", m_CMdiffuse);
		m_DefShader->setVec3("material.CMspecular", m_CMspecular);
		m_DefShader->setFloat("material.CMshininess", m_CMshininess);
	}
	// 1. Material Setting

	// 2. Vertex Setting
	m_DefShader->setMat4("projection", proj);
	
	glm::mat4 viewModel(1.0);
	
	// Model Calculation First
	viewModel = glm::translate(viewModel, this->getPosition());
	// model = glm::rotate(model, ) // TODO: add the rotation function later
	viewModel = glm::scale(viewModel, this->getScale());

	// View Model Calculation Second
	viewModel = view * viewModel;
	m_DefShader->setMat4("viewModel", viewModel);
	m_DefShader->setMat3("MVNormalMatrix", glm::mat3(glm::transpose(glm::inverse(viewModel))));
	// 2. Vertex Setting

	// Now Ready to render. Go render according to the primitive
	renderPrimitive();
}

void CGProj::CGEditProxyObject::UIrender(CGAssetManager& am)
{
	// CGEditObject::UIrender(am);

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
		float temp[3] = { m_CMambient.x,m_CMambient.y, m_CMambient.z };
		ImGui::ColorEdit3("Ambient", temp);
		m_CMambient = { temp[0], temp[1], temp[2] };
		
		temp[0] = m_CMdiffuse.x, temp[1] = m_CMdiffuse.y, temp[2] = m_CMdiffuse.z;
		ImGui::ColorEdit3("Diffuse", temp);
		m_CMdiffuse = { temp[0], temp[1], temp[2] };

		temp[0] = m_CMspecular.x, temp[1] = m_CMspecular.y, temp[2] = m_CMspecular.z;
		ImGui::ColorEdit3("specular", temp);
		m_CMspecular = { temp[0], temp[1], temp[2] };

		float min = 0.001f;
		float max = 1.f;
		ImGui::DragScalar("shininess", ImGuiDataType_Float, &m_CMshininess, 0.005f, &min, &max, "%f", 1.0f);

		int selected = 0;
		if (ImGui::Combo("Set Color Material", &selected, CG_COLOR_MATERIAL_LIST, 24))
		{
			setCMambinet(CG_COLOR_MATERIAL_AMBIENT[selected]);
			setCMdiffuse(CG_COLOR_MATERIAL_DIFFUSE[selected]);
			setCMspecular(CG_COLOR_MATERIAL_SPECULAR[selected]);
			setCMshininess(CG_COLOR_MATERIAL_SHININESS[selected]);
		}
	}


	ImGui::End();
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

void CGProj::CGEditProxyObject::setCMambinet(const glm::vec3 & ambient)
{
	CGEditProxyObject::m_CMambient = ambient;
}

void CGProj::CGEditProxyObject::setCMdiffuse(const glm::vec3 & diffuse)
{
	CGEditProxyObject::m_CMdiffuse = diffuse;
}

void CGProj::CGEditProxyObject::setCMspecular(const glm::vec3 & specular)
{
	CGEditProxyObject::m_CMspecular = specular;
}

void CGProj::CGEditProxyObject::setCMshininess(float s)
{
	CGEditProxyObject::m_CMshininess = s;
}

void CGProj::CGEditProxyObject::setProxyType(EditProxyType e)
{
	CGEditProxyObject::m_ProxyType = e;
}

CGProj::EditProxyType CGProj::CGEditProxyObject::getProxyType()
{
	return m_ProxyType;
}
/*** CG EDIT Proxy Object  ***/
// =================================================================