#include <CGPrecompiled.h>
#include <Graphics/CGEditSpotLight.h>

#include <Imgui/imgui.h>
#include <Imgui/CGimguiUtil.h>
#include <glm/gtc/type_ptr.hpp>

#include <Graphics/CGDefSecondUtil.h>
#include <Graphics/GLPrimitiveUtil.h>
#include <Graphics/CGAssetManager.h>
#include <Graphics/CGEditProxyObject.h>
#include <Graphics/CGEditSpotLightVisualizer.h>
#include <Graphics/CGFrustum.h>

#include <GPED/CGPhysicsUtil.h>

CGProj::CGEditSpotLight::CGEditSpotLight()
{
	m_lightFactors = nullptr;

	m_SpotInnerCutOff = glm::cos(glm::radians(12.5f));
	m_SpotOuterCutOff = glm::cos(glm::radians(17.5f));

	m_DepthMapShader = nullptr;
	m_DebugDepthMapShader = nullptr;

	m_depthMapFBO = m_depthMapTexture = 0;
	m_shadowWidth = m_shadowHeight = 1024;
	m_shadowBias = 0.005f;

	m_perFOV = glm::radians(17.5f * 2); // same as the degreeof spot outer cutoff
	m_perAspect = (float)m_shadowWidth / (float)m_shadowHeight;

	m_shadowNearPlane = 1.0;
	m_shadowFarPlane = 7.5;

	m_shadowLightView = glm::mat4(1.0);
	m_shadowLightProjection = glm::mat4(1.0);
	m_shadowLightSpaceMatrix = glm::mat4(1.0);
}

void CGProj::CGEditSpotLight::initialize(CGAssetManager & am, CGEditLightCommonFactor * factor)
{
	m_lightFactors = factor;
	setShadowFarPlane(m_lightFactors->AttnRadius);

	// Shadow Map initialization
	m_DepthMapShader = am.getShader(SHADER_SPOT_SHADOW_MAP);
	m_InstanceDepthMapShader = am.getShader(SHADER_INSTANCE_SPOT_SHADOW_MAP);
	m_DebugDepthMapShader = am.getShader(SHADER_SPOT_SHADOW_MAP_DEBUG_RENDER);
	// TODO : add the instance debug shadow map shader

	glGenFramebuffers(1, &m_depthMapFBO);

	glGenTextures(1, &m_depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		assert(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CGProj::CGEditSpotLight::debugDepthMapRender(const glm::mat4 & view, const glm::mat4 & proj)
{
	m_DebugDepthMapShader->use();
	m_DebugDepthMapShader->setMat4("view", view);
	m_DebugDepthMapShader->setMat4("projection", proj);
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

void CGProj::CGEditSpotLight::UIrenderForCommon(CGEditSpotLightVisualizer& spotVis)
{
	// Light Direction
	float t_dir[3] = { m_lightFactors->lightDirection.x, m_lightFactors->lightDirection.y, m_lightFactors->lightDirection.z };
	ImGui::InputFloat3("Light Direction", t_dir, "%.2f");
	m_lightFactors->lightDirection.x = t_dir[0], m_lightFactors->lightDirection.y = t_dir[1], m_lightFactors->lightDirection.z = t_dir[2];
	m_lightFactors->lightDirection = glm::normalize(m_lightFactors->lightDirection);

	// Cut off of Spot Light
	float inner_Indegree = glm::acos(m_SpotInnerCutOff) * 180.f / glm::pi<float>();
	float outer_Indegree = glm::acos(m_SpotOuterCutOff) * 180.f / glm::pi<float>();
	if (ImGui::SliderFloat("In", &inner_Indegree, 0, outer_Indegree, "Inner Cutoff In Degree :  %.2f"))
	{
		setInnerCutOffInDegree(inner_Indegree);
		spotVis.setInnerConeInRadians(glm::acos(m_SpotInnerCutOff), m_lightFactors->AttnRadius);

	}
	if (ImGui::SliderFloat("Out", &outer_Indegree, inner_Indegree, 90, "Outer Cutoff In Degree : %.2f"))
	{
		setOuterCutOffInDegree(outer_Indegree);
		m_perFOV = glm::radians(outer_Indegree * 2);
		spotVis.setOuterConeInRadians(glm::acos(m_SpotOuterCutOff), m_lightFactors->AttnRadius);
	}

	// Attenuation
	ImGui::PushItemWidth(50);
	if (ImGui::InputFloat("Light Radius", &m_lightFactors->AttnRadius))
	{
		spotVis.setInnerConeInRadians(glm::acos(m_SpotInnerCutOff), m_lightFactors->AttnRadius);
		spotVis.setOuterConeInRadians(glm::acos(m_SpotOuterCutOff), m_lightFactors->AttnRadius);
		setShadowFarPlane(m_lightFactors->AttnRadius);
	}
	ImGui::SameLine();
	ShowHelpMarker("If you control the attn parameters,\nthe light radius will be set automatically.\nHowever, If you want to set the radius manually,\nyou can set it manually");
	ImGui::PopItemWidth();
	ImGui::Text("Attn Constant : %.2f", m_lightFactors->AttnConstant); ImGui::SameLine();
	ShowHelpMarker("As you know in the code,\nthe radius calculated in updateRadius() is also based on the diffuse color value");
	if (ImGui::SliderFloat("linear", &m_lightFactors->AttnLinear, 0.0001f, 1.0f, "Attn Linear : %f"))
	{
		spotVis.setInnerConeInRadians(glm::acos(m_SpotInnerCutOff), m_lightFactors->AttnRadius);
		spotVis.setOuterConeInRadians(glm::acos(m_SpotOuterCutOff), m_lightFactors->AttnRadius);
		updateRadius();
		setShadowFarPlane(m_lightFactors->AttnRadius);
	}
	if (ImGui::SliderFloat("quadratic", &m_lightFactors->AttnQuadratic, 0.000001f, 1.0f, "Attn Quadratic : %f"))
	{
		spotVis.setInnerConeInRadians(glm::acos(m_SpotInnerCutOff), m_lightFactors->AttnRadius);
		spotVis.setOuterConeInRadians(glm::acos(m_SpotOuterCutOff), m_lightFactors->AttnRadius);
		updateRadius();
		setShadowFarPlane(m_lightFactors->AttnRadius);
	}
}

void CGProj::CGEditSpotLight::UIrenderForShadow()
{
	int wharr = m_shadowWidth;
	if (ImGui::InputInt("shadow width & height", &wharr))
		setShadowWidthHeight(wharr);

	ImGui::InputFloat("shadow near plane", &m_shadowNearPlane);
	ImGui::InputFloat("shadow far plane", &m_shadowFarPlane);
	ImGui::InputFloat("shadow bias", &m_shadowBias);
}

void CGProj::CGEditSpotLight::setLightPropertyOnShader(Shader * shader, const std::string & sLightIndex, const std::string & sShadowIndex, const unsigned shadowIndex)
{
	shader->setVec3("spotLights[" + sLightIndex + "].Position", m_lightFactors->lightPosition);
	shader->setVec3("spotLights[" + sLightIndex + "].Direction", m_lightFactors->lightDirection);

	shader->setFloat("spotLights[" + sLightIndex + "].Inner_CutOff", m_SpotInnerCutOff);
	shader->setFloat("spotLights[" + sLightIndex + "].Outer_CutOff", m_SpotOuterCutOff);

	shader->setFloat("spotLights[" + sLightIndex + "].Constant", m_lightFactors->AttnConstant);
	shader->setFloat("spotLights[" + sLightIndex + "].Linear", m_lightFactors->AttnLinear);
	shader->setFloat("spotLights[" + sLightIndex + "].Quadratic", m_lightFactors->AttnQuadratic);
	shader->setFloat("spotLights[" + sLightIndex + "].Radius", m_lightFactors->AttnRadius);

	shader->setVec3("spotLights[" + sLightIndex + "].Ambient", m_lightFactors->lightAmbient);
	shader->setVec3("spotLights[" + sLightIndex + "].Diffuse", m_lightFactors->lightDiffuse);
	shader->setVec3("spotLights[" + sLightIndex + "].Specular", m_lightFactors->lightSpecular);
	shader->setFloat("spotLights[" + sLightIndex + "].Intensity", m_lightFactors->lightIntensity);
	shader->setInt("spotLights[" + sLightIndex + "].ShadowIndex", SHADOW_INDEX_NONE);

	if (m_lightFactors->isShadow && shadowIndex < NR_SPOT_SHADOWS)
	{
		shader->setInt("spotLights[" + sLightIndex + "].ShadowIndex", shadowIndex);
		shader->setMat4("spotLightSpace[" + sShadowIndex + "]", m_shadowLightSpaceMatrix);
		shader->setFloat("spotBias[" + sShadowIndex + "]", m_shadowBias);

		glActiveTexture(GL_TEXTURE0 + NR_GBUFFER_TEXTURES + NR_DIR_SHADOWS + NR_POINT_SHADOWS + shadowIndex);
		glBindTexture(GL_TEXTURE_2D, m_depthMapTexture);
	}
}

void CGProj::CGEditSpotLight::renderShadowMap(std::vector<CGEditProxyObject>& objects)
{
	// Light Space Setting
	m_shadowLightView = safeLookAt(
		m_lightFactors->lightPosition,
		m_lightFactors->lightPosition + m_lightFactors->lightDirection,
		glm::vec3(0, 1, 0)
	);
	m_shadowLightProjection = glm::perspective(m_perFOV, m_perAspect, m_shadowNearPlane, m_shadowFarPlane);
	m_shadowLightSpaceMatrix = m_shadowLightProjection * m_shadowLightView;
	
	m_DepthMapShader->use();
	m_DepthMapShader->setMat4("lightSpaceMatrix", m_shadowLightSpaceMatrix);
	m_InstanceDepthMapShader->use();
	m_InstanceDepthMapShader->setMat4("lightSpaceMatrix", m_shadowLightSpaceMatrix);
	// Light Space Setting

	// Shadow Mapping Pass
	glViewport(0, 0, m_shadowWidth, m_shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::mat4 model;
	for (unsigned i = 0; i < objects.size(); ++i)
	{
		m_InstanceDepthMapShader->use();
		objects[i].shadowMapRender();
	}

	// Plane
	m_DepthMapShader->use();
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0, -5, 0));
	model = glm::scale(model, glm::vec3(25));
	m_DepthMapShader->setMat4("model", model);
	renderQuad();
	// Shadow Mapping Pass
}

void CGProj::CGEditSpotLight::setInnerCutOffInDegree(const float degree)
{
	// Notice the measure of angle
	m_SpotInnerCutOff = glm::cos(glm::radians(degree));
}

void CGProj::CGEditSpotLight::setInnerCutoffInRadian(const float radian)
{
	m_SpotInnerCutOff = glm::cos(radian);
}

float CGProj::CGEditSpotLight::getInnerCutOff()
{
	return m_SpotInnerCutOff;
}

void CGProj::CGEditSpotLight::setOuterCutOffInDegree(const float degree)
{
	m_SpotOuterCutOff = glm::cos(glm::radians(degree));
}

void CGProj::CGEditSpotLight::setOuterCutOffInRadian(const float radian)
{
	m_SpotOuterCutOff = glm::cos(radian);
}

float CGProj::CGEditSpotLight::getOuterCutOff()
{
	return m_SpotOuterCutOff;
}

CGProj::CGPerFrustum CGProj::CGEditSpotLight::getPerFrustum()
{
	CGPerFrustum f;
	f.fov = m_perFOV;
	f.aspect = m_perAspect;
	f.nearP = m_shadowNearPlane;
	f.farP = m_shadowFarPlane;

	return f;
}

void CGProj::CGEditSpotLight::updateRadius()
{
	// the diffuse color of light is used for the max component of light
	// You need to keep in mind that the radius will be bigger if the m_lightFactors.lightDiffuse will be bigger
	float lightMin = 1 / (1.f / 256.f);
	float lightMax = std::fmaxf(std::fmaxf(m_lightFactors->lightDiffuse.r, m_lightFactors->lightDiffuse.g), m_lightFactors->lightDiffuse.b);

	// Calculate the radius of light volume
	m_lightFactors->AttnRadius =
		(
			-m_lightFactors->AttnLinear +
			std::sqrtf(
				m_lightFactors->AttnLinear * m_lightFactors->AttnLinear - 4 * m_lightFactors->AttnQuadratic *
				(m_lightFactors->AttnConstant - lightMax * lightMin))
			)
		/
		(2 * m_lightFactors->AttnQuadratic);
}

void CGProj::CGEditSpotLight::setShadowWidthHeight(unsigned wh)
{
	m_shadowWidth = wh;
	m_shadowHeight = wh;
	m_perAspect = (float)m_shadowWidth / (float)m_shadowHeight;

	// Set the texture setting again
	glBindTexture(GL_TEXTURE_2D, m_depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMapTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		assert(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CGProj::CGEditSpotLight::setShadowFarPlane(float farP)
{
	m_shadowFarPlane = farP;
}
