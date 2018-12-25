#include <Terrain/CGTerrain.h>

CGProj::CGTerrain::CGTerrain()
	: 
	m_terrainWidth(100), m_terrainHeight(100), // Manual Setting
	m_vertexCount(0), m_indexCount(0),
	m_VAO(0), m_VBO(0)
{
}

void CGProj::CGTerrain::initialize(CGAssetManager & am)
{
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 8;
	m_indexCount = m_vertexCount;

	struct terrainData
	{
		glm::vec3 position;
		glm::vec4 color;
	};

	terrainData* vertices = new terrainData[m_vertexCount];
	unsigned* indices = new unsigned[m_indexCount];

	int positionX = 0;
	int positionZ = 0;
	int index = 0;
	for (unsigned j = 0; j < m_terrainHeight - 1; ++j)
	{
		for (unsigned i = 0; i < m_terrainWidth - 1; ++i)
		{
			// LINE 1
			// Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);

			vertices[index].position = glm::vec3(positionX, 0.0f, positionZ);
			vertices[index].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = glm::vec3(positionX, 0.0f, positionZ);
			vertices[index].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// LINE 2
			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = glm::vec3(positionX, 0.0f, positionZ);
			vertices[index].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;

			vertices[index].position = glm::vec3(positionX, 0.0f, positionZ);
			vertices[index].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// LINE 3
			// Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;

			vertices[index].position = glm::vec3(positionX, 0.0f, positionZ);
			vertices[index].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;

			vertices[index].position = glm::vec3(positionX, 0.0f, positionZ);
			vertices[index].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// LINE 4
			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;

			vertices[index].position = glm::vec3(positionX, 0.0f, positionZ);
			vertices[index].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);

			vertices[index].position = glm::vec3(positionX, 0.0f, positionZ);
			vertices[index].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;
		}
	}

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(terrainData) * m_vertexCount, &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * index, &indices[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)(sizeof(float) * 3));
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vertices;
	delete[] indices;

	m_lineShader = am.getShader(SHADER_CG_LINE);
	
}

void CGProj::CGTerrain::render(const glm::mat4 & view, const glm::mat4 & proj)
{
	m_lineShader->use();
	m_lineShader->setMat4("view", view);
	m_lineShader->setMat4("projection", proj);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_LINES, m_indexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void CGProj::CGTerrain::destroy()
{
	glDeleteBuffers(1, &m_EBO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteVertexArrays(1, &m_VAO);
}