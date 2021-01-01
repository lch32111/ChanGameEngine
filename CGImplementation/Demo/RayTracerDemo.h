#pragma once
#ifndef __RAY_TRACER_DEMO_H__
#define __RAY_TRACER_DEMO_H__

#include <CG_Application.h>

#include <Math/CGVector3.h>

#include <Geometry/CGSphere.h>
#include <Geometry/CGPlane.h>
#include <Geometry/CGTriangle.h>

#include <GPED/CGBroadPhase.h>

#include <Graphics/CGAssetManager.h>

namespace CG
{
	// The whole ray tracer is from https://graphicscodex.com/ and G3D project.
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

		int m_self_index;

		ShapeType m_shape_type;
		CGConvex* m_convex;

		// temporary, triangle texcoord
		CGVector2<float> m_texcoord[3];
	};

	class Surfel
	{
	public:
		// CGVector3<float> m_position;
		// CGVector3<float> m_normal;

		// temporary, for textureing
		int m_primitive_index;
		float m_barycentric[3];	// barycentric
		
		CGVector3<float> m_position;
		CGVector3<float> m_normal;

		float m_reflectivity = 0.5f;

		CGVector3<float> GetEmittedRadiance(const CGVector3<float>& wo) const
		{
			return CGVector3<float>(0.f);
		}

		CGVector3<float> GetFiniteScatteringDensity(const CGVector3<float>& wi, const CGVector3<float>& wo) const
		{
			float wi_n = Dot(wi, m_normal);
			float wo_n = Dot(wo, m_normal);

			if (wi_n > 0 && wo_n > 0)
			{
				return CGVector3<float>(m_reflectivity / CGScalarOp<float>::Pi());
			}
			else
			{
				return CGVector3<float>(0.f);
			}
		}
	};

	// it's point light currently
	class Light
	{
	public:
		CGVector3<float> m_position;
		CGVector3<float> m_color;
		// CGVector3<float> m_attenuation;

		CGVector3<float> GetBiradiance(const CGVector3<float>& x) const
		{
			CGVector3<float> wi = m_position - x;
			float wi_len = Length(wi);
			// wi *= (1.f / wi_len);
			
			CGVector3<float> bi_radiance = m_color / (4.f * CGScalarOp<float>::Pi() * wi_len);

			return bi_radiance;
		}
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

		struct TracingTask
		{
			// [m_start_height, m_end_height)
			u32 m_start_height;
			u32 m_end_height;
		};

		void RayTraceTask(TracingTask task);

		const std::shared_ptr<Surfel> FindIntersection(const CGVector3<float>& x, const CGVector3<float>& wi);
		CGVector3<float> ComputeLightIn(const CGVector3<float>& x, const CGVector3<float>& wi);
		CGVector3<float> ComputeLightOut(const std::shared_ptr<Surfel>& surfel, const CGVector3<float>& wo);
		bool Visible(const CGVector3<float>& x, const CGVector3<float>& y) { return true; }

		u32 m_image_width;
		u32 m_image_height;
		CGVector3<float>* m_image_buffer;
		GLuint m_gl_image_tex;

		RayTracerCamera m_camera;

		CGBroadPhase m_broad_phase;

		std::vector<Primitive> m_primitives;
		std::vector<Light> m_lights;
	private:
		CGAssetManager m_asset_manager;
		Shader* m_simple_shader;

	private:
		bool m_left_mouse_clicked;
		bool m_color_paint_mode;
		CGVector3<float>* m_duplicated_image_buffer;
	};
}

#endif