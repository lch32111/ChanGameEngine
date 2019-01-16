#pragma once
#ifndef __CG_MODEL_MESH_H__
#define __CG_MODEL_MESH_H__

#include <glm/glm.hpp>
#include <string>
#include <vector>

// Forward Declaration
class Shader;

namespace CGProj
{
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
		CGModelMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures);
		void destroy();
		std::vector<Vertex> m_vertices;
		std::vector<unsigned> m_indices;
		std::vector<Texture> m_textures;

		// Deferred Rendering 
		void deferredFirstRender(Shader* shader);
		
		unsigned m_VAO, m_VBO, m_EBO;
		void setupMesh();
	};
}

#endif