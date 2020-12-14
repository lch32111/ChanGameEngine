#include <CGPrecompiled.h>
#include <Graphics/CGRenderLine.h>

#include <Graphics/CGAssetManager.h>

CG::CGRenderLine::CGRenderLine()
{

}

CG::CGRenderLine::CGRenderLine(CGAssetManager & am)
{
	m_lineShader = am.getShader(SHADER_CG_LINE);
	prepareData();
}

void CG::CGRenderLine::setShader(Shader * shader)
{
	m_lineShader = shader;
	prepareData();
}

void CG::CGRenderLine::insertLine(const glm::vec3 & From, const glm::vec3 & To, const glm::vec4 & Color)
{
	assert(m_count < e_maxVertices);
	m_vertices[m_count] = From;
	m_colors[m_count] = Color;
	++m_count;
	
	assert(m_count < e_maxVertices);
	m_vertices[m_count] = To;
	m_colors[m_count] = Color;
	++m_count;
}

void CG::CGRenderLine::renderLine(const glm::mat4 & view, const glm::mat4 & proj, const float lineWidth)
{
	if (m_count == 0) return;

	m_lineShader->use();
	m_lineShader->setMat4("view", view);
	m_lineShader->setMat4("projection", proj);

	glBindVertexArray(m_VAO);
	// Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(glm::vec3), m_vertices);
	
	// Color Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(glm::vec4), m_colors);

	glLineWidth(lineWidth);
	glDrawArrays(GL_LINES, 0, m_count);

	// Setting Default again
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glLineWidth(1.0);

	m_count = 0;
}

void CG::CGRenderLine::prepareData()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(2, m_VBO);

	glBindVertexArray(m_VAO);
	
	// Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
	glEnableVertexAttribArray(0); // vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

	// Color Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
	glEnableVertexAttribArray(1); // color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_count = 0;
}
