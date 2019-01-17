#include <Graphics/CGModelMesh.h>

#include <glad/glad.h>

#include <CGErrorLogger.h>
#include <Graphics/Shader.h>
#include <Graphics/CGModelUtils.h>

CGProj::CGModelMesh::CGModelMesh(
	std::vector<Vertex>& vertices, 
	std::vector<unsigned int>& indices, 
	std::vector<Texture>& textures)
	:
	m_vertices(vertices),
	m_indices(indices),
	m_textures(textures)
{
	setupMesh();
}

void CGProj::CGModelMesh::destroy()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}

void CGProj::CGModelMesh::deferredFirstRender(Shader* shader)
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
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0); glCheckError();

	glBindVertexArray(0);
}
void CGProj::CGModelMesh::shadowFirstRender()
{
	// Draw Mesh
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0); glCheckError();

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
