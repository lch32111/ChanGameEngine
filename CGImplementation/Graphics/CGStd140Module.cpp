#include <Graphics/CGStd140Module.h>

#include <glm/gtc/type_ptr.hpp>

#include <CGErrorLogger.h>
#include <Graphics/Shader.h>



CGProj::CGStd140Module::CGStd140Module()
{
}

void CGProj::CGStd140Module::initialize(const std::string & bufferName)
{
	m_bufferName = bufferName;
	glGenBuffers(1, &m_uboBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboBlock);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uboBlock, 0, sizeof(glm::mat4) * 2);
	glCheckError();

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void CGProj::CGStd140Module::Destroy()
{
	glDeleteBuffers(1, &m_uboBlock);
}

void CGProj::CGStd140Module::addShader(Shader * shader)
{
	unsigned index = glGetUniformBlockIndex(shader->ID, m_bufferName.c_str());
	// Only use 0 block;
	// TODO: utilize lots of CGStd140 Module later
	glUniformBlockBinding(shader->ID, index, 0);

	glCheckError();
}

void CGProj::CGStd140Module::populateBuffer(const glm::mat4 & view, const glm::mat4 & proj)
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboBlock);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
