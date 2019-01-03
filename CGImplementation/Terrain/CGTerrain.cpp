#include <Terrain/CGTerrain.h>

#include <stb_image.h>
#include <fstream>

CGProj::CGTerrain::CGTerrain()
	: 
	m_terrainWidth(32), m_terrainHeight(2), m_terrainDepth(32), // Manual Setting
	m_terrainSubWidth(127), m_terrainSubDepth(127), 
	m_terrainOrigin(glm::vec3(0)),
	m_gridVertexCount((m_terrainSubWidth + 1) * (m_terrainSubDepth + 1)),
	m_indicesCount((m_terrainSubWidth + 1) * 2 * m_terrainSubDepth + (m_terrainSubWidth - 1) * 2),
	m_VAO(0), m_VBO{ 0, 0, 0 }, m_EBO(0)
{
	// This is the comment for the last term of the indicesCount variable!
	// the last term is for the degenerate case of triangle_strip
}

void CGProj::CGTerrain::initialize(bool imageOrGenerator, CGAssetManager& am)
{
	// Graphics Init
	imageOrGenerator ? initializeWithImage(am) : initializeWithGenerator(am);

	// Physics Init
	initializePhysics();
}

void CGProj::CGTerrain::destroy()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(3, m_VBO);
	glDeleteBuffers(1, &m_EBO);

	delete[] m_HeightData;
}


void CGProj::CGTerrain::render(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3& campos)
{
	glm::mat4 model(1.0);
	m_terrainShader->use();
	m_terrainShader->setMat4("mvpMatrix", proj * view * model);
	m_terrainShader->setVec3("cameraPos", campos);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_testTexture);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLE_STRIP, m_indicesCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

unsigned CGProj::CGTerrain::getProxyId()
{
	return m_proxyId;
}

void CGProj::CGTerrain::setProxyId(unsigned id)
{
	m_proxyId = id;
}

GPED::c3AABB CGProj::CGTerrain::getAABB()
{
	return m_localAABB;
}

void CGProj::CGTerrain::getAABB(GPED::c3AABB & out)
{
	out = m_localAABB;
}

CGProj::CGCollisionMesh * CGProj::CGTerrain::getCollisioPrimitivePtr()
{
	return &m_collisionPrimitive;
}

void CGProj::CGTerrain::initializeWithImage(CGAssetManager & am)
{
	m_terrainShader = am.getShader(SHADER_SIMPLE_TERRAIN);
	m_terrainShader->use();
	m_terrainShader->setInt("terrTex", 0);
	m_testTexture = am.getTexture(TEXTURE_FIELD_GRASS, false);

	int textureX, textureY, textureChannel;
	unsigned char* data = stbi_load("ImageFolder/perlinTest.png", &textureX, &textureY, &textureChannel, 0);

	glm::vec3* vertices = new glm::vec3[m_gridVertexCount];
	m_HeightData = new float[m_gridVertexCount];
	glm::vec3* normals = new glm::vec3[m_gridVertexCount];
	glm::vec2* textures = new glm::vec2[m_gridVertexCount];
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
			unsigned Index = i + (m_terrainSubWidth + 1) * j;

			vertices[Index] =
				glm::vec3
				(
					m_terrainWidth * (i * invSubWidth - 0.5f),
					0,
					m_terrainDepth * (j * invSubDepth - 0.5f)
				) + m_terrainOrigin;

			unsigned color = data[(i * pixelWidth + j * pixelHeight * textureX) * textureChannel];
			float height = ((color / 255.f) - 0.5f) * m_terrainHeight;
			vertices[Index].y = height;
			m_HeightData[Index] = height;

			// Translate the vertices into the terrain origin
			vertices[Index] += m_terrainOrigin;
			

			
			textures[Index] = glm::vec2(i * invSubWidth, j * invSubDepth);
		}
	}

	// Normals
	for (unsigned j = 0; j <= m_terrainSubDepth; ++j)
	{
		for (unsigned i = 0; i <= m_terrainSubWidth; ++i)
		{
			int centerIndex = i + j * (m_terrainSubWidth + 1);
			int rightIndex = centerIndex + 1;
			int downIndex = centerIndex + (m_terrainSubWidth + 1);

			// Handle Edge Case.
			if (i == m_terrainSubWidth) rightIndex = centerIndex;
			if (j == m_terrainSubDepth) downIndex = centerIndex;

			float centerV = vertices[centerIndex].y;
			float rightV = vertices[rightIndex].y;
			float downV = vertices[downIndex].y;

			// calculate the normal using the gradient vector of partial derivative math
			normals[centerIndex] = glm::normalize(glm::vec3((rightV - centerV) *  -1.f, 1.f, (downV - centerV) * -1.f));
		}
	}

	// Indices
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

	glm::vec3* vertices = new glm::vec3[m_gridVertexCount];
	m_HeightData = new float[m_gridVertexCount];
	glm::vec3* normals = new glm::vec3[m_gridVertexCount];
	glm::vec2* textures = new glm::vec2[m_gridVertexCount];
	unsigned* indices = new unsigned[m_indicesCount];

	float invSubWidth = 1.f / m_terrainSubWidth;
	float invSubDepth = 1.f / m_terrainSubDepth;

	// prepare the grid and other informations
	CGPerlinNoise perlin(20190101);
	for (unsigned j = 0; j <= m_terrainSubDepth; ++j)
	{
		for (unsigned i = 0; i <= m_terrainSubWidth; ++i)
		{
			unsigned Index = i + (m_terrainSubWidth + 1) * j;

			vertices[Index] =
				glm::vec3
				(
					m_terrainWidth * (i * invSubWidth - 0.5f),
					0,
					m_terrainDepth * (j * invSubDepth - 0.5f)
				);

			glm::vec3 derivs;
			glm::vec3 p = vertices[Index];
			p *= 0.24;
			float perlinHeight = (perlin.eval(p, derivs) + 1.f) * 0.5f;
			perlinHeight = (perlinHeight - 0.5f) * m_terrainHeight;
			vertices[Index].y = perlinHeight;
			m_HeightData[Index] = perlinHeight;

			// Translate the vertices into the terrain origin
			vertices[Index] += m_terrainOrigin;
			

			// Analytical normal from partial derivative
			normals[Index] = glm::normalize(glm::vec3(-derivs.x, 1.f, -derivs.z));

			textures[Index] = glm::vec2(i * invSubWidth, j * invSubDepth);
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

void CGProj::CGTerrain::initializePhysics()
{
	m_localAABB.min = glm::vec3(m_terrainWidth * -0.5, 0, m_terrainDepth * -0.5);
	m_localAABB.max = m_localAABB.min * -1.f;

	// Find min/max height to make fitAABB of the terrain
	float minHeight = FLT_MAX;
	float maxHeight = -FLT_MAX;
	for (unsigned i = 0; i < m_gridVertexCount; ++i)
	{
		float height = m_HeightData[i];

		if (minHeight > height) minHeight = height;
		if (maxHeight < height) maxHeight = height;
	}

	m_localAABB.min.y = minHeight;
	m_localAABB.max.y = maxHeight;
	// Find min/max height to make fitAABB of the terrain

	// Collision Primitive Init
	m_collisionPrimitive.m_meshOrigin = m_terrainOrigin;
	m_collisionPrimitive.m_meshWidth = m_terrainWidth;
	m_collisionPrimitive.m_meshDepth = m_terrainDepth;
	m_collisionPrimitive.m_meshSubWidth = m_terrainSubWidth;
	m_collisionPrimitive.m_meshSubDepth = m_terrainSubDepth;
	m_collisionPrimitive.m_heightData = m_HeightData;
	m_collisionPrimitive.m_heightDataNumb = m_gridVertexCount;
}

float CGProj::CGTerrain::getHeight(unsigned x, unsigned z)
{
	return m_HeightData[x + (m_terrainSubWidth + 1) * z];
}
