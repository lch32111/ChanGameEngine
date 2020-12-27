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
			TRIANGLE
		};

		ShapeType m_shape_type;
		CGSphere m_sphere;
		CGPlane m_plane;
		CGTriangle m_triangle;
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
		void PrepareScene();
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