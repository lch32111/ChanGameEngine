#pragma once
#ifndef __CG_APPLICATION_H__
#define __CG_APPLICATION_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Demo/TryFirst.h>
#include <Demo/DeferredRenderer.h>
#include <Demo/SimpleTerrainDemo.h>

namespace CGProj
{
	
	class Application
	{
	public:
		virtual const char* getTitle();
		virtual void initGraphics();
		virtual void initImgui();
		virtual void setView();
		virtual void deinit();
		
		virtual void execute();
		virtual void update(float deltaTime, float lastFrame);
		virtual void display();
		virtual void key();
		virtual void mouse(double xpos, double ypos);
		virtual void mouseButton(int button, int action, int mods);
		virtual void mousdDrag(double xpos, double ypos);
		virtual void resize(int width, int height);
		virtual void scroll(double yoffset);

		void renderText(int xpos, int ypos, const char* text); // will be implemented later
		GLFWwindow* app_window;
	protected:
		int width = 800;
		int height = 600;
		bool canDisplay = true;

		float deltaTime = 0;
		float lastFrame = 0;
	};

	class PhysicsDemo : public Application
	{
	public:
		virtual const char* getTitle();
		virtual void initGraphics();
		virtual void initImgui();
		virtual void setView();
		virtual void deinit();

		virtual void update(float deltaTime, float lastFrame);
		virtual void display();

		virtual void mouse(double xpos, double ypos);
		virtual void mouseButton(int button, int action, int mods);
		virtual void scroll(double yoffset);
	private:
		TryFirst test1;
	};

	class GraphicsDemo : public Application
	{
	public:
		virtual const char* getTitle();
		virtual void initGraphics();
		virtual void initImgui();
		virtual void setView();
		virtual void deinit();

		virtual void update(float deltaTime, float lastFrame);
		virtual void display();

		virtual void mouse(double xpos, double ypos);
		virtual void mouseButton(int button, int action, int mods);

		virtual void scroll(double yoffset);
		virtual void resize(int width, int height);
	private:
		DeferredRenderer test2;
	};

	class TerrainDemo : public Application
	{
	public:
		virtual const char* getTitle();
		virtual void initGraphics();
		virtual void initImgui();
		virtual void setView();
		virtual void deinit();

		virtual void update(float deltaTime, float lastFrame);
		virtual void display();

		virtual void mouse(double xpos, double ypos);
		virtual void mouseButton(int button, int action, int mods);

		virtual void scroll(double yoffset);
		virtual void resize(int width, int height);
	private:
		SimpleTerrainDemo test3;
	};
}

#endif 