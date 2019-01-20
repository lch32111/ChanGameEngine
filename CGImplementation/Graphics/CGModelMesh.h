#pragma once
#ifndef __CG_MODEL_MESH_H__
#define __CG_MODEL_MESH_H__

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>


namespace CGProj
{
	class Shader;
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
	};

	struct Texture
	{
		unsigned int id;

		/*
		the key to recognize the type
		key construction is..
		diffuse : (1 << (int)('d' -'a')) == 8
		specular : (1 << (int)('s' - 'a')) == 262144
		emissive : (1 << (int)('e' - 'a')) == 16
		reflect : (1 << (int)('r' - 'a')) == 131072
		*/
		int key;
		std::string path;
	};

	class CGModelMesh
	{
	public:
		// Mesh Data
		CGModelMesh(
			std::vector<Vertex>& vertices, 
			std::vector<unsigned int>& indices, 
			std::vector<Texture>& textures,
			unsigned maxInstancingNumb);
		void destroy();
		std::vector<Vertex> m_vertices;
		std::vector<unsigned> m_indices;
		std::vector<Texture> m_textures;

		// Deferred Rendering 
		void setInstanceData(const std::vector<glm::mat4>& model, const std::vector<glm::mat4>& worldNormal);
		void setInstanceData(const std::vector<glm::mat4>& model);
		void deferredFirstRender(Shader* shader, unsigned instanceNumb);
		void shadowFirstRender();
		
	private:
		unsigned m_VAO, m_VBO, m_EBO;
		unsigned m_instanceModelVBO;
		unsigned m_instanceWorldNormalVBO;
		unsigned m_maxInstancingNumb;

		unsigned m_currentInstanceNumb;

		void setupMesh();
		void setupInstancing();
	};
}

#endif