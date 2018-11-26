#include "CGGizmo.h"
#include <Graphics/GLPrimitiveUtil.h>

CGProj::CGGizmo::CGGizmo()
{
	updateAABBs();
}

void CGProj::CGGizmo::registerGizmoOnBroadPhase(CGBroadPhase * broadphase)
{
	m_broadPhase = broadphase;
	m_xAxis_BroadId = m_broadPhase->CreateProxy(m_xAxisBox, this);
	m_yAxis_BroadId = m_broadPhase->CreateProxy(m_yAxisBox, this);
	m_zAxis_BroadId = m_broadPhase->CreateProxy(m_zAxisBox, this);
}

void CGProj::CGGizmo::updateGizmoBroadPhase(glm::vec3 position)
{
	center = position;
	updateAABBs();
	m_broadPhase->UpdateProxy(m_xAxis_BroadId, m_xAxisBox);
	m_broadPhase->UpdateProxy(m_yAxis_BroadId, m_yAxisBox);
	m_broadPhase->UpdateProxy(m_zAxis_BroadId, m_zAxisBox);
}

void CGProj::CGGizmo::renderGizmo(const glm::mat4& view, const glm::mat4& proj)
{
	m_lineRenderer.insertLine(center, center + worldXAxis * m_axisLengthScale, xAxisColor);
	m_lineRenderer.insertLine(center, center + worldYAxis * m_axisLengthScale, yAxisColor);
	m_lineRenderer.insertLine(center, center + worldZAxis * m_axisLengthScale, zAxisColor);
	m_lineRenderer.renderLine(view, proj, m_axisWidth);
}

void CGProj::CGGizmo::renderGizmoBox(const glm::mat4& view, const glm::mat4& proj)
{
	insertAABBwithLine(m_xAxisBox, xAxisColor);
	insertAABBwithLine(m_yAxisBox, yAxisColor);
	insertAABBwithLine(m_zAxisBox, zAxisColor);
	m_lineRenderer.renderLine(view, proj, 1.0);
}

void CGProj::CGGizmo::setAxisWidth(float width)
{
	m_axisWidth = width;
}

void CGProj::CGGizmo::setAxisLength(float length)
{
	m_axisLengthScale = length;
}

void CGProj::CGGizmo::setEditProxyObject(CGEditProxyObject * object)
{
	m_editProxyObject = object;
}

void CGProj::CGGizmo::updateAABBs()
{
	m_xAxisBox.min = center;
	m_xAxisBox.max = center;
	m_xAxisBox.min += glm::vec3(0, -m_axisWidth, -m_axisWidth);
	m_xAxisBox.max += glm::vec3(m_axisLengthScale, m_axisWidth, m_axisWidth);

	m_yAxisBox.min = center;
	m_yAxisBox.max = center;
	m_yAxisBox.min += glm::vec3(-m_axisWidth, 0, -m_axisWidth);
	m_yAxisBox.max += glm::vec3(m_axisWidth, m_axisLengthScale, m_axisWidth);

	m_zAxisBox.min = center;
	m_zAxisBox.max = center;
	m_zAxisBox.min += glm::vec3(-m_axisWidth, -m_axisWidth, 0);
	m_zAxisBox.max += glm::vec3(m_axisWidth, m_axisWidth, m_axisLengthScale);
}

void CGProj::CGGizmo::insertAABBwithLine(const GPED::c3AABB & aabb, glm::vec4 color)
{
	glm::vec3 extents = aabb.max - aabb.min;

	glm::vec3 v1 = aabb.min;
	glm::vec3 v2 = aabb.min + glm::vec3(0, 0, extents.z);
	glm::vec3 v3 = aabb.min + glm::vec3(extents.x, 0, extents.z);
	glm::vec3 v4 = aabb.min + glm::vec3(extents.x, 0, 0);
	glm::vec3 v5 = aabb.min + glm::vec3(0, extents.y, 0);
	glm::vec3 v6 = aabb.min + glm::vec3(0, extents.y, extents.z);
	glm::vec3 v7 = aabb.max;
	glm::vec3 v8 = aabb.min + glm::vec3(extents.x, extents.y, 0);

	// Bottom Plane
	m_lineRenderer.insertLine(v1, v2, color);
	m_lineRenderer.insertLine(v1, v4, color);
	m_lineRenderer.insertLine(v3, v2, color);
	m_lineRenderer.insertLine(v3, v4, color);

	// Upper Plane
	m_lineRenderer.insertLine(v5, v6, color);
	m_lineRenderer.insertLine(v5, v8, color);
	m_lineRenderer.insertLine(v7, v6, color);
	m_lineRenderer.insertLine(v7, v8, color);

	// Height
	m_lineRenderer.insertLine(v1, v5, color);
	m_lineRenderer.insertLine(v2, v6, color);
	m_lineRenderer.insertLine(v3, v7, color);
	m_lineRenderer.insertLine(v4, v8, color);
}
