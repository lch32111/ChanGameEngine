#pragma once
#ifndef __CG_EDIT_SPOT_LIGHT_H__
#define __CG_EDIT_SPOT_LIGHT_H__

#include <Graphics/Shader.h>
#include <Graphics/CGAssetManager.h>
#include <Graphics/CGEditLightCommonFactor.h>
#include <Graphics/CGEditSpotLightVisualizer.h>

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

		void UIrenderForCommon(CGEditSpotLightVisualizer& spotVis);
		void UIrenderForShadow();
		
		void setLightPropertyOnShader(
			Shader* shader,
			const std::string& sLightIndex,
			const std::string& sShadowIndex,
			const unsigned shadowIndex);

		void setInnerCutOffInDegree(const float degree);
		void setInnerCutoffInRadian(const float radian);
		float getInnerCutOff();

		void setOuterCutOffInDegree(const float degree);
		void setOuterCutOffInRadian(const float radian);
		float getOuterCutOff();
	private:
		CGEditLightCommonFactor* m_lightFactors;

		// Spot Light Factors
		// Notice the Cutoff value is the cos value with radians measure.
		float m_SpotInnerCutOff;
		float m_SpotOuterCutOff;

		void updateRadius();
	};
}

#endif
