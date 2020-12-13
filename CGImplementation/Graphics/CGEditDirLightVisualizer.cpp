#include <CGPrecompiled.h>
#include "CGEditDirLightVisualizer.h"
#include <Graphics/GLPrimitiveUtil.h>

void CGProj::CGEditDirLightVisualizer::prepareData(Shader * shader)
{
	m_shader = shader;

	glGenVertexArrays(1, &m_cylinderVAO);
	glGenBuffers(1, &m_cylinderVBO);
	glGenBuffers(1, &m_cylinderEBO);

	glBindVertexArray(m_cylinderVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_cylinderVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (int)e_maxVertices * 3, NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cylinderEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * (int)e_maxVertices, NULL, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
	glBindVertexArray(0);

	glGenVertexArrays(1, &m_coneVAO);
	glGenBuffers(1, &m_coneVBO);
	glGenBuffers(1, &m_coneEBO);

	glBindVertexArray(m_coneVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_coneVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (int)e_maxVertices * 3, NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_coneEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * (int)e_maxVertices, NULL, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);
}

void CGProj::CGEditDirLightVisualizer::setCylinderDimension(float height, float tRadius, float bRadius)
{
	m_cylinderTopRadius = tRadius;
	m_cylinderBottomRadius = bRadius;
	m_cylinderHeight = height;
	
	unsigned t_vertexCount = 0;
	getCylinderVerts(m_cylinderTopRadius, m_cylinderBottomRadius, m_cylinderHeight,
		m_cylinderVertices, m_cylinderIndices, (int)e_maxVertices, t_vertexCount, m_cylinderCount);

	glBindBuffer(GL_ARRAY_BUFFER, m_cylinderVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, t_vertexCount * sizeof(glm::vec3), &m_cylinderVertices[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cylinderEBO);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_cylinderCount * sizeof(unsigned), &m_cylinderIndices[0]);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CGProj::CGEditDirLightVisualizer::setConeDimension(float height, float tRadius, float bRadius)
{
	m_coneTopRadius = tRadius;
	m_coneBottomRadius = bRadius;
	m_coneHeight = height;

	unsigned t_vertexCount = 0;
	getCylinderVerts(m_coneTopRadius, m_coneBottomRadius, m_coneHeight,
		m_coneVertices, m_coneIndices, (int)e_maxVertices, t_vertexCount, m_coneCount);

	glBindBuffer(GL_ARRAY_BUFFER, m_coneVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, t_vertexCount * sizeof(glm::vec3), &m_coneVertices[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_coneEBO);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_coneCount * sizeof(unsigned), &m_coneIndices[0]);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGProj::CGEditDirLightVisualizer::render(const glm::mat4 & view, const glm::mat4 & proj,
	const glm::vec3 & position, const glm::vec3 & direction)
{
	glm::mat4 model = glm::mat4_cast(glm::quat(m_localDirection, direction));
	model[3] = glm::vec4(position, 1.0);
	glm::mat4 mvpMatrix = proj * view * model;
	
	// Cylinder First
	m_shader->use();
	m_shader->setMat4("mvpMatrix", mvpMatrix);
	m_shader->setVec3("Color", m_cylinderColor);

	glBindVertexArray(m_cylinderVAO);
	glDrawElements(GL_TRIANGLES, m_cylinderCount, GL_UNSIGNED_INT, 0);

	// Cone Second
	model[3] = glm::vec4(0, 0, 0, 1);
	model[3] = glm::vec4(position, 1.0) + model * glm::vec4(glm::vec3(0, (m_cylinderHeight), 0) , 0);
	mvpMatrix = proj * view * model;
	m_shader->setMat4("mvpMatrix", mvpMatrix);
	m_shader->setVec3("Color", m_coneColor);
	glBindVertexArray(m_coneVAO);
	glDrawElements(GL_TRIANGLES, m_coneCount, GL_UNSIGNED_INT, 0);
	
	glBindVertexArray(0);
}
