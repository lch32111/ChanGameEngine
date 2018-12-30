#pragma once
#ifndef __CG_TERRAIN_UTIL_H__
#define __CG_TERRAIN_UTIL_H__

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace CGProj
{
	template <typename T = float>
	inline T lerp(const T& lo, const T& hi, const T& t) { return lo * (1 - t) + hi * t; }

	inline float smoothstepRemap(const float& a, const float& b, const float& t)
	{
		// float tRemapSmoothstep = t * t * (3 - 2 * t);

		// Ken Perlin ver.
		float t3 = std::powf(t, 3);
		float tRemapSmoothstep = 6 * t3 * t * t - 15.f * t3 * t + 10 * t3;
		return lerp(a, b, tRemapSmoothstep);
	}

	inline float consineRemap(const float& a, const float& b, const float& t)
	{
		assert(t >= 0 && t <= 1);
		float tRemapCosine = (1 - std::cosf(t * glm::pi<float>())) * 0.5f;
		return lerp(a, b, tRemapCosine);
	}

	inline float smoothStep(const float& t)
	{
		// basic smoothstep
		return t * t * (3 - 2 * t);

		/*
		// Ken Perlin SmoothStep
		float t3 = std::powf(t, 3);
		return 6 * t3 * t * t - 15.f * t3 * t + 10 * t3;
		*/
	}
}

#endif