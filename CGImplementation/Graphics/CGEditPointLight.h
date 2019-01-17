#pragma once
#ifndef __CG_EDIT_POINT_LIGHT_H__
#define __CG_EDIT_POINT_LIGHT_H__

#include <vector>

#include <Graphics/Shader.h>
#include <Graphics/CGEditLightCommonFactor.h>

namespace CGProj
{
	// Forward Declartion
	class CGAssetManager;
	class CGEditProxyObject;

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

		void debugDepthMapRender(const glm::mat4& view, const glm::mat4& proj);

		void UIrenderForCommon();
		void UIrenderForShadow();

		void setLightPropertyOnShader(
			Shader* shader,
			const std::string& sLightIndex,
			const std::string& sShadowIndex,
			const unsigned shadowIndex);

		void renderShadowMap(std::vector<CGEditProxyObject>& objects);

		float getShadowFarPlane();
		float getShadowNearPlane();
	private:
		CGEditLightCommonFactor* m_lightFactors;
		void updateRadius();

		/*** Shadow Properties ***/
		Shader* m_DepthMapShader;
		Shader* m_DebugDepthMapShader;

		unsigned m_depthMapFBO, m_depthCubemap;
		unsigned m_shadowWidth, m_shadowHeight;

		float m_perFOV;
		float m_perAspect;
		float m_shadowNearPlane;
		float m_shadowFarPlane;
		float m_shadowBias;

		glm::mat4 m_shadowTransforms[6];
		glm::mat4 m_shadowLightProjection;
		/*** Shadow Properties ***/

		void setShadowWidthHeight(unsigned wh);
		void setShadowFarPlane(float farP);
	};
}

#endif