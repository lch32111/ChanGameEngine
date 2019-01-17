#include <Graphics/CGAssetTexture.h>
#include <Graphics/GLTextureUtility.h>
#include <CGErrorLogger.h>

CGProj::CGAssetTexture::CGAssetTexture()
{ }

CGProj::CGAssetTexture::CGAssetTexture(const char * path)
	:m_texture(0), m_isloaded(false), m_directory(path)
{ }

bool CGProj::CGAssetTexture::loadTexture(bool gamma_correct)
{
	m_texture = TextureFromFile(m_directory, gamma_correct);
	glCheckError();
	
	m_isloaded = true;
	return true;
}
