#pragma once
#ifndef __DEFERRED_RENDERER_H__
#define __DEFERRED_RENDERER_H__

#include <GLFW/glfw3.h>
#include <Graphics/Shader.h>
#include <Graphics/chanQuatCamera.h>

#include <vector>

namespace CGProj
{
	class DeferredRenderer
	{
	public:
		void initGraphics(int width, int height);
		void initImgui();
		void deinit();

		void updateImgui();
		void updateSimulation(float deltaTime, float lastFrame);
		void display(int width, int height);

		void key(GLFWwindow* app_window, float deltaTime);
		void mouse(double xpos, double ypos);
		void scroll(double yoffset);
		void resize(int width, int height);
	private:
		chanQuatCamera camera;
		float lastX = 400, lastY = 300;
		bool firstMouse = true;
		bool tabKey = false;

		bool UIControl = false;
		bool GameControl = true;

		Shader Deferred_First_Shader;
		Shader Deferred_Second_Shader;
		Shader Deferred_Post_Shader;

		unsigned int gFBO;
		unsigned int gPosition, gNormal, gAlbedoSpec, gEmissive, gBool;
		unsigned gRBO;

		std::vector<glm::vec3> objectPositions;
		unsigned int boxTexture, boxSpecular;
		unsigned int woodTexture;
		unsigned int emissiveTexture;

		unsigned int NR_LIGHTS = 200;
		float constant = 1.0;
		float linear = 0.7;
		float quadratic = 1.8;
		std::vector<glm::vec3> lightPositions;
		std::vector<glm::vec3> lightColors;
		std::vector<float> lightRadius;

		unsigned int cubeVAO = 0;
		unsigned int cubeVBO = 0;
		unsigned int quadVAO = 0;
		unsigned int quadVBO;

		void renderCube();
		void renderQuad();
	};
}


#endif
