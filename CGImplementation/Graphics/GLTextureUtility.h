#pragma once

#ifndef __GL_TEXTURE_UTILITY_H__
#define __GL_TEXTURE_UTILITY_H__

#include <string>
#include <vector>

namespace CG
{
	/**
	 * Get Texture Image with stbi library. if you pass 'true' boolean,
	 * you will get the image with SRGB data format.
	 * the method of wrap is GL_REPEAT
	 * the TEXTURE_MIN_FILTER is GL_LINEAR_MIPMAP_LINEAR
	 * the TEXTURE_MAG_FILTER is GL_LINEAR
	 */
	unsigned int TextureFromFile(const std::string file, bool gamma_correct);

	/**
	 * Get Cube map Texture. the image internal format is SRGB
	 * the output format is RGB. The Texture Parameters
	 * glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	 * glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	 * glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	 * glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	 * glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	 */
	unsigned int loadCubeMap(const std::vector<std::string>& faces);
}

#endif