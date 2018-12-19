#include <Graphics/CGEditSpotLight.h>

#include <Imgui/imgui.h>
#include <Imgui/CGimguiUtil.h>
#include <glm/gtc/type_ptr.hpp>

#include <Graphics/CGDefSecondUtil.h>

CGProj::CGEditSpotLight::CGEditSpotLight()
{
	m_lightFactors = nullptr;

	m_SpotInnerCutOff = glm::radians(12.5f);
	m_SpotOuterCutOff = glm::radians(17.5f);
}

void CGProj::CGEditSpotLight::initialize(CGAssetManager & am, CGEditLightCommonFactor * factor)
{
	m_lightFactors = factor;
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
	if (ImGui::SliderFloat("Out", &outer_Indegree, inner_Indegree, 45, "Outer Cutoff In Degree : %.2f"))
	{
		setOuterCutOffInDegree(outer_Indegree);
		spotVis.setOuterConeInRadians(glm::acos(m_SpotOuterCutOff), m_lightFactors->AttnRadius);
	}

	// Attenuation
	ImGui::PushItemWidth(50);
	if (ImGui::InputFloat("Light Radius", &m_lightFactors->AttnRadius))
	{
		spotVis.setInnerConeInRadians(glm::acos(m_SpotInnerCutOff), m_lightFactors->AttnRadius);
		spotVis.setOuterConeInRadians(glm::acos(m_SpotOuterCutOff), m_lightFactors->AttnRadius);
	}
	ImGui::SameLine();
	ShowHelpMarker("If you control the attn parameters,\nthe light radius will be set automatically.\nHowever, If you want to set the radius manually,\nyou can set it manually");
	ImGui::PopItemWidth();
	ImGui::Text("Attn Constant : %.2f", m_lightFactors->AttnConstant); ImGui::SameLine();
	ShowHelpMarker("As you know in the code,\nthe radius calculated in updateRadius() is also based on the diffuse color value");
	if (ImGui::SliderFloat("linear", &m_lightFactors->AttnLinear, 0.0001, 1.0, "Attn Linear : %f"))
	{
		spotVis.setInnerConeInRadians(glm::acos(m_SpotInnerCutOff), m_lightFactors->AttnRadius);
		spotVis.setOuterConeInRadians(glm::acos(m_SpotOuterCutOff), m_lightFactors->AttnRadius);
		updateRadius();
	}
	if (ImGui::SliderFloat("quadratic", &m_lightFactors->AttnQuadratic, 0.000001, 1.0, "Attn Quadratic : %f"))
	{
		spotVis.setInnerConeInRadians(glm::acos(m_SpotInnerCutOff), m_lightFactors->AttnRadius);
		spotVis.setOuterConeInRadians(glm::acos(m_SpotOuterCutOff), m_lightFactors->AttnRadius);
		updateRadius();
	}
}

void CGProj::CGEditSpotLight::UIrenderForShadow()
{
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

	if (m_lightFactors->isShadow && shadowIndex < NR_SPOT_SHADOWS)
	{
		// TODO: DO the spot light shadow!
	}
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
