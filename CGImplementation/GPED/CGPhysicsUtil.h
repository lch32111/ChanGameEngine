#pragma once
#ifndef __CG_PHYSICS_UTIL_H__
#define __CG_PHYSICS_UTIL_H__

#include <GPED/GPED_Precision.h>
#include <Graphics/chanQuatCamera.h>

namespace CGProj
{
	static glm::vec3 safeNormalize(const glm::vec3& vec)
	{
		GPED::real l2 = glm::dot(vec, vec);
		if (l2 >= real_epsilon * real_epsilon)
		{
			return vec * (GPED::real(1) / real_sqrt(l2));
		}

		return glm::vec3(1, 0, 0);
	}

	// Screen-to-World-Ray From Bullet Physics Library
	static glm::vec3 GetRayTo(int x, int y, 
		chanQuatCamera* camera, 
		int screen_width, int screen_height)
	{
		GPED::real fov = glm::radians(camera->Zoom);

		glm::vec3 rayFrom = camera->Position;
		glm::vec3 rayForward = camera->Front;
		GPED::real farPlane = 10000.f;
		rayForward *= farPlane;

		glm::vec3 vertical(0, 1, 0);
		
		glm::vec3 hor;
		hor = glm::cross(rayForward, vertical);
		hor = safeNormalize(hor);
		vertical = glm::cross(hor, rayForward);
		vertical = safeNormalize(vertical);

		GPED::real tanFov = tanf(0.5f * fov);

		hor *= 2.f * farPlane * tanFov;
		vertical *= 2.f * farPlane * tanFov;
		
		GPED::real aspect = GPED::real(screen_width) / screen_height;

		hor *= aspect;

		glm::vec3 rayToCenter = rayFrom + rayForward;
		glm::vec3 dHor = hor * (GPED::real(1) / screen_width);
		glm::vec3 dVert = vertical * (GPED::real(1) / screen_height);

		glm::vec3 rayTo = rayToCenter + (hor * GPED::real(-0.5)) + (vertical * GPED::real(0.5));
		rayTo += GPED::real(x) * dHor;
		rayTo -= GPED::real(y) * dVert;
		return rayTo;
	}

	static GPED::c3AABB makeAABB(const glm::vec3& pos, const glm::vec3& halfSize)
	{
		GPED::c3AABB aabb;
		aabb.min = glm::vec3(REAL_MAX);
		aabb.max = glm::vec3(-REAL_MAX);

		glm::vec3 v[8] =
		{
			glm::vec3(-halfSize.x,-halfSize.y, -halfSize.z),
			glm::vec3(-halfSize.x,-halfSize.y, halfSize.z),
			glm::vec3(halfSize.x, -halfSize.y, halfSize.z),
			glm::vec3(halfSize.x,-halfSize.y, -halfSize.z),
			glm::vec3(-halfSize.x, halfSize.y, -halfSize.z),
			glm::vec3(-halfSize.x, halfSize.y, halfSize.z),
			glm::vec3(halfSize.x, halfSize.y, halfSize.z),
			glm::vec3(halfSize.x, halfSize.y, -halfSize.z)
		};

		// No Rotation
		for (int i = 0; i < 8; ++i)
		{
			v[i] = pos +  v[i];
			aabb.min = GPED::rMin(aabb.min, v[i]);
			aabb.max = GPED::rMax(aabb.max, v[i]);
		}

		return aabb;
	}
}

#endif
