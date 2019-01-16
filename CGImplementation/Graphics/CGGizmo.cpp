#include <Graphics/CGGizmo.h>

#include <Graphics/GLPrimitiveUtil.h>
#include <Graphics/CGEditProxyObject.h>
#include <Graphics/CGEditLightObject.h>

CGProj::CGGizmo::CGGizmo()
{
	m_editObject = nullptr;
	m_hitBox = GIZMO_BOX_NONE;
	updateAABBs();
}

void CGProj::CGGizmo::initGizmo(CGAssetManager& am)
{
	m_lineRenderer.setShader(am.getShader(SHADER_CG_LINE));
}

void CGProj::CGGizmo::renderGizmo(const glm::mat4& view, const glm::mat4& proj)
{
	m_lineRenderer.insertLine(center, center + worldXAxis * m_axisLengthScale, xAxisColor);
	m_lineRenderer.insertLine(center, center + worldYAxis * m_axisLengthScale, yAxisColor);
	m_lineRenderer.insertLine(center, center + worldZAxis * m_axisLengthScale, zAxisColor);

	// convinience for using Gizmo
	glDisable(GL_DEPTH_TEST);
	m_lineRenderer.renderLine(view, proj, m_axisWidth);
	glEnable(GL_DEPTH_TEST);
}

void CGProj::CGGizmo::renderGizmoBox(const glm::mat4& view, const glm::mat4& proj)
{
	insertAABBwithLine(m_xAxisBox, xAxisColor);
	insertAABBwithLine(m_yAxisBox, yAxisColor);
	insertAABBwithLine(m_zAxisBox, zAxisColor);
	
	// convinience for using Gizmo
	glDisable(GL_DEPTH_TEST);
	m_lineRenderer.renderLine(view, proj, 1.0);
	glEnable(GL_DEPTH_TEST);
}

void CGProj::CGGizmo::setAxisWidth(float width)
{
	m_axisWidth = width;
}

void CGProj::CGGizmo::setAxisLength(float length)
{
	m_axisLengthScale = length;
}

void CGProj::CGGizmo::setEditObject(CGEditObject * object)
{
	if (object == nullptr)
	{
		return;
	}

	if (m_editObject != object)
	{
		center = object->getPosition();
		updateAABBs();
	}

	switch (object->getObjectType())
	{
	case EDIT_OBJECT_PROXY:
		m_editObject = (CGEditProxyObject*)object;
		break;
	case EDIT_OBJECT_LIGHT:
		m_editObject = (CGEditLightObject*)object;
		break;
	default:
		assert(0);
	}
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

void CGProj::CGGizmo::translate(float xoffset, float yoffset, const chanQuatCamera& camera)
{
	assert(m_editObject != nullptr);

	xoffset *= MOUSE_SENSITIVITY;
	yoffset *= MOUSE_SENSITIVITY;

	glm::vec3 objectPosition = m_editObject->getPosition();
	GPED::real deltaPos;
	GPED::real sign;

	switch (m_hitBox)
	{
	case GIZMO_BOX_XAXIS:
	{
		// glm::dot(camera.Right, worldXAxis) makes correct direction for translation.
		// below command is the same as dot equation above.
		sign = (GPED::real)((camera.Right.x > 0) ? 1 : -1);

		deltaPos = objectPosition.x;
		deltaPos += xoffset * sign;
		m_editObject->setXposition(deltaPos);

		// Gizmo Center Update
		center.x = deltaPos;
		break;
	}
	case GIZMO_BOX_YAXIS:
	{
		deltaPos = objectPosition.y;
		deltaPos += yoffset;
		m_editObject->setYposition(deltaPos);

		// Gizmo Center Update
		center.y = deltaPos;
		break;
	}
	case GIZMO_BOX_ZAXIS:
	{
		// screen is 2D(X, Y), and we need to sync z-axis move with x-axis
		// beow command is the same as glm::dot(camera.Right, worldZAxis)
		sign = (GPED::real)((camera.Right.z > 0) ? 1 : -1);

		deltaPos = objectPosition.z;
		deltaPos += xoffset * sign;
		m_editObject->setZposition(deltaPos);

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
	return (m_editObject != nullptr);
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
