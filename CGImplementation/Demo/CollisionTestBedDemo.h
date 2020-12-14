#pragma once
#ifndef __COLLISION_TEST_BED_DEMO_H__
#define __COLLISION_TEST_BED_DEMO_H__

#include <CG_Application.h>

#include <Graphics/chanQuatCamera.h>

#include <Geometry/CGSphere.h>

namespace CG
{
	class CollisionTestBed;
	class CollisionDemo : public Application
	{
	public:
		virtual void Update(float deltaTime, float lastFrame);
		virtual void Display();
		
		virtual void KeyCallback(int key, int scancode, int action, int mods) {}
		virtual void MouseMoveCallback(double xpos, double ypos);
		virtual void MouseButtonCallback(int button, int action, int mods);
		virtual void MouseDragCallback(double xpos, double ypos) {}
		virtual void ScrollCallback(double yoffset);
		virtual void ResizeWindowCallback(int width, int height);
	protected:
		virtual void OnInitialize();
		virtual void OnFinalize();
	private:
		CollisionTestBed* demo;
	};

	class CollisionTestBed
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

		bool isSphereCollided = false;
		bool isRaySphereCollided = false;
		CGContact sphereContact;
		CGSphere aSphere;
		CGSphere bSphere;
	};
}

#endif
