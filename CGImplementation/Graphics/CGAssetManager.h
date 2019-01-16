#pragma once
#ifndef __CG_ASSET_MANAGER_H__
#define __CG_ASSET_MANAGER_H__

#include <Graphics/Shader.h>
#include <Graphics/CGAssetUtil.h>
#include <Graphics/GLTextureUtility.h>
#include <Graphics/CGModel.h>

namespace CGProj
{
	/** 2018-11-27 Chanhaeng Lee 
	 * This class manages all of assets such as model, shader, texture and so on.
	 */
	class CGAssetManager
	{
	public:
		Shader* getShader(CG_SHADER_ENUM _shaderEnum);
		Shader getShader(CG_SHADER_ENUM _shaderEnum, char); // add a char parameter to distinguish pointer-return type
		Shader* getGeoShader(CG_GEO_SHADER_ENUM _shaderEnum);
		Shader getGeoShader(CG_GEO_SHADER_ENUM _shaderEnum, char);

		unsigned getTexture(CG_TEXTURE_ENUM _textureEnum, bool gamma);

		CGModel* getModelData(CG_MODEL_ENUM _modelEnum);

		// Manual Setting
		void assetInit()
		{
			m_shaders[SHADER_DEFERRED_FIRST] = Shader("ShaderFolder/DeferredFirst.vs", "ShaderFolder/DeferredFirst.fs");
			m_shaders[SHADER_DEFERRED_SECOND] = Shader("ShaderFolder/DeferredSecond.vs", "ShaderFolder/DeferredSecond.fs");
			m_shaders[SHADER_CG_LINE] = Shader("ShaderFolder/CGLineShader.vs", "ShaderFolder/CGLineShader.fs");
			m_shaders[SHADER_SIMPLE_RENDER] = Shader("ShaderFolder/simpleRender.vs", "ShaderFolder/simpleRender.fs");
			m_shaders[SHADER_SIMPLE_COLOR_RENDER] = Shader("ShaderFolder/simpleColorRender.vs", "ShaderFolder/simpleColorRender.fs");
			m_shaders[SHADER_WIRE_RENDER] = Shader("ShaderFolder/wireRender.vs", "ShaderFolder/wireRender.fs");
			m_shaders[SHADER_DIR_VISUALIZER] = Shader("ShaderFolder/CGDirVisualizer.vs", "ShaderFolder/CGDirVisualizer.fs");
			m_shaders[SHADER_POINT_VISUALIZER] = Shader("ShaderFolder/CGPointVisualizer.vs", "ShaderFolder/CGPointVisualizer.fs");
			m_shaders[SHADER_SPOT_VISUALIZER] = Shader("ShaderFolder/CGSpotVisualizer.vs", "ShaderFolder/CGSpotVisualizer.fs");
			m_shaders[SHADER_DIR_SHADOW_MAP] = Shader("ShaderFolder/CGDirDepthMap.vs", "ShaderFolder/CGDirDepthMap.fs");
			m_shaders[SHADER_DIR_SHADOW_MAP_DEBUG_RENDER] = Shader("ShaderFolder/CGDirDepthMapDebugRender.vs", "ShaderFolder/CGDirDepthMapDebugRender.fs");
			m_shaders[SHADER_POINT_SHADOW_MAP_DEBUG_RENDER] = Shader("ShaderFolder/CGPointDepthMapDebugRender.vs", "ShaderFolder/CGPointDepthMapDebugRender.fs");
			m_shaders[SHADER_SPOT_SHADOW_MAP] = Shader("ShaderFolder/CGSpotDepthMap.vs", "ShaderFolder/CGSpotDepthMap.fs");
			m_shaders[SHADER_SPOT_SHADOW_MAP_DEBUG_RENDER] = Shader("ShaderFolder/CGSpotDepthMapDebugRender.vs", "ShaderFolder/CGSpotDepthMapDebugRender.fs");
			m_shaders[SHADER_SIMPLE_TERRAIN] = Shader("ShaderFolder/CGSimpleTerrain.vs", "ShaderFolder/CGSimpleTerrain.fs");

			m_geoShaders[SHADER_GEO_POINT_SHADOW_MAP] = Shader("ShaderFolder/CGPointDepthMap.vs", "ShaderFolder/CGPointDepthMap.gs", "ShaderFolder/CGPointDepthMap.fs");

			// TODO: make texture class for on-demand process. TextureFromFile() is just loading the image...
			m_NoGammaTexture[TEXTURE_CONTAINER_DIFFUSE] = TextureFromFile("ResourceFolder/ImageFolder/container2.png", false);
			m_GammaTextures[TEXTURE_CONTAINER_DIFFUSE] = TextureFromFile("ResourceFolder/ImageFolder/container2.png", true);

			m_NoGammaTexture[TEXTURE_CONTAINER_SPECULAR] = TextureFromFile("ResourceFolder/ImageFolder/container2_specular.png", false);
			m_GammaTextures[TEXTURE_CONTAINER_SPECULAR] = TextureFromFile("ResourceFolder/ImageFolder/container2_specular.png", true);

			m_NoGammaTexture[TEXTURE_FIELD_GRASS] = TextureFromFile("ResourceFolder/ImageFolder/fieldGrass.jpg", false);
			m_GammaTextures[TEXTURE_FIELD_GRASS] = TextureFromFile("ResourceFolder/ImageFolder/fieldGrass.jpg", true);

			m_NoGammaTexture[TEXTURE_GOLD] = TextureFromFile("ResourceFolder/ImageFolder/gold.jpg", false);
			m_GammaTextures[TEXTURE_GOLD] = TextureFromFile("ResourceFolder/ImageFolder/gold.jpg", true);
			
			m_NoGammaTexture[TEXTURE_MARBLE] = TextureFromFile("ResourceFolder/ImageFolder/marble.jpg", false);
			m_GammaTextures[TEXTURE_MARBLE] = TextureFromFile("ResourceFolder/ImageFolder/marble.jpg", true);

			m_NoGammaTexture[TEXTURE_RED_MARBLE] = TextureFromFile("ResourceFolder/ImageFolder/redMarble.jpg", false);
			m_GammaTextures[TEXTURE_RED_MARBLE] = TextureFromFile("ResourceFolder/ImageFolder/redMarble.jpg", true);

			m_NoGammaTexture[TEXTURE_MATRIX] = TextureFromFile("ResourceFolder/ImageFolder/matrix.jpg", false);
			m_GammaTextures[TEXTURE_MATRIX] = TextureFromFile("ResourceFolder/ImageFolder/matrix.jpg", true);

			m_NoGammaTexture[TEXTURE_BLUE_MATRIX] = TextureFromFile("ResourceFolder/ImageFolder/bluematrix.jpg", false);
			m_GammaTextures[TEXTURE_BLUE_MATRIX] = TextureFromFile("ResourceFolder/ImageFolder/bluematrix.jpg", true);

			m_NoGammaTexture[TEXTURE_WOOD_PANEL] = TextureFromFile("ResourceFolder/ImageFolder/woodpanel.png", false);
			m_GammaTextures[TEXTURE_WOOD_PANEL] = TextureFromFile("ResourceFolder/ImageFolder/woodpanel.png", true);

			m_models[MODEL_NANO_SUIT] = CGModel("ResourceFolder/ModelFolder/nanosuit/nanosuit.obj");
		}

		void destroy()
		{
			for (unsigned i = 0; i < (unsigned)NUM_CG_MODEL_ENUM; ++i)
				m_models[i].destroy();
		}
	private:
		Shader m_shaders[NUM_CG_SHADER_ENUM];
		Shader m_geoShaders[NUM_CG_GEO_SHADER_ENUM];

		unsigned m_NoGammaTexture[NUM_CG_TEXTURE_ENUM];
		unsigned m_GammaTextures[NUM_CG_TEXTURE_ENUM];

		CGModel m_models[NUM_CG_MODEL_ENUM];
	};
}

#endif