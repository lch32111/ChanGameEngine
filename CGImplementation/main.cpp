#include <CGPrecompiled.h>
#include "CG_Application.h"
#include <Imgui/imgui_impl_glfw.h>

extern CG::Application* getApplication();
CG::Application* app = getApplication();

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	app->ResizeWindowCallback(width, height);
}

void cursorPos_callback(GLFWwindow* window, double xpos, double ypos)
{
	app->MouseMoveCallback(xpos, ypos);
}

void mouseButton_callback(GLFWwindow* window, int button, int action, int mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

	// you can use the mods value.
	// it indicates whether the shift/control/alt/super(windows key) is pressed 
	app->MouseButtonCallback(button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

	app->ScrollCallback(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	app->KeyCallback(key, scancode, action, mods);
}


int main()
{
	app->Initialize();
	{
		glfwSetFramebufferSizeCallback(app->app_window, framebuffer_size_callback);
		glfwSetCursorPosCallback(app->app_window, cursorPos_callback);
		glfwSetMouseButtonCallback(app->app_window, mouseButton_callback);
		glfwSetScrollCallback(app->app_window, scroll_callback);
		glfwSetKeyCallback(app->app_window, key_callback);
	}
	
	app->Execute();
	
	app->Finalize();	

	delete app;

	return 0;
}