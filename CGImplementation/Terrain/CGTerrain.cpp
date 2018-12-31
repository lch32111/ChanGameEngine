#include <Terrain/CGTerrain.h>

#include <stb_image.h>
#include <fstream>

CGProj::CGTerrain::CGTerrain()
	: 
	m_terrainWidth(25), m_terrainHeight(2), m_terrainDepth(25), // Manual Setting
	m_terrainSubWidth(511), m_terrainSubDepth(511),
	m_gridVertexCount((m_terrainSubWidth + 1) * (m_terrainSubDepth + 1)),
	m_VAO(0), m_VBO{ 0, 0, 0 }, m_EBO(0)
{
}

void CGProj::CGTerrain::initializeWithImage(CGAssetManager & am)
{
	m_terrainShader = am.getShader(SHADER_SIMPLE_TERRAIN);

	int textureX, textureY, textureChannel;
	unsigned char* data = stbi_load("ImageFolder/perlinTest.png", &textureX, &textureY, &textureChannel, 0);
	glm::vec3* vertices = new glm::vec3[m_gridVertexCount];
	glm::vec3* normals = new glm::vec3[m_gridVertexCount];
	glm::vec2* textures = new glm::vec2[m_gridVertexCount];
	// the last term with plus is for the degenerate case of triangle_strip
	m_indicesCount = (m_terrainSubWidth + 1) * 2 * m_terrainSubDepth + (m_terrainSubWidth - 1) * 2;
	unsigned* indices = new unsigned[m_indicesCount];

	float invSubWidth = 1.f / m_terrainSubWidth;
	float invSubDepth = 1.f / m_terrainSubDepth;

	// prepare the grid and other informations
	unsigned pixelWidth = ((float)textureX / (m_terrainSubWidth + 1.f));
	unsigned pixelHeight = ((float)textureY / (m_terrainSubDepth + 1.f));
	for (unsigned j = 0; j <= m_terrainSubDepth; ++j)
	{
		for (unsigned i = 0; i <= m_terrainSubWidth; ++i)
		{
			vertices[i + (m_terrainSubWidth + 1) * j] = 
				glm::vec3
				(
					m_terrainWidth * (i * invSubWidth - 0.5f),
					0,
					m_terrainDepth * (j * invSubDepth - 0.5f)
				);
			
			unsigned color = data[(i * pixelWidth + j * pixelHeight * textureX) * textureChannel];
			float height = ((color / 255.f) - 0.5f) * m_terrainHeight;
			vertices[i + (m_terrainSubWidth + 1) * j].y = height;

			// I will add the feature of calcualting the exactl normal later.
			normals[i + (m_terrainSubWidth + 1) * j] = glm::vec3(0);
			textures[i + (m_terrainSubWidth + 1) * j] = glm::vec2(i * invSubWidth, j * invSubDepth);
		}
	}
	
	unsigned index = 0;
	for (unsigned j = 0; j < m_terrainSubDepth; ++j)
	{
		for (unsigned i = 0; i <= m_terrainSubWidth; ++i)
		{
			indices[index++] = i + j * (m_terrainSubWidth + 1);
			indices[index++] = i + (j + 1) * (m_terrainSubWidth + 1);

			if (i == m_terrainSubWidth && j != m_terrainSubDepth - 1)
			{
				// degenerate case for connecting the separate strips
				indices[index++] = i + (j + 1) * (m_terrainSubWidth + 1);
				indices[index++] = (j + 1) * (m_terrainSubWidth + 1);
			}
		}
	}
	
	// Tell the OpenGL
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(3, m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * m_gridVertexCount, &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * m_gridVertexCount, &normals[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[2]);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * m_gridVertexCount, &textures[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indicesCount, &indices[0], GL_STATIC_DRAW);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] indices;
	delete[] textures;
	delete[] normals;
	delete[] vertices;
	stbi_image_free(data);
}

void CGProj::CGTerrain::initializeWithGenerator(CGAssetManager & am)
{
	m_terrainShader = am.getShader(SHADER_SIMPLE_TERRAIN);

	int textureX, textureY, textureChannel;
	glm::vec3* vertices = new glm::vec3[m_gridVertexCount];
	glm::vec3* normals = new glm::vec3[m_gridVertexCount];
	glm::vec2* textures = new glm::vec2[m_gridVertexCount];
	// the last term with plus is for the degenerate case of triangle_strip
	m_indicesCount = (m_terrainSubWidth + 1) * 2 * m_terrainSubDepth + (m_terrainSubWidth - 1) * 2;
	unsigned* indices = new unsigned[m_indicesCount];

	float invSubWidth = 1.f / m_terrainSubWidth;
	float invSubDepth = 1.f / m_terrainSubDepth;

	// prepare the grid and other informations
	CGPerlinNoise perlin(20190101);
	for (unsigned j = 0; j <= m_terrainSubDepth; ++j)
	{
		for (unsigned i = 0; i <= m_terrainSubWidth; ++i)
		{
			vertices[i + (m_terrainSubWidth + 1) * j] =
				glm::vec3
				(
					m_terrainWidth * (i * invSubWidth - 0.5f),
					0,
					m_terrainDepth * (j * invSubDepth - 0.5f)
				);

			glm::vec3 derivs;
			glm::vec3 p = vertices[i + (m_terrainSubWidth + 1) * j];
			p *= 0.24;
			float perlinHeight = (perlin.eval(p, derivs) + 1.f) * 0.5f;
			vertices[i + (m_terrainSubWidth + 1) * j].y = (perlinHeight - 0.5f) * m_terrainHeight;

			// Analytical normal
			normals[i + (m_terrainSubWidth + 1) * j] = glm::normalize(glm::vec3(-derivs.x, 1.f, -derivs.z));

			textures[i + (m_terrainSubWidth + 1) * j] = glm::vec2(i * invSubWidth, j * invSubDepth);
		}
	}

	unsigned index = 0;
	for (unsigned j = 0; j < m_terrainSubDepth; ++j)
	{
		for (unsigned i = 0; i <= m_terrainSubWidth; ++i)
		{
			indices[index++] = i + j * (m_terrainSubWidth + 1);
			indices[index++] = i + (j + 1) * (m_terrainSubWidth + 1);

			if (i == m_terrainSubWidth && j != m_terrainSubDepth - 1)
			{
				// degenerate case for connecting the separate strips
				indices[index++] = i + (j + 1) * (m_terrainSubWidth + 1);
				indices[index++] = (j + 1) * (m_terrainSubWidth + 1);
			}
		}
	}

	// Tell the OpenGL
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(3, m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * m_gridVertexCount, &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * m_gridVertexCount, &normals[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[2]);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * m_gridVertexCount, &textures[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indicesCount, &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] indices;
	delete[] textures;
	delete[] normals;
	delete[] vertices;
}

void CGProj::CGTerrain::render(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3& campos)
{
	glm::mat4 model(1.0);
	m_terrainShader->use();
	m_terrainShader->setMat4("mvpMatrix", proj * view * model);
	m_terrainShader->setVec3("cameraPos", campos);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLE_STRIP, m_indicesCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CGProj::CGTerrain::destroy()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(3, m_VBO);
	glDeleteBuffers(1, &m_EBO);
}