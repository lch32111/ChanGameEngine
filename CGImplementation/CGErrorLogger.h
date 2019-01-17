#pragma once
#ifndef __CG_ERROR_LOGGER_H__
#define __CG_ERROR_LOGGER_H__

#include <string>
#include <iostream>
#include <glad/glad.h>

namespace CGProj
{
	class GLErrorChecker
	{
	public:
		static GLenum glCheckError_(const char* file, int line)
		{
			GLenum errorCode;
			while ((errorCode = glGetError()) != GL_NO_ERROR)
			{
				std::string error;
				switch (errorCode)
				{
				case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
				case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
				case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
				case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
				case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
				case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
				case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
				}
				std::cout << error << " | " << file << " (" << line << ")\n";
				__debugbreak();
			}
			return errorCode;
		}

		static void assert_(const char* file, int line)
		{
			std::cout << "Error happend | " << file << " (" << line << ")\n";
			__debugbreak();
		}
	};
	#define glCheckError() GLErrorChecker::glCheckError_(__FILE__, __LINE__) 
	#define CGassert() GLErrorChecker::assert_(__FILE__, __LINE__)
}

#endif