
#ifndef __CG_MODEL_UTILS_H__
#define __CG_MODEL_UTILS_H__

#include <iostream>
#include <vector>
#include <string>

namespace CGProj
{
	/* 19-01-16 Chanhaeng Lee
	 * I made this util header file to support mesh rendering process.
	 * Especially, This file has a relationship with the Model, Mesh Class.
	 */
	enum CG_MODEL_MATERIAL_ENUM
	{
		DIFFUSE,
		SPECULAR,
		EMISSIVE,
		NORMALMAP,
		DEPTHMAP,
		NUM_CG_MODEL_MATERIAL_ENUM,
	};

	static const std::vector<std::string> materialList =
	{
		"LMdiffuse",
		"LMspecular",
		"LMemissive",
		"NormalMap",
		"DepthMap"
		// "LMreflect",
	};

	static const std::vector<int> materialKeyList =
	{
		(1 << (int)('d' - 'a')), // diffuse == 8 (1 << 3)
		(1 << (int)('s' - 'a')), // specular == 262144 (1 << 18)
		(1 << (int)('e' - 'a')), // emissive == 16 (1 << 4)
		(1 << (int)('n' - 'a')), // normalMap == 8192 (1 << 13)
		(1 << (int)('h' - 'a')), // depthMap('h'eightMap) == 128 (1 << 7)
		// (1 << (int)('r' - 'a')) // reflect == 131072 (1 << 17)
	};
}

#endif 

