#ifndef __TERRAIN_DEMO_H__
#define __TERRAIN_DEMO_H__

#include <GLFW/glfw3.h>

#include <Graphics/chanQuatCamera.h>
#include <Graphics/CGAssetManager.h>

#include <Terrain/CGTerrain.h>


namespace CGProj
{
	class SimpleTerrainDemo
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

		CGAssetManager assetManager;

		CGTerrain myTerrain;
	};
}
#endif