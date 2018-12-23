#include <Graphics/CGPointShadowFrustumVisualizer.h>

#include <Graphics/GLPrimitiveUtil.h>

CGProj::CGPointShadowFrustumVisualizer::CGPointShadowFrustumVisualizer()
{

}

CGProj::CGPointShadowFrustumVisualizer::CGPointShadowFrustumVisualizer(CGAssetManager& am)
{
	m_shader = am.getShader(SHADER_SIMPLE_COLOR_RENDER);
}

void CGProj::CGPointShadowFrustumVisualizer::setShader(Shader* shader)
{
	m_shader = shader;
}

void CGProj::CGPointShadowFrustumVisualizer::render(
	const glm::mat4& view, const glm::mat4& proj,
	const glm::vec3& position,
	const float nearP, const float farP)
{
	// Refer to simpleColorRender file
	m_shader->use();
	m_shader->setMat4("view", view);
	m_shader->setMat4("projection", proj);
	m_shader->setVec3("Color", glm::vec3(0.35, 0.6, 1));

	glm::mat4 model(1.0);
	model = glm::translate(model, position);

	m_shader->setMat4("model", glm::scale(model, glm::vec3(farP)));
	wireRenderCube();

	m_shader->setMat4("model", glm::scale(model, glm::vec3(nearP)));
	wireRenderCube();
}