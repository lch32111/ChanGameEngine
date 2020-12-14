#include <CGPrecompiled.h>
#include <Graphics/CGAssetTexture.h>
#include <Graphics/GLTextureUtility.h>
#include <CGErrorLogger.h>

CG::CGAssetTexture::CGAssetTexture()
{ }

CG::CGAssetTexture::CGAssetTexture(const char * path)
	:m_texture(0), m_isloaded(false), m_directory(path)
{ }

bool CG::CGAssetTexture::loadTexture(bool gamma_correct)
{
	m_texture = TextureFromFile(m_directory, gamma_correct);
	glCheckError();
	
	m_isloaded = true;
	return true;
}
