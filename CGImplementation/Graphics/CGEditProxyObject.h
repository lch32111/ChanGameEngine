#pragma once
#ifndef __CG_EDIT_PROXY_OBJECT_H__
#define __CG_EDIT_PROXY_OBJECT_H__

#include <Graphics/CGEditObject.h>

namespace CGProj
{
	enum EditProxyType
	{
		EDIT_PROXY_STATIC = 0,
		EDIT_PROXY_DYNAMIC
	};

	class CGEditProxyObject : public CGEditObject // Child1 of Base Class
	{
		friend class CGEditLightObject;
		friend class CGEditDirLight;
		friend class CGEditPointLight;
		friend class CGEditSpotLight;
	public:
		CGEditProxyObject(CGAssetManager& am);

		/*** Graphics Method ***/
		virtual void render(const glm::mat4& view, const glm::mat4& proj);
		virtual void UIrender(CGAssetManager& am);

		bool getCMorLM();
		void setCMorLM(bool flag);

		bool isDiffuseOn();
		void setDiffuseFlag(bool flag);
		void setDiffuseTexture(unsigned texId);

		bool isSpecularOn();
		void setSpecularFlag(bool flag);
		void setSpecularTexture(unsigned texId);

		bool isEmissiveOn();
		void setEmissiveFlag(bool flag);
		void setEmissiveTexture(unsigned texId);

		void setCMambinet(const glm::vec3& ambient);
		void setCMdiffuse(const glm::vec3& diffuse);
		void setCMspecular(const glm::vec3& specular);
		void setCMshininess(float s);
		/*** Graphics Method ***/

		/*** Proxy(Physics) Method ***/
		void setProxyType(EditProxyType e);
		EditProxyType getProxyType();
		/*** Proxy(Physics) Method ***/
	private:
		/*** Graphics ***/
		// The variables in this graphics comments are 
		// related to the shader uniform variables
		bool m_CMorLM = false; // CM == false, LM == true

		// Light Map Materal
		bool m_isLMdiffuse = false;
		bool m_isLMspecular = false;
		bool m_isLMemissive = false;
		unsigned m_diffuseTexture = 0;
		unsigned m_specularTexture = 0;
		unsigned m_emissiveTexture = 0;
		// Light Map Materal

		// Color Map Material
		glm::vec3 m_CMambient = glm::vec3(1);
		glm::vec3 m_CMdiffuse = glm::vec3(1);
		glm::vec3 m_CMspecular = glm::vec3(1);
		float m_CMshininess = 1.f;
		// Color Map Material

		/*** Graphics ***/

		EditProxyType m_ProxyType;
	};
}

#endif
