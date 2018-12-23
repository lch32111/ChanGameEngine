#pragma once
#ifndef __CG_EDIT_POINT_LIGHT_VISUALIZER_H__
#define __CG_EDIT_POINT_LIGHT_VISUALIZER_H__

#include <Graphics/Shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace CGProj
{
	class CGEditPointLightVisualizer
	{
	public:
		void prepareData(Shader* shader);
		void render(const glm::mat4& view, const glm::mat4& proj,
			const glm::vec3& position, const float scale);
	private:
		Shader* m_shader;

		glm::vec3 m_circleColor = glm::vec3(0.662, 0.831, 0.87);
	};
}

#endif