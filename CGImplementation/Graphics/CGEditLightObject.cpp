#include <Graphics/CGEditLightObject.h>

#include <Imgui/imgui.h>
#include <Imgui/CGimguiUtil.h>

// =================================================================
/*** CG Light Object  ***/

CGProj::CGEditLightObject::CGEditLightObject()
{

}

CGProj::CGEditLightObject::CGEditLightObject(CGAssetManager& am)
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
	setAmbientColor(glm::vec3(0.1));
	setDiffuseColor(glm::vec3(0.2));
	setSpecularColor(glm::vec3(1.0));

	m_lightFactors.isRangeRender = false;
	m_lightFactors.AttnConstant = 1.0;
	m_lightFactors.AttnLinear = 0.7;
	m_lightFactors.AttnQuadratic = 1.0;
	updateRadius();

	m_lightFactors.isShadow = false;
	m_lightFactors.isShadowMapRender = false;
	m_lightFactors.isShadowFrustumRender = false;
	// Light Common Factors Init

	// Directional Light Init
	m_dirLight.initialize(am, &m_lightFactors);

	m_dirVis.prepareData(am.getShader(SHADER_DIR_VISUALIZER));
	m_dirVis.setCylinderDimension(5, 0.1, 0.1);
	m_dirVis.setConeDimension(1.5, 0, 0.5);

	m_dirShadowVis.setShader(am.getShader(SHADER_CG_LINE));
	// Directional Light Init

	// Point Light Init
	m_pointLight.initialize(am, &m_lightFactors);

	m_pointVis.prepareData(am.getShader(SHADER_POINT_VISUALIZER));

	// Point Light Init

	// Spot Light Init
	m_spotLight.initialize(am, &m_lightFactors);

	m_spotVis.prepareData(am.getShader(SHADER_SPOT_VISUALIZER));
	m_spotVis.setOuterConeInRadians(glm::acos(m_spotLight.getOuterCutOff()), m_lightFactors.AttnRadius);
	m_spotVis.setInnerConeInRadians(glm::acos(m_spotLight.getInnerCutOff()), m_lightFactors.AttnRadius);
	// Spot Light Init
}

void CGProj::CGEditLightObject::setForwardShader(Shader * shader)
{
	m_forwardShader = shader;
}

void CGProj::CGEditLightObject::forwardRender(const glm::mat4 & view, const glm::mat4 & proj)
{
	if (m_lightFactors.isShadowMapRender)
	{
		switch (m_LightType)
		{
		case EDIT_DIRECTION_LIGHT:
		{
			m_dirLight.debugDepthMapRender();
			break;
		}
		case EDIT_POINT_LIGHT:
			break;
		case EDIT_SPOT_LIGHT:
			break;
		}

		return;
	}

	// Refer to the simpleColorRender Shader!
	// Edit Object Render
	glm::mat4 model(1.0);
	model = glm::translate(model, m_lightFactors.lightPosition);
	model = glm::scale(model, this->getScale());

	m_forwardShader->use();
	m_forwardShader->setMat4("projection", proj);
	m_forwardShader->setMat4("view", view);
	m_forwardShader->setMat4("model", model);
	m_forwardShader->setVec3("Color", m_lightFactors.lightDiffuse);
	renderPrimitive();
	// Edit Object Render

	// Light Range Render
	if (m_lightFactors.isRangeRender)
	{
		switch (m_LightType)
		{
		case EDIT_DIRECTION_LIGHT:
			m_dirVis.render(view, proj, m_lightFactors.lightPosition, m_lightFactors.lightDirection);
			break;
		case EDIT_POINT_LIGHT:
			m_pointVis.render(view, proj, m_lightFactors.lightPosition, m_lightFactors.AttnRadius);
			break;
		case EDIT_SPOT_LIGHT:
			m_spotVis.render(view, proj, m_lightFactors.lightPosition, m_lightFactors.lightDirection);
			break;
		}
	}
	// Light Range Render

	// Shadow Frustum Render
	if (m_lightFactors.isShadowFrustumRender)
	{
		switch (m_LightType)
		{
		case EDIT_DIRECTION_LIGHT:
		{
			if (m_dirLight.m_shadowProjection)
				m_dirShadowVis.render(view, proj,
					m_lightFactors.lightPosition, m_lightFactors.lightDirection,
					m_dirLight.m_perFOV, m_dirLight.m_perAspect,
					m_dirLight.m_shadowNearPlane, m_dirLight.m_shadowFarPlane);
			else
				m_dirShadowVis.render(view, proj,
					m_lightFactors.lightPosition, m_lightFactors.lightDirection,
					m_dirLight.m_orthoLeft, m_dirLight.m_orthoRight,
					m_dirLight.m_orthoBottom, m_dirLight.m_orthoTop,
					m_dirLight.m_shadowNearPlane, m_dirLight.m_shadowFarPlane);
			break;
		}
		case EDIT_POINT_LIGHT:
			break;
		case EDIT_SPOT_LIGHT:
			break;
		}
	}
	// Shadow Frustum Render
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

	ImGui::Checkbox("Range Redner", &m_lightFactors.isRangeRender);

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
	float temp[3] = { m_lightFactors.lightAmbient.x,m_lightFactors.lightAmbient.y, m_lightFactors.lightAmbient.z };
	ImGui::ColorEdit3("Ambient", temp);
	m_lightFactors.lightAmbient = { temp[0], temp[1], temp[2] };

	temp[0] = m_lightFactors.lightDiffuse.x, temp[1] = m_lightFactors.lightDiffuse.y, temp[2] = m_lightFactors.lightDiffuse.z;
	ImGui::ColorEdit3("Diffuse", temp);
	m_lightFactors.lightDiffuse = { temp[0], temp[1], temp[2] };

	temp[0] = m_lightFactors.lightSpecular.x, temp[1] = m_lightFactors.lightSpecular.y, temp[2] = m_lightFactors.lightSpecular.z;
	ImGui::ColorEdit3("specular", temp);
	m_lightFactors.lightSpecular = { temp[0], temp[1], temp[2] };

	// Shadow
	ImGui::Checkbox("Shadow Cast", &m_lightFactors.isShadow);
	if (m_lightFactors.isShadow)
	{
		ImGui::Checkbox("ShadowMap Debug Render", &m_lightFactors.isShadowMapRender);
		ImGui::Checkbox("Shadow Frustum Render", &m_lightFactors.isShadowFrustumRender);
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

	m_lightFactors.lightPosition = p;
}

void CGProj::CGEditLightObject::setPosition(const GPED::real x, const GPED::real y, const GPED::real z)
{
	CGEditObject::setPosition(x, y, z);

	m_lightFactors.lightPosition.x = x;
	m_lightFactors.lightPosition.y = y;
	m_lightFactors.lightPosition.z = z;
}

void CGProj::CGEditLightObject::setXposition(const GPED::real x)
{
	CGEditObject::setXposition(x);
	m_lightFactors.lightPosition.x = x;
}

void CGProj::CGEditLightObject::setYposition(const GPED::real y)
{
	CGEditObject::setYposition(y);
	m_lightFactors.lightPosition.y = y;
}

void CGProj::CGEditLightObject::setZposition(const GPED::real z)
{
	CGEditObject::setZposition(z);
	m_lightFactors.lightPosition.z = z;
}

glm::vec3 CGProj::CGEditLightObject::getPosition()
{
	// You can get the edit object position 
	// However I will return the light position for the Obect-Oriented Programming(OOP)
	return m_lightFactors.lightPosition;
}

void CGProj::CGEditLightObject::setLightDirection(const glm::vec3 & d)
{
	// Notice the direction should be normalized
	m_lightFactors.lightDirection = glm::normalize(d);
}

void CGProj::CGEditLightObject::setLightDirection(const GPED::real x, const GPED::real y, const GPED::real z)
{
	m_lightFactors.lightDirection = glm::normalize(glm::vec3(x, y, z));
}

void CGProj::CGEditLightObject::setLightXdirection(const GPED::real x)
{
	m_lightFactors.lightDirection.x = x;
	m_lightFactors.lightDirection = glm::normalize(m_lightFactors.lightDirection);
}

void CGProj::CGEditLightObject::setLightYdirection(const GPED::real y)
{
	m_lightFactors.lightDirection.y = y;
	m_lightFactors.lightDirection = glm::normalize(m_lightFactors.lightDirection);
}

void CGProj::CGEditLightObject::setLightZdirection(const GPED::real z)
{
	m_lightFactors.lightDirection.z = z;
	m_lightFactors.lightDirection = glm::normalize(m_lightFactors.lightDirection);
}

glm::vec3 CGProj::CGEditLightObject::getLightDirection()
{
	return m_lightFactors.lightDirection;
}

void CGProj::CGEditLightObject::setAmbientColor(const glm::vec3 & ac)
{
	m_lightFactors.lightAmbient = ac;
}

void CGProj::CGEditLightObject::setAmbientColor(const GPED::real r, const GPED::real g, const GPED::real b)
{
	m_lightFactors.lightAmbient.r = r;
	m_lightFactors.lightAmbient.g = g;
	m_lightFactors.lightAmbient.b = b;
}

void CGProj::CGEditLightObject::setAmbientRedColor(const GPED::real r)
{
	m_lightFactors.lightAmbient.r = r;
}

void CGProj::CGEditLightObject::setAmbientGreenColor(const GPED::real g)
{
	m_lightFactors.lightAmbient.g = g;
}

void CGProj::CGEditLightObject::setAmbientBlueColor(const GPED::real b)
{
	m_lightFactors.lightAmbient.b = b;
}

glm::vec3 CGProj::CGEditLightObject::getAmbientColor()
{
	return m_lightFactors.lightAmbient;
}

void CGProj::CGEditLightObject::setDiffuseColor(const glm::vec3 & dc)
{
	m_lightFactors.lightDiffuse = dc;
}

void CGProj::CGEditLightObject::setDiffuseColor(const GPED::real r, const GPED::real g, const GPED::real b)
{
	m_lightFactors.lightDiffuse.r = r;
	m_lightFactors.lightDiffuse.g = g;
	m_lightFactors.lightDiffuse.b = b;
}

void CGProj::CGEditLightObject::setDiffuseRedColor(const GPED::real r)
{
	m_lightFactors.lightDiffuse.r = r;
}

void CGProj::CGEditLightObject::setDiffuseGreenColor(const GPED::real g)
{
	m_lightFactors.lightDiffuse.g = g;
}

void CGProj::CGEditLightObject::setDiffuseBlueColor(const GPED::real b)
{
	m_lightFactors.lightDiffuse.b = b;
}

glm::vec3 CGProj::CGEditLightObject::getDiffuseColor()
{
	return m_lightFactors.lightDiffuse;
}

void CGProj::CGEditLightObject::setSpecularColor(const glm::vec3 & sc)
{
	m_lightFactors.lightSpecular = sc;
}

void CGProj::CGEditLightObject::setSpecularColor(const GPED::real r, const GPED::real g, const GPED::real b)
{
	m_lightFactors.lightSpecular.r = r;
	m_lightFactors.lightSpecular.g = g;
	m_lightFactors.lightSpecular.b = b;
}

void CGProj::CGEditLightObject::setSpecularRedColor(const GPED::real r)
{
	m_lightFactors.lightSpecular.r = r;
}

void CGProj::CGEditLightObject::setSpecularGreenColor(const GPED::real g)
{
	m_lightFactors.lightSpecular.g = g;
}

void CGProj::CGEditLightObject::setSpecularBlueColor(const GPED::real b)
{
	m_lightFactors.lightSpecular.b = b;
}

glm::vec3 CGProj::CGEditLightObject::getSpecularColor()
{
	return m_lightFactors.lightSpecular;
}

void CGProj::CGEditLightObject::setAttnConstant(const float c)
{
	m_lightFactors.AttnConstant = c;
	updateRadius();
}

float CGProj::CGEditLightObject::getAttnConsant()
{
	return m_lightFactors.AttnConstant;
}

void CGProj::CGEditLightObject::setAttnLinear(const float l)
{
	m_lightFactors.AttnLinear = l;
	updateRadius();
}

float CGProj::CGEditLightObject::getAttnLinear()
{
	return m_lightFactors.AttnLinear;
}

void CGProj::CGEditLightObject::setAttnQuadratic(const float q)
{
	m_lightFactors.AttnQuadratic = q;
	updateRadius();
}

float CGProj::CGEditLightObject::getAttnQuadratic()
{
	return m_lightFactors.AttnQuadratic;
}

float CGProj::CGEditLightObject::getLightRadius()
{
	return m_lightFactors.AttnRadius;
}

void CGProj::CGEditLightObject::updateRadius()
{
	// the diffuse color of light is used for the max component of light
	// You need to keep in mind that the radius will be bigger if the m_lightFactors.lightDiffuse will be bigger
	float lightMin = 1 / (1.f / 256.f);
	float lightMax = std::fmaxf(std::fmaxf(m_lightFactors.lightDiffuse.r, m_lightFactors.lightDiffuse.g), m_lightFactors.lightDiffuse.b);

	// Calculate the radius of light volume
	m_lightFactors.AttnRadius =
		(
			-m_lightFactors.AttnLinear +
			std::sqrtf(
				m_lightFactors.AttnLinear * m_lightFactors.AttnLinear - 4 * m_lightFactors.AttnQuadratic *
				(m_lightFactors.AttnConstant - lightMax * lightMin))
			)
		/
		(2 * m_lightFactors.AttnQuadratic);
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
	m_lightFactors.isShadow = shadow;
}

bool CGProj::CGEditLightObject::getIsShadowRender()
{
	return m_lightFactors.isShadow;
}

void CGProj::CGEditLightObject::renderShadowMap(std::vector<CGEditProxyObject>& objects)
{
	switch (m_LightType)
	{
	case EDIT_DIRECTION_LIGHT:
		m_dirLight.renderShadowMap(objects);
		break;
	case EDIT_POINT_LIGHT:
		break;
	case EDIT_SPOT_LIGHT:
		break;
	}
}

/*** CG Light Object  ***/
// =================================================================