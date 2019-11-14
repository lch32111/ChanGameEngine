#include "CG_Application.h"
#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_glfw.h>
#include <Imgui/imgui_impl_opengl3.h>
#include <iostream>

#include <Demo/TryFirst.h>
#include <Demo/DeferredRenderer.h>
#include <Demo/SimpleTerrainDemo.h>

/* ### Application ### */
const char * CGProj::Application::getTitle()
{
	return "CG Team Demo";
}

void CGProj::Application::initGraphics()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	app_window = glfwCreateWindow(width, height, "CG Demo", NULL, NULL);
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
}

void CGProj::Application::initImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfw_InitForOpenGL(app_window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	
	ImGui::StyleColorsDark();
}

void CGProj::Application::setView()
{
}

void CGProj::Application::deinit()
{
	glfwTerminate();
}

void CGProj::Application::execute()
{
	while (!glfwWindowShouldClose(app_window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		update(deltaTime, lastFrame);

		if(canDisplay)
			display();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(app_window);
	}
}

void CGProj::Application::update(float deltaTime, float lastFrame)
{
}

void CGProj::Application::display()
{
	glClearColor(0.11f, 0.11f, 0.11f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CGProj::Application::key()
{
}


void CGProj::Application::mouse(double xpos, double ypos)
{
}

void CGProj::Application::mouseButton(int button, int action, int mods)
{
}

void CGProj::Application::mousdDrag(double xpos, double ypos)
{
}

void CGProj::Application::resize(int width, int height)
{
	Application::width = width;
	Application::height = height;
	Application::canDisplay = ((width <= 0 || height <= 0) ? false : true);
	glViewport(0, 0, width, height);
}

void CGProj::Application::scroll(double yoffset)
{
}

void CGProj::Application::renderText(int xpos, int ypos, const char * text)
{
}

/* ### Application ### */
/****************************************************************************************/

CGProj::Application* getApplication()
{
	return new CGProj::GraphicsDemo();
}