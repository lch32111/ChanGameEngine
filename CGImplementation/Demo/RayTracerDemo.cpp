#include <CGPrecompiled.h>
#include "RayTracerDemo.h"

#include <thread>
#ifdef __WIN32__
#include <Windows.h>
#endif


#include <Imgui/imgui.h>

#include <Math/CGMat4.h>
#include <Geometry/CGCollisionFunction.h>
#include <Geometry/CGRay.h>

#include <Graphics/GLPrimitiveUtil.h>
#include <Graphics/CGModel.h>

#include <GPED/CGPhysicsUtil.h>



/****************************************************************************************/
/* ### RayTracerDemo Demo ### */
void CG::RayTracerDemo::OnInitialize()
{
	glfwSwapInterval(0);
	// glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_asset_manager.assetInit();

	m_image_width = 160;
	m_image_height = 100;
	m_image_buffer = new CGVector3<float>[m_image_width * m_image_height];

	m_duplicated_image_buffer = new CGVector3<float>[m_image_width * m_image_height];

	m_left_mouse_clicked = false;
	m_color_paint_mode = false;
	

	InitializeScene();
	RayTrace();

	memcpy(m_duplicated_image_buffer, m_image_buffer, sizeof(CGVector3<float>) * (m_image_width * m_image_height));

	
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

	delete[] m_duplicated_image_buffer;
	delete[] m_image_buffer;
}

void CG::RayTracerDemo::Update(float deltaTime, float lastFrame)
{
	if (ImGui::Checkbox("Paint Mode", &m_color_paint_mode))
	{
		// false -> true
		if (m_color_paint_mode == true)
		{
		}
		// true -> false
		else
		{
			glBindTexture(GL_TEXTURE_2D, m_gl_image_tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_image_width, m_image_height, 0, GL_RGB, GL_FLOAT, m_image_buffer);
			
			memcpy(m_duplicated_image_buffer, m_image_buffer, sizeof(CGVector3<float>) * m_image_width * m_image_height);
		}
	}
	
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

	static unsigned int screenQuadVAO = 0;
	if (screenQuadVAO == 0)
	{
		unsigned int screenQuadVBO;

		float quadYInvertVertices[] = {
			// positions        //Normal       // texture Coords
			-1.0f,  1.0f, 0.0f, 0.f, 0.f, 1.f, 0.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 0.f, 0.f, 1.f, 0.0f, 1.0f,
			 1.0f,  1.0f, 0.0f, 0.f, 0.f, 1.f, 1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 0.f, 0.f, 1.f, 1.0f, 1.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &screenQuadVAO);
		glGenBuffers(1, &screenQuadVBO);
		glBindVertexArray(screenQuadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadYInvertVertices), &quadYInvertVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	}
	glBindVertexArray(screenQuadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void CG::RayTracerDemo::MouseMoveCallback(double x, double y)
{
	if (m_left_mouse_clicked)
	{
		int xpos = (int)x;
		int ypos = (int)y;

		if (xpos >= 0 && xpos < m_width && ypos >= 0 && ypos < m_height)
		{
			xpos = (int)((float)xpos * m_image_width / m_width);
			ypos = (int)((float)ypos * m_image_height / m_height);

			if (m_color_paint_mode)
			{
				CGVector3<float> p, w;
				m_camera.GetPrimaryRay((float)xpos, (float)ypos, (s32)m_image_width, (s32)m_image_height, p, w);
				m_duplicated_image_buffer[ypos * m_image_width + xpos] = CGVector3<float>(1.f, 0.f, 0.f);

				glBindTexture(GL_TEXTURE_2D, m_gl_image_tex);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_image_width, m_image_height, 0, GL_RGB, GL_FLOAT, m_duplicated_image_buffer);
			}
			else
			{
				CGVector3<float> p, w;
				m_camera.GetPrimaryRay((float)xpos, (float)ypos, (s32)m_image_width, (s32)m_image_height, p, w);
				CGVector3<float> c = ComputeLightIn(p, w);
				printf("(%d %d) = %f %f %f\n", xpos, ypos, c[0], c[1], c[2]);
			}
		}
	}
}

void CG::RayTracerDemo::MouseButtonCallback(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		m_left_mouse_clicked = true;

		double x, y;
		glfwGetCursorPos(m_app_window, &x, &y);

		int xpos = (int)x;
		int ypos = (int)y;
		
		if (xpos >= 0 && xpos < m_width && ypos >= 0 && ypos < m_height)
		{
			xpos = (int)((float)xpos * m_image_width / m_width);
			ypos = (int)((float)ypos * m_image_height / m_height);

			if (m_color_paint_mode)
			{
				CGVector3<float> p, w;
				m_camera.GetPrimaryRay((float)xpos, (float)ypos, (s32)m_image_width, (s32)m_image_height, p, w);
				m_duplicated_image_buffer[ypos * m_image_width + xpos] = CGVector3<float>(1.f, 0.f, 0.f);

				glBindTexture(GL_TEXTURE_2D, m_gl_image_tex);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_image_width, m_image_height, 0, GL_RGB, GL_FLOAT, m_duplicated_image_buffer);
			}
			else
			{
				CGVector3<float> p, w;
				m_camera.GetPrimaryRay((float)xpos, (float)ypos, (s32)m_image_width, (s32)m_image_height, p, w);
				CGVector3<float> c = ComputeLightIn(p, w);
				printf("(%d %d) = %f %f %f\n", xpos, ypos, c[0], c[1], c[2]);
			}
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		m_left_mouse_clicked = false;
	}
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


template<typename T> T get_min(T a, T b) { return a < b ? a : b; }
template<typename T> T get_max(T a, T b) { return a < b ? b : a; }

void CG::RayTracerDemo::InitializeScene()
{
	double startTime = glfwGetTime();

	m_camera.m_near = 0.01f;
	m_camera.m_far = 500.f;
	m_camera.m_fov_in_radian = CGScalarOp<float>::Radian(45.f);

	float scale = 0.001f;

	CGVector3<float> teapot_pos(0.f, -0.05f, -0.2f);

	CGMat4<float> transform(1.f);
	transform[0][0] = scale;
	transform[1][1] = scale;
	transform[2][2] = scale;
	transform[3][0] = teapot_pos[0];
	transform[3][1] = teapot_pos[1];
	transform[3][2] = teapot_pos[2];

	CGModel* teapot_model = m_asset_manager.getModelData(MODEL_TEAPOT, 1);

	const std::vector<CGModelMesh>& teapot_meshes = teapot_model->GetMeshes();


	int primitive_size = 0;
	for (size_t i = 0; i < teapot_meshes.size(); ++i)
	{
		const CGModelMesh& mesh = teapot_meshes[i];

		CG_DEBUG_ASSERT((mesh.m_indices.size() % 3) == 0);

		primitive_size += mesh.m_indices.size() / 3;
	}

	m_primitives.resize(primitive_size);


	int primitive_index = 0;
	for (size_t i = 0; i < teapot_meshes.size(); ++i)
	{
		const CGModelMesh& mesh = teapot_meshes[i];

		CG_DEBUG_ASSERT((mesh.m_indices.size() % 3) == 0);

		for (size_t j = 0; j < mesh.m_indices.size(); j += 3)
		{
			unsigned i0 = mesh.m_indices[j];
			unsigned i1 = mesh.m_indices[j + 1];
			unsigned i2 = mesh.m_indices[j + 2];

			const Vertex& v0 = mesh.m_vertices[i0];
			const Vertex& v1 = mesh.m_vertices[i1];
			const Vertex& v2 = mesh.m_vertices[i2];

			Primitive& p = m_primitives[primitive_index];
			p.Initialize(Primitive::TRIANGLE);
			p.m_self_index = primitive_index;

			CGTriangle& t = p.GetConvex<CGTriangle>();

			CGVector4<float> tr = transform * CGVec4(v0.Position[0], v0.Position[1], v0.Position[2], 1.f);
			t.m_vertices[0] = CGVec3{ tr[0], tr[1], tr[2] };

			tr = transform * CGVec4(v1.Position[0], v1.Position[1], v1.Position[2], 1.f);
			t.m_vertices[1] = CGVec3{ tr[0], tr[1], tr[2] };

			tr = transform * CGVec4(v2.Position[0], v2.Position[1], v2.Position[2], 1.f);
			t.m_vertices[2] = CGVec3{ tr[0], tr[1], tr[2] };

			p.m_texcoord[0] = CGVector2<float>(v0.TexCoords.x, v0.TexCoords.y);
			p.m_texcoord[1] = CGVector2<float>(v1.TexCoords.x, v1.TexCoords.y);
			p.m_texcoord[2] = CGVector2<float>(v2.TexCoords.x, v2.TexCoords.y);


			GPED::c3AABB triangle_aabb;
			for (int i = 0; i < 3; ++i)
			{
				triangle_aabb.min[i] = get_min(t.m_vertices[0][i], get_min(t.m_vertices[1][i], t.m_vertices[2][i]));
				triangle_aabb.max[i] = get_max(t.m_vertices[0][i], get_max(t.m_vertices[1][i], t.m_vertices[2][i]));
			}

			m_broad_phase.CreateProxy(triangle_aabb, &p);

			++primitive_index;
		}
	}


	Light point_light0;
	point_light0.m_position = CGVector3<float>(0.0f, 0.1f, 0.f);
	point_light0.m_color = CGVector3<float>(8.f, 4.f, 4.f);
	// point_light0.m_attenuation = CGVector3<float>(1.f, 0.0014f, 0.000007f);
	m_lights.push_back(point_light0);


	double endTime = glfwGetTime();

	double duration = endTime - startTime;
	printf("Initliaze Scene %lf(ms)\n", (duration * 1000.0));
}

void CG::RayTracerDemo::FinalizeScene()
{
	for (Primitive& p : m_primitives)
	{
		p.Finalize();
	}
}

#ifdef __WIN32__
// https://docs.microsoft.com/ko-kr/previous-versions/visualstudio/visual-studio-2015/debugger/how-to-set-a-thread-name-in-native-code?view=vs-2015&redirectedfrom=MSDN
const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)  
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.  
	LPCSTR szName; // Pointer to name (in user addr space).  
	DWORD dwThreadID; // Thread ID (-1=caller thread).  
	DWORD dwFlags; // Reserved for future use, must be zero.  
} THREADNAME_INFO;
#pragma pack(pop)  
void SetThreadName(DWORD dwThreadID, const char* threadName) {
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;
#pragma warning(push)  
#pragma warning(disable: 6320 6322)  
	__try {
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}
#pragma warning(pop)  
}
#endif

void CG::RayTracerDemo::RayTrace()
{
	printf("buffer width height (%u, %u)\n", m_image_width, m_image_height);

#if 0
	printf("thread count : %u\n", 1);
	double startTime = glfwGetTime();
	for (u32 y = 0; y < m_image_height; ++y)
	{
		for (u32 x = 0; x < m_image_width; ++x)
		{
			CGVector3<float> p;
			CGVector3<float> w;
			m_camera.GetPrimaryRay((float)x, (float)y, (s32)m_image_width, (s32)m_image_height, p, w);
			m_image_buffer[y * m_image_width + x] = ComputeLightIn(p, w);
		}
	}
	double endTime = glfwGetTime();
	double duration = endTime - startTime;
	printf("RayTrace Scene %lf(s)\n", duration);
#else

	const unsigned int thread_count = std::thread::hardware_concurrency() * 2;
	printf("thread count : %u\n", thread_count);

	char name_buf[256];
	std::vector<std::string> thread_name(thread_count);

	for (unsigned i = 0; i < thread_count; ++i)
	{
		sprintf(name_buf, "RayTraceTask %u", i);
		thread_name[i] = name_buf;
	}

	std::vector<std::thread> thread_container(thread_count);

	u32 divider = m_image_height / thread_count;

	double startTime = glfwGetTime();

	for (unsigned i = 0; i < thread_count; ++i)
	{
		TracingTask task;
		task.m_start_height = i * divider;
		task.m_end_height = (i + 1) * divider;

		std::thread task_thread(&RayTracerDemo::RayTraceTask, this, task);

#ifdef __WIN32__
		SetThreadName(GetThreadId(static_cast<HANDLE>(task_thread.native_handle())), thread_name[i].c_str());
#endif

		thread_container[i] = std::move(task_thread);
	}

	for (unsigned i = 0; i < thread_count; ++i)
	{
		thread_container[i].join();
	}

	double endTime = glfwGetTime();
	double duration = endTime - startTime;
	printf("RayTrace Scene %lf(s)\n", duration);
#endif
}

void CG::RayTracerDemo::RayTraceTask(TracingTask task)
{
	for (u32 y = task.m_start_height; y < task.m_end_height; ++y)
	{
		for (u32 x = 0; x < m_image_width; ++x)
		{
			CG::CGVector3<float> p;
			CG::CGVector3<float> w;
			m_camera.GetPrimaryRay((float)x, (float)y, (s32)m_image_width, (s32)m_image_height, p, w);
			m_image_buffer[y * m_image_width + x] = ComputeLightIn(p, w);
		}
	}
}

const std::shared_ptr<CG::Surfel> CG::RayTracerDemo::FindIntersection(const CGVector3<float>& pos, const CGVector3<float>& normalizedRay)
{
#if 0
	CGRay ray(pos, pos + normalizedRay, m_camera.m_far * 2.f);

	int primitive_index = -1;
	CGScalar u, v, w;
	CGScalar min_t = FLT_MAX;
	for (size_t i = 0; i < m_primitives.size(); ++i)
	{
		switch (m_primitives[i].m_shape_type)
		{
		case Primitive::TRIANGLE:
		{
			CGScalar t_u, t_v, t_w, t_t;
			if (true == IntersectTruePlane(m_primitives[i].GetConvex<CGTriangle>(), ray, t_u, t_v, t_w, t_t))
			{
				if (t_t < min_t)
				{
					min_t = t_t;
					primitive_index = i;
					u = t_u;
					v = t_v;
					w = t_w;
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

	if (primitive_index != -1)
	{
		Surfel sf;
		sf.m_primitive_index = primitive_index;
		sf.m_barycentric[0] = u;
		sf.m_barycentric[1] = v;
		sf.m_barycentric[2] = w;
		sf.m_position = pos + normalizedRay * min_t;

		CGTriangle& tri = m_primitives[primitive_index].GetConvex<CGTriangle>();
		sf.m_normal = Normalize(Cross(tri[1] - tri[0], tri[2] - tri[0]));

		m_surfels.push_back(sf);
		return m_surfels.size() - 1;
	}
	else
	{
		return -1;
	}
#else
	struct BroadClosesetRayCast : BroadRayCast
	{
		float m_camera_near_plane = 0.f;
		float m_camera_far_plane = 0.f;
		Primitive* m_hit_primitive = nullptr;

		CGScalar min_t = 1000000.f;
		CGScalar hit_u, hit_v, hit_w;

		bool RayCastCallback(const GPED::c3RayInput& input, int nodeId)
		{
			void* data = broadPhase->GetUserData(nodeId);
			Primitive* primitive = (Primitive*)data;

			CGVec3 startPoint(input.startPoint[0], input.startPoint[1], input.startPoint[2]);
			CGVec3 rayDir(input.direction[0], input.direction[1], input.direction[2]);

			CGRay ray(startPoint, startPoint + rayDir * m_camera_near_plane, m_camera_far_plane * 2.f);

			CGScalar u, v, w, t;

			bool hit = IntersectTruePlane(primitive->GetConvex<CGTriangle>(), ray, u, v, w, t);

			if (hit && t < min_t)
			{
				min_t = t;
				hit_u = u;
				hit_v = v;
				hit_w = w;
				m_hit_primitive = primitive;
			}

			return true;
		}

	};

	BroadClosesetRayCast broad_ray_cast;
	broad_ray_cast.broadPhase = &m_broad_phase;
	broad_ray_cast.m_camera_near_plane = m_camera.m_near;
	broad_ray_cast.m_camera_far_plane = m_camera.m_far;

	glm::vec3 ray_from(pos[0], pos[1], pos[2]);
	glm::vec3 ray_to(pos[0] + normalizedRay[0], pos[1] + normalizedRay[1], pos[2] + normalizedRay[2]);
	GPED::c3RayInput ray_input(ray_from, ray_to);

	m_broad_phase.RayCast(&broad_ray_cast, ray_input);
	if (broad_ray_cast.m_hit_primitive != nullptr)
	{
		std::shared_ptr<Surfel> surfel(new Surfel());

		surfel->m_primitive_index = broad_ray_cast.m_hit_primitive->m_self_index;
		surfel->m_barycentric[0] = broad_ray_cast.hit_u;
		surfel->m_barycentric[1] = broad_ray_cast.hit_v;
		surfel->m_barycentric[2] = broad_ray_cast.hit_w;
		surfel->m_position = pos + normalizedRay * broad_ray_cast.min_t;

		CGTriangle& tri = broad_ray_cast.m_hit_primitive->GetConvex<CGTriangle>();
		surfel->m_normal = Normalize(Cross(tri[1] - tri[0], tri[2] - tri[0]));

		return surfel;
	}
	else
	{
		return nullptr;
	}
#endif
}

CG::CGVector3<float> CG::RayTracerDemo::ComputeLightIn(const CGVector3<float>& x, const CGVector3<float>& wi)
{
	std::shared_ptr<Surfel> surfel = FindIntersection(x, wi);

	CGVector3<float> color;
	if (surfel != nullptr)
	{
		color = ComputeLightOut(surfel, -wi);
	}
	else
	{
		color[0] = 0.f;
		color[1] = 0.f;
		color[2] = 0.f;
	}
	
	return color;
}

CG::CGVector3<float> CG::RayTracerDemo::ComputeLightOut(const std::shared_ptr<Surfel>& surfel, const CGVector3<float>& wo)
{
	CGVector3<float> radiance = surfel->GetEmittedRadiance(wo);

	const CGVector3<float>& x = surfel->m_position;
	const CGVector3<float>& n = surfel->m_normal;
	
	for (const Light& light : m_lights)
	{
		const CGVector3<float>& y = light.m_position;

		if (Visible(x, y))
		{
			const CGVector3<float> wi = Normalize(y - x);
			const CGVector3<float> bi_radiance = light.GetBiradiance(x);
			const CGVector3<float> f = surfel->GetFiniteScatteringDensity(wi, wo);

			radiance += bi_radiance * f * CGScalarOp<float>::Abs(Dot(wi, n));
		}
	}
	
	return radiance;
}

/* ### RayTracerDemo Demo ### */
/****************************************************************************************/

void CG::RayTracerCamera::GetPrimaryRay(float x, float y, int width, int height, CGVector3<float>& position, CGVector3<float>& w) const
{
	const float side = 2.f * tan(m_fov_in_radian / 2.f);

	position.m_value[0] = m_near * side * width / height * (x / width - 0.5f);
	position.m_value[1] = m_near * side * -(y / height - 0.5f);
	position.m_value[2] = -m_near;

	w = Normalize(position);
}