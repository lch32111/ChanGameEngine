#pragma once
#ifndef __CG_PHYSICS_UTIL_H__
#define __CG_PHYSICS_UTIL_H__

#include <GPED/GPED_Precision.h>
#include <Graphics/chanQuatCamera.h>
#include <Math/CGVector3.h>

namespace CG
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

	static CGVector3<float> GetRayTo(int x, int y, int screen_width, int screen_height,
		float cam_near_plane_in_radian,
		CGVector3<float> camPos,
		CGVector3<float> camForward
	)
	{
		CGVector3<float> ray_from = camPos;
		CGVector3<float> ray_forward = camForward;
		constexpr float far_plane = 10000.f;
		ray_forward *= far_plane;

		CGVector3<float> vertical(0, 1, 0);
		CGVector3<float> hor = Cross(ray_forward, vertical);
		hor = SafeNormalize(hor);
		vertical = Cross(hor, ray_forward);
		vertical = SafeNormalize(vertical);

		float tan_fov = CGScalarOp<float>::Tan(0.5f * cam_near_plane_in_radian);

		hor *= 2.f * far_plane * tan_fov;
		vertical *= 2.f * far_plane * tan_fov;

		float aspect = (float)screen_width / screen_height;
		
		hor *= aspect;

		CGVector3<float> ray_to_center = ray_from + ray_forward;
		CGVector3<float> d_hor = hor * (1.f / screen_width);
		CGVector3<float> d_vert = vertical * (1.f / screen_height);

		CGVector3<float> ray_to = ray_to_center + (hor * -0.5f) + (vertical * 0.5f);
		ray_to += (float)x * d_hor;
		ray_to -= (float)y * d_vert;
		return ray_to;
	}

	// Screen-to-World-Ray From Bullet Physics Library
	static glm::vec3 GetRayTo(int x, int y, 
		chanQuatCamera* camera, 
		int screen_width, int screen_height)
	{
		GPED::real fov = glm::radians(camera->Zoom);

		glm::vec3 rayFrom = camera->Position;
		glm::vec3 rayForward = camera->Front;
		constexpr GPED::real farPlane = 10000.f;
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

	static glm::mat4 safeLookAt(const glm::vec3& pos, const glm::vec3& eye, const glm::vec3& up)
	{
		glm::mat4 result(1.0);

		glm::vec3 zaxis(glm::normalize(pos - eye));
		glm::vec3 xaxis(safeNormalize(glm::cross(glm::vec3(0, 1, 0), zaxis)));
		glm::vec3 yaxis(glm::cross(zaxis, xaxis));
		result[0][0] = xaxis.x;
		result[1][0] = xaxis.y;
		result[2][0] = xaxis.z;
		result[0][1] = yaxis.x;
		result[1][1] = yaxis.y;
		result[2][1] = yaxis.z;
		result[0][2] = zaxis.x;
		result[1][2] = zaxis.y;
		result[2][2] = zaxis.z;
		result[3][0] = -glm::dot(xaxis, pos);
		result[3][1] = -glm::dot(yaxis, pos);
		result[3][2] = -glm::dot(zaxis, pos);

		return result;
	}
}
#endif