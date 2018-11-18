#pragma once
#ifndef __TRY_FIRST_H__
#define __TRY_FIRST_H__

#include <GLFW/glfw3.h>

#include <Graphics/Shader.h>
#include <Graphics/chanQuatCamera.h>

#include <GPED/GPED_contacts.h>
#include <GPED/GPED_contacts.h>
#include <GPED/GPED_collide_fine.h>
#include <GPED/PS_AmmoRound.h>
#include <GPED/PS_Box.h>
#include <GPED/CGBroadPhase.h>

#include <string>
#include <vector>
#include <Graphics/GLPrimitiveUtil.h>

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

		bool UIControl = false;
		bool GameControl = true;
		bool BroadDebug = false;
		
		void fire();
		void totalFire();
		void reset();
		// User Input Interaction

		// Broad Phase
		CGBroadPhase FirstBroadPhase;
		BroadResultWrapper<GPED::CollisionPrimitive> firstResult;
		BroadRendererWrapper bRender;
		Shader wireShader;
		// Broad Phase
		
		// Narrow Phase
		static const int maxContacts = 1000;
		GPED::Contact contacts[maxContacts];
		GPED::CollisionData cData;
		GPED::ContactResolver resolver;
		// Narrow Phase

		// Simulation Object
		const static unsigned ammoRounds = 100;
		AmmoRound ammo[ammoRounds];
		const static unsigned boxes = 100;
		Box boxData[boxes];
		// Simulation Object

		// Simulation Logic
		void updateObjects(float duration, float lastFrame);
		void SyncAndUpdate();
		void broadPhase();
		void generateContacts(GPED::CollisionData& cData);
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