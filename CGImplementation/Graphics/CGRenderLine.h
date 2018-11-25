#pragma once
#ifndef __CG_RENDER_LINE_H__
#define __CG_RENDER_LINE_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

namespace CGProj
{
	class CGRenderLine
	{
	public:
		CGRenderLine();
		CGRenderLine(const GLchar* vertexFile, const GLchar* fragFile);

		void insertLine(const glm::vec3& From, const glm::vec3& To,
			const glm::vec4& Color = glm::vec4(1.0));
		void renderLine(const glm::mat4& view, const glm::mat4& proj, const float lineWidth);
	private:
		Shader m_lineShader;

		enum {e_maxVertices = 2 * 512};
		glm::vec3 m_vertices[e_maxVertices];
		glm::vec4 m_colors[e_maxVertices];

		unsigned m_count;

		unsigned m_VAO;
		unsigned m_VBO[2];

		void prepareData();
	};
}

#endif