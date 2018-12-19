#ifndef __CG_EDIT_OBJECT_H__
#define __CG_EDIT_OBJECT_H__

#include <GPED/GPED_Precision.h>
#include <GPED/CGBroadPhase.h>

#include <Graphics/CGAssetManager.h>
#include <Graphics/CGEditPrimitive.h>

namespace CGProj
{
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
}

#endif