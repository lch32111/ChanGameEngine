#pragma once
#ifndef __CG_TERRAIN_H__
#define __CG_TERRAIN_H__

#include <Graphics/CGAssetManager.h>

namespace CGProj
{
	class CGTerrain
	{
	public:
		CGTerrain();
		
		void initialize(CGAssetManager& am);
		void render(const glm::mat4& view, const glm::mat4& proj);
		
		void destroy();
	private:
		unsigned m_terrainWidth, m_terrainHeight;
		unsigned m_vertexCount;
		unsigned m_indexCount;

		unsigned m_VAO;
		unsigned m_VBO;
		unsigned m_EBO;

		Shader* m_lineShader;
	};
}

#endif