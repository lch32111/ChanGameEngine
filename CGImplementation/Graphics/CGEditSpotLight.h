#pragma once
#ifndef __CG_EDIT_SPOT_LIGHT_H__
#define __CG_EDIT_SPOT_LIGHT_H__

#include <Graphics/Shader.h>
#include <Graphics/CGAssetManager.h>
#include <Graphics/CGEditLightCommonFactor.h>
#include <Graphics/CGEditSpotLightVisualizer.h>
#include <Graphics/CGEditProxyObject.h>
#include <Graphics/CGFrustum.h>

namespace CGProj
{
	/** 2018-12-19, Chanhaeng Lee
	* This Class includes variables
	* related to the spot light component.
	* Because The CGEditLightObject class is getting bigger,
	* I thought that It's needed to separate the components of each light type.
	* It means These kinds of light class are designed
	* to be used in CGEditLightObject Class.
	* That's the reason why this class is not inherited from CGEditLightObject
	*/
	class CGEditSpotLight
	{
	public:
		// Constructor to init all member variables to prevent 
		// from accessing to the garbage value
		CGEditSpotLight();

		void initialize(CGAssetManager& am, CGEditLightCommonFactor* factor);

		void debugDepthMapRender(const glm::mat4& view, const glm::mat4& proj);

		void UIrenderForCommon(CGEditSpotLightVisualizer& spotVis);
		void UIrenderForShadow();
		
		void setLightPropertyOnShader(
			Shader* shader,
			const std::string& sLightIndex,
			const std::string& sShadowIndex,
			const unsigned shadowIndex);

		void renderShadowMap(std::vector<CGEditProxyObject>& objects);

		void setInnerCutOffInDegree(const float degree);
		void setInnerCutoffInRadian(const float radian);
		float getInnerCutOff();

		void setOuterCutOffInDegree(const float degree);
		void setOuterCutOffInRadian(const float radian);
		float getOuterCutOff();

		CGPerFrustum getPerFrustum();
	private:
		CGEditLightCommonFactor* m_lightFactors;
		void updateRadius();

		// Spot Light Factors
		// Notice the Cutoff value is the cos value with radians measure.
		float m_SpotInnerCutOff;
		float m_SpotOuterCutOff;

		/*** Shadow Properties ***/
		Shader* m_DepthMapShader;
		Shader* m_DebugDepthMapShader;

		unsigned int m_depthMapFBO, m_depthMapTexture;
		unsigned int m_shadowWidth, m_shadowHeight;
		float m_shadowBias;

		float m_perFOV;
		float m_perAspect;

		float m_shadowNearPlane;
		float m_shadowFarPlane;

		glm::mat4 m_shadowLightView;
		glm::mat4 m_shadowLightProjection;
		glm::mat4 m_shadowLightSpaceMatrix;
		/*** Shadow Properties ***/

		void setShadowWidthHeight(unsigned h);
		void setShadowFarPlane(float farP);
	};
}

#endif
