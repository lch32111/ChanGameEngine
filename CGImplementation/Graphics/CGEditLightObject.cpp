#include <CGPrecompiled.h>
#include <Graphics/CGEditLightObject.h>

#include <Imgui/imgui.h>
#include <Imgui/CGimguiUtil.h>

#include <Graphics/CGAssetManager.h>
#include <Graphics/CGEditObject.h>
#include <Graphics/CGEditProxyObject.h>
#include <Graphics/CGFrustum.h>

// =================================================================
/*** CG Light Object  ***/
CGProj::CGEditLightObject::CGEditLightObject()
{

}

void CGProj::CGEditLightObject::initialize(CGAssetManager & am)
{
	// Basic Shader Setup 
	// Note that you should initialize the shader before
	// the edit objects get the shader uninitialized.
	setDefShader(am.getShader(SHADER_DEFERRED_SECOND));
	setForwardShader(am.getShader(SHADER_SIMPLE_COLOR_RENDER)); // for the light util rendering
	// Basic Shader Setup

	// Light Common Factors Init
	setLightType(EDIT_POINT_LIGHT);
	setPosition(glm::vec3(0));
	setLightDirection(glm::vec3(0, 1, 0));
	setAmbientColor(glm::vec3(0.1f));
	setDiffuseColor(glm::vec3(0.2f));
	setSpecularColor(glm::vec3(1.0f));
	m_CommonlightFactors.lightIntensity = 1.f;
	m_CommonlightFactors.isRangeRender = false;
	m_CommonlightFactors.AttnConstant = 1.f;
	m_CommonlightFactors.AttnLinear = 0.7f;
	m_CommonlightFactors.AttnQuadratic = 1.f;
	updateRadius();

	m_CommonlightFactors.isShadow = false;
	m_CommonlightFactors.isShadowMapRender = false;
	m_CommonlightFactors.isShadowFrustumRender = false;
	// Light Common Factors Init

	// Directional Light Init
	m_dirLight.initialize(am, &m_CommonlightFactors);

	m_dirVis.prepareData(am.getShader(SHADER_DIR_VISUALIZER));
	m_dirVis.setCylinderDimension(5.f, 0.1f, 0.1f);
	m_dirVis.setConeDimension(1.5, 0, 0.5);

	m_dirShadowVis.setShader(am.getShader(SHADER_CG_LINE));
	// Directional Light Init

	// Point Light Init
	m_pointLight.initialize(am, &m_CommonlightFactors);

	m_pointVis.prepareData(am.getShader(SHADER_POINT_VISUALIZER));

	m_pointShadowVis.setShader(am.getShader(SHADER_SIMPLE_COLOR_RENDER));
	// Point Light Init

	// Spot Light Init
	m_spotLight.initialize(am, &m_CommonlightFactors);

	m_spotVis.prepareData(am.getShader(SHADER_SPOT_VISUALIZER));
	m_spotVis.setOuterConeInRadians(glm::acos(m_spotLight.getOuterCutOff()), m_CommonlightFactors.AttnRadius);
	m_spotVis.setInnerConeInRadians(glm::acos(m_spotLight.getInnerCutOff()), m_CommonlightFactors.AttnRadius);

	m_spotShadowVis.setShader(am.getShader(SHADER_CG_LINE));
	// Spot Light Init
}

void CGProj::CGEditLightObject::setForwardShader(Shader * shader)
{
	m_forwardShader = shader;
}

void CGProj::CGEditLightObject::forwardRender(const glm::mat4 & view, const glm::mat4 & proj)
{
	// Refer to the simpleColorRender Shader!
	// Edit Object Render
	glm::mat4 model(1.0);
	model = glm::translate(model, m_CommonlightFactors.lightPosition);
	model = glm::scale(model, this->getScale());

	m_forwardShader->use();
	m_forwardShader->setMat4("projection", proj);
	m_forwardShader->setMat4("view", view);
	m_forwardShader->setMat4("model", model);
	m_forwardShader->setVec3("Color", m_CommonlightFactors.lightDiffuse);
	renderPrimitive();
	// Edit Object Render

	// Light Range Render
	if (m_CommonlightFactors.isRangeRender)
	{
		switch (m_LightType)
		{
		case EDIT_DIRECTION_LIGHT:
			m_dirVis.render(view, proj, m_CommonlightFactors.lightPosition, m_CommonlightFactors.lightDirection);
			break;
		case EDIT_POINT_LIGHT:
			m_pointVis.render(view, proj, m_CommonlightFactors.lightPosition, m_CommonlightFactors.AttnRadius);
			break;
		case EDIT_SPOT_LIGHT:
			m_spotVis.render(view, proj, m_CommonlightFactors.lightPosition, m_CommonlightFactors.lightDirection);
			break;
		}
	}
	// Light Range Render

	// Shadow Frustum Render
	if (m_CommonlightFactors.isShadowFrustumRender)
	{
		switch (m_LightType)
		{
		case EDIT_DIRECTION_LIGHT:
		{
			if (m_dirLight.getShadowProjection())
			{
				// Perspective Frustum
				CGPerFrustum f = m_dirLight.getPerFrustum();

				m_dirShadowVis.render
				(
					view, proj,
					m_CommonlightFactors.lightPosition, m_CommonlightFactors.lightDirection,
					f.fov, f.aspect,
					f.nearP, f.farP
				);
			}
			else
			{
				// Orthographic Frustum
				CGOrthoFrustum f = m_dirLight.getOrthoFrustum();

				m_dirShadowVis.render
				(
					view, proj,
					m_CommonlightFactors.lightPosition, m_CommonlightFactors.lightDirection,
					f.left, f.right,
					f.bottom, f.top,
					f.nearP, f.farP
				);
			}
			break;
		}
		case EDIT_POINT_LIGHT:
			m_pointShadowVis.render(view, proj,
				m_CommonlightFactors.lightPosition,
				m_pointLight.getShadowFarPlane(), m_pointLight.getShadowNearPlane());
			break;
		case EDIT_SPOT_LIGHT:
			CGPerFrustum f = m_spotLight.getPerFrustum();

			m_spotShadowVis.render(view, proj,
				m_CommonlightFactors.lightPosition, m_CommonlightFactors.lightDirection,
				f.fov, f.aspect, f.nearP, f.farP);
			break;
		}
	}
	// Shadow Frustum Render

	if (m_CommonlightFactors.isShadowMapRender)
	{
		switch (m_LightType)
		{
		case EDIT_DIRECTION_LIGHT:
			m_dirLight.debugDepthMapRender(view, proj);
			break;
		case EDIT_POINT_LIGHT:
			m_pointLight.debugDepthMapRender(view, proj);
			break;
		case EDIT_SPOT_LIGHT:
			m_spotLight.debugDepthMapRender(view, proj);
			break;
		}
	}
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

	ImGui::Checkbox("Range Redner", &m_CommonlightFactors.isRangeRender);

	switch (m_LightType)
	{
	case EDIT_DIRECTION_LIGHT:
		m_dirLight.UIrenderForCommon();
		break;
	case EDIT_POINT_LIGHT:
		m_pointLight.UIrenderForCommon();
		break;
	case EDIT_SPOT_LIGHT:
		m_spotLight.UIrenderForCommon(m_spotVis);
		break;
	}

	// Light Color
	float temp[3] = { m_CommonlightFactors.lightAmbient.x,m_CommonlightFactors.lightAmbient.y, m_CommonlightFactors.lightAmbient.z };
	ImGui::ColorEdit3("Ambient", temp);
	m_CommonlightFactors.lightAmbient = { temp[0], temp[1], temp[2] };

	temp[0] = m_CommonlightFactors.lightDiffuse.x, temp[1] = m_CommonlightFactors.lightDiffuse.y, temp[2] = m_CommonlightFactors.lightDiffuse.z;
	ImGui::ColorEdit3("Diffuse", temp);
	m_CommonlightFactors.lightDiffuse = { temp[0], temp[1], temp[2] };

	temp[0] = m_CommonlightFactors.lightSpecular.x, temp[1] = m_CommonlightFactors.lightSpecular.y, temp[2] = m_CommonlightFactors.lightSpecular.z;
	ImGui::ColorEdit3("specular", temp);
	m_CommonlightFactors.lightSpecular = { temp[0], temp[1], temp[2] };

	ImGui::InputFloat("Light Intensity", &m_CommonlightFactors.lightIntensity);

	// Shadow
	ImGui::Checkbox("Shadow Cast", &m_CommonlightFactors.isShadow);
	if (m_CommonlightFactors.isShadow)
	{
		ImGui::Checkbox("ShadowMap Debug Render", &m_CommonlightFactors.isShadowMapRender);
		ImGui::Checkbox("Shadow Frustum Render", &m_CommonlightFactors.isShadowFrustumRender);
		ImGui::SameLine(); ShowHelpMarker("Shadow Frustum actually does not fit the actual frustum\nbecause the bias in shader is adjusting!");

		switch (m_LightType)
		{
		case EDIT_DIRECTION_LIGHT:
			m_dirLight.UIrenderForShadow();
			break;
		case EDIT_POINT_LIGHT:
			m_pointLight.UIrenderForShadow();
			break;
		case EDIT_SPOT_LIGHT:
			m_spotLight.UIrenderForShadow();
			break;
		}
	}

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

	m_CommonlightFactors.lightPosition = p;
}

void CGProj::CGEditLightObject::setPosition(const GPED::real x, const GPED::real y, const GPED::real z)
{
	CGEditObject::setPosition(x, y, z);

	m_CommonlightFactors.lightPosition.x = x;
	m_CommonlightFactors.lightPosition.y = y;
	m_CommonlightFactors.lightPosition.z = z;
}

void CGProj::CGEditLightObject::setXposition(const GPED::real x)
{
	CGEditObject::setXposition(x);
	m_CommonlightFactors.lightPosition.x = x;
}

void CGProj::CGEditLightObject::setYposition(const GPED::real y)
{
	CGEditObject::setYposition(y);
	m_CommonlightFactors.lightPosition.y = y;
}

void CGProj::CGEditLightObject::setZposition(const GPED::real z)
{
	CGEditObject::setZposition(z);
	m_CommonlightFactors.lightPosition.z = z;
}

glm::vec3 CGProj::CGEditLightObject::getPosition()
{
	// You can get the edit object position 
	// However I will return the light position for the Obect-Oriented Programming(OOP)
	return m_CommonlightFactors.lightPosition;
}

void CGProj::CGEditLightObject::setLightDirection(const glm::vec3 & d)
{
	// Notice the direction should be normalized
	m_CommonlightFactors.lightDirection = glm::normalize(d);
}

void CGProj::CGEditLightObject::setLightDirection(const GPED::real x, const GPED::real y, const GPED::real z)
{
	m_CommonlightFactors.lightDirection = glm::normalize(glm::vec3(x, y, z));
}

void CGProj::CGEditLightObject::setLightXdirection(const GPED::real x)
{
	m_CommonlightFactors.lightDirection.x = x;
	m_CommonlightFactors.lightDirection = glm::normalize(m_CommonlightFactors.lightDirection);
}

void CGProj::CGEditLightObject::setLightYdirection(const GPED::real y)
{
	m_CommonlightFactors.lightDirection.y = y;
	m_CommonlightFactors.lightDirection = glm::normalize(m_CommonlightFactors.lightDirection);
}

void CGProj::CGEditLightObject::setLightZdirection(const GPED::real z)
{
	m_CommonlightFactors.lightDirection.z = z;
	m_CommonlightFactors.lightDirection = glm::normalize(m_CommonlightFactors.lightDirection);
}

glm::vec3 CGProj::CGEditLightObject::getLightDirection()
{
	return m_CommonlightFactors.lightDirection;
}

void CGProj::CGEditLightObject::setAmbientColor(const glm::vec3 & ac)
{
	m_CommonlightFactors.lightAmbient = ac;
}

void CGProj::CGEditLightObject::setAmbientColor(const GPED::real r, const GPED::real g, const GPED::real b)
{
	m_CommonlightFactors.lightAmbient.r = r;
	m_CommonlightFactors.lightAmbient.g = g;
	m_CommonlightFactors.lightAmbient.b = b;
}

void CGProj::CGEditLightObject::setAmbientRedColor(const GPED::real r)
{
	m_CommonlightFactors.lightAmbient.r = r;
}

void CGProj::CGEditLightObject::setAmbientGreenColor(const GPED::real g)
{
	m_CommonlightFactors.lightAmbient.g = g;
}

void CGProj::CGEditLightObject::setAmbientBlueColor(const GPED::real b)
{
	m_CommonlightFactors.lightAmbient.b = b;
}

glm::vec3 CGProj::CGEditLightObject::getAmbientColor()
{
	return m_CommonlightFactors.lightAmbient;
}

void CGProj::CGEditLightObject::setDiffuseColor(const glm::vec3 & dc)
{
	m_CommonlightFactors.lightDiffuse = dc;
}

void CGProj::CGEditLightObject::setDiffuseColor(const GPED::real r, const GPED::real g, const GPED::real b)
{
	m_CommonlightFactors.lightDiffuse.r = r;
	m_CommonlightFactors.lightDiffuse.g = g;
	m_CommonlightFactors.lightDiffuse.b = b;
}

void CGProj::CGEditLightObject::setDiffuseRedColor(const GPED::real r)
{
	m_CommonlightFactors.lightDiffuse.r = r;
}

void CGProj::CGEditLightObject::setDiffuseGreenColor(const GPED::real g)
{
	m_CommonlightFactors.lightDiffuse.g = g;
}

void CGProj::CGEditLightObject::setDiffuseBlueColor(const GPED::real b)
{
	m_CommonlightFactors.lightDiffuse.b = b;
}

glm::vec3 CGProj::CGEditLightObject::getDiffuseColor()
{
	return m_CommonlightFactors.lightDiffuse;
}

void CGProj::CGEditLightObject::setSpecularColor(const glm::vec3 & sc)
{
	m_CommonlightFactors.lightSpecular = sc;
}

void CGProj::CGEditLightObject::setSpecularColor(const GPED::real r, const GPED::real g, const GPED::real b)
{
	m_CommonlightFactors.lightSpecular.r = r;
	m_CommonlightFactors.lightSpecular.g = g;
	m_CommonlightFactors.lightSpecular.b = b;
}

void CGProj::CGEditLightObject::setSpecularRedColor(const GPED::real r)
{
	m_CommonlightFactors.lightSpecular.r = r;
}

void CGProj::CGEditLightObject::setSpecularGreenColor(const GPED::real g)
{
	m_CommonlightFactors.lightSpecular.g = g;
}

void CGProj::CGEditLightObject::setSpecularBlueColor(const GPED::real b)
{
	m_CommonlightFactors.lightSpecular.b = b;
}

glm::vec3 CGProj::CGEditLightObject::getSpecularColor()
{
	return m_CommonlightFactors.lightSpecular;
}

void CGProj::CGEditLightObject::setAttnConstant(const float c)
{
	m_CommonlightFactors.AttnConstant = c;
	updateRadius();
}

float CGProj::CGEditLightObject::getAttnConsant()
{
	return m_CommonlightFactors.AttnConstant;
}

void CGProj::CGEditLightObject::setAttnLinear(const float l)
{
	m_CommonlightFactors.AttnLinear = l;
	updateRadius();
}

float CGProj::CGEditLightObject::getAttnLinear()
{
	return m_CommonlightFactors.AttnLinear;
}

void CGProj::CGEditLightObject::setAttnQuadratic(const float q)
{
	m_CommonlightFactors.AttnQuadratic = q;
	updateRadius();
}

float CGProj::CGEditLightObject::getAttnQuadratic()
{
	return m_CommonlightFactors.AttnQuadratic;
}

void CGProj::CGEditLightObject::setLightIntensity(const float i)
{
	m_CommonlightFactors.lightIntensity = i;
}

float CGProj::CGEditLightObject::getLightRadius()
{
	return m_CommonlightFactors.AttnRadius;
}

void CGProj::CGEditLightObject::updateRadius()
{
	// the diffuse color of light is used for the max component of light
	// You need to keep in mind that the radius will be bigger if the m_CommonlightFactors.lightDiffuse will be bigger
	float lightMin = 1 / (1.f / 256.f);
	float lightMax = std::fmaxf(std::fmaxf(m_CommonlightFactors.lightDiffuse.r, m_CommonlightFactors.lightDiffuse.g), m_CommonlightFactors.lightDiffuse.b);

	// Calculate the radius of light volume
	m_CommonlightFactors.AttnRadius =
		(
			-m_CommonlightFactors.AttnLinear +
			std::sqrtf(
				m_CommonlightFactors.AttnLinear * m_CommonlightFactors.AttnLinear - 4 * m_CommonlightFactors.AttnQuadratic *
				(m_CommonlightFactors.AttnConstant - lightMax * lightMin))
			)
		/
		(2 * m_CommonlightFactors.AttnQuadratic);
}

void CGProj::CGEditLightObject::setLightPropertyOnShader(unsigned lightIndex, unsigned shadowIndex, const glm::vec3& cameraPos)
{
	// TODO : STL string do the dynamic allocation. Replace it with c-style string for performance.
	std::string sLightIndex = std::to_string(lightIndex);
	std::string sShadowIndex = std::to_string(shadowIndex);

	m_DefShader->use();
	m_DefShader->setVec3("cameraPos", cameraPos);
	switch (m_LightType)
	{
	case EDIT_DIRECTION_LIGHT:
	{
		m_dirLight.setLightPropertyOnShader(m_DefShader,
			sLightIndex, sShadowIndex, shadowIndex);
		break;
	}
	case EDIT_POINT_LIGHT:
	{
		m_pointLight.setLightPropertyOnShader(m_DefShader,
			sLightIndex, sShadowIndex, shadowIndex);
		break;
	}
	case EDIT_SPOT_LIGHT:
	{
		m_spotLight.setLightPropertyOnShader(m_DefShader,
			sLightIndex, sShadowIndex, shadowIndex);
		break;
	}
	default:
		assert(0);
	}
}

void CGProj::CGEditLightObject::setIsShadowRender(bool shadow)
{
	m_CommonlightFactors.isShadow = shadow;
}

bool CGProj::CGEditLightObject::getIsShadowRender()
{
	return m_CommonlightFactors.isShadow;
}

void CGProj::CGEditLightObject::renderShadowMap(std::vector<CGEditProxyObject>& objects)
{
	// glCullFace(GL_FRONT);
	switch (m_LightType)
	{
	case EDIT_DIRECTION_LIGHT:
		m_dirLight.renderShadowMap(objects);
		break;
	case EDIT_POINT_LIGHT:
		m_pointLight.renderShadowMap(objects);
		break;
	case EDIT_SPOT_LIGHT:
		m_spotLight.renderShadowMap(objects);
		break;
	}
	// glCullFace(GL_BACK);
}

/*** CG Light Object  ***/
// =================================================================