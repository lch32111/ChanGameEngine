#include <Graphics/CGSSAOEffect.h>

#include <random> 
#include <functional> 

#include <Graphics/CGAssetManager.h>

CGProj::CGSSAOEffect::CGSSAOEffect()
{ }

#define quickLerp(a, b, c) ((1.f - (c)) * (a) + (c) * (b))
void CGProj::CGSSAOEffect::Initialize(CGAssetManager & am, unsigned seed, unsigned kernelNum, unsigned noiseNum)
{
	m_kernelNum = kernelNum;
	m_noiseNum = noiseNum;

	m_ssaoKernal = new glm::vec3[kernelNum];
	m_ssaoNoise = new glm::vec3[noiseNum];

	// m_SSAOShader = am.getShader(SHADER_SSAO_EFFECT);

	// Sample Kernerl Init
	// Kerner will be in tangent space point toard z direction
	std::mt19937 gen(seed);
	std::uniform_real_distribution<float> distrFloat;
	auto randFloat = std::bind(distrFloat, gen);

	glm::vec3 sample;
	for (unsigned i = 0; i < m_kernelNum; ++i)
	{
		sample.x = randFloat() * 2.f - 1.f;
		sample.y = randFloat() * 2.f - 1.f;
		sample.z = randFloat();

		sample = glm::normalize(sample);

		// Distribute the direction
		// Make Steep Slope when i is large, kind of x^2 graph.
		// It means we will get first closer points to the kerner center
		// later, we will get further points to the kerner center
		float scale = (float)i / m_kernelNum;
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

	unsigned textureDimension = (unsigned)sqrtf((float)m_noiseNum);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, textureDimension, textureDimension, 0, GL_RGB, GL_FLOAT, &m_ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
#undef quickLerp

void CGProj::CGSSAOEffect::Destroy()
{
	delete[] m_ssaoKernal;
	delete[] m_ssaoNoise;
}
