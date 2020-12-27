#pragma once
#ifndef __CG_MODEL_H__
#define __CG_MODEL_H__

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Graphics/CGModelMesh.h>

namespace CG
{
	// Forward Declaration
	class Shader;

	/* 2019-01-16 Chanhaeng Lee
	 * This is the class where you can load model, using the asset manager
	 * Also This model is used by Chan Game Deferred Renderer.
	 * That's the reason why this class has deferredFirstRender.
	 * So, The rendering process of it is made for the purpose of Deferred Rendering.
	 */
	class CGModel
	{
	public:
		bool m_isLoaded;
		
		std::string m_directory;

		CGModel();
		CGModel(const char* path);
		void destroy();

		bool loadModel(unsigned maxInstanceNumb);
		void setInstanceData(const std::vector<glm::mat4>& model, const std::vector<glm::mat4>& worldNormal);
		void setInstanceData(const std::vector<glm::mat4>& model);
		void deferredFirstRender(Shader* shader, unsigned instanceNumb);
		void shadowFirstRender();

		unsigned getMaxInstanceNumb();

		const std::vector<CGModelMesh>& GetMeshes() const
		{
			return m_meshes;
		}

	private:
		/* Model Data */
		std::vector<CGModelMesh> m_meshes;
		std::vector<Texture> m_texture_loaded;

		// Instnacing
		unsigned m_maxInstancingNumb;

		/* Functions */
		void processNode(aiNode* node, const aiScene* scene);
		CGModelMesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	};
}

#endif