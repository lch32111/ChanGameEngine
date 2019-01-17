#pragma once
#ifndef __CG_ASSET_MANAGER_H__
#define __CG_ASSET_MANAGER_H__

#include <Graphics/CGAssetUtil.h>

#include <Graphics/Shader.h>
#include <Graphics/CGModel.h>
#include <Graphics/CGAssetTexture.h>

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

		void assetInit();
		void destroy();
	private:
		Shader m_shaders[NUM_CG_SHADER_ENUM];
		Shader m_geoShaders[NUM_CG_GEO_SHADER_ENUM];

		CGAssetTexture m_NoGammaTexture[NUM_CG_TEXTURE_ENUM];
		CGAssetTexture m_GammaTextures[NUM_CG_TEXTURE_ENUM];

		CGModel m_models[NUM_CG_MODEL_ENUM];
	};
}

#endif