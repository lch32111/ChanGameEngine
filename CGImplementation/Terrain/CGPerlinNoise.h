#pragma once
#ifndef __CG_PERLIN_NOISE_H__
#define __CG_PERLIN_NOISE_H__

#include <random>
#include <functional>

#include <glm/glm.hpp>

#include <Terrain/CGTerrainUtil.h>

namespace CG
{
	class CGPerlinNoise
	{
		static const unsigned tableSize = 256;
		static const unsigned tableSizeMask = tableSize - 1;
		unsigned permutationTable[tableSize * 2];

		uint8_t hash(const int&x, const int& y, const int& z) const
		{
			return permutationTable[permutationTable[permutationTable[x] + y] + z];
		}
	public:
		CGPerlinNoise(unsigned seed = 2018)
		{
			for (unsigned i = 0; i < tableSize; ++i)
				permutationTable[i] = i;

			std::mt19937 generator(seed);
			std::uniform_int_distribution<int> distributionInt;
			auto diceInt = std::bind(distributionInt, generator);
			// create permutation table
			for (unsigned i = 0; i < tableSize; ++i)
			{
				std::swap(permutationTable[i], permutationTable[diceInt() & tableSizeMask]);
				permutationTable[i + tableSize] = permutationTable[i];
			}
		}

		// Improved Perlin Noise
		float eval(const glm::vec3& p, glm::vec3& derivs) const
		{
			int xi0 = ((int)std::floor(p.x)) & tableSizeMask;
			int yi0 = ((int)std::floor(p.y)) & tableSizeMask;
			int zi0 = ((int)std::floor(p.z)) & tableSizeMask;

			int xi1 = (xi0 + 1) & tableSizeMask;
			int yi1 = (yi0 + 1) & tableSizeMask;
			int zi1 = (zi0 + 1) & tableSizeMask;

			float tx = p.x - ((int)std::floor(p.x));
			float ty = p.y - ((int)std::floor(p.y));
			float tz = p.z - ((int)std::floor(p.z));

			float x0 = tx, x1 = tx - 1;
			float y0 = ty, y1 = ty - 1;
			float z0 = tz, z1 = tz - 1;

			float a = gradientDotV(hash(xi0, yi0, zi0), x0, y0, z0);
			float b = gradientDotV(hash(xi1, yi0, zi0), x1, y0, z0);
			float c = gradientDotV(hash(xi0, yi1, zi0), x0, y1, z0);
			float d = gradientDotV(hash(xi1, yi1, zi0), x1, y1, z0);
			float e = gradientDotV(hash(xi0, yi0, zi1), x0, y0, z1);
			float f = gradientDotV(hash(xi1, yi0, zi1), x1, y0, z1);
			float g = gradientDotV(hash(xi0, yi1, zi1), x0, y1, z1);
			float h = gradientDotV(hash(xi1, yi1, zi1), x1, y1, z1);

			float u = quintic(tx);
			float v = quintic(ty);
			float w = quintic(tz);

			float du = quinticDeriv(tx);
			float dv = quinticDeriv(ty);
			float dw = quinticDeriv(tz);

			float k0 = a;
			float k1 = (b - a);
			float k2 = (c - a);
			float k3 = (e - a);
			float k4 = (a + d - b - c);
			float k5 = (a + f - b - e);
			float k6 = (a + g - c - e);
			float k7 = (b + c + e + h - a - d - f - g);

			derivs.x = du * (k1 + k4 * v + k5 * w + k7 * v * w);
			derivs.y = dv * (k2 + k4 * u + k6 * w + k7 * v * w);
			derivs.z = dw * (k3 + k5 * u + k6 * v + k7 * v * w);

			return k0 + k1 * u + k2 * v + k3 * w + k4 * u * v + k5 * u * w + k6 * v * w + k7 * u * v * w;
		}

		float gradientDotV(uint8_t perm, float x, float y, float z) const
		{
			// perm : a value between 0 and 255
			// xyz :  vector from one of the corners of the cell to the point where the noise function is computed
			switch (perm & 15)
			{
			case 0: return x + y;	// (1, 1, 0)
			case 1: return -x + y;	// (-1, 1, 0)
			case 2: return x - y;	// (1, -1, 0)
			case 3: return -x - y;	// (-1, -1, 0)
			case 4: return x + z;	// (1, 0, 1)
			case 5: return -x + z;	// (-1, 0, 1)
			case 6: return x - z;	// (1, 0, -1)
			case 7: return -x - z;	// (-1, 0, -1)
			case 8: return y + z;	// (0, 1, 1)
			case 9: return -y + z;	// (0, -1, 1)
			case 10: return y - z;	// (0, 1, -1),
			case 11: return -y - z;	// (0, -1, -1)
			case 12: return y + x;	// (1, 1, 0)
			case 13: return -x + y;	// (-1, 1, 0)
			case 14: return -y + z;	// (0, -1, 1)
			case 15: return -y - z;	// (0, -1, -1)
			}

			return -1;
		}
	};
}

#endif 