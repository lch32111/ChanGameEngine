#include "CGGizmo.h"
#include <Graphics/GLPrimitiveUtil.h>

CGProj::CGGizmo::CGGizmo()
{
	m_editProxyObject = nullptr;
	m_hitBox = GIZMO_BOX_NONE;
	updateAABBs();
}

void CGProj::CGGizmo::initGizmo()
{
	m_lineRenderer = CGRenderLine("ShaderFolder/CGLineShader.vs", "ShaderFolder/CGLineShader.fs");
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
	if (object != nullptr && m_editProxyObject != object)
	{
		center = object->getPosition();
		updateAABBs();
	}

	m_editProxyObject = object;
}

bool CGProj::CGGizmo::rayOverlapBoxes(const GPED::c3RayInput & rayInput)
{
	bool OverlapResult = false;
	GPED::c3RayOutput rayOutput;
	GPED::real minT = REAL_MAX;
	m_hitBox = GIZMO_BOX_NONE;

	if (GPED::rayaabbIntersection(rayOutput, rayInput, m_xAxisBox))
	{
		OverlapResult = true;
		minT = rayOutput.t;
		m_hitBox = GIZMO_BOX_XAXIS;
	}
	
	if (GPED::rayaabbIntersection(rayOutput, rayInput, m_yAxisBox))
	{
		OverlapResult = true;
		if (rayOutput.t < minT)
		{
			minT = rayOutput.t;
			m_hitBox = GIZMO_BOX_YAXIS;
		}
	}

	if (GPED::rayaabbIntersection(rayOutput, rayInput, m_zAxisBox))
	{
		OverlapResult = true;
		if (rayOutput.t < minT)
		{
			m_hitBox = GIZMO_BOX_ZAXIS;
		}
	}

	return OverlapResult;
}

void CGProj::CGGizmo::translate(float xoffset, float yoffset)
{
	assert(m_editProxyObject != nullptr);

	xoffset *= MOUSE_SENSITIVITY;
	yoffset *= MOUSE_SENSITIVITY;



	glm::vec3 proxyPosition = m_editProxyObject->getPosition();
	GPED::real deltaPos;

	switch (m_hitBox)
	{
	case GIZMO_BOX_XAXIS:
	{
		deltaPos = proxyPosition.x;
		deltaPos += xoffset;
		m_editProxyObject->setXposition(deltaPos);

		// Gizmo Center Update
		center.x = deltaPos;
		break;
	}
	case GIZMO_BOX_YAXIS:
	{
		deltaPos = proxyPosition.y;
		deltaPos += yoffset;
		m_editProxyObject->setYposition(deltaPos);

		// Gizmo Center Update
		center.y = deltaPos;
		break;
	}
	case GIZMO_BOX_ZAXIS:
	{
		deltaPos = proxyPosition.z;
		deltaPos += xoffset;
		m_editProxyObject->setZposition(deltaPos);

		// Gizmo Center Update
		center.z = deltaPos;
		break;
	}
	default:
		// If you come to this case,
		// your collision detection between ray and aabb failed.
		assert(0);
		break;
	}

	updateAABBs();
}

bool CGProj::CGGizmo::isActivated()
{
	return (m_editProxyObject != nullptr);
}

bool CGProj::CGGizmo::isHitActivated()
{
	return (m_hitBox != GIZMO_BOX_NONE);
}

void CGProj::CGGizmo::updateAABBs()
{
	float temp = m_axisWidth;
	m_axisWidth *= m_axisWidthScale;

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

	m_axisWidth = temp;
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
