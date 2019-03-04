#include <Graphics/CGEditPointLight.h>

#include <Imgui/imgui.h>
#include <Imgui/CGimguiUtil.h>

#include <Graphics/CGDefSecondUtil.h>
#include <Graphics/GLPrimitiveUtil.h>
#include <Graphics/CGAssetManager.h>
#include <Graphics/CGEditProxyObject.h>

CGProj::CGEditPointLight::CGEditPointLight()
{
	m_lightFactors = nullptr;
	
	m_DepthMapShader = nullptr;
	m_DebugDepthMapShader = nullptr;

	m_depthMapFBO = m_depthCubemap = 0;
	m_shadowWidth = m_shadowHeight = 1024;

	m_perFOV = glm::radians(90.f);
	m_perAspect = (float)m_shadowWidth / (float)m_shadowHeight;
	m_shadowNearPlane = 0.5f;
	m_shadowFarPlane = 10.0f;
	m_shadowBias = 0.005f;
}

void CGProj::CGEditPointLight::initialize(CGAssetManager & am, CGEditLightCommonFactor * factor)
{
	m_lightFactors = factor;

	m_DepthMapShader = am.getGeoShader(SHADER_GEO_POINT_SHADOW_MAP);
	m_InstanceDepthMapShader = am.getGeoShader(SHADER_GEO_INSTANCE_POINT_SHADOW_MAP);
	m_DebugDepthMapShader = am.getShader(SHADER_POINT_SHADOW_MAP_DEBUG_RENDER);
	// TODO : add the Instance Debug Shader

	// Shadow Map Initialization
	glGenFramebuffers(1, &m_depthMapFBO);

	glGenTextures(1, &m_depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);
	for (unsigned i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		assert(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Shadow Map Initialization
}

void CGProj::CGEditPointLight::debugDepthMapRender(const glm::mat4& view, const glm::mat4& proj)
{
	// Refer to CGPointDepthMapDebugRender Shader
	m_DebugDepthMapShader->use();
	m_DebugDepthMapShader->setMat4("view", view);
	m_DebugDepthMapShader->setMat4("projection", proj);

	glm::mat4 model(1.0);
	model = glm::translate(model, m_lightFactors->lightPosition);
	model = glm::scale(model, glm::vec3(2.0));
	m_DebugDepthMapShader->setMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);
	renderCube();
}

void CGProj::CGEditPointLight::UIrenderForCommon()
{
	ImGui::PushItemWidth(50);
	if(ImGui::InputFloat("Light Radius", &m_lightFactors->AttnRadius)) setShadowFarPlane(m_lightFactors->AttnRadius);
	ImGui::SameLine();
	ShowHelpMarker("If you control the attn parameters,\nthe light radius will be set automatically.\nHowever, If you want to set the radius manually,\nyou can set it manually");
	ImGui::PopItemWidth();
	ImGui::Text("Attn Constant : %.2f", m_lightFactors->AttnConstant); ImGui::SameLine();
	ShowHelpMarker("As you know in the code,\nthe radius calculated in updateRadius() is also based on the diffuse color value");

	if (ImGui::SliderFloat("linear", &m_lightFactors->AttnLinear, 0.0001f, 1.0f, "Attn Linear : %f"))
	{
		updateRadius();
		setShadowFarPlane(m_lightFactors->AttnRadius);
	}

	if (ImGui::SliderFloat("quadratic", &m_lightFactors->AttnQuadratic, 0.000001f, 1.0f, "Attn Quadratic : %f"))
	{
		updateRadius();
		setShadowFarPlane(m_lightFactors->AttnRadius);
	}
}

void CGProj::CGEditPointLight::UIrenderForShadow()
{
	int wharr = m_shadowWidth;
	if (ImGui::InputInt("shadow width & height", &wharr))
		setShadowWidthHeight(wharr);

	ImGui::InputFloat("Shadow Near Plane", &m_shadowNearPlane);
	ImGui::InputFloat("Shadow Far Plane", &m_shadowFarPlane);
	ImGui::InputFloat("Shadow Bias", &m_shadowBias);
}

void CGProj::CGEditPointLight::setLightPropertyOnShader(Shader * shader, 
	const std::string & sLightIndex, const std::string & sShadowIndex, const unsigned shadowIndex)
{
	shader->setVec3("pointLights[" + sLightIndex + "].Position", m_lightFactors->lightPosition);

	shader->setFloat("pointLights[" + sLightIndex + "].Constant", m_lightFactors->AttnConstant);
	shader->setFloat("pointLights[" + sLightIndex + "].Linear", m_lightFactors->AttnLinear);
	shader->setFloat("pointLights[" + sLightIndex + "].Quadratic", m_lightFactors->AttnQuadratic);
	shader->setFloat("pointLights[" + sLightIndex + "].Radius", m_lightFactors->AttnRadius);

	shader->setVec3("pointLights[" + sLightIndex + "].Ambient", m_lightFactors->lightAmbient);
	shader->setVec3("pointLights[" + sLightIndex + "].Diffuse", m_lightFactors->lightDiffuse);
	shader->setVec3("pointLights[" + sLightIndex + "].Specular", m_lightFactors->lightSpecular);
	shader->setFloat("pointLights[" + sLightIndex + "].Intensity", m_lightFactors->lightIntensity);
	shader->setInt("pointLights[" + sLightIndex + "].ShadowIndex", SHADOW_INDEX_NONE);

	if (m_lightFactors->isShadow && shadowIndex < NR_POINT_SHADOWS)
	{
		shader->setInt("pointLights[" + sLightIndex + "].ShadowIndex", shadowIndex);
		shader->setFloat("pointFarPlane[" + sShadowIndex + "]", m_shadowFarPlane);
		shader->setFloat("pointBias[" + sShadowIndex + "]", m_shadowBias);

		glActiveTexture(GL_TEXTURE0 + NR_GBUFFER_TEXTURES + NR_DIR_SHADOWS + shadowIndex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);
	}
}

void CGProj::CGEditPointLight::renderShadowMap(std::vector<CGEditProxyObject>& objects)
{
	// Light Space Setting
	m_shadowLightProjection = glm::perspective(m_perFOV, m_perAspect, m_shadowNearPlane, m_shadowFarPlane);
	m_shadowTransforms[0] = m_shadowLightProjection * glm::lookAt(m_lightFactors->lightPosition, m_lightFactors->lightPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	m_shadowTransforms[1] = m_shadowLightProjection * glm::lookAt(m_lightFactors->lightPosition, m_lightFactors->lightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	m_shadowTransforms[2] = m_shadowLightProjection * glm::lookAt(m_lightFactors->lightPosition, m_lightFactors->lightPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
	m_shadowTransforms[3] = m_shadowLightProjection * glm::lookAt(m_lightFactors->lightPosition, m_lightFactors->lightPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
	m_shadowTransforms[4] = m_shadowLightProjection * glm::lookAt(m_lightFactors->lightPosition, m_lightFactors->lightPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
	m_shadowTransforms[5] = m_shadowLightProjection * glm::lookAt(m_lightFactors->lightPosition, m_lightFactors->lightPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
	
	// Geometry Shader Setting
	// Refer to CGPointDepthMap Shader!
	m_DepthMapShader->use();
	m_DepthMapShader->setMat4("shadowTransforms[0]", m_shadowTransforms[0]);
	m_DepthMapShader->setMat4("shadowTransforms[1]", m_shadowTransforms[1]);
	m_DepthMapShader->setMat4("shadowTransforms[2]", m_shadowTransforms[2]);
	m_DepthMapShader->setMat4("shadowTransforms[3]", m_shadowTransforms[3]);
	m_DepthMapShader->setMat4("shadowTransforms[4]", m_shadowTransforms[4]);
	m_DepthMapShader->setMat4("shadowTransforms[5]", m_shadowTransforms[5]);

	// Fragment Shader Setting
	m_DepthMapShader->setVec3("lightPos", m_lightFactors->lightPosition);
	m_DepthMapShader->setFloat("far_plane", m_shadowFarPlane);

	// Instancing
	m_InstanceDepthMapShader->use();
	m_InstanceDepthMapShader->setMat4("shadowTransforms[0]", m_shadowTransforms[0]);
	m_InstanceDepthMapShader->setMat4("shadowTransforms[1]", m_shadowTransforms[1]);
	m_InstanceDepthMapShader->setMat4("shadowTransforms[2]", m_shadowTransforms[2]);
	m_InstanceDepthMapShader->setMat4("shadowTransforms[3]", m_shadowTransforms[3]);
	m_InstanceDepthMapShader->setMat4("shadowTransforms[4]", m_shadowTransforms[4]);
	m_InstanceDepthMapShader->setMat4("shadowTransforms[5]", m_shadowTransforms[5]);
	m_InstanceDepthMapShader->setVec3("lightPos", m_lightFactors->lightPosition);
	m_InstanceDepthMapShader->setFloat("far_plane", m_shadowFarPlane);

	// Vertex Shader and then Draw!
	glViewport(0, 0, m_shadowWidth, m_shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	// TODO : Dothe Frustum Culling
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
}

float CGProj::CGEditPointLight::getShadowFarPlane()
{
	return m_shadowFarPlane;
}

float CGProj::CGEditPointLight::getShadowNearPlane()
{
	return m_shadowNearPlane;
}

void CGProj::CGEditPointLight::updateRadius()
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

void CGProj::CGEditPointLight::setShadowWidthHeight(unsigned wh)
{
	m_shadowWidth = wh;
	m_shadowHeight = wh;

	// Set the texture setting again
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);
	for (unsigned i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubemap, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		assert(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CGProj::CGEditPointLight::setShadowFarPlane(float farP)
{
	m_shadowFarPlane = farP;
}
