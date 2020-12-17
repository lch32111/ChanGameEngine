#include <CGPrecompiled.h>
#include "RayTracerDemo.h"

#include <Imgui/imgui.h>

#include <Math/CGMat4.h>
#include <Geometry/CGCollisionFunction.h>
#include <Geometry/CGRay.h>
#include <Graphics/GLPrimitiveUtil.h>



/****************************************************************************************/
/* ### RayTracerDemo Demo ### */
void CG::RayTracerDemo::OnInitialize()
{
	glfwSwapInterval(0);
	// glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_image_width = 256;
	m_image_height = 256;
	m_image_buffer = new CGVector3<float>[m_image_width * m_image_height];

	PrepareScene();
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

void CG::RayTracerDemo::OnFinalize()
{
	glDeleteTextures(1, &m_gl_image_tex);
	m_asset_manager.destroy();
	delete[] m_image_buffer;
}

void CG::RayTracerDemo::Update(float deltaTime, float lastFrame)
{
	bool change = false;

	if (ImGui::DragFloat("Sphere Radius", &(m_primitives[0].m_sphere.m_radius), 0.001f))
	{
		change = true;
	}

	if (ImGui::DragFloat("Sphere Z", &(m_primitives[0].m_sphere.m_pos[2]), 0.001f))
	{
		change = true;
	}

	if (ImGui::DragFloat("Plane Distance", &(m_primitives[1].m_plane.m_distance), 0.001f))
	{
		change = true;
	}

	if (ImGui::InputFloat("Plane X", &(m_primitives[1].m_plane.m_normal[0]), 0.001f))
	{
		m_primitives[1].m_plane.m_normal = SafeNormalize(m_primitives[1].m_plane.m_normal);
		change = true;
	}

	if (ImGui::InputFloat("Plane Y", &(m_primitives[1].m_plane.m_normal[1]), 0.001f))
	{
		m_primitives[1].m_plane.m_normal = SafeNormalize(m_primitives[1].m_plane.m_normal);
		change = true;
	}

	if (ImGui::InputFloat("Plane Z", &(m_primitives[1].m_plane.m_normal[2]), 0.001f))
	{
		m_primitives[1].m_plane.m_normal = SafeNormalize(m_primitives[1].m_plane.m_normal);
		change = true;
	}

	if (change == true)
	{
		RayTrace();
		glBindTexture(GL_TEXTURE_2D, m_gl_image_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_image_width, m_image_height, 0, GL_RGB, GL_FLOAT, m_image_buffer);
	}
}

void CG::RayTracerDemo::Display()
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

void CG::RayTracerDemo::MouseMoveCallback(double xpos, double ypos)
{
}

void CG::RayTracerDemo::MouseButtonCallback(int button, int action, int mods)
{
}

void CG::RayTracerDemo::ScrollCallback(double yoffset)
{
}

void CG::RayTracerDemo::ResizeWindowCallback(int width, int height)
{
	Application::m_width = width;
	Application::m_height = height;
	glViewport(0, 0, width, height);
}


void CG::RayTracerDemo::PrepareScene()
{
	m_camera.m_near = 0.01f;
	m_camera.m_far = 500.f;
	m_camera.m_fov_in_radian = CGScalarOp<float>::Radian(45.f);

	m_primitives.resize(2);
	m_primitives[0].m_sphere.m_pos = CGVec3(0.f, 0.f, -1.f);
	m_primitives[0].m_sphere.m_radius = 0.1f;
	m_primitives[0].m_shape_type = Primitive::SPHERE;

	m_primitives[1].m_plane.m_normal = CGVec3(0.f, 1.f, 0.f);
	m_primitives[1].m_plane.m_distance = -10.f;
	m_primitives[1].m_shape_type = Primitive::PLANE;
}

void CG::RayTracerDemo::RayTrace()
{
	for (u32 y = 0; y < m_image_height; ++y)
	{
		for (u32 x = 0; x < m_image_width; ++x)
		{
			CGVector3<float> p;
			CGVector3<float> w;
			m_camera.GetPrimaryRay((float)x, (float)y, (s32)m_image_width, (s32)m_image_height, p, w);
			m_image_buffer[y * m_image_width + x] = ComputeLight(p, w);
		}
	}
}

CG::CGVector3<float> CG::RayTracerDemo::ComputeLight(CGVector3<float> pos, CGVector3<float> normalizedRay)
{
	const CG::Surfel* surfel = FindIntersection(pos, normalizedRay);

	CGVector3<float> color;
	if (surfel != nullptr)
	{
		color[0] = 1.f;
		color[1] = 1.f;
		color[2] = 1.f;
	}
	else
	{
		color[0] = 0.f;
		color[1] = 0.f;
		color[2] = 0.f;
	}
	
	return color;
}

const CG::Surfel* CG::RayTracerDemo::FindIntersection(CGVector3<float> pos, CGVector3<float> normalizedRay)
{
	CGRay ray;
	ray.m_source = pos;
	ray.m_target = pos + normalizedRay;
	ray.m_maxFraction = m_camera.m_far * 2.f;

	bool find_surfel = false;
	for (size_t i = 0; i < m_primitives.size(); ++i)
	{
		switch (m_primitives[i].m_shape_type)
		{
		case Primitive::SPHERE:
		{
			if (Intersect(m_primitives[i].m_sphere, ray))
			{
				find_surfel = true;
				Surfel sf;
				m_surfels.push_back(sf);
			}
			break;
		}
		case Primitive::PLANE:
		{
			if (IntersectTruePlane(m_primitives[i].m_plane, ray))
			{
				find_surfel = true;
				Surfel sf;
				m_surfels.push_back(sf);
			}
		}
		}
	}

	if (find_surfel)
	{
		return &(m_surfels[0]);
	}
	else
	{
		return nullptr;
	}
}

/* ### RayTracerDemo Demo ### */
/****************************************************************************************/


void CG::RayTracerCamera::GetPrimaryRay(float x, float y, int width, int height, CGVector3<float>& position, CGVector3<float>& w) const
{
	const float side = 2.f * tan(m_fov_in_radian / 2.f);

	position.m_value[0] = m_near * side * width / height * (x / width - 0.5f);
	position.m_value[1] = m_near * side * (y / height - 0.5f);
	position.m_value[2] = -m_near;

	w = Normalize(position);
}

