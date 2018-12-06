#ifndef __CG_EDIT_OBJECT_H__
#define __CG_EDIT_OBJECT_H__

#include <GPED/GPED_Precision.h>
#include <GPED/CGBroadPhase.h>

#include <Graphics/CGAssetManager.h>
#include <Graphics/CGEditSpotLightVisualizer.h>

namespace CGProj
{
	enum EditPrimitiveType // this enum is used to represent object collider
	{
		EDIT_PRIMITIVE_AABB = 0,
		EDIT_PRIMITIVE_OBB,
		EDIT_PRIMITIVE_SPHERE
	};

	class CGEditBox
	{
	public:
		CGEditBox();
		CGEditBox(const glm::vec3& pos, const glm::vec3& halfExtents, const glm::quat& m_orient = glm::quat(0, 0, 0, -1));

		void setPosition(const glm::vec3& p);
		void setPosition(const GPED::real x, const GPED::real y, const GPED::real z);
		void setXposition(const GPED::real x);
		void setYposition(const GPED::real y);
		void setZposition(const GPED::real z);
		glm::vec3 getPosition();

		void setHalfSize(const glm::vec3& h);
		void setHalfSize(const GPED::real x, GPED::real y, GPED::real z);
		void setXHalfSize(const GPED::real x);
		void setYHalfSize(const GPED::real y);
		void setZHalfSize(const GPED::real z);
		glm::vec3 getHalfSize();

		GPED::c3AABB getFitAABB();

		void setPrimitiveType(EditPrimitiveType e);
		EditPrimitiveType getPrimitiveType();
	private:
		EditPrimitiveType m_BoxType; // AABB for Default Setting
		glm::vec3 m_position; // center
		glm::vec3 m_halfExtents; // half size
		glm::quat m_orientation; // orientation for OBB
		GPED::c3AABB m_fitAABB;
	
		void updateAABB();
	};

	class CGEditSpere
	{
	public:
		CGEditSpere();
		CGEditSpere(const glm::vec3& pos, const GPED::real radius);

		void setPosition(const glm::vec3& p);
		void setPosition(const GPED::real x, const GPED::real y, const GPED::real z);
		void setXposition(const GPED::real x);
		void setYposition(const GPED::real y);
		void setZposition(const GPED::real z);
		glm::vec3 getPosition();

		void setRaidus(GPED::real r);
		GPED::real getRadius();

		GPED::c3AABB getFitAABB();
	private:
		glm::vec3 m_position; // center
		GPED::real m_radius;
		GPED::c3AABB m_fitAABB;
	
		void updateAABB();
	};
	
	enum EditObjectType
	{
		EDIT_OBJECT_PROXY,
		EDIT_OBJECT_LIGHT
	};

	class CGEditObject // Base Class
	{
	public:
		CGEditObject();

		// Must Init Method
		void connectBroadPhase(CGBroadPhase* broad);
		void setBroadPhaseId(int id);
		int getBroadPhaseId();

		// The basic primitive render is deferred rendering.
		// Proxy Object should have first pass deferred shader !
		// Light Object should have second pass deferred shader !
		void setDefShader(Shader* shader); 
		// Must Init Method

		/*** Primitive Method ***/
		
		// Common Method
		void setEditShape(EditPrimitiveType e);
		EditPrimitiveType getEditShape();
		virtual void setPosition(const glm::vec3& p);
		virtual void setPosition(const GPED::real x, const GPED::real y, const GPED::real z);
		virtual void setXposition(const GPED::real x);
		virtual void setYposition(const GPED::real y);
		virtual void setZposition(const GPED::real z);
		virtual glm::vec3 getPosition();

		// smae size for XYZ axes
		void setScale(float scale);
		glm::vec3 getScale();

		GPED::c3AABB getFitAABB();
		// Common Method
		/*** Primitive Method ***/

		/*** Graphics Method ***/
		virtual void render(const glm::mat4& view, const glm::mat4& proj);
		virtual void UIrender(CGAssetManager& am);
		/*** Graphics Method ***/

		EditObjectType getObjectType();
		void setObjectType(EditObjectType e);
	protected:
		EditObjectType m_ObjectType;

		EditPrimitiveType m_PrimitiveType;
		CGEditBox m_EditBox;
		CGEditSpere m_EditSphere;
		bool m_EditPrimitiveDraw = false;

		int m_BroadPhaseId = Node_Null;
		CGBroadPhase* m_BroadPhase;
		void updateBroadPhaseProxy();

		Shader* m_DefShader = nullptr;
		void renderPrimitive();

		// Box Specific Method
		void setHalfSize(const glm::vec3 & h);
		void setHalfSize(const GPED::real x, GPED::real y, GPED::real z);
		void setXHalfSize(const GPED::real x);
		void setYHalfSize(const GPED::real y);
		void setZHalfSize(const GPED::real z);
		glm::vec3 getHalfSize();
		// Box Specific Method

		// Sphere Specific Method
		void setRaidus(GPED::real r);
		GPED::real getRadius();
		// Sphere Specific Method
	};


	enum EditProxyType
	{
		EDIT_PROXY_STATIC = 0,
		EDIT_PROXY_DYNAMIC
	};

	class CGEditProxyObject : public CGEditObject // Child1 of Base Class
	{
	public:
		CGEditProxyObject();

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

		void setInnerCutOffInDegree(const float degree);
		void setInnerCutoffInRadian(const float radian);
		float getInnerCutOff();

		void setOuterCutOffInDegree(const float degree);
		void setOuterCutOffInRadian(const float radian);
		float getOuterCutOff();

		// lighting Setting in World Space
		void setLightPropertyOnShader(int index, const glm::vec3& cameraPos);
		/*** Light Method ***/

	private:
		Shader* m_forwardShader;
		EditLightType m_LightType;
		CGEditSpotLightVisualizer m_spotVis;

		// Point and Spot Light(Flash Light)
		glm::vec3 m_lightPosition;

		// Directional and Spot Light
		glm::vec3 m_lightDirection;

		// Light Colors
		glm::vec3 m_lightAmbient;
		glm::vec3 m_lightDiffuse;
		glm::vec3 m_lightSpecular;

		// Attenuation 
		bool m_isRangeRender = false;
		float m_AttnConstant;
		float m_AttnLinear;
		float m_AttnQuadratic;
		float m_AttnRadius;
		void updateRadius();

		// Spot Light Factors
		// Notice the Cutoff value is the cos value with radians measure.
		float m_SpotInnerCutOff;
		float m_SpotOuterCutOff;
	};

}

#endif