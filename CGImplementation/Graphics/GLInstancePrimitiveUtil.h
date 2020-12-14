#pragma once
#ifndef __GL_INSTNACE_PRIMITIVE_UTIL_H__
#define __GL_INSTNACE_PRIMITIVE_UTIL_H__

#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <CGErrorLogger.h>



namespace CG
{
	static unsigned cubeVAO;
	static unsigned cubeVBO;
	static unsigned cubeOneInstanceModelVBO;
	static unsigned cubeOneInstanceWorldNormalVBO;
	static unsigned cubeInstanceNumb;

	static unsigned sphereVAO;
	static unsigned sphereVBO;
	static unsigned sphereEBO;
	static unsigned sphereindexCount;
	static unsigned sphereOneInstanceModelVBO;
	static unsigned sphereOneInstanceWorldNormalVBO;

	static unsigned quadVAO;
	static unsigned quadVBO;
	static unsigned quadOneInstanceModelVBO;
	static unsigned quadOneInstanceWorldNormalVBO;

	// This class is used to render primitives for the deferred shader
	// using instancing
	class CGInstancePrimitiveUtil
	{
	public:
		static void setCubeOneInstanceData(
			const std::vector<glm::mat4>& model,
			const std::vector<glm::mat4>& worldNormal)
		{
			if (cubeVAO == 0)
			{
				float vertices[] = {
					// back face
					-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
					 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
					 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
					 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
					-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
					-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
					// front face
					-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
					 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
					 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
					 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
					-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
					-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
					// left face
					-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
					-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
					-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
					-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
					-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
					-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
					// right face
					 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
					 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
					 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
					 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
					 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
					 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
					// bottom face
					-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
					 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
					 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
					 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
					-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
					-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
					// top face
					-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
					 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
					 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
					 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
					-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
					-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
				};
				glGenVertexArrays(1, &cubeVAO);
				glGenBuffers(1, &cubeVBO);
				// fill buffer
				glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
				// link vertex attributes
				glBindVertexArray(cubeVAO);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
				
				GLsizei vec4Size = sizeof(glm::vec4);

				glGenBuffers(1, &cubeOneInstanceModelVBO);
				glBindBuffer(GL_ARRAY_BUFFER, cubeOneInstanceModelVBO);
				glBufferData(GL_ARRAY_BUFFER, model.size() * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
				for (int i = 4; i < 4 + 4; ++i)
				{
					glEnableVertexAttribArray(i);
					glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size * (i - 4)));
					glVertexAttribDivisor(i, 1);
				}

				glGenBuffers(1, &cubeOneInstanceWorldNormalVBO);
				glBindBuffer(GL_ARRAY_BUFFER, cubeOneInstanceWorldNormalVBO);
				glBufferData(GL_ARRAY_BUFFER, model.size() * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
				for (int i = 8; i < 8 + 4; ++i)
				{
					glEnableVertexAttribArray(i);
					glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size * (i - 8)));
					glVertexAttribDivisor(i, 1);
				}

				glBindVertexArray(0);
				cubeInstanceNumb = model.size();

				glCheckError();
			}

			glBindBuffer(GL_ARRAY_BUFFER, cubeOneInstanceModelVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * model.size(), glm::value_ptr(model[0]));

			glBindBuffer(GL_ARRAY_BUFFER, cubeOneInstanceWorldNormalVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * worldNormal.size(), glm::value_ptr(worldNormal[0]));
		}

		static void setSphereOneInstanceData(
			const std::vector<glm::mat4>& model,
			const std::vector<glm::mat4>& worldNormal)
		{
			if (sphereVAO == 0)
			{
				glGenVertexArrays(1, &sphereVAO);

				glGenBuffers(1, &sphereVBO);
				glGenBuffers(1, &sphereEBO);

				std::vector<glm::vec3> positions;
				std::vector<glm::vec2> uv;
				std::vector<glm::vec3> normals;
				std::vector<unsigned int> indices;

				const unsigned int X_SEGMENTS = 64;
				const unsigned int Y_SEGMENTS = 64;
				const float PI = 3.14159265359f;
				for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
				{
					for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
					{
						float xSegment = (float)x / (float)X_SEGMENTS;
						float ySegment = (float)y / (float)Y_SEGMENTS;
						float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
						float yPos = std::cos(ySegment * PI);
						float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

						positions.push_back(glm::vec3(xPos, yPos, zPos));
						uv.push_back(glm::vec2(xSegment, ySegment));
						normals.push_back(glm::vec3(xPos, yPos, zPos));
					}
				}

				bool oddRow = false;
				for (int y = 0; y < Y_SEGMENTS; ++y)
				{
					if (!oddRow) // even rows: y == 0, y == 2; and so on
					{
						for (int x = 0; x <= X_SEGMENTS; ++x)
						{
							indices.push_back(y       * (X_SEGMENTS + 1) + x);
							indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
						}
					}
					else
					{
						for (int x = X_SEGMENTS; x >= 0; --x)
						{
							indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
							indices.push_back(y       * (X_SEGMENTS + 1) + x);
						}
					}
					oddRow = !oddRow;
				}
				sphereindexCount = indices.size();

				std::vector<float> data;
				for (unsigned i = 0; i < positions.size(); ++i)
				{
					data.push_back(positions[i].x);
					data.push_back(positions[i].y);
					data.push_back(positions[i].z);
					if (uv.size() > 0)
					{
						data.push_back(uv[i].x);
						data.push_back(uv[i].y);
					}
					if (normals.size() > 0)
					{
						data.push_back(normals[i].x);
						data.push_back(normals[i].y);
						data.push_back(normals[i].z);
					}
				}
				glBindVertexArray(sphereVAO);
				glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
				glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
				GLsizei stride = (3 + 2 + 3) * sizeof(float);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));

				GLsizei vec4Size = sizeof(glm::vec4);

				glGenBuffers(1, &sphereOneInstanceModelVBO);
				glBindBuffer(GL_ARRAY_BUFFER, sphereOneInstanceModelVBO);
				glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
				for (int i = 4; i < 4 + 4; ++i)
				{
					glEnableVertexAttribArray(i);
					glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size * (i - 4)));
					glVertexAttribDivisor(i, 1);
				}

				glGenBuffers(1, &sphereOneInstanceWorldNormalVBO);
				glBindBuffer(GL_ARRAY_BUFFER, sphereOneInstanceWorldNormalVBO);
				glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
				for (int i = 8; i < 8 + 4; ++i)
				{
					glEnableVertexAttribArray(i);
					glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size * (i - 8)));
					glVertexAttribDivisor(i, 1);
				}

				glBindVertexArray(0);
				glCheckError();
			}

			glBindBuffer(GL_ARRAY_BUFFER, sphereOneInstanceModelVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * model.size(), glm::value_ptr(model[0]));

			glBindBuffer(GL_ARRAY_BUFFER, sphereOneInstanceWorldNormalVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * worldNormal.size(), glm::value_ptr(worldNormal[0]));
		}

		static void setQuadOneInstanceData(
			const std::vector<glm::mat4>& model,
			const std::vector<glm::mat4>& worldNormal)
		{
			if (quadVAO == 0)
			{
				float quadVertices[] = {
					// positions         // normal          // texture Coords
					-1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
					1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
					-1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
					 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
				};

				// setup plane VAO
				glGenVertexArrays(1, &quadVAO);
				glGenBuffers(1, &quadVBO);

				glBindVertexArray(quadVAO);

				glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

				GLsizei vec4Size = sizeof(glm::vec4);

				glGenBuffers(1, &quadOneInstanceModelVBO);
				glBindBuffer(GL_ARRAY_BUFFER, quadOneInstanceModelVBO);
				glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
				for (int i = 4; i < 4 + 4; ++i)
				{
					glEnableVertexAttribArray(i);
					glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size * (i - 4)));
					glVertexAttribDivisor(i, 1);
				}

				glGenBuffers(1, &quadOneInstanceWorldNormalVBO);
				glBindBuffer(GL_ARRAY_BUFFER, quadOneInstanceWorldNormalVBO);
				glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
				for (int i = 8; i < 8 + 4; ++i)
				{
					glEnableVertexAttribArray(i);
					glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size * (i - 8)));
					glVertexAttribDivisor(i, 1);
				}

				glBindVertexArray(0);
				glCheckError();
			}
			
			glBindBuffer(GL_ARRAY_BUFFER, quadOneInstanceModelVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * model.size(), glm::value_ptr(model[0]));

			glBindBuffer(GL_ARRAY_BUFFER, quadOneInstanceWorldNormalVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * worldNormal.size(), glm::value_ptr(worldNormal[0]));
		}

		static void renderCube()
		{
			glBindVertexArray(cubeVAO);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 36, cubeInstanceNumb);

			glBindVertexArray(0);
		}

		static void renderSphere()
		{
			glBindVertexArray(sphereVAO);
			glDrawElementsInstanced(GL_TRIANGLES, sphereindexCount, GL_UNSIGNED_INT, 0, 1);
			
			glBindVertexArray(0);
		}

		static void renderQuad()
		{
			glBindVertexArray(quadVAO);
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);
			
			glBindVertexArray(0);
		}

		static void destroy()
		{
			glDeleteBuffers(1, &cubeVBO);
			glDeleteBuffers(1, &cubeOneInstanceModelVBO);
			glDeleteBuffers(1, &cubeOneInstanceWorldNormalVBO);
			glDeleteVertexArrays(1, &cubeVAO);

			glDeleteBuffers(1, &sphereVBO);
			glDeleteBuffers(1, &sphereEBO);
			glDeleteBuffers(1, &sphereOneInstanceModelVBO);
			glDeleteBuffers(1, &sphereOneInstanceWorldNormalVBO);
			glDeleteVertexArrays(1, &sphereVAO);

			glDeleteBuffers(1, &quadVBO);
			glDeleteBuffers(1, &quadOneInstanceModelVBO);
			glDeleteBuffers(1, &quadOneInstanceWorldNormalVBO);
			glDeleteVertexArrays(1, &quadVAO);
		}
	};
}

#endif