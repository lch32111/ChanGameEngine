#include "chanRenderLine.h"

lineRenderer::lineRenderer()
{

}

lineRenderer::lineRenderer(const GLchar * vertexFile, 
	const GLchar * geoFile, const GLchar * fragFile)
{
	m_lineShader = Shader(vertexFile, geoFile, fragFile);
	m_lineShader.loadShaderWithGeo();
	prepareData();
}

void lineRenderer::prepareData()
{
	float lineVertices[] =
	{
		0.f, 0.f, 0.f,
	};

	glGenVertexArrays(1, &m_lineVAO);
	glGenBuffers(1, &m_lineVBO);
	glBindVertexArray(m_lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), &lineVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}


void lineRenderer::renderline(
	const glm::mat4 & view, const glm::mat4 & proj, 
	const glm::vec3 & From, const glm::vec3 & To,
	const glm::vec3 & Color)
{
	glm::mat4 model(1.0);
	model = glm::translate(model, From);

	m_lineShader.use();
	m_lineShader.setMat4("projection", proj);
	m_lineShader.setMat4("view", view);
	m_lineShader.setMat4("model", model);
	m_lineShader.setVec3("toPoint", To);
	m_lineShader.setVec3("lineColor", Color);

	glBindVertexArray(m_lineVAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}


