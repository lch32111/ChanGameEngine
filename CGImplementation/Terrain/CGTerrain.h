#pragma once
#ifndef __CG_TERRAIN_H__
#define __CG_TERRAIN_H__

#include <Terrain/CGPerlinNoise.h>

#include <Graphics/CGAssetManager.h>
#include <Graphics/CGSizableRenderLine.h>
#include <Graphics/CGRenderLine.h>

#include <GPED/GPED_Precision.h>

namespace CGProj
{
	class CGTerrain
	{
	public:
		CGTerrain();
		
		// Init & DeInit
		void initialize(bool imageOrGenerator, CGAssetManager& am); // true - image / false generator
		void destroy();

		// Rendering
		void render(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& campos);
		
		// Physics
		unsigned getProxyId();
		void setProxyId(unsigned id);

		GPED::c3AABB getAABB();
		void getAABB(GPED::c3AABB& out);
	private:
		// Common
		unsigned m_terrainWidth, m_terrainHeight, m_terrainDepth;
		unsigned m_terrainSubWidth, m_terrainSubDepth;

		float* m_HeightData;
	private:
		// Graphics Area
		void initializeWithImage(CGAssetManager& am);
		void initializeWithGenerator(CGAssetManager& am);

		unsigned m_gridVertexCount;
		unsigned m_indicesCount;

		unsigned m_VAO;
		unsigned m_VBO[3];
		unsigned m_EBO;

		Shader* m_terrainShader;

		unsigned m_testTexture;
	private:
		// Physics Area
		void initializePhysics();

		unsigned m_proxyId;
		GPED::c3AABB m_localAABB;

		float getHeight(unsigned x, unsigned z);
	};
}

#endif