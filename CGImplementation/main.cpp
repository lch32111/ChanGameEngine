#include "CG_Application.h"
#include <Imgui/imgui_impl_glfw.h>

extern CGProj::Application* getApplication();
CGProj::Application* app = getApplication();

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	app->resize(width, height);
}

void cursorPos_callback(GLFWwindow* window, double xpos, double ypos)
{
	app->mouse(xpos, ypos);
}

void mouseButton_callback(GLFWwindow* window, int button, int action, int mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

	// you can use the mods value.
	// it indicates whether the shift/control/alt/super(windows key) is pressed 
	app->mouseButton(button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

	app->scroll(yoffset);
}


int main()
{
	app->initGraphics();
	glfwSetFramebufferSizeCallback(app->app_window, framebuffer_size_callback);
	glfwSetCursorPosCallback(app->app_window, cursorPos_callback);
	glfwSetMouseButtonCallback(app->app_window, mouseButton_callback);
	glfwSetScrollCallback(app->app_window, scroll_callback);
	
	app->initImgui();
	
	app->execute();
	
	app->deinit();

	delete app;

	return 0;
}