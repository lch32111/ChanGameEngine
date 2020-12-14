#include <CGPrecompiled.h>
#include <Graphics/CGDirShadowFrustumVisualizer.h>

#include <GPED/CGPhysicsUtil.h>
#include <Graphics/CGAssetManager.h>

CG::CGDirShadowFrustumVisualizer::CGDirShadowFrustumVisualizer()
{
}

CG::CGDirShadowFrustumVisualizer::CGDirShadowFrustumVisualizer(CGAssetManager & am)
{
	m_lineShader = am.getShader(SHADER_CG_LINE);
	prepareData();
}

void CG::CGDirShadowFrustumVisualizer::setShader(Shader * shader)
{
	m_lineShader = shader;
	prepareData();
}

void CG::CGDirShadowFrustumVisualizer::render(
	const glm::mat4& view, const glm::mat4& proj,
	const glm::vec3 & position, const glm::vec3 & direction,
	float fov, float aspect, float nearP, float farP
	)
{
	float tanHalfFov = std::tanf(0.5f * fov);

	glm::vec3 vertices[8];

	// Near Plane
	float nearHeight = tanHalfFov * nearP * 2;
	float nearWidth = nearHeight * aspect;

	// Near Left Bottom
	vertices[0].x = nearWidth * -0.5f;
	vertices[0].y = nearHeight * -0.5f;
 	vertices[0].z = -nearP;

	// Near Right Bottom
	vertices[1].x = nearWidth * 0.5f;
	vertices[1].y = nearHeight * -0.5f;
	vertices[1].z = -nearP;

	// Near Left Top
	vertices[2].x = nearWidth * -0.5f;
	vertices[2].y = nearHeight * 0.5f;
	vertices[2].z = -nearP;

	// Near Right Top
	vertices[3].x = nearWidth * 0.5f;
	vertices[3].y = nearHeight * 0.5f;
	vertices[3].z = -nearP;

	// Far Plane
	float farHeight = tanHalfFov * farP * 2;
	float farWidth = farHeight * aspect;

	// Far Left Bottom
	vertices[4].x = farWidth * -0.5f;
	vertices[4].y = farHeight * -0.5f;
	vertices[4].z = -farP;

	// Far Right Bottom
	vertices[5].x = farWidth * 0.5f;
	vertices[5].y = farHeight * -0.5f;
	vertices[5].z = -farP;

	// Far Left Top
	vertices[6].x = farWidth * -0.5f;
	vertices[6].y = farHeight * 0.5f;
	vertices[6].z = -farP;

	// Far Right Top
	vertices[7].x = farWidth * 0.5f;
	vertices[7].y = farHeight * 0.5f;
	vertices[7].z = -farP;

	glm::mat4 model(1.0);

	// Rotation and Translation
	glm::vec3 zaxis(glm::normalize(-direction));
	glm::vec3 xaxis(safeNormalize(glm::cross(glm::vec3(0, 1, 0), zaxis)));
	glm::vec3 yaxis(glm::cross(zaxis, xaxis));
	model[0][0] = xaxis.x, model[0][1] = xaxis.y, model[0][2] = xaxis.z;
	model[1][0] = yaxis.x, model[1][1] = yaxis.y, model[1][2] = yaxis.z;
	model[2][0] = zaxis.x, model[2][1] = zaxis.y, model[2][2] = zaxis.z;
	model[3][0] = position.x, model[3][1] = position.y, model[3][2] = position.z;

	// Transform
	for (int i = 0; i < 8; ++i) vertices[i] = model * glm::vec4(vertices[i], 1.0);

	// Draw Line
	insertLine(vertices[0], vertices[1], m_FrustumColor);
	insertLine(vertices[2], vertices[3], m_FrustumColor);
	insertLine(vertices[4], vertices[5], m_FrustumColor);
	insertLine(vertices[6], vertices[7], m_FrustumColor);

	insertLine(vertices[0], vertices[4], m_FrustumColor);
	insertLine(vertices[1], vertices[5], m_FrustumColor);
	insertLine(vertices[2], vertices[6], m_FrustumColor);
	insertLine(vertices[3], vertices[7], m_FrustumColor);

	insertLine(vertices[0], vertices[2], m_FrustumColor);
	insertLine(vertices[1], vertices[3], m_FrustumColor);
	insertLine(vertices[4], vertices[6], m_FrustumColor);
	insertLine(vertices[5], vertices[7], m_FrustumColor);

	renderLine(view, proj);
}

void CG::CGDirShadowFrustumVisualizer::render(
	const glm::mat4& view, const glm::mat4& proj,
	const glm::vec3 & position, const glm::vec3 & direction, 
	float l, float r, float b, float t, float nearP, float farP)
{
	glm::vec3 vertices[8];

	// Near Left Bottom
	vertices[0].x = l;
	vertices[0].y = b;
	vertices[0].z = -nearP;

	// Near Right Bottom
	vertices[1].x = r;
	vertices[1].y = b;
	vertices[1].z = -nearP;

	// Near Left Top
	vertices[2].x = l;
	vertices[2].y = t;
	vertices[2].z = -nearP;

	// Near Right Top
	vertices[3].x = r;
	vertices[3].y = t;
	vertices[3].z = -nearP;

	// Far Left Bottom
	vertices[4].x = l;
	vertices[4].y = b;
	vertices[4].z = -farP;

	// Far Right Bottom
	vertices[5].x = r;
	vertices[5].y = b;
	vertices[5].z = -farP;

	// Far Left Top
	vertices[6].x = l;
	vertices[6].y = t;
	vertices[6].z = -farP;

	// Far Right Top
	vertices[7].x = r;
	vertices[7].y = t;
	vertices[7].z = -farP;

	glm::mat4 model(1.0);

	// Rotation and Translation
	glm::vec3 zaxis(glm::normalize(-direction));
	glm::vec3 xaxis(safeNormalize(glm::cross(glm::vec3(0, 1, 0), zaxis)));
	glm::vec3 yaxis(glm::cross(zaxis, xaxis));
	model[0][0] = xaxis.x, model[0][1] = xaxis.y, model[0][2] = xaxis.z;
	model[1][0] = yaxis.x, model[1][1] = yaxis.y, model[1][2] = yaxis.z;
	model[2][0] = zaxis.x, model[2][1] = zaxis.y, model[2][2] = zaxis.z;
	model[3][0] = position.x, model[3][1] = position.y, model[3][2] = position.z;

	// Transform
	for (int i = 0; i < 8; ++i) vertices[i] = model * glm::vec4(vertices[i], 1.0);

	// Draw Line
	insertLine(vertices[0], vertices[1], m_FrustumColor);
	insertLine(vertices[2], vertices[3], m_FrustumColor);
	insertLine(vertices[4], vertices[5], m_FrustumColor);
	insertLine(vertices[6], vertices[7], m_FrustumColor);

	insertLine(vertices[0], vertices[4], m_FrustumColor);
	insertLine(vertices[1], vertices[5], m_FrustumColor);
	insertLine(vertices[2], vertices[6], m_FrustumColor);
	insertLine(vertices[3], vertices[7], m_FrustumColor);

	insertLine(vertices[0], vertices[2], m_FrustumColor);
	insertLine(vertices[1], vertices[3], m_FrustumColor);
	insertLine(vertices[4], vertices[6], m_FrustumColor);
	insertLine(vertices[5], vertices[7], m_FrustumColor);

	renderLine(view, proj);
}

void CG::CGDirShadowFrustumVisualizer::prepareData()
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

void CG::CGDirShadowFrustumVisualizer::insertLine(const glm::vec3 & From, const glm::vec3 & To, const glm::vec4 & Color)
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

void CG::CGDirShadowFrustumVisualizer::renderLine(const glm::mat4 & view, const glm::mat4 & proj, const float lineWidth)
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
