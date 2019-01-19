#pragma once
#ifndef __CG_BLOOM_EFFECT_H__
#define __CG_BLOOM_EFFECT_H__

#include <Graphics/Shader.h>

namespace CGProj
{
	// Forward Declaration
	class CGAssetManager;

	class CGBloomEffect
	{
	public:
		CGBloomEffect();
		
		void Initialize(CGAssetManager& am, 
			int width, int height,
			unsigned iteration);
		void Destroy();
		
		void setTextureDimension(int width, int height);
		
		void setIteration(unsigned num);
		unsigned getIteration();

		unsigned getBloomedTexture(unsigned brightTexture);

		// Deferred Second Uniform Variable to detect brightness
		glm::vec3 m_BrightnessExtractor;
		float m_BrightnessThreShold;

		// TODO : Give more control to the gaussian blur later
		// Gaussian Blur Shader Uniform Variable to set weight
		// float m_GaussianWeight[5];
	private:
		unsigned m_FBO[2];
		unsigned m_ColorBuffer[2];
		bool m_horizontal;
		unsigned m_iteration;

		Shader* m_GaussianShaer;

		void GaussianBlur(unsigned firstTex);
	};
}

#endif