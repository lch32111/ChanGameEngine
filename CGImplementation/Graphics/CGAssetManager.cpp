#include <CGPrecompiled.h>
#include <Graphics/CGAssetManager.h>
#include <CGErrorLogger.h>

CG::Shader * CG::CGAssetManager::getShader(CG_SHADER_ENUM _shaderEnum)
{
	if (m_shaders[_shaderEnum].isloadad == false)
	{
		if (m_shaders[_shaderEnum].loadShader() == false)
		{
			std::cout << "CG_ASSET_MANAGER::GETSHADER::The number of Shader Enum : " << _shaderEnum << '\n';
			CGassert();
			return nullptr;
		}
	}

	return &m_shaders[_shaderEnum];
}

CG::Shader CG::CGAssetManager::getShader(CG_SHADER_ENUM _shaderEnum, char none = 0)
{
	if (m_shaders[_shaderEnum].isloadad == false)
	{
		if (m_shaders[_shaderEnum].loadShader() == false)
		{
			std::cout << "CG_ASSET_MANAGER::GETSHADER::The number of Shader Enum : " << _shaderEnum << '\n';
			CGassert();
			return Shader();
		}
	}

	return m_shaders[_shaderEnum];
}

CG::Shader * CG::CGAssetManager::getGeoShader(CG_GEO_SHADER_ENUM _shaderEnum)
{
	if (m_geoShaders[_shaderEnum].isloadad == false)
	{
		if (m_geoShaders[_shaderEnum].loadShaderWithGeo() == false)
		{
			std::cout << "CG_ASSET_MANAGER::GETGEOSHADER::The number of Shader Enum : " << _shaderEnum << '\n';
			CGassert();
			return nullptr;
		}
	}

	return &m_geoShaders[_shaderEnum];
}

CG::Shader CG::CGAssetManager::getGeoShader(CG_GEO_SHADER_ENUM _shaderEnum, char none = 0)
{
	if (m_geoShaders[_shaderEnum].isloadad == false)
	{
		if (m_geoShaders[_shaderEnum].loadShaderWithGeo() == false)
		{
			std::cout << "CG_ASSET_MANAGER::GETGEOSHADER::The number of Shader Enum : " << _shaderEnum << '\n';
			CGassert();
			return Shader();
		}
	}

	return m_geoShaders[_shaderEnum];
}

unsigned CG::CGAssetManager::getTexture(CG_TEXTURE_ENUM _textureEnum, bool gamma)
{
	// TODO: make texture class for on-demand process. CGAssetTexture() is just loading the image...
	if (gamma)
	{
		if (m_GammaTextures[_textureEnum].m_isloaded == false)
		{
			if (m_GammaTextures[_textureEnum].loadTexture(gamma) == false)
			{
				CGassert();
			}
		}

		return m_GammaTextures[_textureEnum].m_texture;
	}
	else
	{
		if (m_NoGammaTexture[_textureEnum].m_isloaded == false)
		{
			if (m_NoGammaTexture[_textureEnum].loadTexture(gamma) == false)
			{
				CGassert();
			}
		}

		return m_NoGammaTexture[_textureEnum].m_texture;
	}

	return -1;
}

CG::CGModel* CG::CGAssetManager::getModelData(CG_MODEL_ENUM _modelEnum, unsigned maxInstanceNumb)
{
	if (m_models[_modelEnum].m_isLoaded == false)
	{
		if (m_models[_modelEnum].loadModel(maxInstanceNumb) == false)
		{
			std::cout << "CHAN_ASSET_MANAGER::GETMODEL::The number of Model Enum : " << _modelEnum << '\n';
			CGassert();
			return nullptr;
		}
	}

	return &m_models[_modelEnum];
}

// Manaual Setting
void CG::CGAssetManager::assetInit()
{
	m_shaders[SHADER_DEFERRED_FIRST] = Shader("ShaderFolder/DeferredFirst.vs", "ShaderFolder/DeferredFirst.fs");
	m_shaders[SHADER_DEFERRED_SECOND] = Shader("ShaderFolder/DeferredSecond.vs", "ShaderFolder/DeferredSecond.fs");
	m_shaders[SHADER_DEFERRED_POST] = Shader("ShaderFolder/DeferredPost.vs", "ShaderFolder/DeferredPost.fs");
	m_shaders[SHADER_GAUSSIAN_BLUR] = Shader("ShaderFolder/CGGaussianBlur.vs", "ShaderFolder/CGGaussianBlur.fs");
	m_shaders[SHADER_SSAO_EFFECT] = Shader("ShaderFolder/CGSSAOEffect.vs", "ShaderFolder/CGSSAOEffect.fs");
	m_shaders[SHADER_SSAO_BLUR] = Shader("ShaderFolder/CGSSAOBlur.vs", "ShaderFolder/CGSSAOBlur.fs");
	m_shaders[SHADER_SSAO_DEBUG] = Shader("ShaderFolder/CGSSAODebug.vs", "ShaderFolder/CGSSAODebug.fs");
	m_shaders[SHADER_CG_LINE] = Shader("ShaderFolder/CGLineShader.vs", "ShaderFolder/CGLineShader.fs");
	m_shaders[SHADER_SIMPLE_RENDER] = Shader("ShaderFolder/simpleRender.vs", "ShaderFolder/simpleRender.fs");
	m_shaders[SHADER_SIMPLE_COLOR_RENDER] = Shader("ShaderFolder/simpleColorRender.vs", "ShaderFolder/simpleColorRender.fs");
	m_shaders[SHADER_WIRE_RENDER] = Shader("ShaderFolder/wireRender.vs", "ShaderFolder/wireRender.fs");
	m_shaders[SHADER_DIR_VISUALIZER] = Shader("ShaderFolder/CGDirVisualizer.vs", "ShaderFolder/CGDirVisualizer.fs");
	m_shaders[SHADER_POINT_VISUALIZER] = Shader("ShaderFolder/CGPointVisualizer.vs", "ShaderFolder/CGPointVisualizer.fs");
	m_shaders[SHADER_SPOT_VISUALIZER] = Shader("ShaderFolder/CGSpotVisualizer.vs", "ShaderFolder/CGSpotVisualizer.fs");
	m_shaders[SHADER_DIR_SHADOW_MAP] = Shader("ShaderFolder/CGDirDepthMap.vs", "ShaderFolder/CGDirDepthMap.fs");
	m_shaders[SHADER_INSTANCE_DIR_SHADOW_MAP] = Shader("ShaderFolder/CGInstanceDirDepthMap.vs", "ShaderFolder/CGInstanceDirDepthMap.fs");
	m_shaders[SHADER_DIR_SHADOW_MAP_DEBUG_RENDER] = Shader("ShaderFolder/CGDirDepthMapDebugRender.vs", "ShaderFolder/CGDirDepthMapDebugRender.fs");
	m_shaders[SHADER_POINT_SHADOW_MAP_DEBUG_RENDER] = Shader("ShaderFolder/CGPointDepthMapDebugRender.vs", "ShaderFolder/CGPointDepthMapDebugRender.fs");
	m_shaders[SHADER_SPOT_SHADOW_MAP] = Shader("ShaderFolder/CGSpotDepthMap.vs", "ShaderFolder/CGSpotDepthMap.fs");
	m_shaders[SHADER_INSTANCE_SPOT_SHADOW_MAP] = Shader("ShaderFolder/CGInstanceSpotDepthMap.vs", "ShaderFolder/CGInstanceSpotDepthMap.fs");
	m_shaders[SHADER_SPOT_SHADOW_MAP_DEBUG_RENDER] = Shader("ShaderFolder/CGSpotDepthMapDebugRender.vs", "ShaderFolder/CGSpotDepthMapDebugRender.fs");
	m_shaders[SHADER_SIMPLE_TERRAIN] = Shader("ShaderFolder/CGSimpleTerrain.vs", "ShaderFolder/CGSimpleTerrain.fs");

	m_geoShaders[SHADER_GEO_POINT_SHADOW_MAP] = Shader("ShaderFolder/CGPointDepthMap.vs", "ShaderFolder/CGPointDepthMap.gs", "ShaderFolder/CGPointDepthMap.fs");
	m_geoShaders[SHADER_GEO_INSTANCE_POINT_SHADOW_MAP] = Shader("ShaderFolder/CGInstancePointDepthMap.vs", "ShaderFolder/CGInstancePointDepthMap.gs", "ShaderFolder/CGInstancePointDepthMap.fs");

	// TODO: make texture class for on-demand process. CGAssetTexture() is just loading the image...
	m_NoGammaTexture[TEXTURE_CONTAINER_DIFFUSE] = CGAssetTexture("ResourceFolder/ImageFolder/container2.png");
	m_GammaTextures[TEXTURE_CONTAINER_DIFFUSE] = CGAssetTexture("ResourceFolder/ImageFolder/container2.png");

	m_NoGammaTexture[TEXTURE_CONTAINER_SPECULAR] = CGAssetTexture("ResourceFolder/ImageFolder/container2_specular.png");
	m_GammaTextures[TEXTURE_CONTAINER_SPECULAR] = CGAssetTexture("ResourceFolder/ImageFolder/container2_specular.png");

	m_NoGammaTexture[TEXTURE_FIELD_GRASS] = CGAssetTexture("ResourceFolder/ImageFolder/fieldGrass.jpg");
	m_GammaTextures[TEXTURE_FIELD_GRASS] = CGAssetTexture("ResourceFolder/ImageFolder/fieldGrass.jpg");

	m_NoGammaTexture[TEXTURE_GOLD] = CGAssetTexture("ResourceFolder/ImageFolder/gold.jpg");
	m_GammaTextures[TEXTURE_GOLD] = CGAssetTexture("ResourceFolder/ImageFolder/gold.jpg");

	m_NoGammaTexture[TEXTURE_MARBLE] = CGAssetTexture("ResourceFolder/ImageFolder/marble.jpg");
	m_GammaTextures[TEXTURE_MARBLE] = CGAssetTexture("ResourceFolder/ImageFolder/marble.jpg");

	m_NoGammaTexture[TEXTURE_RED_MARBLE] = CGAssetTexture("ResourceFolder/ImageFolder/redMarble.jpg");
	m_GammaTextures[TEXTURE_RED_MARBLE] = CGAssetTexture("ResourceFolder/ImageFolder/redMarble.jpg");

	m_NoGammaTexture[TEXTURE_MATRIX] = CGAssetTexture("ResourceFolder/ImageFolder/matrix.jpg");
	m_GammaTextures[TEXTURE_MATRIX] = CGAssetTexture("ResourceFolder/ImageFolder/matrix.jpg");

	m_NoGammaTexture[TEXTURE_BLUE_MATRIX] = CGAssetTexture("ResourceFolder/ImageFolder/bluematrix.jpg");
	m_GammaTextures[TEXTURE_BLUE_MATRIX] = CGAssetTexture("ResourceFolder/ImageFolder/bluematrix.jpg");

	m_NoGammaTexture[TEXTURE_WOOD_PANEL] = CGAssetTexture("ResourceFolder/ImageFolder/woodpanel.png");
	m_GammaTextures[TEXTURE_WOOD_PANEL] = CGAssetTexture("ResourceFolder/ImageFolder/woodpanel.png");

	m_NoGammaTexture[TEXTURE_BRICK_WALL] = CGAssetTexture("ResourceFolder/ImageFolder/brickwall.jpg");
	m_GammaTextures[TEXTURE_BRICK_WALL] = CGAssetTexture("ResourceFolder/ImageFolder/brickwall.jpg");
	
	m_NoGammaTexture[TEXTURE_BRICK_WALL_NORMAL] = CGAssetTexture("ResourceFolder/ImageFolder/brickwall_normal.jpg");
	m_GammaTextures[TEXTURE_BRICK_WALL_NORMAL] = CGAssetTexture("ResourceFolder/ImageFolder/brickwall_normal.jpg");

	m_NoGammaTexture[TEXTURE_BRICK2] = CGAssetTexture("ResourceFolder/ImageFolder/bricks2.jpg");
	m_GammaTextures[TEXTURE_BRICK2] = CGAssetTexture("ResourceFolder/ImageFolder/bricks2.jpg");

	m_NoGammaTexture[TEXTURE_BRICK2_NORMAL] = CGAssetTexture("ResourceFolder/ImageFolder/bricks2_normal.jpg");
	m_GammaTextures[TEXTURE_BRICK2_NORMAL] = CGAssetTexture("ResourceFolder/ImageFolder/bricks2_normal.jpg");

	m_NoGammaTexture[TEXTURE_BRICK2_HEIGHT] = CGAssetTexture("ResourceFolder/ImageFolder/bricks2_disp.jpg");
	m_GammaTextures[TEXTURE_BRICK2_HEIGHT] = CGAssetTexture("ResourceFolder/ImageFolder/bricks2_disp.jpg");

	m_models[MODEL_NANO_SUIT] = CGModel("ResourceFolder/ModelFolder/nanosuit/nanosuit.obj");
	m_models[MODEL_SPONZA_BUILDING] = CGModel("ResourceFolder/ModelFolder/sponza/sponza.obj");
	m_models[MODEL_ROYAL_ROOSTER] = CGModel("ResourceFolder/ModelFolder/royal-rooster/rooster.dae");
	m_models[MODEL_BUNNY] = CGModel("ResourceFolder/ModelFolder/bunny/bunny.obj");
	m_models[MODEL_HOUSE_OBJ] = CGModel("ResourceFolder/ModelFolder/house_obj/house_obj.obj");
	m_models[MODEL_OLD_HOUSE] = CGModel("ResourceFolder/ModelFolder/old_house/house_01.obj");
}

void CG::CGAssetManager::destroy()
{
	for (unsigned i = 0; i < (unsigned)NUM_CG_MODEL_ENUM; ++i)
		m_models[i].destroy();
}