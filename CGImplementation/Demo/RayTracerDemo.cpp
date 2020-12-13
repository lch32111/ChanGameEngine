#include <CGPrecompiled.h>
#include "RayTracerDemo.h"

#include <Graphics/GLPrimitiveUtil.h>
#include <Math/CGMat4.h>


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

	m_asset_manager.assetInit();
	m_simple_shader = m_asset_manager.getShader(SHADER_SIMPLE_RENDER);
	m_simple_shader->use();
	m_simple_shader->setInt("texture1", 0);

	glGenTextures(1, &m_gl_image_tex);
	glBindTexture(GL_TEXTURE_2D, m_gl_image_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_image_width, m_image_height, 0, GL_RGB, GL_FLOAT, m_image_buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void CGProj::RayTracerDemo::OnFinalize()
{
	glDeleteTextures(1, &m_gl_image_tex);
	m_asset_manager.destroy();
	delete[] m_image_buffer;
}

void CGProj::RayTracerDemo::Update(float deltaTime, float lastFrame)
{
}

void CGProj::RayTracerDemo::Display()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glViewport(0, 0, m_width, m_height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_gl_image_tex);

	CGMat4<float> identity(1.f);
	m_simple_shader->SetMat4("model", identity);
	m_simple_shader->SetMat4("view", identity);
	m_simple_shader->SetMat4("projection", identity);
	renderScreenQuad();
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
			/*
			CGVector3<float> p;
			CGVector3<float> w;
			*/


			CGVector3<float> color;
			color.m_value[0] = 1.f;
			color.m_value[1] = 1.f;
			color.m_value[2] = 1.f;

			m_image_buffer[y * m_image_width + x] = color;
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