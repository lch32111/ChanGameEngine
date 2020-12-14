#include <CGPrecompiled.h>
#include <Graphics/CGEditPointLightVisualizer.h>
#include <Graphics/GLPrimitiveUtil.h>

void CG::CGEditPointLightVisualizer::prepareData(Shader * shader)
{
	m_shader = shader;
}

void CG::CGEditPointLightVisualizer::render(const glm::mat4 & view, const glm::mat4 & proj, 
	const glm::vec3 & position, const float scale)
{
	glm::mat4 vp = proj * view;
	glm::mat4 model(1.0);
	glm::mat4 rot(1.0);
	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(scale));
	rot = glm::rotate(rot, glm::radians(90.f), glm::vec3(1, 0, 0));

	m_shader->use();
	m_shader->setVec3("Color", m_circleColor);

	// XZ Circle
	m_shader->setMat4("mvpMatrix", vp * model);
	renderWireCircle2D();

	// XY Circle
	m_shader->setMat4("mvpMatrix", vp * model * rot);
	renderWireCircle2D();

	// YZ Circle
	rot = glm::rotate(glm::mat4(1.0), glm::radians(90.f), glm::vec3(0, 0, 1));
	m_shader->setMat4("mvpMatrix", vp * model * rot);
	renderWireCircle2D();
}
