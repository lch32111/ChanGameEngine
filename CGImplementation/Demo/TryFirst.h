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
		/* Imgui Control Parameters*/
		ShotType currentShotType;
		GPED::real contactFriction = 0.9;
		GPED::real contactRestitution = 0.1;
		GPED::real contactTolerance = 0.1;

		Shader simpleShader;
		chanQuatCamera camera;
		float lastX = 400, lastY = 300;
		bool firstMouse = true;
		bool start = false;
		bool Fkey = false;
		bool tabKey = false;

		bool UIControl = false;
		bool GameControl = true;

		CGBroadPhase FirstBroadPhase;
		BroadResultWrapper<GPED::CollisionPrimitive> firstResult;
		BroadRendererWrapper bRender;
		Shader wireShader;
		
		
		static const int maxContacts = 1000;
		GPED::Contact contacts[maxContacts];
		GPED::CollisionData cData;
		GPED::ContactResolver resolver;

		const static unsigned ammoRounds = 100;
		AmmoRound ammo[ammoRounds];

		const static unsigned boxes = 70;
		Box boxData[boxes];

		void updateObjects(float duration, float lastFrame);
		void SyncAndUpdate();
		void broadPhase();
		void generateContacts(GPED::CollisionData& cData);

		void fire();
		void totalFire();
		void reset();

		unsigned int woodTexture;
		unsigned int containerTexture;
	};
}

#endif