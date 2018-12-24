#include "SimpleTerrainDemo.h"

#include <Imgui/imgui.h>

void CGProj::SimpleTerrainDemo::initGraphics(int width, int height)
{
	assetManager.assetInit();
	camera.Position = glm::vec3(10, 10, 10);
	myTerrain.initialize(assetManager);
}

void CGProj::SimpleTerrainDemo::initImgui()
{
}

void CGProj::SimpleTerrainDemo::deinit()
{
	myTerrain.destroy();
}

void CGProj::SimpleTerrainDemo::updateImgui()
{
	GLFWwindow* app_window = (GLFWwindow*)ImGui::GetIO().ClipboardUserData;

	ImGui::Begin("Simple Terrain Demo"); // Create a window called " " and append into it.

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Camera Position %.1f %.1f %.1f", camera.Position.x, camera.Position.y, camera.Position.z);

	ImGui::End();
}

void CGProj::SimpleTerrainDemo::updateSimulation(float deltaTime, float lastFrame)
{

}

void CGProj::SimpleTerrainDemo::display(int width, int height)
{
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 1000.f);

	myTerrain.render(view, projection);
}

void CGProj::SimpleTerrainDemo::key(GLFWwindow * app_window, float deltaTime)
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

void CGProj::SimpleTerrainDemo::mouse(double xpos, double ypos)
{
	if (GameControl)
	{
		if (firstMouse)
		{
			firstMouse = false;
			GamelastX = xpos;
			GamelastY = ypos;
		}

		float xoffset = xpos - GamelastX;
		float yoffset = GamelastY - ypos;
		GamelastX = xpos;
		GamelastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

void CGProj::SimpleTerrainDemo::mouseButton(GLFWwindow * app_window, int button, int action, int mods, int screen_width, int screen_height)
{
}

void CGProj::SimpleTerrainDemo::scroll(double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void CGProj::SimpleTerrainDemo::resize(int width, int height)
{
}
