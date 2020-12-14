#pragma once
#ifndef __CG_ASSET_TEXTURE_H__
#define __CG_ASSET_TEXTURE_H__

#include <string>

namespace CG
{
	class CGAssetTexture
	{
	public:
		CGAssetTexture();
		CGAssetTexture(const char* path);
		bool loadTexture(bool gamma_correct);

		unsigned m_texture;
		bool m_isloaded;
		std::string m_directory;
	};
}

#endif