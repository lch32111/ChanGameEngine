#include "RayTracerDemo.h"

/****************************************************************************************/
/* ### Collision Demo ### */
void CGProj::RayTracerDemo::OnInitialize()
{
	glfwSwapInterval(0);
	glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

}

void CGProj::RayTracerDemo::OnFinalize()
{
}

void CGProj::RayTracerDemo::Update(float deltaTime, float lastFrame)
{
}

void CGProj::RayTracerDemo::Display()
{
}

void CGProj::RayTracerDemo::MouseMoveCallback(double xpos, double ypos)
{
}

void CGProj::RayTracerDemo::MouseButtonCallback(int button, int action, int mods)
{
}

void CGProj::RayTracerDemo::ScrollCallback(double yoffset)
{
}

void CGProj::RayTracerDemo::ResizeWindowCallback(int width, int height)
{
	Application::m_width = width;
	Application::m_height = height;
	glViewport(0, 0, width, height);
}

/* ### Collision Demo ### */
/****************************************************************************************/