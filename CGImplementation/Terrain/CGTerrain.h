#pragma once
#ifndef __CG_TERRAIN_H__
#define __CG_TERRAIN_H__

#include <Graphics/CGAssetManager.h>
#include <Terrain/CGPerlinNoise.h>
#include <Graphics/CGSizableRenderLine.h>
#include <Graphics/CGRenderLine.h>

namespace CGProj
{
	class CGTerrain
	{
	public:
		CGTerrain();
		
		void initializeWithImage(CGAssetManager& am);
		void initializeWithGenerator(CGAssetManager& am);
		void render(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& campos);
		
		void destroy();
	private:
		unsigned m_terrainWidth, m_terrainHeight, m_terrainDepth;
		unsigned m_terrainSubWidth, m_terrainSubDepth;
		
		unsigned m_gridVertexCount;
		unsigned m_indicesCount;

		unsigned m_VAO;
		unsigned m_VBO[3];
		unsigned m_EBO;

		Shader* m_terrainShader;

		unsigned m_testTexture;
	};
}

#endif