#pragma once
#ifndef __CG_GIZMO_H__
#define __CG_GIZMO_H__

#include <GPED/CGBroadPhase.h>
#include <Graphics/chanQuatCamera.h>
#include <Graphics/CGRenderLine.h>
#include <Graphics/CGEditObject.h>

namespace CGProj
{
	class CGGizmo
	{
	public:
		CGGizmo();

		void initGizmo();

		void renderGizmo(const glm::mat4& view, const glm::mat4& proj);
		void renderGizmoBox(const glm::mat4& view, const glm::mat4& proj);

		void setAxisWidth(float width);
		void setAxisLength(float length);
		void setEditObject(CGEditObject* object);

		bool isActivated();
		bool isHitActivated();
		bool rayOverlapBoxes(const GPED::c3RayInput& rayInput);

		void translate(float xoffset, float yoffset, const chanQuatCamera& camera);
	private:
		CGEditObject* m_editObject;
		CGRenderLine m_lineRenderer;
		
		float m_axisWidth = 1.0f;
		float m_axisLengthScale = 1.0f;
		float m_axisWidthScale = 0.01f;
		float MOUSE_SENSITIVITY = 0.01F;

		GPED::c3AABB m_xAxisBox;
		GPED::c3AABB m_yAxisBox;
		GPED::c3AABB m_zAxisBox;

		glm::vec3 worldXAxis = glm::vec3(1, 0, 0);
		glm::vec3 worldYAxis = glm::vec3(0, 1, 0);
		glm::vec3 worldZAxis = glm::vec3(0, 0, 1);
		glm::vec4 xAxisColor = glm::vec4(1, 0, 0, 1);
		glm::vec4 yAxisColor = glm::vec4(0, 1, 0, 1);
		glm::vec4 zAxisColor = glm::vec4(0, 0, 1, 1);

		glm::vec3 center = glm::vec3(0);

		enum GizmoBox
		{
			GIZMO_BOX_XAXIS = 0,
			GIZMO_BOX_YAXIS,
			GIZMO_BOX_ZAXIS,
			GIZMO_BOX_NONE,
		};
		GizmoBox m_hitBox;

		void updateAABBs();
		void insertAABBwithLine(const GPED::c3AABB& aabb, glm::vec4 color);
	};
}

#endif