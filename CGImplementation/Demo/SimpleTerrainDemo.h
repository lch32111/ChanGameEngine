#ifndef __TERRAIN_DEMO_H__
#define __TERRAIN_DEMO_H__

#include <GLFW/glfw3.h>

#include <CG_Application.h>

#include <Graphics/chanQuatCamera.h>
#include <Graphics/CGAssetManager.h>

#include <Terrain/CGTerrain.h>
#include <Terrain/CGValueNoise1D.h>

#include <GPED/CGBroadPhase.h>
#include <GPED/CGCollisionPrimitive.h>
#include <GPED/CGCollisionAlgorithm.h>
#include <GPED/GPED_contacts.h>
#include <GPED/CGContactManager.h>
#include <GPED/GPED_ContactResolver.h>
#include <GPED/PS_AmmoRound.h>
#include <GPED/PS_Box.h>

namespace CG
{
	// Forward Decalration
	class SimpleTerrainDemo;
	class TerrainDemo : public Application
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
		SimpleTerrainDemo* test3;
	};

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
		// Program Common Factors
		chanQuatCamera camera;
		float GamelastX = 400, GamelastY = 300;
		float UILastX = 400, UILastY = 300;
		bool firstMouse = true;
		bool start = false;
		bool Fkey = false;
		bool tabKey = false;
		bool mouseClick = false;

		bool UIControl = false;
		bool GameControl = true;
		bool BroadDebug = false;
		

		bool wireDraw = false;

		CGAssetManager assetManager;
		
	private:
		// Terrain
		CGTerrain myTerrain;

	private:
		// Physics
		ShotType currentShotType;

		// Broad Phase
		CGBroadPhase SecondBroadPhase;
		BroadResultWrapper<CGCollisionPrimitive> SecondResult;
		BroadRendererWrapper bRender;
		Shader* wireShader;
		// Broad Phase

		// Narrow Phase
		CGCollisionAlgorithm cAlgo;
		GPED::ContactResolver resolver;
		CGContactManager cManager;
		// Narrow Phase

		// Simulation Object;
		const static unsigned ammoRounds = 100;
		AmmoRound ammo[ammoRounds];
		const static unsigned boxes = 64;
		Box boxData[boxes];
		Shader* simpleShader;
		// Simulation Object

		// Simulation Logic
		void updateObjects(float duration, float lastFrame);
		void SyncAndUpdate(float duration);
		void broadPhase();
		void generateContacts(CGContactManager& cData);
		// Simulation Logic

		// Simulation Action
		void fire();
		// Simulation Action
	private:
		// Graphics
		unsigned int woodTexture;
		unsigned int containerTexture;
	};
}
#endif