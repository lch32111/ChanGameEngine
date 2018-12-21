#pragma once
#ifndef __CG_EDIT_POINT_LIGHT_H__
#define __CG_EDIT_POINT_LIGHT_H__

#include <Graphics/Shader.h>
#include <Graphics/CGAssetManager.h>
#include <Graphics/CGEditLightCommonFactor.h>

namespace CGProj
{
	/** 2018-12-19, Chanhaeng Lee
	* This Class includes variables
	* related to the point light component.
	* Because The CGEditLightObject class is getting bigger,
	* I thought that It's needed to separate the components of each light type.
	* It means These kinds of light class are designed
	* to be used in CGEditLightObject Class.
	* That's the reason why this class is not inherited from CGEditLightObject
	*/
	class CGEditPointLight
	{
	public:
		// Constructor to init all member variables to prevent 
		// from accessing to the garbage value
		CGEditPointLight();

		void initialize(CGAssetManager& am, CGEditLightCommonFactor* factor);

		void debugDepthMapRender();

		void UIrenderForCommon();
		void UIrenderForShadow();

		void setLightPropertyOnShader(
			Shader* shader,
			const std::string& sLightIndex,
			const std::string& sShadowIndex,
			const unsigned shadowIndex);

		void renderShadowMap(std::vector<CGEditProxyObject>& objects);
	private:
		CGEditLightCommonFactor* m_lightFactors;
		void updateRadius();

		unsigned m_depthMapFBO, m_depthCubemap;
		unsigned m_shadowWidth, m_shadowHeight;
	};
}

#endif