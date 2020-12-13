#include <CGPrecompiled.h>
#include "CollisionTestBedDemo.h"

#include <string>

#include <Imgui/imgui.h>

#include <Graphics/CGAssetManager.h>
#include <Graphics/CGSizableRenderLine.h>
#include <Graphics/GLPrimitiveUtil.h>

#include <CollisionDetection/CGCollisionFunction.h>

#include <GPED/GPED_random.h>
#include <GPED/CGPhysicsUtil.h>

using namespace CGProj::CollisionDetection;

/****************************************************************************************/
/* ### Collision Demo ### */
void CGProj::CollisionDemo::OnInitialize()
{
	glfwSwapInterval(0);
	glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	demo = new CollisionTestBed();
	demo->initGraphics(m_width, m_height);
	demo->initImgui();
}

void CGProj::CollisionDemo::OnFinalize()
{
	demo->deinit();
	delete demo;
}

void CGProj::CollisionDemo::Update(float deltaTime, float lastFrame)
{
	demo->key(app_window, deltaTime);
	demo->updateImgui();
	demo->updateSimulation(deltaTime, lastFrame);
}

void CGProj::CollisionDemo::Display()
{
	demo->display(m_width, m_height);
}

void CGProj::CollisionDemo::MouseMoveCallback(double xpos, double ypos)
{
	demo->mouse(xpos, ypos);
}

void CGProj::CollisionDemo::MouseButtonCallback(int button, int action, int mods)
{
	demo->mouseButton(app_window, button, action, mods, m_width, m_height);
}

void CGProj::CollisionDemo::ScrollCallback(double yoffset)
{
	demo->scroll(yoffset);
}

void CGProj::CollisionDemo::ResizeWindowCallback(int width, int height)
{
	Application::m_width = width;
	Application::m_height = height;
	glViewport(0, 0, width, height);

	demo->resize(width, height);
}

/* ### Collision Demo ### */
/****************************************************************************************/

static CGProj::CGAssetManager s_AssetManager;
static CGProj::Shader* s_simpleColorShader;
static CGProj::CGSizableRenderLine* gridRenderer;
static CGProj::CGSizableRenderLine* AllLineRenderer;
void CGProj::CollisionTestBed::initGraphics(int width, int height)
{
	s_AssetManager.assetInit();

	s_simpleColorShader = s_AssetManager.getShader(CG_SHADER_ENUM::SHADER_SIMPLE_COLOR_RENDER);

	
	// setting for grid
	int gridLen = 1000;
	int gridWidth = 2;
	int gridY = 0;
	int gridLeft = -gridLen, gridRight = gridLen;
	int gridZMinus = -gridLen, gridZPlus = gridLen;
	int lineNumb = 4000;

	gridRenderer = new CGSizableRenderLine(s_AssetManager, lineNumb);
	gridRenderer->setFlush(false);
	glm::vec4 lineColor(0.7f, 0.7f, 0.7f, 1.0f);
	for (int i = -gridLen; i <= gridLen; i += gridWidth)
	{
		// vertical
		gridRenderer->insertLine(glm::vec3(i, gridY, gridZMinus), glm::vec3(i, gridY, gridZPlus), lineColor);

		// horizontal
		gridRenderer->insertLine(glm::vec3(gridLeft, gridY, i), glm::vec3(gridRight, gridY, i), lineColor);
	}

	AllLineRenderer = new CGSizableRenderLine(s_AssetManager, 300);
	AllLineRenderer->setFlush(true);

	aSphere.m_radius = 1.0f;
	aSphere.m_pos = CGVec3(-1.f, 1.f, 0.f);

	bSphere.m_radius = 0.5f;
	bSphere.m_pos = CGVec3(1.f, 1.f, 0.f);
}

void CGProj::CollisionTestBed::initImgui()
{
	
}

void CGProj::CollisionTestBed::deinit()
{
	s_AssetManager.destroy();

	delete gridRenderer;
}


void CGProj::CollisionTestBed::updateImgui()
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

void CGProj::CollisionTestBed::updateSimulation(float dletaTime, float lastTime)
{
	isSphereCollided = CollisionDetection::intersect(aSphere, bSphere, sphereContact);
}

void CGProj::CollisionTestBed::display(int width, int height)
{
	glClearColor(0.11f, 0.11f, 0.11f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view; camera.GetViewMatrix(view);
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 1000.f);

	// clearing Color on Default Frame buffer in the base class
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	{
		// Bind Simple Color Pipeline
		s_simpleColorShader->use();
		s_simpleColorShader->setMat4("projection", projection);
		s_simpleColorShader->setMat4("view", view);

		// Sphere A
		{
			glm::mat4 modelA(1.0);
			glm::vec3 convert(aSphere.m_pos.m_value[0],
				aSphere.m_pos.m_value[1],
				aSphere.m_pos.m_value[2]);
			modelA = glm::translate(modelA, convert);
			modelA = glm::scale(modelA, glm::vec3(aSphere.m_radius));

			glm::vec3 color(1.f, 0.f, 0.f);

			if (isSphereCollided)
				color = glm::vec3(0.5, 0.5, 0.5);

			if (isRaySphereCollided)
				color = glm::vec3(0.32, 0.112, 0.78);


			s_simpleColorShader->setMat4("model", modelA);
			s_simpleColorShader->setVec3("Color", color);
		}
		renderSphere();

		// Sphere B
		{
			glm::vec3 convert(bSphere.m_pos.m_value[0],
				bSphere.m_pos.m_value[1],
				bSphere.m_pos.m_value[2]);
			glm::mat4 modelB(1.0);
			modelB = glm::translate(modelB, convert);
			modelB = glm::scale(modelB, glm::vec3(bSphere.m_radius));

			glm::vec3 color(0.f, 0.f, 1.f);
			if (isSphereCollided)
				color = glm::vec3(0.5, 0.5, 0.5);

			s_simpleColorShader->setMat4("model", modelB);
			s_simpleColorShader->setVec3("Color", color);
		}
		renderSphere();

		// Sphere Contact
		{
			if (isSphereCollided)
			{
				AllLineRenderer->insertLine(aSphere.m_pos, sphereContact.position, CGVec4(0.2f, 0.765f, 0.432f, 1.f));
				AllLineRenderer->renderLine(view, projection);
			}
		}

		// render grid
		gridRenderer->renderLine(view, projection);
	}
}

void CGProj::CollisionTestBed::key(GLFWwindow* app_window, float deltaTime)
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
	else
	{
		if (glfwGetKey(app_window, GLFW_KEY_W) == GLFW_PRESS)
			aSphere.m_pos.m_value[2] -= 0.001f;
		if (glfwGetKey(app_window, GLFW_KEY_S) == GLFW_PRESS)
			aSphere.m_pos.m_value[2] += 0.001f;
		if (glfwGetKey(app_window, GLFW_KEY_A) == GLFW_PRESS)
			aSphere.m_pos.m_value[0] -= 0.001f;
		if (glfwGetKey(app_window, GLFW_KEY_D) == GLFW_PRESS)
			aSphere.m_pos.m_value[0] += 0.001f;
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

void CGProj::CollisionTestBed::mouse(double xpos, double ypos)
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

void CGProj::CollisionTestBed::mouseButton(GLFWwindow * app_window,
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
			glm::vec3 rayFrom = camera.Position;
			glm::vec3 rayTo = GetRayTo((int)x, (int)y, &camera, screen_width, screen_height);
			rayTo /= 10.f;

			CGCollisionRay r;
			r.m_source = CGVec3(rayFrom.x, rayFrom.y, rayFrom.z);
			r.m_target = CGVec3(rayTo.x, rayTo.y, rayTo.z);

			isRaySphereCollided = CollisionDetection::intersect(aSphere, r);
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		mouseClick = false;
	}
}

void CGProj::CollisionTestBed::scroll(double yoffset)
{
	// Exit out if mouse clicks on Imgui GUI
	if (ImGui::IsMouseHoveringAnyWindow()) return;

	camera.ProcessMouseScroll((float)yoffset);
}

void CGProj::CollisionTestBed::resize(int width, int height)
{

}