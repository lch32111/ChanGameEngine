#pragma once
#ifndef __CG_EDIT_SPOT_LIGHT_VISUALIZER_H__
#define __CG_EDIT_SPOT_LIGHT_VISUALIZER_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace CGProj
{
	class CGEditSpotLightVisualizer
	{
	public:
		void prepareData();
		void setInnerConeInRadians(float inner_radian, float radius);
		void setOuterConeInRadians(float outer_radian, float radius);
		void render(const glm::mat4& view, const glm::mat4& proj,
			const glm::vec3& position, const glm::vec3& direction);
	private:
		float m_InnerCutoffInRadian;
		float m_OuterCutoffInRadian;

		glm::vec3 m_OuterColor = glm::vec3(0, 0.38, 1.0);
		glm::vec3 m_InnerColor = glm::vec3(0, 0.38, 0.47);
		glm::vec3 m_localDirection = glm::vec3(0, -1, 0);

		enum {e_maxVertices = 2 * 512};
		glm::vec3 m_vertices[e_maxVertices];

		unsigned m_shaderID;
		unsigned m_VAO;
		unsigned m_VBO;

		int cutArray[4] = { 0, };
		int cutNumArray[4] = { 0, };
		unsigned m_count;
	};
}


#endif
