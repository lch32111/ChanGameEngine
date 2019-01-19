#include <Graphics/CGSSAOEffect.h>

#include <random> 
#include <functional> 

#include <glm/gtc/type_ptr.hpp>

#include <CGErrorLogger.h>
#include <Graphics/CGAssetManager.h>
#include <Graphics/GLPrimitiveUtil.h>
#include <Graphics/CGDefSecondUtil.h>


CGProj::CGSSAOEffect::CGSSAOEffect()
{ }

#define quickLerp(a, b, c) ((1.f - (c)) * (a) + (c) * (b))
void CGProj::CGSSAOEffect::Initialize(CGAssetManager & am, unsigned seed, 
	unsigned noiseNum, float radius, float bias,
	int width, int height)
{
	m_ssaoShader = am.getShader(SHADER_SSAO_EFFECT);
	m_ssaoBlurShader = am.getShader(SHADER_SSAO_BLUR);

	m_noiseNum = noiseNum;

	m_ssaoKernal = new glm::vec3[NR_SSAO_KERNEL];
	m_ssaoNoise = new glm::vec3[noiseNum];

	m_screenWidth = width, m_screenHeight = height;

	m_ssaoRadius = radius;
	m_ssaoBias = bias;

	// Sample Kernerl Init
	// Kerner will be in tangent space point toard z direction
	std::mt19937 gen(seed);
	std::uniform_real_distribution<float> distrFloat;
	auto randFloat = std::bind(distrFloat, gen);

	glm::vec3 sample;
	for (unsigned i = 0; i < NR_SSAO_KERNEL; ++i)
	{
		sample.x = randFloat() * 2.f - 1.f;
		sample.y = randFloat() * 2.f - 1.f;
		sample.z = randFloat();

		sample = glm::normalize(sample);

		// Distribute the direction
		// Make Steep Slope when i is large, kind of x^2 graph.
		// It means we will get first closer points to the kerner center
		// later, we will get further points to the kerner center
		float scale = (float)i / (float)NR_SSAO_KERNEL;
		scale = quickLerp(0.1f, 1.f, scale * scale);
		m_ssaoKernal[i] = sample * scale;
	}

	for (unsigned i = 0; i < m_noiseNum; ++i)
	{
		m_ssaoNoise[i].x = randFloat() * 2.f - 1.f;
		m_ssaoNoise[i].y = randFloat() * 2.f - 1.f;
		m_ssaoNoise[i].z = 0;

		// Normalization
		m_ssaoNoise[i] *= (1.f / (m_ssaoNoise[i].x * m_ssaoNoise[i].x + m_ssaoNoise[i].y * m_ssaoNoise[i].y));
	}
	glGenTextures(1, &m_noiseTexture);
	glBindTexture(GL_TEXTURE_2D, m_noiseTexture);

	m_noiseNum = (unsigned)sqrtf((float)m_noiseNum);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_noiseNum, m_noiseNum, 0, GL_RGB, GL_FLOAT, &m_ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenFramebuffers(1, &m_ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO);

	glGenTextures(1, &m_ssaoRawTexture);
	glBindTexture(GL_TEXTURE_2D, m_ssaoRawTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoRawTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		CGassert();
	glCheckError();

	glGenFramebuffers(1, &m_ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoBlurFBO);

	glGenTextures(1, &m_ssaoBlurredTexture);
	glBindTexture(GL_TEXTURE_2D, m_ssaoBlurredTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoBlurredTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		CGassert();
	glCheckError();

}
#undef quickLerp

void CGProj::CGSSAOEffect::Destroy()
{
	delete[] m_ssaoKernal;
	delete[] m_ssaoNoise;
}

unsigned CGProj::CGSSAOEffect::getSSAOTexture(
	unsigned gPosition, unsigned gNormal,
	const glm::mat4& view, const glm::mat4& proj)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	// First Pass of SSAO : get occlusions by samples
	m_ssaoShader->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_noiseTexture);

	glUniform3fv(glGetUniformLocation(m_ssaoShader->ID, "samples"), NR_SSAO_KERNEL, glm::value_ptr(m_ssaoKernal[0]));
	m_ssaoShader->setMat4("view", view);
	m_ssaoShader->setMat4("projection", proj);
	m_ssaoShader->setInt("screenWidth", m_screenWidth);
	m_ssaoShader->setInt("screenHeight", m_screenHeight);
	m_ssaoShader->setInt("noiseTexDimension", m_noiseNum);
	m_ssaoShader->setFloat("radius", m_ssaoRadius);
	m_ssaoShader->setFloat("bias", m_ssaoBias);
	renderScreenQuad();

	// Second pass of SSAO : make the occlusions blurred to get the better result.
	glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoBlurFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	m_ssaoBlurShader->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ssaoRawTexture);

	renderScreenQuad();

	return m_ssaoBlurredTexture;
}

void CGProj::CGSSAOEffect::setTextureDimension(int width, int height)
{
	m_screenWidth = width;
	m_screenHeight = height;

	glBindTexture(GL_TEXTURE_2D, m_ssaoRawTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	
	glCheckError();

	glBindTexture(GL_TEXTURE_2D, m_ssaoBlurredTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	
	glCheckError();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void CGProj::CGSSAOEffect::setRadius(float radius)
{
	m_ssaoRadius = radius;
}

float CGProj::CGSSAOEffect::getRadius()
{
	return m_ssaoRadius;
}

void CGProj::CGSSAOEffect::setBias(float bias)
{
	m_ssaoBias = bias;
}

float CGProj::CGSSAOEffect::getBias()
{
	return m_ssaoBias;
}

void CGProj::CGSSAOEffect::setNoiseNum(unsigned noiseNum)
{
	glm::vec3* oldNoise = m_ssaoNoise;
	m_ssaoNoise = new glm::vec3[noiseNum];
	
	if(m_noiseNum * m_noiseNum >= noiseNum)
		memcpy(m_ssaoNoise, oldNoise, sizeof(glm::vec3) * noiseNum);
	else
	{
		memcpy(m_ssaoNoise, oldNoise, sizeof(glm::vec3) * m_noiseNum * m_noiseNum);

		// Generate more noise values
		std::mt19937 gen(998877);
		std::uniform_real_distribution<float> distrFloat;
		auto randFloat = std::bind(distrFloat, gen);

		for (unsigned i = m_noiseNum * m_noiseNum; i < noiseNum; ++i)
		{
			m_ssaoNoise[i].x = randFloat() * 2.f - 1.f;
			m_ssaoNoise[i].y = randFloat() * 2.f - 1.f;
			m_ssaoNoise[i].z = 0;

			// Normalization
			m_ssaoNoise[i] *= (1.f / (m_ssaoNoise[i].x * m_ssaoNoise[i].x + m_ssaoNoise[i].y * m_ssaoNoise[i].y));
		}
	}

	delete[] oldNoise;
	oldNoise = nullptr;

	m_noiseNum = (unsigned)sqrtf((float)noiseNum);

	glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_noiseNum, m_noiseNum, 0, GL_RGB, GL_FLOAT, &m_ssaoNoise[0]);

	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned CGProj::CGSSAOEffect::getNoiseNum()
{
	return m_noiseNum * m_noiseNum;
}
