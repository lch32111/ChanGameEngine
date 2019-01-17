#pragma once
#ifndef __CG_SIZABLE_RENDER_LINE_H__
#define __CG_SIZABLE_RENDER_LINE_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Graphics/Shader.h>

namespace CGProj
{
	// Forward Declaration
	class CGAssetManager;

	class CGSizableRenderLine
	{
	public:
		CGSizableRenderLine();
		CGSizableRenderLine(CGAssetManager& am, unsigned maxLineNumb);
		void initialize(Shader* shader, unsigned maxLineNumb);
		void destroy();

		void insertLine(const glm::vec3& From, const glm::vec3& To,
			const glm::vec4& Color = glm::vec4(1.0));
		void renderLine(const glm::mat4& view, const glm::mat4& proj, const float lineWidth = 1.f);

		void setFlush(bool f);
	private:
		Shader* m_lineShader;

		unsigned m_bufferSize;
		glm::vec3* m_vertices;
		glm::vec4* m_colors;

		unsigned m_count;
		unsigned m_VAO;
		unsigned m_VBO[2];

		bool m_isFlush;

		void prepareData();
	};
}

#endif