#pragma once
#ifndef __RAY_TRACER_DEMO_H__
#define __RAY_TRACER_DEMO_H__

#include <CG_Application.h>
#include <Math/CGVector3.h>

namespace CGProj
{
	// The whole ray tracer is from https://graphicscodex.com/
	class RayTracerCamera
	{
	public:

		void GetPrimaryRay(float x, float y, int width, int height,
			CGProj::Math::CGVector3<float>& position,
			CGProj::Math::CGVector3<float>& w) const;

		float m_near;
		float m_far;
		float m_fov_in_radian;
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
		void RayTrace();

		u32 m_image_width;
		u32 m_image_height;
		CGProj::Math::CGVector3<float>* m_image_buffer;
		GLuint m_gl_image_tex;

	private:
		RayTracerCamera m_camera;
	};
}

#endif