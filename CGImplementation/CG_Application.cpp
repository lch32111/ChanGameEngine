#include "CG_Application.h"
#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_glfw.h>
#include <Imgui/imgui_impl_opengl3.h>
#include <iostream>

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
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	while (!glfwWindowShouldClose(app_window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		update(deltaTime, lastFrame);
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
	glClearColor(0.11, 0.11, 0.11, 1.0);
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

/* ### Physics Demo ### */
const char * CGProj::PhysicsDemo::getTitle()
{
	std::string str = Application::getTitle();
	str += " > PhysicsDemo";
	return str.c_str();
}

void CGProj::PhysicsDemo::initGraphics()
{
	Application::initGraphics();
	
	glfwSwapInterval(0); // Turn off Vsync and measure the FPS
	// glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	test1.initGraphics();
}

void CGProj::PhysicsDemo::initImgui()
{
	Application::initImgui();
}

void CGProj::PhysicsDemo::setView()
{
	// Application::setView();
}

void CGProj::PhysicsDemo::deinit()
{
	Application::deinit();

	test1.deinit();
}

void CGProj::PhysicsDemo::update(float deltaTime, float lastFrame)
{
	// Application::update(deltaTime, lastFrame);

	test1.key(app_window, deltaTime);
	test1.updateImgui();
	test1.updateSimulation(deltaTime, lastFrame);
}

void CGProj::PhysicsDemo::display()
{
	Application::display();

	test1.display(width, height);
}

void CGProj::PhysicsDemo::mouse(double xpos, double ypos)
{
	// Application::mouse(xpos, ypos);

	test1.mouse(xpos, ypos);
}

void CGProj::PhysicsDemo::mouseButton(int button, int action, int mods)
{
	// Application::mouseButton(button, action, mods)

	test1.mouseButton(app_window, button, action, mods, width, height);
}


void CGProj::PhysicsDemo::scroll(double yoffset)
{
	// Application::scroll(yoffset);

	test1.scroll(yoffset);
}

/* ### Physics Demo ### */

/****************************************************************************************/

/* ### Graphics Demo ### */
const char * CGProj::GraphicsDemo::getTitle()
{
	std::string str = Application::getTitle();
	str += " > GraphicsDemo";
	return str.c_str();
}

void CGProj::GraphicsDemo::initGraphics()
{
	Application::initGraphics();
	glfwSwapInterval(0); // Turn off Vsync and measure the FPS
	glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	test2.initGraphics(width, height);
}

void CGProj::GraphicsDemo::initImgui()
{
	Application::initImgui();

	test2.initImgui();
}

void CGProj::GraphicsDemo::setView()
{
}

void CGProj::GraphicsDemo::deinit()
{
	Application::deinit();

	test2.deinit();
}

void CGProj::GraphicsDemo::update(float deltaTime, float lastFrame)
{
	test2.key(app_window, deltaTime);
	test2.updateImgui();
}

void CGProj::GraphicsDemo::display()
{
	Application::display();
	
	test2.display(width, height);
}

void CGProj::GraphicsDemo::mouse(double xpos, double ypos)
{
	test2.mouse(xpos, ypos);
}

void CGProj::GraphicsDemo::scroll(double yoffset)
{
	test2.scroll(yoffset);
}

void CGProj::GraphicsDemo::resize(int width, int height)
{
	Application::resize(width, height);

	test2.resize(width, height);
}

/* ### Graphics Demo ### */
/****************************************************************************************/

CGProj::Application* getApplication()
{
	return new CGProj::GraphicsDemo();
}
