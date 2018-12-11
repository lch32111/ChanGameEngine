#pragma once
#ifndef __CG_EDIT_DIR_LIGHT_VISUALIZER_H__
#define __CG_EDIT_DIR_LIGHT_VISUALIZER_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Graphics/Shader.h>

namespace CGProj
{
	class CGEditDirLightVisualizer
	{
	public:
		void prepareData(Shader* shader);
		void setCylinderDimension(float height, float tRadius, float bRadius);
		void setConeDimension(float height, float tRadius, float bRadius);

		void render(const glm::mat4& view, const glm::mat4& proj,
			const glm::vec3& position, const glm::vec3& direction);
	private:
		glm::vec3 m_cylinderColor = glm::vec3(0.90, 0.27, 0.71);
		glm::vec3 m_coneColor = glm::vec3(0.63, 0.86, 0.109);
		glm::vec3 m_localDirection = glm::vec3(0, 1, 0);

		enum { e_maxVertices = 512 };
		glm::vec3 m_cylinderVertices[e_maxVertices];
		unsigned m_cylinderIndices[e_maxVertices];
		unsigned m_cylinderCount;
		float m_cylinderHeight;
		float m_cylinderTopRadius;
		float m_cylinderBottomRadius;

		glm::vec3 m_coneVertices[e_maxVertices];
		unsigned m_coneIndices[e_maxVertices];
		unsigned m_coneCount;
		float m_coneHeight;
		float m_coneTopRadius;
		float m_coneBottomRadius;

		Shader* m_shader;
		unsigned m_cylinderVAO;
		unsigned m_cylinderVBO;
		unsigned m_cylinderEBO;

		unsigned m_coneVAO;
		unsigned m_coneVBO;
		unsigned m_coneEBO;



	};
}
#endif