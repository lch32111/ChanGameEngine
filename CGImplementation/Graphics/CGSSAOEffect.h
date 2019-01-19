#pragma once
#ifndef __CG_SSAO_EFFECT_H__
#define __CG_SSAO_EFFECT_H__

#include <Graphics/Shader.h>

namespace CGProj
{
	// Forward Declaration
	class CGAssetManager;

	class CGSSAOEffect
	{
	public:
		CGSSAOEffect();

		void Initialize(CGAssetManager& am, unsigned seed, 
		unsigned noiseNum, float radius, float bias, float power,
		int width, int height);
		void Destroy();

		unsigned getSSAOTexture(unsigned gPosition, unsigned gNormal,
			const glm::mat4& view, const glm::mat4& proj);

		void setTextureDimension(int width, int height);

		void setRadius(float radius);
		float getRadius();

		void setBias(float bias);
		float getBias();

		void setPower(float power);
		float getPower();

		void setNoiseNum(unsigned noiseNum);
		unsigned getNoiseNum();

		void setBlurState(bool b);
		bool getBlurState();
	private:
		Shader* m_ssaoShader;
		Shader* m_ssaoBlurShader;
		
		glm::vec3* m_ssaoKernal;
		
		glm::vec3* m_ssaoNoise;
		unsigned m_noiseNum;
		unsigned m_noiseTexture;

		unsigned m_ssaoFBO;
		unsigned m_ssaoRawTexture;
		unsigned m_screenWidth, m_screenHeight;

		unsigned m_ssaoBlurFBO;
		unsigned m_ssaoBlurredTexture;

		float m_ssaoRadius;
		float m_ssaoBias;
		float m_ssaoPower;
		
		bool m_useBlur;
	};
}

#endif