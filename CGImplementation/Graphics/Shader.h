/*
18-07-23 Chanhaneg Lee
This code structure is derived from learnopengl.com
I added a little thing to this with my taste
*/

#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace CGProj
{
	class Shader
	{
	public:
		bool isloadad = false;

		// the porgram ID
		unsigned int ID;

		// constructor reads and builds the shader
		Shader();
		Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
		Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath);

		// prepare Shader
		bool loadShader();
		bool loadShaderWithGeo();

		// use/activate the shader
		void use();

		// utility uniform functions
		void setBool(const std::string& name, bool value) const;
		void setUInt(const std::string& name, unsigned int value) const;
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;
		void setFloat4(const std::string& name, float value[4]) const;
		void setVec2(const std::string& name, const glm::vec2& value) const;
		void setVec2(const std::string& name, float x, float y) const;
		void setVec3(const std::string& name, const glm::vec3& value) const;
		void setVec3(const std::string& name, float x, float y, float z) const;
		void setVec4(const std::string& name, const glm::vec4& value) const;
		void setVec4(const std::string& name, float x, float y, float z, float w) const;
		void setMat2(const std::string& name, const glm::mat2& mat) const;
		void setMat3(const std::string& name, const glm::mat3& mat) const;
		void setMat4(const std::string& name, const glm::mat4& mat) const;
	private:
		std::string m_vertexPath, m_geometryPath, m_fragmentPath;
		bool checkCompileErrors(GLuint shader, std::string type);
	};
}

#endif