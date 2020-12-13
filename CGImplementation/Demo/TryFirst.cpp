#include "TryFirst.h"

#include <vector>

#include <Imgui/imgui.h>
#include <GPED/GPED_random.h>
#include <GPED/CGPhysicsUtil.h>

#include <Graphics/GLPrimitiveUtil.h>
#include <Graphics/GLTextureUtility.h>

/* ### Physics Demo ### */

void CGProj::PhysicsDemo::OnInitialize()
{
	glfwSwapInterval(0); // Turn off Vsync and measure the FPS
	// glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	test1 = new TryFirst();
	test1->initGraphics();
}

void CGProj::PhysicsDemo::OnFinalize()
{
	test1->deinit();
	delete test1;
}

void CGProj::PhysicsDemo::Update(float deltaTime, float lastFrame)
{
	test1->key(app_window, deltaTime);
	test1->updateImgui();
	test1->updateSimulation(deltaTime, lastFrame);
}

void CGProj::PhysicsDemo::Display()
{
	test1->display(m_width, m_height);
}

void CGProj::PhysicsDemo::MouseMoveCallback(double xpos, double ypos)
{
	test1->mouse(xpos, ypos);
}

void CGProj::PhysicsDemo::MouseButtonCallback(int button, int action, int mods)
{
	test1->mouseButton(app_window, button, action, mods, m_width, m_height);
}


void CGProj::PhysicsDemo::ScrollCallback(double yoffset)
{
	test1->scroll(yoffset);
}

void CGProj::PhysicsDemo::ResizeWindowCallback(int width, int height)
{
	Application::m_width = width;
	Application::m_height = height;
	glViewport(0, 0, width, height);
}

/* ### Physics Demo ### */

void CGProj::TryFirst::initGraphics()
{
	simpleShader = Shader("ShaderFolder/simpleRender.vs", "ShaderFolder/simpleRender.fs");
	simpleShader.loadShader();
	simpleShader.use();
	simpleShader.setInt("texture1", 0);

	wireShader = Shader("ShaderFolder/wireRender.vs", "ShaderFolder/wireRender.fs");
	wireShader.loadShader();

	woodTexture = TextureFromFile("ResourceFolder/ImageFolder/fieldGrass.jpg", false);
	containerTexture = TextureFromFile("ResourceFolder/ImageFolder/container2.png", false);

	resolver = GPED::ContactResolver(256 * 8);

	// Initialise the box
	GPED::Random ranGen(334455);
	GPED::c3AABB broadAABB;
	for (Box *box = boxData; box < boxData + boxes; box++)
	{
		float xRan = ranGen.randomReal(-10, 10);
		float yRan = ranGen.randomReal(3, 20);
		float zRan = ranGen.randomReal(0, 50);
		box->setState(xRan, yRan, zRan);
		GPED::convertFromCollisionPrimitive(*box, broadAABB);
		box->proxyId = FirstBroadPhase.CreateProxy(broadAABB, box);
	}
	currentShotType = ShotType::SHOT_PISTOL;

	bRender.connectTree(FirstBroadPhase.getTree());
	bRender.setColor(glm::vec3(1, 0, 0), glm::vec3(1, 1, 0));
	bRender.setLineWidth(1.5f, 1.f);

	bRayWrapper.broadPhase = &FirstBroadPhase;
	lineShader = Shader("ShaderFolder/CGLineShader.vs", "ShaderFolder/CGLineShader.fs");
	lineShader.loadShader();
	lineRen.setShader(&lineShader);
}

void CGProj::TryFirst::initImgui()
{

}

void CGProj::TryFirst::deinit()
{
}


void CGProj::TryFirst::updateImgui()
{
	GLFWwindow* app_window = (GLFWwindow*)ImGui::GetIO().ClipboardUserData;

	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Rigid Box Simulation"); // Create a window called " " and append into it.

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Camera Position %.1f %.1f %.1f", camera.Position.x, camera.Position.y, camera.Position.z);
	ImGui::TextColored(ImVec4(0.99f, 0.4f, 0.37f, 1.0f), "Press Tab Button to convert GAME/UI Mode");
	if (GameControl) ImGui::TextColored(ImVec4(0.78f, 0.17f, 0.54f, 1.0f), "GAME mode");
	else ImGui::TextColored(ImVec4(0.11f, 0.7f, 0.81f, 1.0f), "UI mode");

	if (ImGui::Button("Simulation Start / End")) start = start ? false : true; ImGui::SameLine();
	
	if (start) ImGui::TextColored(ImVec4(1.0f, 0.89f, 0.f, 1.0f), "Simulating...");
	else ImGui::TextColored(ImVec4(1.0f, 0.f, 0.f, 1.0f), "Not simulating!");

	if (ImGui::Button("Reset Simulation")) reset();

	if (ImGui::Button("Total Fire")) totalFire();

	static int shotSelection = 0;
	const char* items[] = { "Pistol", "Artillery", "FireBall", "Laser" };
	ImGui::Combo("Weapon Type", &shotSelection, items, 4);
	switch (shotSelection)
	{
	case 0: case 1: case 2: case 3:
		currentShotType = ShotType(shotSelection + 1);
		break;
	default:
		currentShotType = ShotType::SHOT_PISTOL;
		break;
	}
	ImGui::SliderFloat("Friction", &contactFriction, 0.f, 1.f);
	ImGui::SliderFloat("Restitution", &contactRestitution, 0.f, 1.f);
	ImGui::SliderFloat("Tolerance", &contactTolerance, 0.f, 1.f);

	ImGui::Checkbox("Broad Debug Render", &BroadDebug);
	
	ImGui::End();
}

void CGProj::TryFirst::updateSimulation(float deltaTime, float lastFrame)
{
	if (start)
	{
		/*
			Simulation Process
			1. Integration of the rigid bodies
			2. Sync with Broad Phase
			3. Broad Phase of Collision Detection
			4. Narrow Phase of Collision Detection
			5. Solve the contacts from Narrow Phase
		*/

		updateObjects(deltaTime, lastFrame);
		SyncAndUpdate(); // sync between client object and rigid body in broadPhase
		broadPhase(); // literally broadphase.
		generateContacts(cManager); // narrow phase from broadphase
		resolver.resolveContacts(&cManager, deltaTime);
	}
}

void CGProj::TryFirst::display(int width, int height)
{
	glClearColor(0.11f, 0.11f, 0.11f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 1000.f);
	glm::mat4 model(1.0);

	// floor
	simpleShader.use();
	simpleShader.setMat4("projection", projection);
	simpleShader.setMat4("view", view);
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.f));
	model = glm::scale(model, glm::vec3(50.f, 0.f, 50.f));
	simpleShader.setMat4("model", model);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	renderQuad();
	// floor

	// +Z-Wall 
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.f, 0.f, 50.f));
	model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(50.f, 0.f, 50.f));
	simpleShader.setMat4("model", model);
	renderQuad();

	simpleShader.use();
	simpleShader.setMat4("projection", projection);
	simpleShader.setMat4("view", view);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, containerTexture);
	for (AmmoRound* shot = ammo; shot < ammo + ammoRounds; ++shot)
	{
		if (shot->m_shotType != ShotType::SHOT_UNUSED)
		{
			model = glm::mat4(1.0);
			model = shot->body->getTransform();
			model = glm::scale(model, glm::vec3(shot->radius));
			simpleShader.setMat4("model", model);
			renderSphere();
		}
	}

	for (Box* box = boxData; box < boxData + boxes; ++box)
	{
		model = glm::mat4(1.0);
		model = box->body->getTransform();
		model = glm::scale(model, box->halfSize);
		simpleShader.setMat4("model", model);
		renderCube();
	}

	// Broad Phase Debug Rendering
	if(BroadDebug)
		bRender.draw(&wireShader, &projection, &view);

	// ray casting test render
	for (unsigned i = 0; i < rayCollector.size(); ++i)
		lineRen.insertLine(rayCollector[i].first, rayCollector[i].second);
	lineRen.renderLine(view, projection);
}

void CGProj::TryFirst::key(GLFWwindow* app_window, float deltaTime)
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

		if (glfwGetKey(app_window, GLFW_KEY_F) == GLFW_PRESS && !Fkey)
		{
			Fkey = true;
			fire();
		}

		if (glfwGetKey(app_window, GLFW_KEY_F) == GLFW_RELEASE)
		{
			Fkey = false;
		}
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
			glfwSetCursorPos(app_window, lastX, lastY);
		}
	}

	if (glfwGetKey(app_window, GLFW_KEY_TAB) == GLFW_RELEASE)
	{
		tabKey = false;
	}
}

void CGProj::TryFirst::mouse(double xpos, double ypos)
{
	if (GameControl)
	{
		if (firstMouse)
		{
			firstMouse = false;
			lastX = (float)xpos;
			lastY = (float)ypos;
		}

		float xoffset = (float)(xpos - lastX);
		float yoffset = (float)(lastY - ypos);
		lastX = (float)xpos;
		lastY = (float)ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	
}

void CGProj::TryFirst::mouseButton(GLFWwindow* app_window, 
	int button, int action, int mods,
	int screen_width, int screen_height)
{
	if (!mouseClick)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			mouseClick = true;
			double x, y;
			glfwGetCursorPos(app_window, &x, &y);
			
			glm::vec3 rayFrom = camera.Position;
			glm::vec3 rayTo = GetRayTo((int)x, (int)y, &camera, screen_width, screen_height);
			GPED::c3RayInput rayInput(rayFrom, rayTo);
			// rayCollector.push_back({ rayFrom, rayTo });
			struct CGTestClickCastCallback : CGRayCastCallback
			{
				virtual bool process
				(
					const GPED::c3RayOutput& output,
					const GPED::c3RayInput& input ,
					void* userData
				)
				{
					if (userData)
					{
						CGCollisionPrimitive* cP = (CGCollisionPrimitive*)userData;
						glm::vec3 Position = cP->body->getPosition();
						cP->body->setPosition(Position.x, 30, Position.z);
						cP->body->setAwake();
						return true;
					}

					return false;
				}
			};

			CGTestClickCastCallback tempCallback;
			bRayWrapper.callback = &tempCallback;
			FirstBroadPhase.RayCast(&bRayWrapper, rayInput);
		}
	}
	
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		mouseClick = false;
	}
}

void CGProj::TryFirst::scroll(double yoffset)
{
	if (GameControl)
	{
		camera.ProcessMouseScroll((float)yoffset);
	}
}

void CGProj::TryFirst::updateObjects(float duration, float lastFrame)
{
	for (AmmoRound* shot = ammo; shot < ammo + ammoRounds; ++shot)
	{
		if (shot->m_shotType != ShotType::SHOT_UNUSED)
		{
			glm::vec3 pos = shot->body->getPosition();

			if (shot->startTime + 5 < lastFrame || pos.y < 0 || pos.z > 50)
			{
				// We simply set the shot type to be unused, so the
				// memory it occupies can be reused by another shot.
				shot->m_shotType = ShotType::SHOT_UNUSED;
				
				FirstBroadPhase.DestroyProxy(shot->proxyId);
			}
			else
			{
				// Run the physics
				shot->body->integrate(duration);
				shot->calculateInternals();
			}
		}
	}

	// Update the boxes
	for (Box* box = boxData; box < boxData + boxes; ++box)
	{
		// Run the physics
		box->body->integrate(duration);
		box->calculateInternals();
	}
}

// After integration of rigid bodies,
// Sync the body AABBs in broad phase
void CGProj::TryFirst::SyncAndUpdate()
{
	GPED::c3AABB broadAABB;
	glm::vec3 displacement;
	for (int i = 0; i < boxes; ++i)
	{
		GPED::convertFromCollisionPrimitive(boxData[i], broadAABB);
		displacement = boxData[i].body->getPosition() - boxData[i].body->getLastFramePosition();
		FirstBroadPhase.UpdateProxy(boxData[i].proxyId, broadAABB, displacement);
	}

	for (int i = 0; i < ammoRounds; ++i)
		if (ammo[i].m_shotType != ShotType::SHOT_UNUSED)
		{
			GPED::convertFromCollisionPrimitive(ammo[i], broadAABB);
			displacement = ammo[i].body->getPosition() - ammo[i].body->getLastFramePosition();
			FirstBroadPhase.UpdateProxy(ammo[i].proxyId, broadAABB);
		}
}

// Update Broad Phase Pairs
void CGProj::TryFirst::broadPhase()
{
	firstResult.vPairs.clear();
	// Perform Tree Queries for all moving proxies
	// on this step, we gather the pairs and then 
	// pass the pairs to the generateContacts process for narrow phase
	FirstBroadPhase.UpdatePairs(&firstResult);
}

// narrow phase
void CGProj::TryFirst::generateContacts(CGContactManager& cData)
{
	CGCollisionPlane planeGround;
	planeGround.direction = glm::vec3(0, 1, 0);
	planeGround.offset = 0;

	CGCollisionPlane planeZWall;
	planeZWall.direction = glm::vec3(0, 0, -1);
	planeZWall.offset = -50;

	// Set up the collision data structure
	cData.reset();
	cData.friction = contactFriction;
	cData.restitution = contactRestitution;
	cData.tolerance = contactRestitution;

	// we will generate contacts from the pairs detected by broadphase
	// In addition, we will generate contacts manually with planes
	const std::vector<std::pair<CGCollisionPrimitive*, CGCollisionPrimitive*>>& t_pair
		= firstResult.vPairs;
	// std::cout << t_pair.size() << '\n';
	for (unsigned i = 0; i < t_pair.size(); ++i)
	{
		cAlgo.findNarrowAlgorithmAndProcess(t_pair[i].first, t_pair[i].second, &cData);
	}

	for (int i = 0; i < boxes; ++i)
	{
		if (!boxData[i].body->getAwake()) continue;
		CGCollisionNarrow::OBBAndHalfSpace(boxData[i], planeGround, &cData);
		CGCollisionNarrow::OBBAndHalfSpace(boxData[i], planeZWall, &cData);
	}
}

void CGProj::TryFirst::fire()
{
	int shotIndex = 0;
	for (int i = 0; i < ammoRounds; ++i)
		if (ammo[i].m_shotType == ShotType::SHOT_UNUSED)
		{
			shotIndex = i;
			break;
		}

	// If we didn't find a round, then exit- we can't fire.
	if (shotIndex >= ammoRounds) return;

	// Set the shot
	GPED::c3AABB broadAABB;
	ammo[shotIndex].setState(currentShotType, camera);

	GPED::convertFromCollisionPrimitive(ammo[shotIndex], broadAABB);
	ammo[shotIndex].proxyId = FirstBroadPhase.CreateProxy(broadAABB, &ammo[shotIndex]);
}

void CGProj::TryFirst::totalFire()
{
	int index = 0;
	for (int i = 0; i < ammoRounds; ++i)
	{
		if (ammo[i].m_shotType == ShotType::SHOT_UNUSED)
		{
			index = i;
			break;
		}
	}

	// If we didn't find a round, then exit- we can't fire.
	if (index >= ammoRounds) return;

	GPED::Random ranGen((unsigned)glfwGetTime());
	float x;
	float y;
	float z = 0;
	GPED::c3AABB broadAABB;
	for(int i = index, j = 0; i < ammoRounds && j < 10; ++i)
	{
		x = ranGen.randomBinomial(50);
		y = ranGen.randomReal(0, 5);
		if (ammo[i].m_shotType == ShotType::SHOT_UNUSED)
		{
			ammo[i].setState(ShotType::SHOT_ARTILLERY, glm::vec3(x, y, z), glm::vec3(0, 3, 40));
			GPED::convertFromCollisionPrimitive(ammo[i], broadAABB);
			ammo[i].proxyId = FirstBroadPhase.CreateProxy(broadAABB, &ammo[i]);
			++j;
			x += 0.6f;
		}
	}
}

void CGProj::TryFirst::reset()
{
	GPED::Random ranGen((unsigned)glfwGetTime());
	for (Box *box = boxData; box < boxData + boxes; box++)
	{
		float xRan = ranGen.randomReal(-10, 10);
		float yRan = ranGen.randomReal(3, 20);
		float zRan = ranGen.randomReal(0, 50);
		box->setState(xRan, yRan, zRan);
	}

	AmmoRound* shot;
	for (shot = ammo; shot < ammo + ammoRounds; ++shot)
	{
		if (shot->m_shotType != ShotType::SHOT_UNUSED)
			FirstBroadPhase.DestroyProxy(shot->proxyId);

		shot->m_shotType = ShotType::SHOT_UNUSED;
	}

}