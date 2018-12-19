#pragma once
#ifndef __CG_EDIT_LIGHT_COMMON_FACTOR_H__
#define __CG_EDIT_LIGHT_COMMON_FACTOR_H__

#include <glm/common.hpp>

namespace CGProj
{
	/** 2018-12-19, Chanhaeng Lee
	 * This structure should help communicate 
	 * between EditLightObject class and each type of light Classes
	 * The light classes means CGEditDir, Point, SpotLight classes.
	 * The common factor values will not be changed in those classes.
	 * Only the CGEditLightObject will change the values. It means that
	 * The light classes will only refer to these values.
	 */
	struct CGEditLightCommonFactor
	{
		glm::vec3 lightPosition;
		glm::vec3 lightDirection;

		// Light Colors
		glm::vec3 lightAmbient;
		glm::vec3 lightDiffuse;
		glm::vec3 lightSpecular;

		// Attenuation except Dir Light
		bool isRangeRender;
		float AttnConstant;
		float AttnLinear;
		float AttnQuadratic;
		float AttnRadius;

		bool isShadow;
		bool isShadowMapRender;
		bool isShadowFrustumRender;
	};
}

#endif