#include <CGPrecompiled.h>
#include "CG_Application.h"
#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_glfw.h>
#include <Imgui/imgui_impl_opengl3.h>
#include <iostream>

#include <Demo/TryFirst.h>
#include <Demo/DeferredRenderer.h>
#include <Demo/SimpleTerrainDemo.h>
#include <Demo/CollisionTestBedDemo.h>
#include <Demo/RayTracerDemo.h>

/* ### Application ### */
void CGProj::Application::Initialize(bool shouldBaseInit)
{
	m_isBaseInit = shouldBaseInit;

	if (shouldBaseInit)
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// glfw window creation
		app_window = glfwCreateWindow(m_width, m_height, "CG Demo", NULL, NULL);
		if (app_window == NULL)
		{
			std::cout << "CJProj::Application::initGraphics::Failed to create GLFW window\n";
			glfwTerminate();
		}

		glfwMakeContextCurrent(app_window);

		// glad : load all OpenGL function pointers
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "CJProj::Application::initGraphics::Failed to initializae GLAD\n";
			glfwTerminate();
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImGui_ImplGlfw_InitForOpenGL(app_window, true);
		ImGui_ImplOpenGL3_Init("#version 130");

		ImGui::StyleColorsDark();
	}
	
	OnInitialize();
}

void CGProj::Application::Finalize()
{
	OnFinalize();

	if (m_isBaseInit)
	{
		glfwTerminate();
	}
}

void CGProj::Application::Execute()
{
	while (!glfwWindowShouldClose(app_window))
	{
		float currentFrame = (float)glfwGetTime();
		m_deltaTime = currentFrame - m_lastFrame;
		m_lastFrame = currentFrame;
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		Update(m_deltaTime, m_lastFrame);

		Display();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(app_window);
	}
}

/* ### Application ### */
/****************************************************************************************/

namespace CGProj
{
	class ApplicationSelector : public CGProj::Application
	{
	public:

		ApplicationSelector()
			: m_app(nullptr)
			, m_appSelected(false)
		{}

		~ApplicationSelector()
		{
			if (m_appSelected)
			{
				m_app->Finalize();
				delete m_app;
			}
		}

		virtual void Execute() override
		{
			while (!glfwWindowShouldClose(app_window))
			{
				float currentFrame = (float)glfwGetTime();
				m_deltaTime = currentFrame - m_lastFrame;
				m_lastFrame = currentFrame;
				glfwPollEvents();

				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				Update(m_deltaTime, m_lastFrame);
				Display();

				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

				glfwSwapBuffers(app_window);
			}

		}

		virtual void OnInitialize() { }
		virtual void OnFinalize() { }

		virtual void Update(float deltaTime, float lastFrame) override
		{
			if (m_appSelected == true)
			{
				m_app->Update(deltaTime, lastFrame);
			}
			else
			{
				bool appNotSelected = !m_appSelected;

				auto openApp = [&](Application* app)
				{
					m_appSelected = true;
					m_app->app_window = this->app_window;
					m_app->Initialize(false);
					m_app->ResizeWindowCallback(m_width, m_height);
				};

				ImGui::Begin("Application Selector", &appNotSelected);
				{
					if (ImGui::Button("PhysicsDemo"))
					{
						m_app = new PhysicsDemo();
						openApp(m_app);
					}
					
					if (ImGui::Button("Graphics Demo"))
					{
						m_app = new GraphicsDemo();
						openApp(m_app);
					}

					if (ImGui::Button("Terrain Demo"))
					{
						m_app = new TerrainDemo();
						openApp(m_app);
					}

					if (ImGui::Button("New Collision System Working"))
					{
						m_app = new CollisionDemo();
						openApp(m_app);
					}

					if (ImGui::Button("New RayTracer Demo Working"))
					{
						m_app = new RayTracerDemo();
						openApp(m_app);
					}
				}
				ImGui::End();
			}
		}

		virtual void Display() override
		{
			if (m_appSelected == true)
			{
				m_app->Display();
			}
			else
			{
				glClearColor(0.f, 0.5f, 0.5f, 1.f);
				glClear(GL_COLOR_BUFFER_BIT);
			}
		}

		virtual void KeyCallback(int key, int scancode, int action, int mods)
		{
			if (key == GLFW_KEY_B && mods == GLFW_MOD_CONTROL)
			{
				if (m_appSelected == true)
				{
					m_app->Finalize();
					delete m_app;
					m_app = nullptr;
					m_appSelected = false;

					glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
			}

			if (m_appSelected == true)
			{
				m_app->KeyCallback(key, scancode, action, mods);
			}
		}

		virtual void MouseMoveCallback(double xpos, double ypos)
		{
			if (m_appSelected)
			{
				m_app->MouseMoveCallback(xpos, ypos);
			}
		}

		virtual void MouseButtonCallback(int button, int action, int mods)
		{
			if (m_appSelected)
			{
				m_app->MouseButtonCallback(button, action, mods);
			}
		}

		virtual void MouseDragCallback(double xpos, double ypos)
		{
			if (m_appSelected)
			{
				m_app->MouseDragCallback(xpos, ypos);
			}
		}

		virtual void ResizeWindowCallback(int width, int height)
		{
			Application::m_width = width;
			Application::m_height = height;

			if (m_appSelected)
			{
				m_app->ResizeWindowCallback(width, height);
			}
		}

		virtual void ScrollCallback(double yoffset)
		{
			if (m_appSelected)
			{
				m_app->ScrollCallback(yoffset);
			}
		}

	private:
		CGProj::Application* m_app;
		bool m_appSelected;
	};
}

CGProj::Application* getApplication()
{
	return new CGProj::ApplicationSelector();
}