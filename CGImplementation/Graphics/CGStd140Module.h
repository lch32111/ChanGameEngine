#pragma once
#ifndef __CG_STD_140_MODULE_H__
#define __CG_STD_140_MODULE_H__

#include <string>
#include <glm/glm.hpp>

namespace CG
{
	// Forward Declaration
	class Shader;

	// The Class for uniform buffer sharing view and projection matrix
	class CGStd140Module
	{
	public:
		CGStd140Module();

		void initialize(const std::string& bufferName);
		void Destroy();
		void addShader(Shader* shader);

		void populateBuffer(const glm::mat4& view, const glm::mat4& proj);
	private:
		unsigned m_uboBlock;
		std::string m_bufferName;
	};
}


#endif