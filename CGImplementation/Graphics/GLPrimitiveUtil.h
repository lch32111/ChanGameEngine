#pragma once
#ifndef __GL_PRIMITIVE_UTIL_H__
#define __GL_PRIMITIVE_UTIL_H__

#include <vector>

#include <glad/glad.h>

namespace CGProj
{
	static inline void renderCube()
	{
		static unsigned int cubeVAO = 0;
		static unsigned int cubeVBO = 0;

		// initialize (if necessary)
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
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	static inline void wireRenderCube(float linewidth)
	{
		static unsigned int wireCubeVAO = 0;
		static unsigned int wireCubeEBO = 0;
		static unsigned int wireCubeVBO = 0;

		// initialize (if necessary)
		if (wireCubeVAO == 0)
		{
			float vertices[] =
			{
				   -1.0, -1.0, -1.0,
				   -1.0, -1.0, 1.0,
				   1.0, -1.0, 1.0,
				   1.0, -1.0, -1.0,
				   -1.0, 1.0, -1.0,
				   -1.0, 1.0, 1.0,
				   1.0, 1.0, 1.0,
				   1.0, 1.0, -1.0
			};

			unsigned int indices[] =
			{
				0, 1,
				1, 2,
				2, 3,
				3, 0,
				0, 4,
				1, 5,
				2, 6,
				3, 7,
				4, 5,
				5, 6,
				6, 7,
				7, 4
			};
			glGenVertexArrays(1, &wireCubeVAO);
			glGenBuffers(1, &wireCubeVBO);
			glGenBuffers(1, &wireCubeEBO);

			glBindVertexArray(wireCubeVAO);

			glBindBuffer(GL_ARRAY_BUFFER, wireCubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireCubeEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

		}
		// render Cube
		glBindVertexArray(wireCubeVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireCubeEBO);
		glLineWidth(linewidth);
		glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	static inline void renderQuad()
	{
		static unsigned int quadVAO = 0;
		static unsigned int quadVBO;

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
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	static inline void renderScreenQuad()
	{
		static unsigned int screenQuadVAO = 0;
		static unsigned int screenQuadVBO;

		if (screenQuadVAO == 0)
		{
			float quadVertices[] = {
				// positions        //Normal       // texture Coords
				-1.0f,  1.0f, 0.0f, 0.f, 0.f, 1.f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.f, 0.f, 1.f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 0.f, 0.f, 1.f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 0.f, 0.f, 1.f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &screenQuadVAO);
			glGenBuffers(1, &screenQuadVBO);
			glBindVertexArray(screenQuadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		}
		glBindVertexArray(screenQuadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
	static inline void renderSphere()
	{
		static unsigned int sphereVAO = 0;
		static unsigned int indexCount;

		if (sphereVAO == 0)
		{
			glGenVertexArrays(1, &sphereVAO);

			unsigned int vbo, ebo;
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);

			std::vector<glm::vec3> positions;
			std::vector<glm::vec2> uv;
			std::vector<glm::vec3> normals;
			std::vector<unsigned int> indices;

			const unsigned int X_SEGMENTS = 64;
			const unsigned int Y_SEGMENTS = 64;
			const float PI = 3.14159265359;
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
			indexCount = indices.size();

			std::vector<float> data;
			for (int i = 0; i < positions.size(); ++i)
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
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
			float stride = (3 + 2 + 3) * sizeof(float);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
		}

		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
	}
}

#endif
