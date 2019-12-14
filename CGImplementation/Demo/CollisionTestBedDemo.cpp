#include "CollisionTestBedDemo.h"

#include <string>

#include <Imgui/imgui.h>

#include <Graphics/CGAssetManager.h>
#include <Graphics/CGSizableRenderLine.h>
#include <Graphics/GLPrimitiveUtil.h>

#include <CollisionDetection/CGCollisionFunction.h>

#include <GPED/GPED_random.h>

/****************************************************************************************/
/* ### Collision Demo ### */
const char* CGProj::CollisionDemo::getTitle()
{
	std::string str = Application::getTitle();
	str += " > CollisionDemo";
	return str.c_str();
}

void CGProj::CollisionDemo::initGraphics()
{
	Application::initGraphics();
	glfwSwapInterval(0);
	glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	demo = new CollisionTestBed();
	demo->initGraphics(width, height);
}

void CGProj::CollisionDemo::initImgui()
{
	Application::initImgui();

	demo->initImgui();
}

void CGProj::CollisionDemo::setView()
{
	
}

void CGProj::CollisionDemo::deinit()
{
	Application::deinit();

	demo->deinit();
	delete demo;
}

void CGProj::CollisionDemo::update(float deltaTime, float lastFrame)
{
	Application::update(deltaTime, lastFrame);

	demo->key(app_window, deltaTime);
	demo->updateImgui();
	demo->updateSimulation(deltaTime, lastFrame);
}

void CGProj::CollisionDemo::display()
{
	// TODO : change this kind of use to take care of what the base class is doing into
	// the structure that split the behavior of application with its child.
	Application::display();

	demo->display(width, height);
}

void CGProj::CollisionDemo::mouse(double xpos, double ypos)
{
	demo->mouse(xpos, ypos);
}

void CGProj::CollisionDemo::mouseButton(int button, int action, int mods)
{
	demo->mouseButton(app_window, button, action, mods, width, height);
}

void CGProj::CollisionDemo::scroll(double yoffset)
{
	demo->scroll(yoffset);
}

void CGProj::CollisionDemo::resize(int width, int height)
{
	Application::resize(width, height);

	demo->resize(width, height);
}

/* ### Collision Demo ### */
/****************************************************************************************/

static CGProj::CGAssetManager s_AssetManager;
static CGProj::Shader* s_simpleColorShader;
static CGProj::CGSizableRenderLine* gridRenderer;
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
	

	aSphere.m_radius = 1.0f;
	aSphere.m_pos = CGVec3(-1.f, 1.f, 0.f);

	bSphere.m_radius = 0.5f;
	bSphere.m_pos = CGVec3(1.f, 1.f, 0.f);
}

template<typename Scalar>
static inline bool isSame(const CGProj::Math::CGMat4<Scalar>& cm, const glm::mat4& gm)
{
	for (int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
			if (CGScalarUtil::abs(cm[i][j] - gm[i][j]) > CGScalarUtil::epsilon())
			{
				return false;
			}
	}
	
	return true;
}

template<typename Scalar>
static inline bool isSame(const CGProj::Math::CGMat3<Scalar>& cm, const glm::mat3& gm)
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
			if (CGScalarUtil::abs(cm[i][j] - gm[i][j]) > CGScalarUtil::epsilon())
			{
				return false;
			}
	}

	return true;
}


void CGProj::CollisionTestBed::initImgui()
{
	std::cout.precision(30);
	GPED::Random ran(20191215);
	const int TEST_NUMB = 2000;
	CGMat4 tCGMat4[TEST_NUMB];
	CGMat4 tOppositeCGMat4[TEST_NUMB];
	glm::mat4 gMat4[TEST_NUMB];
	glm::mat4 gOppositeMat4[TEST_NUMB];
	for (int i = 0; i < TEST_NUMB; ++i)
	{
		CGScalar s[16];
		CGScalar s2[16];
		for (int j = 0; j < 16; ++j)
		{
			s[j] = ran.randomReal(-10, 10);
			s2[j] = ran.randomReal(-10, 10);;
		}

		memcpy(&tCGMat4[i], s, sizeof(CGMat4));
		memcpy(&gMat4[i], s, sizeof(CGMat4));
		memcpy(&tOppositeCGMat4[i], s2, sizeof(CGMat4));
		memcpy(&gOppositeMat4[i], s2, sizeof(CGMat4));
	}

	CGMat4 tCGMat4R[TEST_NUMB];

	double startT = glfwGetTime();
	for (int i = 0; i < TEST_NUMB; ++i)
	{
		tCGMat4R[i] = tCGMat4[i] * tOppositeCGMat4[i];
	}
	double endT = glfwGetTime();
	double term = (endT - startT);
	std::cout << "my * : " << term << '\n';

	glm::mat4 gMat4R[TEST_NUMB];
	
	startT = glfwGetTime();
	for (int i = 0; i < TEST_NUMB; ++i)
	{
		gMat4R[i] = gMat4[i] * gOppositeMat4[i];
	}
	endT = glfwGetTime();
	term = (endT - startT);
	std::cout << "glm * : " << term << '\n';

	CGScalar tCGDetV[TEST_NUMB];
	CGScalar tCGDetV2[TEST_NUMB];
	startT = glfwGetTime();
	for (int i = 0; i < TEST_NUMB; ++i)
	{
		tCGDetV[i] = CGProj::Math::Determinant(tCGMat4[i]);
		tCGDetV2[i] = CGProj::Math::Determinant(tOppositeCGMat4[i]);
	}
	endT = glfwGetTime();
	term = (endT - startT);
	std::cout << "my det : " << term << '\n';

	float gDetV[TEST_NUMB];
	float gDetV2[TEST_NUMB];
	startT = glfwGetTime();
	for (int i = 0; i < TEST_NUMB; ++i)
	{
		gDetV[i] = glm::determinant(gMat4[i]);
		gDetV2[i] = glm::determinant(gOppositeMat4[i]);
	}
	endT = glfwGetTime();
	term = (endT - startT);
	std::cout << "glm det : " << term << '\n';

	startT = glfwGetTime();
	for (int i = 0; i < TEST_NUMB; ++i)
	{
		tCGMat4R[i] = CGProj::Math::Inverse(tCGMat4[i]);
		tCGMat4R[i] = CGProj::Math::Inverse(tOppositeCGMat4[i]);
	}
	endT = glfwGetTime();
	term = (endT - startT);
	std::cout << "my Inverse : " << term << '\n';

	startT = glfwGetTime();
	for (int i = 0; i < TEST_NUMB; ++i)
	{
		gMat4R[i] = glm::inverse(gMat4[i]);
		gMat4R[i] = glm::inverse(gOppositeMat4[i]);
	}
	endT = glfwGetTime();
	term = (endT - startT);
	std::cout << "glm Inverse : " << term << '\n';
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
	isSphereCollided = CollisionDetection::intersect(aSphere, bSphere);
}

void CGProj::CollisionTestBed::display(int width, int height)
{
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