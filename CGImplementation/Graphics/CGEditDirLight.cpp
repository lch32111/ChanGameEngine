#include <Graphics/CGEditDirLight.h>

#include <Imgui/imgui.h>
#include <Imgui/CGimguiUtil.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Graphics/GLPrimitiveUtil.h>
#include <Graphics/CGDefSecondUtil.h>
#include <Graphics/CGEditObject.h>
#include <Graphics/CGAssetManager.h>
#include <Graphics/CGEditProxyObject.h>
#include <Graphics/CGFrustum.h>

#include <GPED/CGPhysicsUtil.h>


CGProj::CGEditDirLight::CGEditDirLight()
{
	m_lightFactors = nullptr;

	m_DepthMapShader = nullptr;
	m_DebugDepthMapShader = nullptr;

	m_depthMapFBO = m_depthMapTexture = 0;
	m_shadowWidth = m_shadowHeight = 1024;

	m_shadowBias = 0.005f;

	m_shadowProjection = false; // Orthographic Projection

	m_orthoLeft = -10.f;
	m_orthoRight = 10.f;
	m_orthoBottom = -10.f;
	m_orthoTop = 10.f;

	m_perFOV = glm::radians(45.f);
	m_perAspect = (float)m_shadowWidth / (float)m_shadowHeight;

	m_shadowNearPlane = 1.f;
	m_shadowFarPlane = 7.5f;

	m_shadowLightView = glm::mat4(1.f);
	m_shadowLightProjection = glm::mat4(1.f);
	m_shadowLightSpaceMatrix = glm::mat4(1.f);
}

void CGProj::CGEditDirLight::initialize(CGAssetManager & am, CGEditLightCommonFactor * factor)
{
	m_lightFactors = factor;

	// Shadow Map Initialization
	// Also note that the shadow map shader should be initialized before this constructor!
	m_DepthMapShader = am.getShader(SHADER_DIR_SHADOW_MAP);
	m_InstanceDepthMapShader = am.getShader(SHADER_INSTANCE_DIR_SHADOW_MAP);
	m_DebugDepthMapShader = am.getShader(SHADER_DIR_SHADOW_MAP_DEBUG_RENDER);
	// TODO : Add the Instance DebugDepthMap Shader
	
	// Shadow Map Initialization
	glGenFramebuffers(1, &m_depthMapFBO);

	glGenTextures(1, &m_depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		assert(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Shadow Map Initialization
}

void CGProj::CGEditDirLight::debugDepthMapRender(const glm::mat4& view, const glm::mat4& proj)
{
	m_DebugDepthMapShader->use();
	m_DebugDepthMapShader->setMat4("view", view);
	m_DebugDepthMapShader->setMat4("projection", proj);
	m_DebugDepthMapShader->setBool("shadowProjection", m_shadowProjection);
	m_DebugDepthMapShader->setFloat("near_plane", m_shadowNearPlane);
	m_DebugDepthMapShader->setFloat("far_plane", m_shadowFarPlane);

	glm::mat4 model(1.0);
	model = glm::translate(model, m_lightFactors->lightPosition);
	model = glm::scale(model, glm::vec3(2.0));
	m_DebugDepthMapShader->setMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_depthMapTexture);
	renderCube();
}

void CGProj::CGEditDirLight::UIrenderForCommon()
{
	// Light Direction
	float t_dir[3] = { m_lightFactors->lightDirection.x, m_lightFactors->lightDirection.y, m_lightFactors->lightDirection.z };
	ImGui::InputFloat3("Light Direction", t_dir, "%.2f");
	m_lightFactors->lightDirection.x = t_dir[0], m_lightFactors->lightDirection.y = t_dir[1], m_lightFactors->lightDirection.z = t_dir[2];
	m_lightFactors->lightDirection = glm::normalize(m_lightFactors->lightDirection);
}

void CGProj::CGEditDirLight::UIrenderForShadow()
{
	ImGui::Checkbox("Shadow Projection", &m_shadowProjection);
	ImGui::SameLine(); ShowHelpMarker("Check -> Perspective, NonCheck -> Orthographic");

	int wharr[2] = { static_cast<int>(m_shadowWidth), static_cast<int>(m_shadowHeight) };
	if (ImGui::InputInt2("shadow width & height", wharr))
	{
		setShadowWidthHeight(wharr[0], wharr[1]);
	}

	if (m_shadowProjection)
	{
		float fovIndegree = m_perFOV * 180.f / glm::pi<float>();
		ImGui::InputFloat("FOV In Degree", &fovIndegree);
		m_perFOV = glm::radians(fovIndegree);
		ImGui::InputFloat("Aspect", &m_perAspect);
	}
	else
	{
		ImGui::InputFloat("ortho Left", &m_orthoLeft);
		ImGui::InputFloat("ortho Right", &m_orthoRight);
		ImGui::InputFloat("ortho Bottom", &m_orthoBottom);
		ImGui::InputFloat("ortho Top", &m_orthoTop);
	}

	ImGui::InputFloat("shadow near plane", &m_shadowNearPlane);
	ImGui::InputFloat("shadow far plane", &m_shadowFarPlane);
	ImGui::InputFloat("shadow bias", &m_shadowBias);
}

void CGProj::CGEditDirLight::setLightPropertyOnShader(Shader* shader, 
	const std::string & sLightIndex, const std::string & sShadowIndex, const unsigned shadowIndex)
{
	shader->setVec3("dirLights[" + sLightIndex + "].Direction", m_lightFactors->lightDirection);

	shader->setVec3("dirLights[" + sLightIndex + "].Ambient", m_lightFactors->lightAmbient);
	shader->setVec3("dirLights[" + sLightIndex + "].Diffuse", m_lightFactors->lightDiffuse);
	shader->setVec3("dirLights[" + sLightIndex + "].Specular", m_lightFactors->lightSpecular);
	shader->setFloat("dirLights[" + sLightIndex + "].Intensity", m_lightFactors->lightIntensity);
	shader->setInt("dirLights[" + sLightIndex + "].ShadowIndex", SHADOW_INDEX_NONE);

	// NR_DIR_SHADOW is limitation of shadow mapping texture
	if (m_lightFactors->isShadow && shadowIndex < NR_DIR_SHADOWS)
	{
		shader->setInt("dirLights[" + sLightIndex + "].ShadowIndex", shadowIndex);
		shader->setMat4("dirLightSpace[" + sShadowIndex + "]", m_shadowLightSpaceMatrix);
		shader->setFloat("dirBias[" + sShadowIndex + "]", m_shadowBias);

		glActiveTexture(GL_TEXTURE0 + NR_GBUFFER_TEXTURES + shadowIndex);
		glBindTexture(GL_TEXTURE_2D, m_depthMapTexture);
	}
}

void CGProj::CGEditDirLight::renderShadowMap(std::vector<CGEditProxyObject>& objects)
{
	// Light Space Setting
	m_shadowLightView = safeLookAt(
		m_lightFactors->lightPosition,
		m_lightFactors->lightPosition + m_lightFactors->lightDirection,
		glm::vec3(0, 1, 0));

	if (m_shadowProjection)
	{
		m_shadowLightProjection = glm::perspective
		(
			m_perFOV, m_perAspect,
			m_shadowNearPlane, m_shadowFarPlane
		);
	}
	else
	{
		m_shadowLightProjection = glm::ortho
		(
			m_orthoLeft, m_orthoRight, m_orthoBottom, m_orthoTop,
			m_shadowNearPlane, m_shadowFarPlane
		);
	}
	m_shadowLightSpaceMatrix = m_shadowLightProjection * m_shadowLightView;

	// Shadow Mapping Pass
	glViewport(0, 0, m_shadowWidth, m_shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	m_DepthMapShader->use();
	m_DepthMapShader->setMat4("lightSpaceMatrix", m_shadowLightSpaceMatrix);
	m_InstanceDepthMapShader->use();
	m_InstanceDepthMapShader->setMat4("lightSpaceMatrix", m_shadowLightSpaceMatrix);

	for (unsigned i = 0; i < objects.size(); ++i)
	{
		if (objects[i].isModelData())
		{
			m_InstanceDepthMapShader->use();
		}
		else
		{
			m_DepthMapShader->use();

			// TODO : Do the Frustum Culling!
			glm::mat4 model;
			model = glm::mat4(1.0);
			model = glm::translate(model, objects[i].getPosition());
			model = glm::scale(model, objects[i].getScale());
			m_DepthMapShader->setMat4("model", model);
		}

		objects[i].shadowMapRender();
	}

	// Plane
	m_DepthMapShader->use();
	glm::mat4 model;
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0, -5, 0));
	model = glm::scale(model, glm::vec3(25));
	m_DepthMapShader->setMat4("model", model);
	renderQuad();
}

bool CGProj::CGEditDirLight::getShadowProjection()
{
	return m_shadowProjection;
}

CGProj::CGOrthoFrustum CGProj::CGEditDirLight::getOrthoFrustum()
{
	CGOrthoFrustum f;
	f.left = m_orthoLeft;
	f.right = m_orthoRight;
	f.bottom = m_orthoBottom;
	f.top = m_orthoTop;
	f.nearP = m_shadowNearPlane;
	f.farP = m_shadowFarPlane;

	return f;
}

CGProj::CGPerFrustum CGProj::CGEditDirLight::getPerFrustum()
{
	CGPerFrustum f;
	f.fov = m_perFOV;
	f.aspect = m_perAspect;
	f.nearP = m_shadowNearPlane;
	f.farP = m_shadowFarPlane;

	return f;
}

void CGProj::CGEditDirLight::setShadowWidthHeight(unsigned w, unsigned h)
{
	m_shadowWidth = w;
	m_shadowHeight = h;

	// Set the texture setting again
	glBindTexture(GL_TEXTURE_2D, m_depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMapTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		assert(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
