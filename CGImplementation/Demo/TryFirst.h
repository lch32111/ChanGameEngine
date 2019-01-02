#pragma once
#ifndef __TRY_FIRST_H__
#define __TRY_FIRST_H__

#include <GLFW/glfw3.h>

#include <Graphics/Shader.h>
#include <Graphics/chanQuatCamera.h>
#include <Graphics/CGRenderLine.h>

#include <GPED/GPED_contacts.h>
#include <GPED/CGContactManager.h>
#include <GPED/GPED_ContactResolver.h>
#include <GPED/CGCollisionPrimitive.h>
#include <GPED/CGCollisionAlgorithm.h>
#include <GPED/PS_AmmoRound.h>
#include <GPED/PS_Box.h>
#include <GPED/CGBroadPhase.h>


#include <string>
#include <vector>


namespace CGProj
{
	class TryFirst
	{
	public:
		void initGraphics();
		void initImgui();
		void deinit();

		void updateImgui();
		void updateSimulation(float deltaTime, float lastFrame);
		void display(int width, int height);

		void key(GLFWwindow* app_window, float deltaTime);
		void mouse(double xpos, double ypos);
		void mouseButton(GLFWwindow* app_window, int button, int action, int mods
		, int screen_width, int screen_height);
		void scroll(double yoffset);
	private:
		// User Input Interaction
		ShotType currentShotType;
		GPED::real contactFriction = 0.9;
		GPED::real contactRestitution = 0.1;
		GPED::real contactTolerance = 0.1;

		float lastX = 400, lastY = 300;
		bool firstMouse = true;
		bool start = false;
		bool Fkey = false;
		bool tabKey = false;
		bool mouseClick = false;

		bool UIControl = false;
		bool GameControl = true;
		bool BroadDebug = false;
		
		void fire();
		void totalFire();
		void reset();
		// User Input Interaction

		// Broad Phase
		CGBroadPhase FirstBroadPhase;
		BroadResultWrapper<CGCollisionPrimitive> firstResult;
		BroadRendererWrapper bRender;
		BroadRayCastWrapper bRayWrapper;
		Shader lineShader;
		CGRenderLine lineRen;
		std::vector<std::pair<glm::vec3, glm::vec3>> rayCollector;
		Shader wireShader;
		// Broad Phase
		
		// Narrow Phase
		CGCollisionAlgorithm cAlgo;
		GPED::ContactResolver resolver;
		CGContactManager cManager;
		// Narrow Phase

		// Simulation Object
		const static unsigned ammoRounds = 100;
		AmmoRound ammo[ammoRounds];
		const static unsigned boxes = 64;
		Box boxData[boxes];
		// Simulation Object

		// Simulation Logic
		void updateObjects(float duration, float lastFrame);
		void SyncAndUpdate();
		void broadPhase();
		void generateContacts(CGContactManager& cData);
		// Simulation Logic

		// Miscellaneous
		unsigned int woodTexture;
		unsigned int containerTexture;
		Shader simpleShader;
		chanQuatCamera camera;
		// Miscellaneous
	};
}

#endif