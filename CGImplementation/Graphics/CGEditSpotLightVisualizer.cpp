#include <Graphics/CGEditSpotLightVisualizer.h>
#include <glad/glad.h>

void CGProj::CGEditSpotLightVisualizer::setInnerConeInRadians(float inner_radian, float radius)
{
	m_count = cutArray[2];

	m_InnerCutoffInRadian = inner_radian;

	// Inner Cone Seconds
	float radiusLocalX = std::sin(m_InnerCutoffInRadian) * radius;
	float radiusLocalY = std::cos(m_OuterCutoffInRadian) * radius;

	int width = 360 / 20;
	float radiPI = glm::pi<float>() / 180.f;

	// Cone
	for (int i = 0; i <= 360; i += width)
	{
		float angle = (float)i * radiPI;

		m_vertices[m_count++] = glm::vec3(0); // connection with origin
		m_vertices[m_count++] = glm::vec3(radiusLocalX * std::cosf(angle), -radiusLocalY, std::sinf(angle) * radiusLocalX);
	}

	// XZ Circle
	for (int i = 0; i <= 360; i += width)
	{
		float angle = (float)i * radiPI;
		m_vertices[m_count++] = glm::vec3(radiusLocalX * std::cosf(angle), -radiusLocalY, std::sinf(angle) * radiusLocalX);
	}

	// XY Half Circle
	float HalfLocalY = radius - radiusLocalY;
	for (int i = 180; i <= 360; i += width)
	{
		float angle = (float)i * radiPI;
		m_vertices[m_count++] = glm::vec3(-radiusLocalX * std::cosf(angle), -radiusLocalY + HalfLocalY * std::sinf(angle), 0);
	}
	cutArray[3] = m_count;
	cutNumArray[2] = cutArray[3] - cutArray[2];

	// YZ Half Circle
	for (int i = 180; i <= 360; i += width)
	{
		float angle = (float)i * radiPI;
		m_vertices[m_count++] = glm::vec3(0, -radiusLocalY + HalfLocalY * std::sinf(angle), radiusLocalX * std::cosf(angle));
	}

	cutNumArray[3] = m_count - cutArray[3];
	
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, cutArray[2] * sizeof(glm::vec3), (m_count - cutArray[2]) * sizeof(glm::vec3), m_vertices + cutArray[2]);

	glBindVertexArray(0);
}

void CGProj::CGEditSpotLightVisualizer::setOuterConeInRadians(float outer_radian, float radius)
{
	m_count = 0;

	m_OuterCutoffInRadian = outer_radian;

	float radiusLocalX = std::sin(m_OuterCutoffInRadian) * radius;
	float radiusLocalY = std::cos(m_OuterCutoffInRadian) * radius;

	int width = 360 / 20;
	float radiPI = glm::pi<float>() / 180.f;

	// Cone
	for (int i = 0; i <= 360; i += width)
	{
		float angle = i * radiPI;

		m_vertices[m_count++] = glm::vec3(0); // connection with origin
		m_vertices[m_count++] = glm::vec4(glm::vec3(radiusLocalX * std::cosf(angle), -radiusLocalY, std::sinf(angle) * radiusLocalX), 1.0);
	}

	// XZ Circle
	for (int i = 0; i <= 360; i += width)
	{
		float angle = i * radiPI;
		m_vertices[m_count++] = glm::vec3(radiusLocalX * std::cosf(angle), -radiusLocalY, std::sinf(angle) * radiusLocalX);
	}

	// XY Half Circle
	float HalfLocalY = radius - radiusLocalY;
	for (int i = 180; i <= 360; i += width)
	{
		float angle = i * radiPI;
		m_vertices[m_count++] = glm::vec3(-radiusLocalX * std::cosf(angle), -radiusLocalY + HalfLocalY * std::sinf(angle), 0);
	}

	cutArray[1] = m_count; // Cut Index for first draw call
	cutNumArray[0] = m_count; // number of vertices for first draw call

	// YZ Half Circle
	for (int i = 180; i <= 360; i += width)
	{
		float angle = i * radiPI;
		m_vertices[m_count++] = glm::vec3(0, -radiusLocalY + HalfLocalY * std::sinf(angle), radiusLocalX * std::cosf(angle));
	}

	cutArray[2] = m_count;
	cutNumArray[1] = cutArray[2] - cutArray[1];

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(glm::vec3), m_vertices);

	glBindVertexArray(0);
}

void CGProj::CGEditSpotLightVisualizer::render(const glm::mat4& view, const glm::mat4& proj, 
	const glm::vec3& position, const glm::vec3& direction)
{

	glm::mat4 mvpMatrix = glm::mat4_cast(glm::quat(m_localDirection, direction)); // rotation
	mvpMatrix[3] = glm::vec4(position, 1); // translate
	mvpMatrix = proj * view * mvpMatrix;

	// Outer Cone First
	m_shader->use();
	m_shader->setMat4("mvpMatrix", mvpMatrix);
	
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	// Outer Cone
	m_shader->setVec3("Color", m_OuterColor);
	glMultiDrawArrays(GL_LINE_STRIP, cutArray, cutNumArray, 2);

	// Inner Cone
	m_shader->setVec3("Color", m_InnerColor);
	glMultiDrawArrays(GL_LINE_STRIP, cutArray + 2, cutNumArray + 2, 2);

	glBindVertexArray(0);
}

void CGProj::CGEditSpotLightVisualizer::prepareData(Shader* shader)
{
	m_shader = shader;

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_count = 0;
}
