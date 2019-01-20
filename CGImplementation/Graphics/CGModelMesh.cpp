#include <Graphics/CGModelMesh.h>

#include <glad/glad.h>

#include <CGErrorLogger.h>
#include <Graphics/Shader.h>
#include <Graphics/CGModelUtils.h>

CGProj::CGModelMesh::CGModelMesh(
	std::vector<Vertex>& vertices, 
	std::vector<unsigned int>& indices, 
	std::vector<Texture>& textures,
	unsigned maxInstancingNumb)
	:
	m_vertices(vertices),
	m_indices(indices),
	m_textures(textures),
	m_maxInstancingNumb(maxInstancingNumb)
{
	setupMesh();
	setupInstancing();
}

void CGProj::CGModelMesh::destroy()
{
	glDeleteBuffers(1, &m_instanceModelVBO);
	glDeleteBuffers(1, &m_instanceWorldNormalVBO);
	
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);

	glDeleteVertexArrays(1, &m_VAO);
}

void CGProj::CGModelMesh::setInstanceData(const std::vector<glm::mat4>& model, const std::vector<glm::mat4>& worldNormal)
{
	m_currentInstanceNumb = model.size();

	glBindBuffer(GL_ARRAY_BUFFER, m_instanceModelVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * m_currentInstanceNumb, glm::value_ptr(model[0]));

	glBindBuffer(GL_ARRAY_BUFFER, m_instanceWorldNormalVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * m_currentInstanceNumb, glm::value_ptr(worldNormal[0]));
}

void CGProj::CGModelMesh::setInstanceData(const std::vector<glm::mat4>& model)
{
	m_currentInstanceNumb = model.size();
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceModelVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * m_currentInstanceNumb, glm::value_ptr(model[0]));
}

void CGProj::CGModelMesh::deferredFirstRender(Shader* shader, unsigned instanceNumb)
{
	// Deferred First Pass Shader already set the texture unit number
	// CGModel class already set the transform matrix before this scope
	
	// The key to distinguish which textures should be set on the flags
	unsigned int texture_key = 0;
	for (unsigned int i = 0; i < m_textures.size(); ++i)
	{
		// pack the all information related to textures
		texture_key = texture_key | m_textures[i].key;
		glActiveTexture(GL_TEXTURE0 + getIndexFromKey(m_textures[i].key)); // active proper texture unit before binding
		glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
	}
	
	for (unsigned int i = 0; i < materialKeyList.size(); ++i)
	{
		if (materialKeyList[i] & texture_key)
		{
			shader->setBool("material.is" + materialList[i], true);
		}
		else
		{
			shader->setBool("material.is" + materialList[i], false);
		}
	}

	// Check if the shader should calculate TBN Matrix
	if ((texture_key & materialKeyList[CG_MODEL_MATERIAL_ENUM::NORMALMAP]) ||
		(texture_key & materialKeyList[CG_MODEL_MATERIAL_ENUM::DEPTHMAP]))
		shader->setBool("IsUseTangentSpace", true);
	else
		shader->setBool("IsUseTangentSpace", false);

	// Draw Mesh
	glBindVertexArray(m_VAO);
	// glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0); glCheckError();
	glDrawElementsInstanced(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0, instanceNumb); glCheckError();

	glBindVertexArray(0);
}
void CGProj::CGModelMesh::shadowFirstRender()
{
	// Draw Mesh
	glBindVertexArray(m_VAO);
	glDrawElementsInstanced(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0, m_currentInstanceNumb); glCheckError();

	glBindVertexArray(0);
}


void CGProj::CGModelMesh::setupMesh()
{
	size_t stride = sizeof(Vertex);

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * stride, &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

	// vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	// vertex normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, Vertex::Normal));
	glEnableVertexAttribArray(1);

	// vertex texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, Vertex::TexCoords));
	glEnableVertexAttribArray(2);

	// vertex tangent
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, Vertex::Tangent));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glCheckError();
}

void CGProj::CGModelMesh::setupInstancing()
{
	glBindVertexArray(m_VAO);

	// Allocate Memory
	glGenBuffers(1, &m_instanceModelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceModelVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxInstancingNumb * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);

	glGenBuffers(1, &m_instanceWorldNormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceWorldNormalVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxInstancingNumb * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	// Allocate Memory

	GLsizei vec4Size = sizeof(glm::vec4);

	// Refer to the DeferredFirst.vs Shader!
	// Model Matrix Instance Vertex Attribute Setting by manual setting 
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceModelVBO);
	for (int i = 4; i < 4 + 4; ++i)
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size * (i - 4)));
		glVertexAttribDivisor(i, 1);
	}

	// World Normal Matrix Instance Vertex Attribute Setting by manual setting 
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceWorldNormalVBO);
	for (int i = 8; i < 8 + 4; ++i)
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size * (i - 8)));
		glVertexAttribDivisor(i, 1);
	}

	glBindVertexArray(0);
}
