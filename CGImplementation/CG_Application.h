#pragma once
#ifndef __CG_APPLICATION_H__
#define __CG_APPLICATION_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace CGProj
{
	class Application
	{
	public:
		virtual ~Application() {}
		GLFWwindow* app_window;

		void Initialize(bool shouldBaseInit = true);
		void Finalize();
		virtual void Execute();

	public:
		virtual void Update(float deltaTime, float lastFrame) = 0;
		virtual void Display() = 0;

		virtual void KeyCallback(int key, int scancode, int action, int mods) = 0;
		virtual void MouseMoveCallback(double xpos, double ypos) = 0;
		virtual void MouseButtonCallback(int button, int action, int mods) = 0;
		virtual void MouseDragCallback(double xpos, double ypos) = 0;
		virtual void ScrollCallback(double yoffset) = 0;
		virtual void ResizeWindowCallback(int width, int height) = 0;

	protected:
		
		virtual void OnInitialize() = 0;
		virtual void OnFinalize() = 0;

		s32 m_width = 800;
		s32 m_height = 600;

		float m_deltaTime = 0.f;
		float m_lastFrame = 0.f;

		bool m_isBaseInit = false;
	};
}

#endif 