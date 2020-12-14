#pragma once
#ifndef __CG_VALUE_NOISE_1D_H__
#define __CG_VALUE_NOISE_1D_H__

#include <GPED/GPED_random.h>
#include <Terrain/CGTerrainUtil.h>

namespace CG
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
	};
}

#endif