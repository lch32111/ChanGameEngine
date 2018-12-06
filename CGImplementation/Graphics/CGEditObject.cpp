#include "CGEditObject.h"

#include <Imgui/imgui.h>
#include <Imgui/CGimguiUtil.h>

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
	m_DefShader->setMat4("view", view);
	
	glm::mat4 model(1.0);
	model = glm::translate(model, this->getPosition());
	// model = glm::rotate(model, ) // TODO: add the rotation function later
	model = glm::scale(model, this->getScale());

	m_DefShader->setMat4("model", model);
	m_DefShader->setMat3("ModelNormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
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

		float min = 0.1f;
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


// =================================================================
/*** CG Light Object  ***/

CGProj::CGEditLightObject::CGEditLightObject()
{
	setLightType(EDIT_POINT_LIGHT);
	setPosition(glm::vec3(0));
	setLightDirection(glm::vec3(0, 1, 0));
	setAmbientColor(glm::vec3(0.1));
	setDiffuseColor(glm::vec3(0.2));
	setSpecularColor(glm::vec3(1.0));
	
	m_AttnConstant = 1.0;
	m_AttnLinear = 0.7;
	m_AttnQuadratic = 1.0;
	updateRadius();

	setInnerCutOffInDegree(12.5);
	setOuterCutOffInDegree(17.5);

	m_spotVis.prepareData();
	m_spotVis.setOuterConeInRadians(glm::acos(m_SpotOuterCutOff), m_AttnRadius);
	m_spotVis.setInnerConeInRadians(glm::acos(m_SpotInnerCutOff), m_AttnRadius);
}

void CGProj::CGEditLightObject::setForwardShader(Shader * shader)
{
	m_forwardShader = shader;
}

void CGProj::CGEditLightObject::forwardRender(const glm::mat4 & view, const glm::mat4 & proj)
{
	// Refer to the simpleColorRender!

	// Edit Object Render
	glm::mat4 model(1.0);
	model = glm::translate(model, m_lightPosition);
	model = glm::scale(model, this->getScale());

	m_forwardShader->use();
	m_forwardShader->setMat4("projection", proj);
	m_forwardShader->setMat4("view", view);
	m_forwardShader->setMat4("model", model);
	m_forwardShader->setVec3("Color", m_lightDiffuse);
	renderPrimitive();
	// Edit Object Render

	// Light Range Render
	if (m_isRangeRender)
	{
		switch (m_LightType)
		{
		case EDIT_POINT_LIGHT:
			m_forwardShader->setVec3("Color", glm::vec3(0.662, 0.831, 0.87));

			model = glm::mat4(1.0);
			model = glm::translate(model, m_lightPosition);
			model = glm::scale(model, glm::vec3(m_AttnRadius));

			// XZ Circle
			m_forwardShader->setMat4("model", model);
			renderWireCircle2D();

			// XY Circle
			glm::mat4 rot(1.0);
			rot = glm::rotate(rot, glm::radians(90.f), glm::vec3(1, 0, 0));
			m_forwardShader->setMat4("model", model * rot);
			renderWireCircle2D();

			// YZ Circle
			rot = glm::mat4(1.0);
			rot = glm::rotate(rot, glm::radians(90.f), glm::vec3(0, 0, 1));
			m_forwardShader->setMat4("model", model * rot);
			renderWireCircle2D();
			break;
		case EDIT_SPOT_LIGHT:
			m_spotVis.render(view, proj, m_lightPosition, m_lightDirection);
			break;
		}
	}
	// Light Range Render
}

void CGProj::CGEditLightObject::UIrender(CGAssetManager & am)
{
	// CGEditObject::UIrender(am);

	ImGui::Begin("Edit Object");

	// Proxy Id
	ImGui::Text("Proxy Id : %d", m_BroadPhaseId);

	// Primitive Type
	switch (CGEditLightObject::m_PrimitiveType)
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
	switch (CGEditLightObject::m_PrimitiveType)
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

	// Light Type
	int lightType = (int)m_LightType;
	ImGui::RadioButton("Dir Light", &lightType, 0); ImGui::SameLine();
	ImGui::RadioButton("Point Light", &lightType, 1); ImGui::SameLine();
	ImGui::RadioButton("Spot Light", &lightType, 2);
	m_LightType = (EditLightType)lightType;

	ImGui::Checkbox("Range Redner", &m_isRangeRender);

	switch (m_LightType)
	{
	case EDIT_DIRECTION_LIGHT:
	{
		// Light Direction
		float t_dir[3] = { m_lightDirection.x, m_lightDirection.y, m_lightDirection.z };
		ImGui::InputFloat3("Light Direction", t_dir, "%.2f");
		m_lightDirection.x = t_dir[0], m_lightDirection.y = t_dir[1], m_lightDirection.z = t_dir[2];
		m_lightDirection = glm::normalize(m_lightDirection);
		break;
	}
	case EDIT_POINT_LIGHT:
	{
		// Attenuation
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("Light Radius", &m_AttnRadius); ImGui::SameLine();
		ShowHelpMarker("If you control the attn parameters,\nthe light radius will be set automatically.\nHowever, If you want to set the radius manually,\nyou can set it manually");
		ImGui::PopItemWidth();
		ImGui::Text("Attn Constant : %.2f", m_AttnConstant); ImGui::SameLine();
		ShowHelpMarker("As you know in the code,\nthe radius calculated in updateRadius() is also based on the diffuse color value");
		if (ImGui::SliderFloat("linear", &m_AttnLinear, 0.0001, 1.0, "Attn Linear : %f")) updateRadius();
		if (ImGui::SliderFloat("quadratic", &m_AttnQuadratic, 0.000001, 1.0, "Attn Quadratic : %f")) updateRadius();
		break;
	}
	case EDIT_SPOT_LIGHT:
	{
		// Light Direction
		float t_dir[3] = { m_lightDirection.x, m_lightDirection.y, m_lightDirection.z };
		ImGui::InputFloat3("Light Direction", t_dir, "%.2f");
		m_lightDirection.x = t_dir[0], m_lightDirection.y = t_dir[1], m_lightDirection.z = t_dir[2];
		m_lightDirection = glm::normalize(m_lightDirection);

		// Cut off of Spot Light
		float inner_Indegree = glm::acos(m_SpotInnerCutOff) * 180.f / glm::pi<float>();
		float outer_Indegree = glm::acos(m_SpotOuterCutOff) * 180.f / glm::pi<float>();
		if (ImGui::SliderFloat("In", &inner_Indegree, 0, outer_Indegree, "Inner Cutoff In Degree :  %.2f"))
		{
			setInnerCutOffInDegree(inner_Indegree);
			m_spotVis.setInnerConeInRadians(glm::acos(m_SpotInnerCutOff), m_AttnRadius);
			
		}
		if (ImGui::SliderFloat("Out", &outer_Indegree, inner_Indegree, 45, "Outer Cutoff In Degree : %.2f"))
		{
			setOuterCutOffInDegree(outer_Indegree);
			m_spotVis.setOuterConeInRadians(glm::acos(m_SpotOuterCutOff), m_AttnRadius);
		}

		// Attenuation
		ImGui::PushItemWidth(50);
		if (ImGui::InputFloat("Light Radius", &m_AttnRadius))
		{
			m_spotVis.setInnerConeInRadians(glm::acos(m_SpotInnerCutOff), m_AttnRadius);
			m_spotVis.setOuterConeInRadians(glm::acos(m_SpotOuterCutOff), m_AttnRadius);
		}
		ImGui::SameLine();
		ShowHelpMarker("If you control the attn parameters,\nthe light radius will be set automatically.\nHowever, If you want to set the radius manually,\nyou can set it manually");
		ImGui::PopItemWidth();
		ImGui::Text("Attn Constant : %.2f", m_AttnConstant); ImGui::SameLine();
		ShowHelpMarker("As you know in the code,\nthe radius calculated in updateRadius() is also based on the diffuse color value");
		if (ImGui::SliderFloat("linear", &m_AttnLinear, 0.0001, 1.0, "Attn Linear : %f"))
		{
			m_spotVis.setInnerConeInRadians(glm::acos(m_SpotInnerCutOff), m_AttnRadius);
			m_spotVis.setOuterConeInRadians(glm::acos(m_SpotOuterCutOff), m_AttnRadius);
			updateRadius();
		}
		if (ImGui::SliderFloat("quadratic", &m_AttnQuadratic, 0.000001, 1.0, "Attn Quadratic : %f"))
		{
			m_spotVis.setInnerConeInRadians(glm::acos(m_SpotInnerCutOff), m_AttnRadius);
			m_spotVis.setOuterConeInRadians(glm::acos(m_SpotOuterCutOff), m_AttnRadius);
			updateRadius();
		}
		break;
	}
	}

	// Light Color
	float temp[3] = { m_lightAmbient.x,m_lightAmbient.y, m_lightAmbient.z };
	ImGui::ColorEdit3("Ambient", temp);
	m_lightAmbient = { temp[0], temp[1], temp[2] };

	temp[0] = m_lightDiffuse.x, temp[1] = m_lightDiffuse.y, temp[2] = m_lightDiffuse.z;
	ImGui::ColorEdit3("Diffuse", temp);
	m_lightDiffuse = { temp[0], temp[1], temp[2] };

	temp[0] = m_lightSpecular.x, temp[1] = m_lightSpecular.y, temp[2] = m_lightSpecular.z;
	ImGui::ColorEdit3("specular", temp);
	m_lightSpecular = { temp[0], temp[1], temp[2] };

	ImGui::End();
}

void CGProj::CGEditLightObject::setLightType(EditLightType e)
{
	m_LightType = e;
}

CGProj::EditLightType CGProj::CGEditLightObject::getLightType()
{
	return m_LightType;
}

void CGProj::CGEditLightObject::setPosition(const glm::vec3 & p)
{
	CGEditObject::setPosition(p); // set Edit Object Position 

	// and then set Light Position
	m_lightPosition = p;
}

void CGProj::CGEditLightObject::setPosition(const GPED::real x, const GPED::real y, const GPED::real z)
{
	CGEditObject::setPosition(x, y, z);

	m_lightPosition.x = x;
	m_lightPosition.y = y;
	m_lightPosition.z = z;
}

void CGProj::CGEditLightObject::setXposition(const GPED::real x)
{
	CGEditObject::setXposition(x);
	m_lightPosition.x = x;
}

void CGProj::CGEditLightObject::setYposition(const GPED::real y)
{
	CGEditObject::setYposition(y);
	m_lightPosition.y = y;
}

void CGProj::CGEditLightObject::setZposition(const GPED::real z)
{
	CGEditObject::setZposition(z);
	m_lightPosition.z = z;
}

glm::vec3 CGProj::CGEditLightObject::getPosition()
{
	// You can get the edit object position 
	// However I will return the light position for the Obect-Oriented Programming(OOP)
	return m_lightPosition;
}

void CGProj::CGEditLightObject::setLightDirection(const glm::vec3 & d)
{
	// Notice the direction should be normalized
	m_lightDirection = glm::normalize(d);
}

void CGProj::CGEditLightObject::setLightDirection(const GPED::real x, const GPED::real y, const GPED::real z)
{
	m_lightDirection = glm::normalize(glm::vec3(x, y, z));
}

void CGProj::CGEditLightObject::setLightXdirection(const GPED::real x)
{
	m_lightDirection.x = x;
	m_lightDirection = glm::normalize(m_lightDirection);
}

void CGProj::CGEditLightObject::setLightYdirection(const GPED::real y)
{
	m_lightDirection.y = y;
	m_lightDirection = glm::normalize(m_lightDirection);
}

void CGProj::CGEditLightObject::setLightZdirection(const GPED::real z)
{
	m_lightDirection.z = z;
	m_lightDirection = glm::normalize(m_lightDirection);
}

glm::vec3 CGProj::CGEditLightObject::getLightDirection()
{
	return m_lightDirection;
}

void CGProj::CGEditLightObject::setAmbientColor(const glm::vec3 & ac)
{
	m_lightAmbient = ac;
}

void CGProj::CGEditLightObject::setAmbientColor(const GPED::real r, const GPED::real g, const GPED::real b)
{
	m_lightAmbient.r = r;
	m_lightAmbient.g = g;
	m_lightAmbient.b = b;
}

void CGProj::CGEditLightObject::setAmbientRedColor(const GPED::real r)
{
	m_lightAmbient.r = r;
}

void CGProj::CGEditLightObject::setAmbientGreenColor(const GPED::real g)
{
	m_lightAmbient.g = g;
}

void CGProj::CGEditLightObject::setAmbientBlueColor(const GPED::real b)
{
	m_lightAmbient.b = b;
}

glm::vec3 CGProj::CGEditLightObject::getAmbientColor()
{
	return m_lightAmbient;
}

void CGProj::CGEditLightObject::setDiffuseColor(const glm::vec3 & dc)
{
	m_lightDiffuse = dc;
}

void CGProj::CGEditLightObject::setDiffuseColor(const GPED::real r, const GPED::real g, const GPED::real b)
{
	m_lightDiffuse.r = r;
	m_lightDiffuse.g = g;
	m_lightDiffuse.b = b;
}

void CGProj::CGEditLightObject::setDiffuseRedColor(const GPED::real r)
{
	m_lightDiffuse.r = r;
}

void CGProj::CGEditLightObject::setDiffuseGreenColor(const GPED::real g)
{
	m_lightDiffuse.g = g;
}

void CGProj::CGEditLightObject::setDiffuseBlueColor(const GPED::real b)
{
	m_lightDiffuse.b = b;
}

glm::vec3 CGProj::CGEditLightObject::getDiffuseColor()
{
	return m_lightDiffuse;
}

void CGProj::CGEditLightObject::setSpecularColor(const glm::vec3 & sc)
{
	m_lightSpecular = sc;
}

void CGProj::CGEditLightObject::setSpecularColor(const GPED::real r, const GPED::real g, const GPED::real b)
{
	m_lightSpecular.r = r;
	m_lightSpecular.g = g;
	m_lightSpecular.b = b;
}

void CGProj::CGEditLightObject::setSpecularRedColor(const GPED::real r)
{
	m_lightSpecular.r = r;
}

void CGProj::CGEditLightObject::setSpecularGreenColor(const GPED::real g)
{
	m_lightSpecular.g = g;
}

void CGProj::CGEditLightObject::setSpecularBlueColor(const GPED::real b)
{
	m_lightSpecular.b = b;
}

glm::vec3 CGProj::CGEditLightObject::getSpecularColor()
{
	return m_lightSpecular;
}

void CGProj::CGEditLightObject::setAttnConstant(const float c)
{
	m_AttnConstant = c;
	updateRadius();
}

float CGProj::CGEditLightObject::getAttnConsant()
{
	return m_AttnConstant;
}

void CGProj::CGEditLightObject::setAttnLinear(const float l)
{
	m_AttnLinear = l;
	updateRadius();
}

float CGProj::CGEditLightObject::getAttnLinear()
{
	return m_AttnLinear;
}

void CGProj::CGEditLightObject::setAttnQuadratic(const float q)
{
	m_AttnQuadratic = q;
	updateRadius();
}

float CGProj::CGEditLightObject::getAttnQuadratic()
{
	return m_AttnQuadratic;
}

float CGProj::CGEditLightObject::getLightRadius()
{
	return m_AttnRadius;
}

void CGProj::CGEditLightObject::updateRadius()
{
	// the diffuse color of light is used for the max component of light
	// You need to keep in mind that the radius will be bigger if the m_lightDiffuse will be bigger
	float lightMin = 1 / (1.f / 256.f);
	float lightMax = std::fmaxf(std::fmaxf(m_lightDiffuse.r, m_lightDiffuse.g), m_lightDiffuse.b);

	// Calculate the radius of light volume
	
	m_AttnRadius =
		(
			-m_AttnLinear +
			std::sqrtf(
				m_AttnLinear * m_AttnLinear - 4 * m_AttnQuadratic *
				(m_AttnConstant - lightMax * lightMin))
			)
		/
		(2 * m_AttnQuadratic);
}

void CGProj::CGEditLightObject::setInnerCutOffInDegree(const float degree)
{
	// Notice the measure of angle
	m_SpotInnerCutOff = glm::cos(glm::radians(degree));
}

void CGProj::CGEditLightObject::setInnerCutoffInRadian(const float radian)
{
	m_SpotInnerCutOff = glm::cos(radian);
}

float CGProj::CGEditLightObject::getInnerCutOff()
{
	return m_SpotInnerCutOff;
}

void CGProj::CGEditLightObject::setOuterCutOffInDegree(const float degree)
{
	m_SpotOuterCutOff = glm::cos(glm::radians(degree));
}

void CGProj::CGEditLightObject::setOuterCutOffInRadian(const float radian)
{
	m_SpotOuterCutOff = glm::cos(radian);
}

float CGProj::CGEditLightObject::getOuterCutOff()
{
	return m_SpotOuterCutOff;
}

void CGProj::CGEditLightObject::setLightPropertyOnShader(int index, const glm::vec3& cameraPos)
{
	std::string sIndex = std::to_string(index);

	m_DefShader->setVec3("cameraPos", cameraPos);
	switch (m_LightType)
	{
	case EDIT_DIRECTION_LIGHT:
	{
		m_DefShader->setVec3("dirLights[" + sIndex + "].Direction", m_lightDirection);

		m_DefShader->setVec3("dirLights[" + sIndex + "].Ambient", m_lightAmbient);
		m_DefShader->setVec3("dirLights[" + sIndex + "].Diffuse", m_lightDiffuse);
		m_DefShader->setVec3("dirLights[" + sIndex + "].Specular", m_lightSpecular);
		break;
	}
	case EDIT_POINT_LIGHT:
	{	
		m_DefShader->setVec3("pointLights[" + sIndex + "].Position", m_lightPosition);

		m_DefShader->setFloat("pointLights[" + sIndex + "].Constant", m_AttnConstant);
		m_DefShader->setFloat("pointLights[" + sIndex + "].Linear", m_AttnLinear);
		m_DefShader->setFloat("pointLights[" + sIndex + "].Quadratic", m_AttnQuadratic);
		m_DefShader->setFloat("pointLights[" + sIndex + "].Radius", m_AttnRadius);

		m_DefShader->setVec3("pointLights[" + sIndex + "].Ambient", m_lightAmbient);
		m_DefShader->setVec3("pointLights[" + sIndex + "].Diffuse", m_lightDiffuse);
		m_DefShader->setVec3("pointLights[" + sIndex + "].Specular", m_lightSpecular);
		break;
	}
	case EDIT_SPOT_LIGHT:
	{	
		m_DefShader->setVec3("spotLights[" + sIndex + "].Position", m_lightPosition);
		m_DefShader->setVec3("spotLights[" + sIndex + "].Direction", m_lightDirection);

		m_DefShader->setFloat("spotLights[" + sIndex + "].Inner_CutOff", m_SpotInnerCutOff);
		m_DefShader->setFloat("spotLights[" + sIndex + "].Outer_CutOff", m_SpotOuterCutOff);

		m_DefShader->setFloat("spotLights[" + sIndex + "].Constant", m_AttnConstant);
		m_DefShader->setFloat("spotLights[" + sIndex + "].Linear", m_AttnLinear);
		m_DefShader->setFloat("spotLights[" + sIndex + "].Quadratic", m_AttnQuadratic);
		m_DefShader->setFloat("spotLights[" + sIndex + "].Radius", m_AttnRadius);

		m_DefShader->setVec3("spotLights[" + sIndex + "].Ambient", m_lightAmbient);
		m_DefShader->setVec3("spotLights[" + sIndex + "].Diffuse", m_lightDiffuse);
		m_DefShader->setVec3("spotLights[" + sIndex + "].Specular", m_lightSpecular);
		break;
	}
	default:
		assert(0);
	}
}


/*** CG Light Object  ***/
// =================================================================