#include <CGPrecompiled.h>
#include "RayTracerDemo.h"

using namespace CGProj::Math;

/****************************************************************************************/
/* ### RayTracerDemo Demo ### */
void CGProj::RayTracerDemo::OnInitialize()
{
	glfwSwapInterval(0);
	glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_image_width = 256;
	m_image_height = 256;
	m_image_buffer = new CGVector3<float>[m_image_width * m_image_height];

	m_camera.m_near = 0.01f;
	m_camera.m_far = 500.f;
	m_camera.m_fov_in_radian = ScalarOp<float>::Radian(45.f);
	
	RayTrace();

	glGenTextures(1, &m_gl_image_tex);
	glBindTexture(GL_TEXTURE_2D, m_gl_image_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_image_width, m_image_height, 0, GL_RGB, GL_FLOAT, m_image_buffer);
}

void CGProj::RayTracerDemo::OnFinalize()
{
	delete[] m_image_buffer;
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

void CGProj::RayTracerDemo::RayTrace()
{
	for (u32 y = 0; y < m_image_height; ++y)
	{
		for (u32 x = 0; x < m_image_width; ++x)
		{
			CGVector3<float> p;
			CGVector3<float> w;


		}
	}
}

/* ### RayTracerDemo Demo ### */
/****************************************************************************************/


void CGProj::RayTracerCamera::GetPrimaryRay(float x, float y, int width, int height, CGProj::Math::CGVector3<float>& position, CGProj::Math::CGVector3<float>& w) const
{
	const float side = -2.f * tan(m_fov_in_radian / 2.f);

	position.m_value[0] = m_near * (x / width - 0.5f) * side * width / height;
	position.m_value[1] = m_near * -(y / height - 0.5f) * side;
	position.m_value[2] = m_near;

	w = Normalize(position);
}