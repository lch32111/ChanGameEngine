#include "CGSizableRenderLine.h"

CGProj::CGSizableRenderLine::CGSizableRenderLine()
{

}

CGProj::CGSizableRenderLine::CGSizableRenderLine(CGAssetManager & am, unsigned maxLineNumb)
{
	m_lineShader = am.getShader(SHADER_CG_LINE);
	
	m_bufferSize = maxLineNumb * 2;

	m_vertices = new glm::vec3[m_bufferSize];
	m_colors = new glm::vec4[m_bufferSize];

	memset(m_vertices, 0, sizeof(glm::vec3) * m_bufferSize);
	memset(m_colors, 0, sizeof(glm::vec4) * m_bufferSize);

	m_isFlush = true;
	prepareData();
}

void CGProj::CGSizableRenderLine::initialize(Shader * shader, unsigned maxLineNumb)
{
	m_lineShader = shader;

	m_bufferSize = maxLineNumb * 2;

	m_vertices = new glm::vec3[m_bufferSize];
	m_colors = new glm::vec4[m_bufferSize];

	memset(m_vertices, 0, sizeof(glm::vec3) * m_bufferSize);
	memset(m_colors, 0, sizeof(glm::vec4) * m_bufferSize);

	m_isFlush = true;
	prepareData();
}

void CGProj::CGSizableRenderLine::destroy()
{
	delete[] m_vertices;
	delete[] m_colors;

	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(2, m_VBO);
}

void CGProj::CGSizableRenderLine::insertLine(const glm::vec3 & From, const glm::vec3 & To, const glm::vec4 & Color)
{
	assert(m_count < m_bufferSize);
	m_vertices[m_count] = From;
	m_colors[m_count] = Color;
	++m_count;

	assert(m_count < m_bufferSize);
	m_vertices[m_count] = To;
	m_colors[m_count] = Color;
	++m_count;
}

void CGProj::CGSizableRenderLine::renderLine(const glm::mat4 & view, const glm::mat4 & proj, const float lineWidth)
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

	if(m_isFlush)
		m_count = 0;
}

void CGProj::CGSizableRenderLine::setFlush(bool f)
{
	m_isFlush = f;
}

void CGProj::CGSizableRenderLine::prepareData()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(2, m_VBO);

	glBindVertexArray(m_VAO);

	// Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
	glEnableVertexAttribArray(0); // vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_bufferSize, m_vertices, GL_DYNAMIC_DRAW);

	// Color Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
	glEnableVertexAttribArray(1); // color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * m_bufferSize, m_colors, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_count = 0;
}
