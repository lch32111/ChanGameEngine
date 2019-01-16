#include "CGAssetManager.h"



Shader * CGProj::CGAssetManager::getShader(CG_SHADER_ENUM _shaderEnum)
{
	if (m_shaders[_shaderEnum].isloadad == false)
	{
		if (m_shaders[_shaderEnum].loadShader() == false)
		{
			std::cout << "CG_ASSET_MANAGER::GETSHADER::The number of Shader Enum : " << _shaderEnum << '\n';
			return nullptr;
		}
	}

	return &m_shaders[_shaderEnum];
}

Shader CGProj::CGAssetManager::getShader(CG_SHADER_ENUM _shaderEnum, char none = 0)
{
	if (m_shaders[_shaderEnum].isloadad == false)
	{
		if (m_shaders[_shaderEnum].loadShader() == false)
		{
			std::cout << "CG_ASSET_MANAGER::GETSHADER::The number of Shader Enum : " << _shaderEnum << '\n';
			return Shader();
		}
	}

	return m_shaders[_shaderEnum];
}

Shader * CGProj::CGAssetManager::getGeoShader(CG_GEO_SHADER_ENUM _shaderEnum)
{
	if (m_geoShaders[_shaderEnum].isloadad == false)
	{
		if (m_geoShaders[_shaderEnum].loadShaderWithGeo() == false)
		{
			std::cout << "CG_ASSET_MANAGER::GETGEOSHADER::The number of Shader Enum : " << _shaderEnum << '\n';
			return nullptr;
		}
	}

	return &m_geoShaders[_shaderEnum];
}

Shader CGProj::CGAssetManager::getGeoShader(CG_GEO_SHADER_ENUM _shaderEnum, char none = 0)
{
	if (m_geoShaders[_shaderEnum].isloadad == false)
	{
		if (m_geoShaders[_shaderEnum].loadShaderWithGeo() == false)
		{
			std::cout << "CG_ASSET_MANAGER::GETGEOSHADER::The number of Shader Enum : " << _shaderEnum << '\n';
			return Shader();
		}
	}

	return m_geoShaders[_shaderEnum];
}

unsigned CGProj::CGAssetManager::getTexture(CG_TEXTURE_ENUM _textureEnum, bool gamma)
{
	// TODO: make texture class for on-demand process. TextureFromFile() is just loading the image...
	if (gamma) return m_GammaTextures[_textureEnum];
	else return m_NoGammaTexture[_textureEnum];
}

CGProj::CGModel* CGProj::CGAssetManager::getModelData(CG_MODEL_ENUM _modelEnum)
{
	if (m_models[_modelEnum].m_isLoaded == false)
	{
		if (m_models[_modelEnum].loadModel() == false)
		{
			std::cout << "CHAN_ASSET_MANAGER::GETMODEL::The number of Model Enum : " << _modelEnum << '\n';
			return nullptr;
		}
	}

	return &m_models[_modelEnum];
}
