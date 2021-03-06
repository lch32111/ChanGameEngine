#include <CGPrecompiled.h>
#include <Graphics/CGEditProxyObject.h>

#include <Imgui/imgui.h>

#include <Graphics/CGAssetManager.h>
#include <Graphics/GLInstancePrimitiveUtil.h>


// =================================================================
/*** CG EDIT Proxy Object  ***/

CG::CGEditProxyObject::CGEditProxyObject(CGAssetManager& am)
{
	setProxyType(EDIT_PROXY_STATIC);

	// Note that the Def shader should be already initialized before this constructor
	setDefShader(am.getShader(SHADER_DEFERRED_FIRST));

	// other member variables of this class should be in the class header declaration!
}

void CG::CGEditProxyObject::setInstanceData(const std::vector<glm::mat4>& model, const std::vector<glm::mat4>& worldNormal)
{
	m_instanceNumb = model.size();

	if (m_useModelData)
	{
		// Just Decrease the instance numb to the max
		if (m_Model->getMaxInstanceNumb() < m_instanceNumb)
			CGassert();

		m_Model->setInstanceData(model, worldNormal);
	}
	else
	{
		switch (m_PrimitiveType)
		{
		case EDIT_PRIMITIVE_AABB:
		case EDIT_PRIMITIVE_OBB:
			CGInstancePrimitiveUtil::setCubeOneInstanceData(model, worldNormal);
			break;
		case EDIT_PRIMITIVE_SPHERE:
			CGInstancePrimitiveUtil::setSphereOneInstanceData(model, worldNormal);
			break;
		default:
			assert(0);
			break;
		}
	}
}

void CG::CGEditProxyObject::render(const glm::vec3& cameraPos)
{
	// CGEditObject::render(view, proj);
	m_DefShader->use();

	if (m_useModelData == false) // for the primitive rendering
	{
		// 1. Material Setting
		m_DefShader->setBool("material.CMorLM", m_CMorLM);
		
		if (m_CMorLM) // CM == false, LM == true
		{
			if (m_isLMdiffuse) glActiveTexture(GL_TEXTURE0), glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);
			if (m_isLMspecular) glActiveTexture(GL_TEXTURE1), glBindTexture(GL_TEXTURE_2D, m_specularTexture);
			if (m_isLMemissive) glActiveTexture(GL_TEXTURE2), glBindTexture(GL_TEXTURE_2D, m_emissiveTexture);

			m_DefShader->setBool("material.isLMdiffuse", m_isLMdiffuse);
			m_DefShader->setBool("material.isLMspecular", m_isLMspecular);
			m_DefShader->setBool("material.isLMemissive", m_isLMemissive);
			m_DefShader->setBool("material.isNormalMap", m_isNormalMap);
			m_DefShader->setBool("material.isDepthMap", m_isDepthMap);

			if (m_isNormalMap || m_isDepthMap)
			{
				m_DefShader->setBool("IsUseTangentSpace", true); // Calculate TBN Matrix
				m_DefShader->setVec3("cameraPos", cameraPos);
			}
			else
			{
				m_DefShader->setBool("IsUseTangentSpace", false);
			}
		}
		else
		{
			m_DefShader->setVec3("material.CMambient", m_CMambient);
			m_DefShader->setVec3("material.CMdiffuse", m_CMdiffuse);
			m_DefShader->setVec3("material.CMspecular", m_CMspecular);
			m_DefShader->setFloat("material.CMshininess", m_CMshininess);
		}
	}
	else if (m_useModelData == true) // for the loaded model rendering
	{
		m_DefShader->setBool("material.CMorLM", true); // use light map

		// the below flags will be set automatically by CGModel class.
		// So I'm initializing to prevent the shader for using garbage value.
		m_DefShader->setBool("material.isLMdiffuse", false);
		m_DefShader->setBool("material.isLMspecular", false);
		m_DefShader->setBool("material.isLMemissive", false);
		m_DefShader->setBool("material.isNormalMap", false);
		m_DefShader->setBool("material.isDepthMap", false);

		m_DefShader->setVec3("cameraPos", cameraPos);
	}

	// Now Ready to render. Go render according to the flags
	if (m_useModelData)
		m_Model->deferredFirstRender(m_DefShader, m_instanceNumb);
	else
		renderOneInstancePrimitive();
}

void CG::CGEditProxyObject::renderOneInstancePrimitive()
{
	switch (m_PrimitiveType)
	{
	case EDIT_PRIMITIVE_AABB:
	case EDIT_PRIMITIVE_OBB:
		CGInstancePrimitiveUtil::renderCube();
		break;
	case EDIT_PRIMITIVE_SPHERE:
		CGInstancePrimitiveUtil::renderSphere();
		break;
	default:
		assert(0);
		break;
	}
}

void CG::CGEditProxyObject::shadowMapRender()
{
	if (m_useModelData)
		m_Model->shadowFirstRender();
	else
		renderOneInstancePrimitive();
}

void CG::CGEditProxyObject::UIrender(CGAssetManager& am)
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

	if (m_useModelData == false) // Just Manual Vertex Data
	{
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
				if (ImGui::Combo("Set Diffuse", &selected, CG_TEXTURE_LIST, NUM_CG_TEXTURE_ENUM))
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
	}
	else if (m_useModelData == true)// Loaded Model Dat
	{

	}

	ImGui::End();
}

bool CG::CGEditProxyObject::getCMorLM()
{
	return CGEditProxyObject::m_CMorLM;
}

void CG::CGEditProxyObject::setCMorLM(bool flag)
{
	CGEditProxyObject::m_CMorLM = flag;
}

bool CG::CGEditProxyObject::isDiffuseOn()
{
	return CGEditProxyObject::m_isLMdiffuse;
}

void CG::CGEditProxyObject::setDiffuseFlag(bool flag)
{
	CGEditProxyObject::m_isLMdiffuse = flag;
}

void CG::CGEditProxyObject::setDiffuseTexture(unsigned texId)
{
	CGEditProxyObject::m_diffuseTexture = texId;
}

bool CG::CGEditProxyObject::isSpecularOn()
{
	return CGEditProxyObject::m_isLMspecular;
}

void CG::CGEditProxyObject::setSpecularFlag(bool flag)
{
	CGEditProxyObject::m_isLMspecular = flag;
}

void CG::CGEditProxyObject::setSpecularTexture(unsigned texId)
{
	CGEditProxyObject::m_specularTexture = texId;
}

bool CG::CGEditProxyObject::isEmissiveOn()
{
	return CGEditProxyObject::m_isLMemissive;
}

void CG::CGEditProxyObject::setEmissiveFlag(bool flag)
{
	CGEditProxyObject::m_isLMemissive = flag;
}

void CG::CGEditProxyObject::setEmissiveTexture(unsigned texId)
{
	CGEditProxyObject::m_emissiveTexture = texId;
}

bool CG::CGEditProxyObject::isNormalMapOn()
{
	return m_isNormalMap;
}

void CG::CGEditProxyObject::setNormalMapFlag(bool flag)
{
	m_isNormalMap = flag;
}

void CG::CGEditProxyObject::setNormalMapTexture(unsigned texId)
{
	m_normalMapTexture = texId;
}

bool CG::CGEditProxyObject::isDepthMapon()
{
	return m_isDepthMap;
}

void CG::CGEditProxyObject::setDepthMapFalg(bool flag)
{
	m_isDepthMap = flag;
}

void CG::CGEditProxyObject::setDepthMapTexture(unsigned texId)
{
	m_depthMapTexture = texId;
}

void CG::CGEditProxyObject::setCMambinet(const glm::vec3 & ambient)
{
	CGEditProxyObject::m_CMambient = ambient;
}

void CG::CGEditProxyObject::setCMdiffuse(const glm::vec3 & diffuse)
{
	CGEditProxyObject::m_CMdiffuse = diffuse;
}

void CG::CGEditProxyObject::setCMspecular(const glm::vec3 & specular)
{
	CGEditProxyObject::m_CMspecular = specular;
}

void CG::CGEditProxyObject::setCMshininess(float s)
{
	CGEditProxyObject::m_CMshininess = s;
}

bool CG::CGEditProxyObject::isModelData()
{
	return m_useModelData;
}

void CG::CGEditProxyObject::setModelData(bool m)
{
	m_useModelData = m;
}

void CG::CGEditProxyObject::setModel(CGModel * model)
{
	m_Model = model;
}

void CG::CGEditProxyObject::setProxyType(EditProxyType e)
{
	CGEditProxyObject::m_ProxyType = e;
}

CG::EditProxyType CG::CGEditProxyObject::getProxyType()
{
	return m_ProxyType;
}
/*** CG EDIT Proxy Object  ***/
// =================================================================