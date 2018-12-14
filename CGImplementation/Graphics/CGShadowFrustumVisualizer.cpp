#include "CGShadowFrustumVisualizer.h"

CGProj::CGShadowFrustumVisualizer::CGShadowFrustumVisualizer()
{
}

CGProj::CGShadowFrustumVisualizer::CGShadowFrustumVisualizer(CGAssetManager & am)
{
	m_lineShader = am.getShader(SHADER_CG_LINE);
	prepareData();
}

void CGProj::CGShadowFrustumVisualizer::setShader(Shader * shader)
{
	m_lineShader = shader;
	prepareData();
}

void CGProj::CGShadowFrustumVisualizer::render(
	bool projectionType, const glm::mat4 & lightSpcae, const glm::vec3& position,
	const glm::mat4& view, const glm::mat4& proj)
{
	glm::vec3 vertices[2][2][2];

	for (int z = 0; z < 2; ++z)
	{
		for (int y = 0; y < 2; ++y)
		{
			for (int x = 0; x < 2; ++x)
			{
				glm::vec4 locals(
					x ? -1.0 : 1.0,
					y ? -1.0 : 1.0,
					z ? 0 : 1.0,
					1.0);

				vertices[x][y][z] = glm::vec3(lightSpcae * locals);
			}
		}
	}

	insertLine(vertices[0][0][0], vertices[0][0][1], m_FrustumColor);
	insertLine(vertices[1][0][0], vertices[1][0][1], m_FrustumColor);
	insertLine(vertices[0][1][0], vertices[0][1][1], m_FrustumColor);
	insertLine(vertices[1][1][0], vertices[1][1][1], m_FrustumColor);

	insertLine(vertices[0][0][0], vertices[0][1][0], m_FrustumColor);
	insertLine(vertices[1][0][0], vertices[1][1][0], m_FrustumColor);
	insertLine(vertices[0][0][1], vertices[0][1][1], m_FrustumColor);
	insertLine(vertices[1][0][1], vertices[1][1][1], m_FrustumColor);

	insertLine(vertices[0][0][0], vertices[1][0][0], m_FrustumColor);
	insertLine(vertices[0][1][0], vertices[1][1][0], m_FrustumColor);
	insertLine(vertices[0][0][1], vertices[1][0][1], m_FrustumColor);
	insertLine(vertices[0][1][1], vertices[1][1][1], m_FrustumColor);

	renderLine(view, proj);
}

void CGProj::CGShadowFrustumVisualizer::render(
	const glm::mat4& view, const glm::mat4& proj,
	const glm::vec3 & position, const glm::vec3 & direction, 
	float l, float r, float b, float t, float nearP, float farP)
{
	glm::vec3 vertices[8];

	// Near Left Bottom
	vertices[0] = glm::vec3(l, b, -nearP);

	// Near Right Bottom
	vertices[1] = glm::vec3(r, b, -nearP);

	// Near Left Top
	vertices[2] = glm::vec3(l, t, -nearP);

	// Near Right Top
	vertices[3] = glm::vec3(r, t, -nearP);

	// Far Left Bottom
	vertices[4] = glm::vec3(l, b,  -farP);

	// Far Right Bottom
	vertices[5] = glm::vec3(r, b, -farP);

	// Far Left Top
	vertices[6] = glm::vec3(l, t, -farP);

	// Far Right Top
	vertices[7] = glm::vec3(r, t, -farP);

	glm::mat4 model = glm::mat4_cast(glm::quat(glm::vec3(0, 0, -1), direction));
	model[3] = glm::vec4(position, 1.0);

	for (int i = 0; i < 8; ++i) vertices[i] = model * glm::vec4(vertices[i], 1.0);

	insertLine(vertices[0], vertices[1], m_TestFrustumColor);
	insertLine(vertices[2], vertices[3], m_TestFrustumColor);
	insertLine(vertices[4], vertices[5], m_TestFrustumColor);
	insertLine(vertices[6], vertices[7], m_TestFrustumColor);

	insertLine(vertices[0], vertices[4], m_TestFrustumColor);
	insertLine(vertices[1], vertices[5], m_TestFrustumColor);
	insertLine(vertices[2], vertices[6], m_TestFrustumColor);
	insertLine(vertices[3], vertices[7], m_TestFrustumColor);

	insertLine(vertices[0], vertices[2], m_TestFrustumColor);
	insertLine(vertices[1], vertices[3], m_TestFrustumColor);
	insertLine(vertices[4], vertices[6], m_TestFrustumColor);
	insertLine(vertices[5], vertices[7], m_TestFrustumColor);

	renderLine(view, proj);
}

void CGProj::CGShadowFrustumVisualizer::prepareData()
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

void CGProj::CGShadowFrustumVisualizer::insertLine(const glm::vec3 & From, const glm::vec3 & To, const glm::vec4 & Color)
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

void CGProj::CGShadowFrustumVisualizer::renderLine(const glm::mat4 & view, const glm::mat4 & proj, const float lineWidth)
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
