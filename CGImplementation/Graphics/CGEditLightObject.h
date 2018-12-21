#pragma once
#ifndef __CG_EDIT_LIGHT_OBJECT_H__
#define __CG_EDIT_LIGHT_OBJECT_H__

#include <Graphics/CGEditObject.h>
#include <Graphics/CGEditProxyObject.h>

#include <Graphics/CGEditLightCommonFactor.h>

#include <Graphics/CGEditDirLight.h>
#include <Graphics/CGEditDirLightVisualizer.h>
#include <Graphics/CGDirShadowFrustumVisualizer.h>

#include <Graphics/CGEditPointLight.h>
#include <Graphics/CGEditPointLightVisualizer.h>

#include <Graphics/CGEditSpotLight.h>
#include <Graphics/CGEditSpotLightVisualizer.h>

namespace CGProj
{
	enum EditLightType
	{
		EDIT_DIRECTION_LIGHT = 0,
		EDIT_POINT_LIGHT,
		EDIT_SPOT_LIGHT
	};

	class CGEditLightObject : public CGEditObject // Child2 of Base Class
	{
	public:
		CGEditLightObject();
		void initialize(CGAssetManager& am);

		/*** Light Method ***/
		void setForwardShader(Shader* shader);
		void forwardRender(const glm::mat4& view, const glm::mat4& proj);
		virtual void UIrender(CGAssetManager& am);

		void setLightType(EditLightType e);
		EditLightType getLightType();

		// These Methods are overrided from CGEditObect
		// in order to sync the edit positoin and light position.
		// This structure makes clear that the names of edit object position and
		// light position are different, but the values of them should be same.
		virtual void setPosition(const glm::vec3& p) override;
		virtual void setPosition(const GPED::real x, const GPED::real y, const GPED::real z) override;
		virtual void setXposition(const GPED::real x) override;
		virtual void setYposition(const GPED::real y) override;
		virtual void setZposition(const GPED::real z) override;
		virtual glm::vec3 getPosition() override;

		void setLightDirection(const glm::vec3& d);
		void setLightDirection(const GPED::real x, const GPED::real y, const GPED::real z);
		void setLightXdirection(const GPED::real x);
		void setLightYdirection(const GPED::real y);
		void setLightZdirection(const GPED::real z);
		glm::vec3 getLightDirection();

		void setAmbientColor(const glm::vec3& ac);
		void setAmbientColor(const GPED::real r, const GPED::real g, const GPED::real b);
		void setAmbientRedColor(const GPED::real r);
		void setAmbientGreenColor(const GPED::real g);
		void setAmbientBlueColor(const GPED::real b);
		glm::vec3 getAmbientColor();

		void setDiffuseColor(const glm::vec3& dc);
		void setDiffuseColor(const GPED::real r, const GPED::real g, const GPED::real b);
		void setDiffuseRedColor(const GPED::real r);
		void setDiffuseGreenColor(const GPED::real g);
		void setDiffuseBlueColor(const GPED::real b);
		glm::vec3 getDiffuseColor();

		void setSpecularColor(const glm::vec3& sc);
		void setSpecularColor(const GPED::real r, const GPED::real g, const GPED::real b);
		void setSpecularRedColor(const GPED::real r);
		void setSpecularGreenColor(const GPED::real g);
		void setSpecularBlueColor(const GPED::real b);
		glm::vec3 getSpecularColor();

		void setAttnConstant(const float c);
		float getAttnConsant();

		void setAttnLinear(const float l);
		float getAttnLinear();

		void setAttnQuadratic(const float q);
		float getAttnQuadratic();

		float getLightRadius();

		// lighting Setting in World Space
		void setLightPropertyOnShader(unsigned lightIndex, unsigned shadowIndex, const glm::vec3& cameraPos);
		/*** Light Method ***/

		/*** Shadow Method ***/
		void renderShadowMap(std::vector<CGEditProxyObject>& objects);

		void setIsShadowRender(bool shadow);
		bool getIsShadowRender();
		/*** Shadow Method ***/
	private:
		Shader* m_forwardShader;
		EditLightType m_LightType;

		CGEditDirLight m_dirLight;
		CGEditDirLightVisualizer m_dirVis;
		CGDirShadowFrustumVisualizer m_dirShadowVis;

		CGEditPointLight m_pointLight;
		CGEditPointLightVisualizer m_pointVis;

		CGEditSpotLight m_spotLight;
		CGEditSpotLightVisualizer m_spotVis;

		/*** Light Common Properties ***/
		CGEditLightCommonFactor m_CommonlightFactors;
		void updateRadius();
		/*** Light Common Properties ***/
	};
}

#endif