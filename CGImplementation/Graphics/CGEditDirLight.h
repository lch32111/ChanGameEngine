#pragma once
#ifndef __CG_EDIT_DIR_LIGHT_H__
#define __CG_EDIT_DIR_LIGHT_H__

#include <Graphics/Shader.h>
#include <Graphics/CGAssetManager.h>
#include <Graphics/CGEditLightCommonFactor.h>
#include <Graphics/CGEditProxyObject.h>
#include <Graphics/CGFrustum.h>

namespace CGProj
{
	/** 2018-12-19, Chanhaeng Lee
	 * This Class includes variables 
	 * related to the directional light component.
	 * Because The CGEditLightObject class is getting bigger,
	 * I thought that It's needed to separate the components of each light type.
	 * It means These kinds of light class are designed 
	 * to be used in CGEditLightObject Class.
	 * That's the reason why this class is not inherited from CGEditLightObject
	 */
	class CGEditDirLight
	{
	public:
		// Constructor to init all member variables to prevent 
		// from accessing to the garbage value
		CGEditDirLight();
		void initialize(CGAssetManager& am, CGEditLightCommonFactor* factor);
		
		// Methods to use In CGEditLightObjects
		void debugDepthMapRender();

		void UIrenderForCommon();
		void UIrenderForShadow();
		
		void setLightPropertyOnShader(
			Shader* shader,
			const std::string& sLightIndex,
			const std::string& sShadowIndex,
			const unsigned shadowIndex);

		void renderShadowMap(std::vector<CGEditProxyObject>& objects);

		bool getShadowProjection();
		CGOrthoFrustum getOrthoFrustum();
		CGPerFrustum getPerFrustum();
		// Methods to use In CGEditLightObjects
	private:
		// Light Common Factor
		CGEditLightCommonFactor* m_lightFactors;

		/*** Shadow Properties ***/
		Shader* m_DepthMapShader;
		Shader* m_DebugDepthMapShader;

		unsigned int m_depthMapFBO, m_depthMapTexture;
		unsigned int m_shadowWidth, m_shadowHeight;

		bool m_shadowProjection; // true : perspective, false orthogonal

		// In case of Orthographic
		float m_orthoLeft;
		float m_orthoRight;
		float m_orthoBottom;
		float m_orthoTop;

		// In case of Perspective
		float m_perFOV;
		float m_perAspect;

		float m_shadowNearPlane;
		float m_shadowFarPlane;

		glm::mat4 m_shadowLightView;
		glm::mat4 m_shadowLightProjection;
		glm::mat4 m_shadowLightSpaceMatrix;
		/*** Shadow Properties ***/

		void setShadowWidthHeight(unsigned w, unsigned h);
	};
}

#endif