#include <CGPrecompiled.h>
#include "RayTracerDemo.h"

#include <Imgui/imgui.h>

#include <Math/CGMat4.h>
#include <Geometry/CGCollisionFunction.h>
#include <Geometry/CGRay.h>

#include <Graphics/GLPrimitiveUtil.h>
#include <Graphics/CGModel.h>



/****************************************************************************************/
/* ### RayTracerDemo Demo ### */
void CG::RayTracerDemo::OnInitialize()
{
	glfwSwapInterval(0);
	// glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_asset_manager.assetInit();

	m_image_width = 32;
	m_image_height = 32;
	m_image_buffer = new CGVector3<float>[m_image_width * m_image_height];

	InitializeScene();
	RayTrace();
	
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

	FinalizeScene();

	delete[] m_image_buffer;
}

void CG::RayTracerDemo::Update(float deltaTime, float lastFrame)
{
	bool change = false;
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


void CG::RayTracerDemo::InitializeScene()
{
	double startTime = glfwGetTime();

	m_camera.m_near = 0.01f;
	m_camera.m_far = 500.f;
	m_camera.m_fov_in_radian = CGScalarOp<float>::Radian(45.f);

	float scale = 0.001f;

	CGMat4<float> transform(1.f);
	transform[0][0] = scale;
	transform[1][1] = scale;
	transform[2][2] = scale;
	transform[3][0] = 0.f;
	transform[3][1] = -0.05f;
	transform[3][2] = -0.2f;

	CGModel* teapot_model = m_asset_manager.getModelData(MODEL_TEAPOT, 1);

	const std::vector<CGModelMesh>& teapot_meshes = teapot_model->GetMeshes();

	for (size_t i = 0; i < teapot_meshes.size(); ++i)
	{
		const CGModelMesh& mesh = teapot_meshes[i];

		CG_DEBUG_ASSERT((mesh.m_indices.size() % 3) == 0);

		m_primitives.reserve(m_primitives.size() + mesh.m_indices.size() / 3);

		for (size_t j = 0; j < mesh.m_indices.size(); j += 3)
		{
			unsigned i0 = mesh.m_indices[j];
			unsigned i1 = mesh.m_indices[j + 1];
			unsigned i2 = mesh.m_indices[j + 2];

			const Vertex& v0 = mesh.m_vertices[i0];
			const Vertex& v1 = mesh.m_vertices[i1];
			const Vertex& v2 = mesh.m_vertices[i2];

			m_primitives.push_back(Primitive());
			Primitive& p = m_primitives.back();
			p.Initialize(Primitive::TRIANGLE);

			CGTriangle& t = p.GetConvex<CGTriangle>();

			CGVector4<float> tr = transform * CGVec4(v0.Position[0], v0.Position[1], v0.Position[2], 1.f);
			t.m_vertices[0] = CGVec3{ tr[0], tr[1], tr[2] };

			tr = transform * CGVec4(v1.Position[0], v1.Position[1], v1.Position[2], 1.f);
			t.m_vertices[1] = CGVec3{ tr[0], tr[1], tr[2] };

			tr = transform * CGVec4(v2.Position[0], v2.Position[1], v2.Position[2], 1.f);
			t.m_vertices[2] = CGVec3{ tr[0], tr[1], tr[2] };
		}
	}

	double endTime = glfwGetTime();

	double duration = endTime - startTime;
	printf("Initliaze Scene ms %lf\n", (duration / 1000.0));
}

void CG::RayTracerDemo::FinalizeScene()
{
	for (Primitive& p : m_primitives)
	{
		p.Finalize();
	}
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
	CGRay ray(pos, pos + normalizedRay, m_camera.m_far * 2.f);

	bool find_surfel = false;
	float min_t = FLT_MAX;
	for (size_t i = 0; i < m_primitives.size(); ++i)
	{
		switch (m_primitives[i].m_shape_type)
		{
		case Primitive::TRIANGLE:
		{
			CGScalar t;

			if (IntersectTruePlane(m_primitives[i].GetConvex<CGTriangle>(), ray, t))
			{
				if (t < min_t)
				{
					min_t = t;

					if (find_surfel == false)
					{
						Surfel sf;
						m_surfels.push_back(sf);
						find_surfel = true;
					}
				}
			}
			break;
		}
		default:
		{
			CG_DEBUG_ASSERT(false);
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

