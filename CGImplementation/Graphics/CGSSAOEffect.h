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
			unsigned kernelNum, unsigned noiseNum);
		void Destroy();


	private:
		Shader* m_SSAOShader;
		
		glm::vec3* m_ssaoKernal;
		unsigned m_kernelNum;
		
		glm::vec3* m_ssaoNoise;
		unsigned m_noiseNum;
		unsigned m_noiseTexture;
	};
}

#endif