#include <Graphics/CGEditPointLight.h>

#include <Imgui/imgui.h>
#include <Imgui/CGimguiUtil.h>

#include <Graphics/CGDefSecondUtil.h>

CGProj::CGEditPointLight::CGEditPointLight()
{
	m_lightFactors = nullptr;
	
	m_depthMapFBO = m_depthCubemap = 0;
	m_shadowWidth = m_shadowHeight = 1024;
}

void CGProj::CGEditPointLight::initialize(CGAssetManager & am, CGEditLightCommonFactor * factor)
{
	m_lightFactors = factor;

	// Shadow Map Initialization
	glGenFramebuffers(1, &m_depthMapFBO);

	glGenTextures(1, &m_depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);
	for (unsigned i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		assert(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Shadow Map Initialization
}

void CGProj::CGEditPointLight::UIrenderForCommon()
{
	ImGui::PushItemWidth(50);
	ImGui::InputFloat("Light Radius", &m_lightFactors->AttnRadius); ImGui::SameLine();
	ShowHelpMarker("If you control the attn parameters,\nthe light radius will be set automatically.\nHowever, If you want to set the radius manually,\nyou can set it manually");
	ImGui::PopItemWidth();
	ImGui::Text("Attn Constant : %.2f", m_lightFactors->AttnConstant); ImGui::SameLine();
	ShowHelpMarker("As you know in the code,\nthe radius calculated in updateRadius() is also based on the diffuse color value");
	if (ImGui::SliderFloat("linear", &m_lightFactors->AttnLinear, 0.0001, 1.0, "Attn Linear : %f")) updateRadius();
	if (ImGui::SliderFloat("quadratic", &m_lightFactors->AttnQuadratic, 0.000001, 1.0, "Attn Quadratic : %f")) updateRadius();
}

void CGProj::CGEditPointLight::UIrenderForShadow()
{
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

	if (m_lightFactors->isShadow && shadowIndex < NR_POINT_SHADOWS)
	{
		// TODO : Do the point light shadow!
	}
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
