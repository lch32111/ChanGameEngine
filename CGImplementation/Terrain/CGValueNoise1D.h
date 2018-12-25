#pragma once
#ifndef __CG_VALUE_NOISE_1D_H__
#define __CG_VALUE_NOISE_1D_H__

#include <GPED/GPED_random.h>
#include <Terrain/CGTerrainUtil.h>

namespace CGProj
{
	class CGValueNoise1D
	{
	public:
		CGValueNoise1D() { }

		void initialize(unsigned seed = 2011, unsigned maxVertices = 10)
		{
			kMaxVertices = maxVertices;
			r = new float[kMaxVertices];

			GPED::Random rand(seed);

			for (unsigned i = 0; i < kMaxVertices; ++i)
				r[i] = rand.randomReal();
		}

		void destroy()
		{
			delete[] r;
			r = nullptr;
		}

		// Evaluate the noise function at position x
		float eval(const float& x)
		{
			int xi = (int)x;
			int xMin = xi % (int)kMaxVertices;
			float t = x - xi;
			int xMax = (xMin == kMaxVertices - 1) ? 0 : xMin + 1;

			// return lerp(r[xMin], r[xMax], t);
			// return consineRemap(r[xMin], r[xMax], t);
			return smoothstepRemap(r[xMin], r[xMax], t);
		}
	private:
		unsigned kMaxVertices;
		float* r;

		float consineRemap(const float& a, const float& b, const float& t)
		{
			assert(t >= 0 && t <= 1);
			float tRemapCosine = (1 - std::cosf(t * glm::pi<float>())) * 0.5f;
			return lerp(a, b, tRemapCosine);
		}

		float smoothstepRemap(const float& a, const float& b, const float& t)
		{
			// float tRemapSmoothstep = t * t * (3 - 2 * t);

			// Ken Perlin ver.
			float t3 = std::powf(t, 3);
			float tRemapSmoothstep = 6 * t3 * t * t - 15.f * t3 * t + 10 * t3;
			return lerp(a, b, tRemapSmoothstep);
		}
	};
}

#endif