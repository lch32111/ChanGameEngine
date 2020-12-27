#pragma once
#ifndef __RAY_TRACER_DEMO_H__
#define __RAY_TRACER_DEMO_H__

#include <CG_Application.h>
#include <Math/CGVector3.h>
#include <Geometry/CGSphere.h>
#include <Geometry/CGPlane.h>
#include <Geometry/CGTriangle.h>
#include <Graphics/CGAssetManager.h>

namespace CG
{
	// The whole ray tracer is from https://graphicscodex.com/
	class RayTracerCamera
	{
	public:
		/* 
		It's assumed that the camera is located on (0,0,0) in the world, 
		and then watching the negative z-direction (0, 0, -1) (toward screen).
		*/
		void GetPrimaryRay(float x, float y, int width, int height,
			CGVector3<float>& position,
			CGVector3<float>& w) const;

		float m_near;
		float m_far;
		float m_fov_in_radian;
	};

	class Primitive
	{
	public:

		enum ShapeType
		{
			SPHERE,
			PLANE,
			TRIANGLE,
			NONE
		};

		Primitive()
			: m_shape_type(NONE)
		{}

		void Initialize(ShapeType type)
		{
			m_shape_type = type;

			switch (type)
			{
			case SPHERE:
				m_convex = new CGSphere();
				break;
			case PLANE:
				m_convex = new CGPlane();
				break;
			case TRIANGLE:
				m_convex = new CGTriangle();
				break;
			}
		}

		void Finalize()
		{
			if (m_shape_type != NONE)
			{
				delete m_convex;
			}
		}

		template<typename T, bool is_base_of = std::is_base_of<CGConvex, T>::value>
		T& GetConvex()
		{
			CG_DEBUG_ASSERT(m_shape_type != NONE);
			return *((T*)m_convex);
		}

		ShapeType m_shape_type;
		CGConvex* m_convex;
	};

	class Surfel
	{
	public:
		CGVector3<float> m_position;
		CGVector3<float> m_normal;
	};

	class RayTracerDemo : public Application
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
		void InitializeScene();
		void FinalizeScene();
		void RayTrace();
		CGVector3<float> ComputeLight(CGVector3<float> pos, CGVector3<float> normalizedRay);
		const Surfel* FindIntersection(CGVector3<float> pos, CGVector3<float> normalizedRay);


		u32 m_image_width;
		u32 m_image_height;
		CGVector3<float>* m_image_buffer;
		GLuint m_gl_image_tex;

		RayTracerCamera m_camera;

		std::vector<Primitive> m_primitives;
		std::vector<Surfel> m_surfels;
	private:
		CGAssetManager m_asset_manager;
		Shader* m_simple_shader;
	};
}

#endif