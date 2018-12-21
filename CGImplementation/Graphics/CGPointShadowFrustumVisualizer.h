#pragma once
#ifndef __CG_POINT_SHADOW_FRUSTUM_VISUALIZER_H__
#define __CG_POINT_SHADOW_FRUSTUM_VISUALIZER_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Graphics/Shader.h>
#include <Graphics/CGAssetManager.h>

namespace CGProj
{
	class CGPointShadowFrustumVisualizer
	{
	public:
		CGPointShadowFrustumVisualizer();
		CGPointShadowFrustumVisualizer(CGAssetManager& am);

		void setShader(Shader* shader);

		void render
		(
			const glm::mat4& view, const glm::mat4& proj,
			const glm::vec3& position,
			const float nearP, const float farP
		);
	private:
		Shader* m_shader;

	};
}

#endif