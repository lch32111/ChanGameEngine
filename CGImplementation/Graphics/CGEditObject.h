#ifndef __CG_EDIT_OBJECT_H__
#define __CG_EDIT_OBJECT_H__

#include <GPED/GPED_Precision.h>
#include <GPED/CGBroadPhase.h>

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
	protected:
		EditPrimitiveType m_BoxType; // AABB for Default Setting
		glm::vec3 m_position; // center
		glm::vec3 m_halfExtents; // half size
		glm::quat m_orientation; // orientation for OBB
		GPED::c3AABB m_fitAABB;
	private:
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
	protected:
		glm::vec3 m_position; // center
		GPED::real m_radius;
		GPED::c3AABB m_fitAABB;
	private:
		void updateAABB();
	};

	enum EditProxyType
	{
		EDIT_PROXY_STATIC = 0,
		EDIT_PROXY_DYNAMIC
	};

	class CGEditProxyObject : private CGEditBox, private CGEditSpere
	{
	public:
		CGEditProxyObject();

		/***  Init Method(you should use these methods to activate this class) ***/
		// You should connect EditProxyObject with BroadPhase
		void connectBroadPhase(CGBroadPhase* broad);
		void setBroadPhaseId(int id);
		int getBroadPhaseId();

		void setFirstPassDefShader(Shader* shader);

		/***  Init Method ***/

		/*** Primitive Method ***/
		void setEditShape(EditPrimitiveType e);
		EditPrimitiveType getEditShape();

		// Common Method for Box, Sphere
		void setPosition(const glm::vec3& p);
		void setPosition(const GPED::real x, const GPED::real y, const GPED::real z);
		void setXposition(const GPED::real x);
		void setYposition(const GPED::real y);
		void setZposition(const GPED::real z);
		glm::vec3 getPosition();

		glm::vec3 getScale();

		GPED::c3AABB getFitAABB();
		// Common Method for Box, Sphere

		// Box Specific Method
		void setHalfSize(const glm::vec3& h);
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
		/*** Primitive Method ***/

		/*** Graphics Method ***/
		void render(const glm::mat4& view, const glm::mat4& proj);

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
		/*** Graphics Method ***/

		/*** Proxy(Physics) Method ***/
		void setProxyType(EditProxyType e);
		EditProxyType getProxyType();
		/*** Proxy(Physics) Method ***/
	private:
		int m_BroadPhaseId = Node_Null;
		CGBroadPhase* m_BroadPhase;
		void updateBroadPhaseProxy();

		EditPrimitiveType m_PrimitiveType;

		// Graphics
		Shader* m_FirstPassDefShader = nullptr; 

		bool m_CMorLM = false; // CM == false, LM == true

		// Light Map Materal
		bool m_isLMdiffuse = false;
		bool m_isLMspecular = false;
		bool m_isLMemissive = false;
		unsigned m_diffuseTexture = 0;
		unsigned m_specularTexture = 0;
		unsigned m_emissiveTexture = 0;
		// Light Map Materal

		void renderPrimitive();
		// Graphics

		EditProxyType m_ProxyType;
	};

	enum EditLightType
	{
		EDIT_DIRECTION_LIGHT = 0,
		EDIT_POINT_LIGHT,
		EDIT_SPOT_LIGHT
	};

	class CGEditLightObject : private CGEditBox, private CGEditSpere
	{
	public:

	private:
		EditLightType m_LightType;
	};

}

#endif