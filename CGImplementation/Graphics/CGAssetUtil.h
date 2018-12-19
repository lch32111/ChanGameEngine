#pragma once
#ifndef __CG_ASSET_UTIL_H__
#define __CG_ASSET_UTIL_H__

#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace CGProj
{
	enum CG_SHADER_ENUM
	{
		SHADER_DEFERRED_FIRST,
		SHADER_DEFERRED_SECOND,
		SHADER_CG_LINE,
		SHADER_SIMPLE_RENDER,
		SHADER_SIMPLE_COLOR_RENDER,
		SHADER_WIRE_RENDER,
		SHADER_DIR_VISUALIZER,
		SHADER_POINT_VISUALIZER,
		SHADER_SPOT_VISUALIZER,
		SHADER_DIR_SHADOW_MAP,
		SHADER_DIR_SHADOW_MAP_DEBUG_RENDER,
		NUM_CG_SHADER_ENUM
	};

	enum CG_TEXTURE_ENUM
	{
		TEXTURE_CONTAINER_DIFFUSE,
		TEXTURE_CONTAINER_SPECULAR,
		TEXTURE_FIELD_GRASS,
		TEXTURE_GOLD,
		TEXTURE_MARBLE,
		TEXTURE_RED_MARBLE,
		TEXTURE_MATRIX,
		TEXTURE_BLUE_MATRIX,
		TEXTURE_WOOD_PANEL,
		NUM_CG_TEXTURE_ENUM
	};

	static const char* CG_TEXTURE_LIST[] =
	{
		"TEXTURE_CONTAINER_DIFFUSE",
		"TEXTURE_CONTAINER_SPECULAR",
		"TEXTURE_FIELD_GRASS",
		"TEXTURE_GOLD",
		"TEXTURE_MARBLE",
		"TEXTURE_RED_MARBLE",
		"TEXTURE_MATRIX",
		"TEXTURE_BLUE_MATRIX",
		"TEXTURE_WOOD_PANEL"
	};

	static const char* CG_COLOR_MATERIAL_LIST[] =
	{
		"COLOR_MATERIAL_EMERALD",
		"COLOR_MATERIAL_JADE",
		"COLOR_MATERIAL_OBSIDIAN",
		"COLOR_MATERIAL_PEARL",
		"COLOR_MATERIAL_RUBY",
		"COLOR_MATERIAL_TURQUOISE",
		"COLOR_MATERIAL_BRASS",
		"COLOR_MATERIAL_BRONZE",
		"COLOR_MATERIAL_CHROME",
		"COLOR_MATERIAL_COPPER",
		"COLOR_MATERIAL_GOLD",
		"COLOR_MATERIAL_SILVER",
		"COLOR_MATERIAL_BLACK_PLASTIC",
		"COLOR_MATERIAL_CYAN_PLASTIC",
		"COLOR_MATERIAL_GREEN_PLASTIC",
		"COLOR_MATERIAL_RED_PLASTIC",
		"COLOR_MATERIAL_WHITE_PLASTIC",
		"COLOR_MATERIAL_YELLOW_PLASTIC",
		"COLOR_MATERIAL_BLACK_RUBBER",
		"COLOR_MATERIAL_CYAN_RUBBER",
		"COLOR_MATERIAL_GREEN_RUBBER",
		"COLOR_MATERIAL_RED_RUBBER",
		"COLOR_MATERIAL_WHITE_RUBBER",
		"COLOR_MATERIAL_YELLOW_RUBBER",
	};

	static glm::vec3 CG_COLOR_MATERIAL_AMBIENT[] = 
	{
		glm::vec3(0.0215, 0.1745, 0.0215),
		glm::vec3(0.135, 0.2225, 0.1575),
		glm::vec3(0.05375, 0.05, 0.06625),
		glm::vec3(0.25, 0.20725, 0.20725),
		glm::vec3(0.1745, 0.01175, 0.01175),
		glm::vec3(0.1 , 0.18725, 0.1745),
		glm::vec3(0.329412, 0.223529, 0.027451),
		glm::vec3(0.2125, 0.1275, 0.054),
		glm::vec3(0.25, 0.25, 0.25),
		glm::vec3(0.19125, 0.0735, 0.0225),
		glm::vec3(0.24725, 0.1995, 0.0745),
		glm::vec3(0.19225, 0.19225, 0.19225),
		glm::vec3(0),
		glm::vec3(0.0 , 0.1 , 0.06),
		glm::vec3(0),
		glm::vec3(0),
		glm::vec3(0),
		glm::vec3(0),
		glm::vec3(0.02),
		glm::vec3(0.0, 0.05, 0.05),
		glm::vec3(0.0, 0.05, 0.0),
		glm::vec3(0.05, 0.0, 0.0),
		glm::vec3(0.05),
		glm::vec3(0.05, 0.05, 0.0)
	};

	static glm::vec3 CG_COLOR_MATERIAL_DIFFUSE[] = 
	{
		glm::vec3(0.075678, 0.61424, 0.07568),
		glm::vec3(0.54, 0.89, 0.63),
		glm::vec3(0.18275, 0.17, 0.22525),
		glm::vec3(1, 0.829, 0.829),
		glm::vec3(0.61424, 0.04136, 0.04136),
		glm::vec3(0.396, 0.74151, 0.69102),
		glm::vec3(0.780392, 0.568627, 0.113725),
		glm::vec3(0.714, 0.4284, 0.18144),
		glm::vec3(0.4),
		glm::vec3(0.7038, 0.27048, 0.0828),
		glm::vec3(0.75164, 0.60648, 0.22648),
		glm::vec3(0.50754),
		glm::vec3(0.01),
		glm::vec3(0.0, 0.50980392, 0.50980392),
		glm::vec3(0.1, 0.35, 0.1),
		glm::vec3(0.5, 0.0, 0.0),
		glm::vec3(0.55),
		glm::vec3(0.5, 0.5, 0.0),
		glm::vec3(0.01),
		glm::vec3(0.4, 0.5, 0.5),
		glm::vec3(0.4, 0.5, 0.4),
		glm::vec3(0.5, 0.4, 0.4),
		glm::vec3(0.5),
		glm::vec3(0.5, 0.5, 0.4)
	};

	static glm::vec3 CG_COLOR_MATERIAL_SPECULAR[] =
	{
		glm::vec3(0.633, 0.727811, 0.633),
		glm::vec3(0.316228),
		glm::vec3(0.332741, 0.328634, 0.346435),
		glm::vec3(0.296648),
		glm::vec3(0.727811, 0.626959, 0.626959),
		glm::vec3(0.297254, 0.30829, 0.306678),
		glm::vec3(0.992157, 0.941176, 0.807843),
		glm::vec3(0.393548, 0.271906, 0.166721),
		glm::vec3(0.774597),
		glm::vec3(0.256777, 0.137622, 0.086014),
		glm::vec3(0.628281, 0.555802, 0.366065),
		glm::vec3(0.508273, 0.508273, 0.508273),
		glm::vec3(0.50),
		glm::vec3(0.50196078),
		glm::vec3(0.45, 0.55, 0.45),
		glm::vec3(0.7, 0.6, 0.6),
		glm::vec3(0.70),
		glm::vec3(0.60, 0.60, 0.50),
		glm::vec3(0.4),
		glm::vec3(0.04, 0.7, 0.7),
		glm::vec3(0.04, 0.7, 0.04),
		glm::vec3(0.7, 0.04, 0.04),
		glm::vec3(0.7),
		glm::vec3(0.7, 0.7, 0.04)
	};

	static float CG_COLOR_MATERIAL_SHININESS[] =
	{
		0.6,
		0.1,
		0.3,
		0.088,
		0.6,
		0.1,
		0.21794872,
		0.2,
		0.6,
		0.1,
		0.4,
		0.4,
		0.25,
		0.25,
		0.25,
		0.25,
		0.25,
		0.25,
		0.078125,
		0.078125,
		0.078125,
		0.078125,
		0.078125,
		0.078125,
	};
}

#endif 