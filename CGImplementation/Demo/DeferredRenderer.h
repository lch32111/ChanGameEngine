#pragma once
#ifndef __DEFERRED_RENDERER_H__
#define __DEFERRED_RENDERER_H__

#include <GLFW/glfw3.h>

#include <Graphics/Shader.h>
#include <Graphics/chanQuatCamera.h>
#include <Graphics/CGRenderLine.h>
#include <Graphics/CGEditObject.h>

#include <GPED/CGBroadPhase.h>

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
		void mouseButton(GLFWwindow* app_window,
			int button, int action, int mods,
			int screen_width, int screen_height);
		void scroll(double yoffset);
		void resize(int width, int height);
	private:
		chanQuatCamera camera;
		float lastX = 400, lastY = 300;
		bool firstMouse = true;
		bool tabKey = false;

		bool UIControl = false;
		bool GameControl = true;
		bool mouseClick = false;

		bool lightDraw = false;
		bool BroadDebug = false;
		bool wireDraw = false;
		bool clickDraw = false;
		bool rayHitDraw = false;

		Shader Deferred_First_Shader;
		Shader Deferred_Second_Shader;
		Shader Deferred_Post_Shader;
		Shader Simple_Shader;
		Shader wireShader;

		unsigned int gFBO;
		unsigned int gPosition, gNormal, gAlbedoSpec, gEmissive, gBool;
		unsigned gRBO;

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

		// Broad Phase
		CGBroadPhase dBroadPhase;
		BroadRendererWrapper bRender;
		
		struct BroadClosestRayCast : BroadRayCast
		{
			bool RayCastCallback(const GPED::c3RayInput& input, int nodeId)
			{
				void* data = broadPhase->GetUserData(nodeId);
				CGEditBox* box = (CGEditBox*)data;
				
				GPED::c3RayOutput output;
				bool hit = GPED::rayaabbIntersection(output, input, box->getFitAABB());

				if (hit)
				{
					if (output.t < maxFraction)
					{
						maxFraction = output.t;
						rayOutput = output;
						userData = data;
					}
				}

				return true;
			}

			GPED::real maxFraction = REAL_MAX;
			GPED::c3RayOutput rayOutput;
			void* userData = nullptr;
		};
		CGRenderLine lineRen;
		std::vector<std::pair<glm::vec3, glm::vec3>> rayCollector;
		CGRenderLine rayRen;
		std::vector<std::pair<glm::vec3, glm::vec3>> hitCollector;
		// Broad Phase

		const static int editBoxNumb = 9;
		CGEditBox editBoxes[editBoxNumb];
	};
}


#endif
