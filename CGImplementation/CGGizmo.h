#pragma once
#ifndef __CG_GIZMO_H__
#define __CG_GIZMO_H__

#include <GPED/CGBroadPhase.h>
#include <Graphics/CGRenderLine.h>
#include <Graphics/CGEditObject.h>

namespace CGProj
{
	class CGGizmo
	{
	public:
		CGGizmo();

		void registerGizmoOnBroadPhase(CGBroadPhase* broadphase);
		void updateGizmoBroadPhase(glm::vec3 position);

		void renderGizmo(const glm::mat4& view, const glm::mat4& proj);
		void renderGizmoBox(const glm::mat4& view, const glm::mat4& proj);

		void setAxisWidth(float width);
		void setAxisLength(float length);

		void setEditProxyObject(CGEditProxyObject* object);
	private:
		CGBroadPhase* m_broadPhase;
		CGEditProxyObject* m_editProxyObject;
		CGRenderLine m_lineRenderer;
		
		float m_axisWidth = 1.0f;
		float m_axisLengthScale = 1.0f;

		int m_xAxis_BroadId;
		GPED::c3AABB m_xAxisBox;

		int m_yAxis_BroadId;
		GPED::c3AABB m_yAxisBox;

		int m_zAxis_BroadId;
		GPED::c3AABB m_zAxisBox;

		glm::vec3 worldXAxis = glm::vec3(1, 0, 0);
		glm::vec3 worldYAxis = glm::vec3(0, 1, 0);
		glm::vec3 worldZAxis = glm::vec3(0, 0, 1);
		glm::vec4 xAxisColor = glm::vec4(1, 0, 0, 1);
		glm::vec4 yAxisColor = glm::vec4(0, 1, 0, 1);
		glm::vec4 zAxisColor = glm::vec4(0, 0, 1, 1);

		glm::vec3 center = glm::vec3(0);

		void updateAABBs();
		void insertAABBwithLine(const GPED::c3AABB& aabb, glm::vec4 color);
	};
}

#endif