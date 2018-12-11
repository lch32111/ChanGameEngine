#include "CGEditDirLightVisualizer.h"
#include <Graphics/GLPrimitiveUtil.h>

void CGProj::CGEditDirLightVisualizer::prepareData(Shader * shader)
{
	m_shader = shader;

	glGenVertexArrays(1, &m_cylinderVAO);
	glGenBuffers(1, &m_cylinderVBO);
	glGenBuffers(1, &m_cylinderEBO);

	glBindVertexArray(m_cylinderVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_cylinderVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_cylinderVertices), m_cylinderVertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cylinderEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_cylinderIndices), m_cylinderIndices, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &m_coneVAO);
	glGenBuffers(1, &m_coneVBO);
	glGenBuffers(1, &m_coneEBO);

	glBindVertexArray(m_coneVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_coneVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_coneVertices), m_coneVertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_coneEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_coneIndices), m_coneIndices, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
}

void CGProj::CGEditDirLightVisualizer::setCylinderDimension(float height, float tRadius, float bRadius)
{
	m_cylinderTopRadius = tRadius;
	m_cylinderBottomRadius = bRadius;
	m_cylinderHeight = height;
	
	getCylinderVerts(m_cylinderTopRadius, m_cylinderBottomRadius, m_cylinderHeight,
		m_cylinderVertices, m_cylinderIndices, (int)e_maxVertices, m_cylinderCount);
}

void CGProj::CGEditDirLightVisualizer::setConeDimension(float height, float tRadius, float bRadius)
{
	m_coneTopRadius = tRadius;
	m_coneBottomRadius = bRadius;
	m_coneHeight = height;

	getCylinderVerts(m_coneTopRadius, m_coneBottomRadius, m_coneHeight,
		m_coneVertices, m_coneIndices, (int)e_maxVertices, m_coneCount);
}

void CGProj::CGEditDirLightVisualizer::render(const glm::mat4 & view, const glm::mat4 & proj,
	const glm::vec3 & position, const glm::vec3 & direction)
{
	glm::mat4 model = glm::mat4_cast(glm::quat(m_localDirection, direction));
	model[3] = glm::vec4(position, 1.0);
	glm::mat4 mvpMatrix = proj * view * model;
	
	glDisable(GL_CULL_FACE);

	// Cylinder First
	m_shader->use();
	m_shader->setMat4("mvpMatrix", mvpMatrix);
	m_shader->setVec3("Color", m_cylinderColor);

	/*
	glBindVertexArray(m_cylinderVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cylinderEBO);
	glDrawElements(GL_TRIANGLE_STRIP, m_cylinderCount, GL_UNSIGNED_INT, 0);
	*/
	renderTestCylinder();

	/*
	// Cone Second
	model[3] = glm::vec4(position + m_cylinderHeight + m_coneHeight / 2, 1.0);
	mvpMatrix = proj * view * model;
	m_shader->setMat4("mvpMatrix", mvpMatrix);
	m_shader->setVec3("Color", m_coneColor);
	glBindVertexArray(m_coneVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_coneEBO);
	glDrawElements(GL_TRIANGLES, m_coneCount, GL_UNSIGNED_INT, 0);
	*/

	glEnable(GL_CULL_FACE);

	glBindVertexArray(0);
}
