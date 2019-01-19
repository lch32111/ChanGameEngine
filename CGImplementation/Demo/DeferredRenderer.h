#pragma once
#ifndef __DEFERRED_RENDERER_H__
#define __DEFERRED_RENDERER_H__

#include <GLFW/glfw3.h>

#include <Graphics/Shader.h>
#include <Graphics/chanQuatCamera.h>
#include <Graphics/CGRenderLine.h>
#include <Graphics/CGEditObject.h>
#include <Graphics/CGGizmo.h>
#include <Graphics/CGAssetManager.h>
#include <Graphics/CGEditProxyObject.h>
#include <Graphics/CGEditLightObject.h>
#include <Graphics/CGBloomEffect.h>
#include <Graphics/CGSSAOEffect.h>
#include <Graphics/CGStd140Module.h>

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
		float GamelastX = 400, GamelastY = 300;
		float UILastX = 400, UILastY = 300;
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

		CGStd140Module Deferred_STD140;
		Shader* Deferred_First_Shader;
		Shader* Deferred_Second_Shader;
		Shader* Deferred_Post_Shader;
		Shader* wireShader;

		unsigned int gFBO;
		unsigned int gPosition, gNormal, gAlbedoSpec, gEmissive, gBool;
		unsigned gRBO;

		unsigned int dSecondFBO;
		unsigned int dSecondHDRColorBuffer;
		unsigned int dSecondHDRBloomBuffer;

		float pGamma;
		float pExposure;
		bool isBloom;
		CGBloomEffect myBloom;

		bool isSSAO;
		bool isSSAODebug;
		CGSSAOEffect mySSAO;
		Shader* SSAODebugShader;

		// Broad Phase
		CGBroadPhase dBroadPhase;
		BroadRendererWrapper bRender;
		
		struct BroadClosestRayCast : BroadRayCast
		{
			bool RayCastCallback(const GPED::c3RayInput& input, int nodeId)
			{
				void* data = broadPhase->GetUserData(nodeId);
				CGEditObject* box = (CGEditObject*)data;
				
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
		CGRenderLine orinentLineRen;
		// Broad Phase

		std::vector<CGEditProxyObject> editProxies;
		std::vector<CGEditLightObject> editLights;

		unsigned num_dir_light = 0;
		unsigned num_dir_shadow = 0;
		
		unsigned num_point_light = 0;
		unsigned num_point_shadow = 0;

		unsigned num_spot_light = 0;
		unsigned num_spot_shadow = 0;

		CGEditObject* pickedEditBox = nullptr;
		CGGizmo gizmoTest;

		CGAssetManager assetManager;
	};

}


#endif
