#pragma once
#ifndef __CG_PERLIN_NOISE_H__
#define __CG_PERLIN_NOISE_H__

#include <random>
#include <functional>

#include <glm/glm.hpp>

#include <Terrain/CGTerrainUtil.h>

namespace CGProj
{
	class CGPerlinNoise
	{
		static const unsigned tableSize = 256;
		static const unsigned tableSizeMask = tableSize - 1;
		glm::vec3 gradients[tableSize];
		unsigned permutationTable[tableSize * 2];

		int hash(const int&x, const int& y, const int& z) const
		{
			return permutationTable[permutationTable[permutationTable[x] + y] + z];
		}
	public:
		CGPerlinNoise(unsigned seed = 2018)
		{
			std::mt19937 generator(seed);
			std::uniform_real_distribution<float> distribution;
			auto dice = std::bind(distribution, generator);
			float PI = 3.14159265358979323846264338327950288f;
			for (unsigned i = 0; i < tableSize; ++i)
			{
				float theta = acos(2 * dice() - 1);
				float phi = 2 * dice() * PI;

				gradients[i].x = cos(phi) * sin(theta);
				gradients[i].y = sin(phi) * sin(theta);
				gradients[i].z = cos(theta);

				permutationTable[i] = i;
			}


			std::uniform_int_distribution<int> distributionInt;
			auto diceInt = std::bind(distributionInt, generator);
			// create permutation table
			for (unsigned i = 0; i < tableSize; ++i)
			{
				std::swap(permutationTable[i], permutationTable[diceInt() & tableSizeMask]);
				permutationTable[i + tableSize] = permutationTable[i];
			}
		}

	float eval(const glm::vec3& p) const
	{
		int xi0 = ((int)std::floor(p.x)) & tableSizeMask;
		int yi0 = ((int)std::floor(p.y)) & tableSizeMask;
		int zi0 = ((int)std::floor(p.z)) & tableSizeMask;

		int xi1 = (xi0 + 1) & tableSizeMask;
		int yi1 = (yi0 + 1) & tableSizeMask;
		int zi1 = (zi0 + 1) & tableSizeMask;

		// gradients at the corner of the cell
		const glm::vec3& c000 = gradients[hash(xi0, yi0, zi0)];
		const glm::vec3& c100 = gradients[hash(xi1, yi0, zi0)];
		const glm::vec3& c010 = gradients[hash(xi0, yi1, zi0)];
		const glm::vec3& c110 = gradients[hash(xi1, yi1, zi0)];

		const glm::vec3& c001 = gradients[hash(xi0, yi0, zi1)];
		const glm::vec3& c101 = gradients[hash(xi1, yi0, zi1)];
		const glm::vec3& c011 = gradients[hash(xi0, yi1, zi1)];
		const glm::vec3& c111 = gradients[hash(xi1, yi1, zi1)];

		float tx = p.x - ((int)std::floor(p.x));
		float ty = p.y - ((int)std::floor(p.y));
		float tz = p.z - ((int)std::floor(p.z));

		// generate vectors going from the grid points to p
		/*** 18-12-28 Chanhaneg Lee 
		To clear why this code is as it is, 
		if you assume c000 is (0, 0, 0), then the vector from grid points to p is
		Vector = P - (Grid Point c000). And tx, ty tz is the thing, c000 
		because p.x - ((int)floor(p.x)) makes it. 
		And the reason x1 is subtracted by 1, such as p100, 
		the coordinate c100 is (1, 0, 0).
		The equation of calculating the vector P - (Grid Point c100).
		Therefore, tx should be added by -1 (c100 is added by 1, compared c000)
		***/

		float x0 = tx, x1 = tx - 1;
		float y0 = ty, y1 = ty - 1;
		float z0 = tz, z1 = tz - 1;

		glm::vec3 p000 = glm::vec3(x0, y0, z0);
		glm::vec3 p100 = glm::vec3(x1, y0, z0);
		glm::vec3 p010 = glm::vec3(x0, y1, z0);
		glm::vec3 p110 = glm::vec3(x1, y1, z0);

		glm::vec3 p001 = glm::vec3(x0, y0, z1);
		glm::vec3 p101 = glm::vec3(x1, y0, z1);
		glm::vec3 p011 = glm::vec3(x0, y1, z1);
		glm::vec3 p111 = glm::vec3(x1, y1, z1);

		float u = smoothStep(tx);
		float v = smoothStep(ty);
		float w = smoothStep(tz);

		// linear interpolation
		float a = lerp(glm::dot(c000, p000), glm::dot(c100, p100), u);
		float b = lerp(glm::dot(c010, p010), glm::dot(c110, p110), u);
		float c = lerp(glm::dot(c001, p001), glm::dot(c101, p101), u);
		float d = lerp(glm::dot(c011, p011), glm::dot(c111, p111), u);

		float e = lerp(a, b, v);
		float f = lerp(c, d, v);

		return lerp(e, f, w);
	}
	};
}

#endif 