#include <CGPrecompiled.h>
#include "CullingDemo.h"

#include <string>

#include <Imgui/imgui.h>

/****************************************************************************************/
/* ### Collision Demo ### */

void CGProj::CullingDemo::OnInitialize()
{
	glfwSwapInterval(0);
	glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	demo = new CullingTestBed();
	demo->initGraphics(m_width, m_height);
	demo->initImgui();
}

void CGProj::CullingDemo::OnFinalize()
{
	demo->deinit();
	delete demo;
}

void CGProj::CullingDemo::Update(float deltaTime, float lastFrame)
{
	demo->key(app_window, deltaTime);
	demo->updateImgui();
	demo->updateSimulation(deltaTime, lastFrame);
}

void CGProj::CullingDemo::Display()
{
	demo->display(m_width, m_height);
}

void CGProj::CullingDemo::MouseMoveCallback(double xpos, double ypos)
{
	demo->mouse(xpos, ypos);
}

void CGProj::CullingDemo::MouseButtonCallback(int button, int action, int mods)
{
	demo->mouseButton(app_window, button, action, mods, m_width, m_height);
}

void CGProj::CullingDemo::ScrollCallback(double yoffset)
{
	demo->scroll(yoffset);
}

void CGProj::CullingDemo::ResizeWindowCallback(int width, int height)
{
	Application::m_width = width;
	Application::m_height = height;
	glViewport(0, 0, width, height);

	demo->resize(width, height);
}

/* ### Collision Demo ### */
/****************************************************************************************/

void CGProj::CullingTestBed::initGraphics(int width, int height)
{
}


void CGProj::CullingTestBed::initImgui()
{

}

void CGProj::CullingTestBed::deinit()
{
}


void CGProj::CullingTestBed::updateImgui()
{
	ImGui::Begin("Collision Test Bed");
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Camera Position %.1f %.1f %.1f", camera.Position.x, camera.Position.y, camera.Position.z);

		ImGui::TextColored(ImVec4(0.99f, 0.4f, 0.37f, 1.0f), "Press Tab Button to convert GAME/UI Mode");
		if (GameControl) ImGui::TextColored(ImVec4(0.78f, 0.17f, 0.54f, 1.0f), "GAME mode");
		else ImGui::TextColored(ImVec4(0.11f, 0.7f, 0.81f, 1.0f), "UI mode");
	}
	ImGui::End();
}

void CGProj::CullingTestBed::updateSimulation(float dletaTime, float lastTime)
{

}

void CGProj::CullingTestBed::display(int width, int height)
{
	glClearColor(0.11f, 0.11f, 0.11f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view; camera.GetViewMatrix(view);
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 1000.f);

	// clearing Color on Default Frame buffer in the base class
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}

void CGProj::CullingTestBed::key(GLFWwindow* app_window, float deltaTime)
{
	if (GameControl)
	{
		if (glfwGetKey(app_window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(chanQuatCamera::Camera_Movement::FORWARD, deltaTime);
		if (glfwGetKey(app_window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(chanQuatCamera::Camera_Movement::BACKWARD, deltaTime);
		if (glfwGetKey(app_window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(chanQuatCamera::Camera_Movement::LEFT, deltaTime);
		if (glfwGetKey(app_window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(chanQuatCamera::Camera_Movement::RIGHT, deltaTime);
	}

	if (glfwGetKey(app_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(app_window, 1);

	if (glfwGetKey(app_window, GLFW_KEY_TAB) == GLFW_PRESS && !tabKey)
	{
		tabKey = true;
		if (GameControl) // GAME -> UI
		{
			GameControl = false;
			UIControl = true;
			glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			// Locate the cursor pos on the center of screen
			glfwSetCursorPos(app_window, ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
		}
		else // UI -> GAME
		{
			UIControl = false;
			GameControl = true;
			glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			// Locate the cursor pos on the last position 
			// because of continuous movement of mouse
			glfwSetCursorPos(app_window, GamelastX, GamelastY);
		}
	}

	if (glfwGetKey(app_window, GLFW_KEY_TAB) == GLFW_RELEASE)
	{
		tabKey = false;
	}
}

void CGProj::CullingTestBed::mouse(double xpos, double ypos)
{
	if (GameControl)
	{
		if (firstMouse)
		{
			firstMouse = false;
			GamelastX = (float)xpos;
			GamelastY = (float)ypos;
		}

		float xoffset = (float)(xpos - GamelastX);
		float yoffset = (float)(GamelastY - ypos);
		GamelastX = (float)xpos;
		GamelastY = (float)ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	else
	{
		float xoffset = (float)(xpos - UILastX);
		float yoffset = (float)(UILastY - ypos);
		UILastX = (float)xpos;
		UILastY = (float)ypos;

		if (mouseClick) // Mouse Holding and Moving
		{
		}
	}
}

void CGProj::CullingTestBed::mouseButton(GLFWwindow * app_window,
	int button, int action, int mods,
	int screen_width, int screen_height)
{
	// Exit out if mouse clicks on Imgui GUI
	if (ImGui::IsMouseHoveringAnyWindow()) return;

	if (!mouseClick)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			mouseClick = true;
			double x, y;
			glfwGetCursorPos(app_window, &x, &y);

			// TODO : Ray Casting Collision Detection
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		mouseClick = false;
	}
}

void CGProj::CullingTestBed::scroll(double yoffset)
{
	// Exit out if mouse clicks on Imgui GUI
	if (ImGui::IsMouseHoveringAnyWindow()) return;

	camera.ProcessMouseScroll((float)yoffset);
}

void CGProj::CullingTestBed::resize(int width, int height)
{

}