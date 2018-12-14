#pragma once
#ifndef __CG_SHADOW_FRUSTUM_VISUALIZER_H__
#define __CG_SHADOW_FRUSTUM_VISUALIZER_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Graphics/Shader.h>
#include <Graphics/CGAssetManager.h>

namespace CGProj
{
	class CGShadowFrustumVisualizer
	{
	public:
		CGShadowFrustumVisualizer();
		CGShadowFrustumVisualizer(CGAssetManager& am);

		void setShader(Shader* shader);
		void render(bool projectionType, const glm::mat4& lightSpcae, const glm::vec3& position,
			const glm::mat4& view, const glm::mat4& proj);
		void render(
			const glm::mat4& view, const glm::mat4& proj,
			const glm::vec3& position, const glm::vec3& direction,
			float l, float r, float b, float t, float nearP, float farP);
		
	private:
		Shader* m_lineShader;
		glm::vec4 m_FrustumColor = glm::vec4(1.0, 0.61, 0, 1.0);
		glm::vec4 m_TestFrustumColor = glm::vec4(1.0, 0.0, 0, 1.0);

		enum {e_maxVertices = 24};
		glm::vec3 m_vertices[e_maxVertices];
		glm::vec4 m_colors[e_maxVertices];

		unsigned m_count;
		
		unsigned m_VAO;
		unsigned m_VBO[2];

		void prepareData();
		void insertLine(const glm::vec3 & From, const glm::vec3 & To, const glm::vec4 & Color);
		void renderLine(const glm::mat4 & view, const glm::mat4 & proj, const float lineWidth = 1.0);
	};
}

#endif
