#include <CGPrecompiled.h>
#include <Graphics/CGBloomEffect.h>

#include <CGErrorLogger.h>
#include <Graphics/CGAssetManager.h>
#include <Graphics/GLPrimitiveUtil.h>

CG::CGBloomEffect::CGBloomEffect()
{ }

void CG::CGBloomEffect::Initialize(CGAssetManager & am, int width, int height, unsigned iteration)
{ 
	m_GaussianShaer = am.getShader(SHADER_GAUSSIAN_BLUR);
	m_iteration = iteration;

	glGenFramebuffers(2, m_FBO);
	glGenTextures(2, m_ColorBuffer);

	for (unsigned i = 0; i < 2; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO[i]);
		glBindTexture(GL_TEXTURE_2D, m_ColorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorBuffer[i], 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			CGassert();
		glCheckError();
	}
}

void CG::CGBloomEffect::Destroy()
{
	glDeleteTextures(2, m_ColorBuffer);
	glDeleteFramebuffers(2, m_FBO);
}

void CG::CGBloomEffect::setTextureDimension(int width, int height)
{
	for (unsigned i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_ColorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

		glCheckError();
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void CG::CGBloomEffect::setIteration(unsigned num)
{
	m_iteration = num;
}

unsigned CG::CGBloomEffect::getIteration()
{
	return m_iteration;
}

unsigned CG::CGBloomEffect::getBloomedTexture(unsigned brightTexture)
{
	GaussianBlur(brightTexture);
	return m_ColorBuffer[!m_horizontal];
}

void CG::CGBloomEffect::GaussianBlur(unsigned firstTex)
{
	m_GaussianShaer->use();
	m_horizontal = true;
	bool first = true;
	for (unsigned i = 0; i < m_iteration; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO[m_horizontal]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first ? firstTex : m_ColorBuffer[!m_horizontal]);
		m_GaussianShaer->setBool("horizontal", m_horizontal);
		renderScreenQuad();

		m_horizontal = !m_horizontal;
		if (first) first = false;
	}
}
