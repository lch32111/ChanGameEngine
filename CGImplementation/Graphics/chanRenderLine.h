#ifndef __CHAN_RENDER_LINE_H__
#define __CHAN_RENDER_LINE_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

/**
 * Class to render a line from A to B.
 * You can share this class anywhere in the code.
 */
class lineRenderer
{
public:
	lineRenderer(); // Manual Setting for the file path
	lineRenderer(const GLchar* vertexFile, 
		const GLchar* geoFile,
		const GLchar* fragFile);

	void renderline(const glm::mat4& view, const glm::mat4& proj,
		const glm::vec3& From, const glm::vec3& To,
		const glm::vec3& Color = glm::vec3(1.0));
private:
	Shader m_lineShader;
	unsigned int m_lineVAO = 0;
	unsigned int m_lineVBO;

	void prepareData();
};

#endif
