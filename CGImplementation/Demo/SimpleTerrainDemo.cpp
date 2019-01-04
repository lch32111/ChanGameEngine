#include "SimpleTerrainDemo.h"

#include <Imgui/imgui.h>

#include <Graphics/GLPrimitiveUtil.h>

void CGProj::SimpleTerrainDemo::initGraphics(int width, int height)
{
	assetManager.assetInit();
	camera.Position = glm::vec3(0, 0, 0);
	myTerrain.initialize(true, assetManager);
	// myTerrain.initialize(false, assetManager);
	myTerrain.setProxyId(SecondBroadPhase.CreateProxy(myTerrain.getAABB(), myTerrain.getCollisioPrimitivePtr()));
	// Physics Init


	resolver = GPED::ContactResolver(256 * 8);

	// Init the boxes
	GPED::Random ranGen(20190102);
	GPED::c3AABB broadAABB;
	for (Box *box = boxData; box < boxData + boxes; box++)
	{
		float xRan = ranGen.randomReal(-10, 10);
		float yRan = ranGen.randomReal(15, 20);
		float zRan = ranGen.randomReal(-10, 10);
		box->setState(xRan, yRan, zRan);
		GPED::convertFromCollisionPrimitive(*box, broadAABB);
		box->proxyId = SecondBroadPhase.CreateProxy(broadAABB, box);
	}
	currentShotType = ShotType::SHOT_PISTOL;

	bRender.connectTree(SecondBroadPhase.getTree());
	bRender.setColor(glm::vec3(1, 0, 0), glm::vec3(1, 1, 0));
	bRender.setLineWidth(1.5f, 1.f);
	// Physics Init

	// Graphics Init
	wireShader = assetManager.getShader(SHADER_WIRE_RENDER);
	simpleShader = assetManager.getShader(SHADER_SIMPLE_RENDER);
	simpleShader->use();
	simpleShader->setInt("texture1", 0);

	woodTexture = assetManager.getTexture(TEXTURE_WOOD_PANEL, false);
	containerTexture = assetManager.getTexture(TEXTURE_CONTAINER_DIFFUSE, false);
	// Graphics Init
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
	ImGui::Checkbox("Wire Mode", &wireDraw);

	ImGui::TextColored(ImVec4(0.99, 0.4, 0.37, 1.0), "Press Tab Button to convert GAME/UI Mode");
	if (GameControl) ImGui::TextColored(ImVec4(0.78, 0.17, 0.54, 1.0), "GAME mode");
	else ImGui::TextColored(ImVec4(0.11, 0.7, 0.81, 1.0), "UI mode");

	if (ImGui::Button("Simulation Start / End")) start = start ? false : true; ImGui::SameLine();
	if (start) ImGui::TextColored(ImVec4(1.0, 0.89, 0, 1.0), "Simulating...");
	else ImGui::TextColored(ImVec4(1.0, 0, 0, 1.0), "Not simulating!");

	ImGui::Checkbox("Broad Debug Render", &BroadDebug);

	ImGui::End();
}

void CGProj::SimpleTerrainDemo::updateSimulation(float deltaTime, float lastFrame)
{
	if (start)
	{
		/*
			Simulatino Process
			1. Integration of the rigid bodies
			2. Sync with Broad Phase
			3. Broad Phase of Collision Detection
			4. Narrow Phase of Collision Detection
			5. Solve the contacts from Narrow Phase
		*/

		updateObjects(deltaTime, lastFrame);
		SyncAndUpdate(deltaTime); // sync between clientobject and rigid body in broadPhase
		broadPhase(); // literally broadphase.
		generateContacts(cManager); // narrow phase from broadphase
		resolver.resolveContacts(&cManager, deltaTime);
	}
}

void CGProj::SimpleTerrainDemo::display(int width, int height)
{
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 1000.f);
	glm::mat4 model;

	if(wireDraw) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	myTerrain.render(view, projection, camera.Position);

	simpleShader->use();
	simpleShader->setMat4("projection", projection);
	simpleShader->setMat4("view", view);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, containerTexture);
	for (AmmoRound* shot = ammo; shot < ammo + ammoRounds; ++shot)
	{
		if (shot->m_shotType != ShotType::SHOT_UNUSED)
		{
			model = glm::mat4(1.0);
			model = shot->body->getTransform();
			model = glm::scale(model, glm::vec3(shot->radius));
			simpleShader->setMat4("model", model);
			renderSphere();
		}
	}

	for (Box* box = boxData; box < boxData + boxes; ++box)
	{
		model = glm::mat4(1.0);
		model = box->body->getTransform();
		model = glm::scale(model, box->halfSize);
		simpleShader->setMat4("model", model);
		renderCube();
	}

	// Broad Phase Debug Rendering
	if (BroadDebug)
		bRender.draw(wireShader, &projection, &view);
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

void CGProj::SimpleTerrainDemo::updateObjects(float duration, float lastFrame)
{
	for (AmmoRound* shot = ammo; shot < ammo + ammoRounds; ++shot)
	{
		if (shot->m_shotType != ShotType::SHOT_UNUSED)
		{
			glm::vec3 pos = shot->body->getPosition();

			if (shot->startTime + 5 < lastFrame || pos.y < -5)
			{
				// We simply set the shot type to be unused, so the
				// memory it occupies can be reused by another shot.
				shot->m_shotType = ShotType::SHOT_UNUSED;

				SecondBroadPhase.DestroyProxy(shot->proxyId);
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

void CGProj::SimpleTerrainDemo::SyncAndUpdate(float duration)
{
	GPED::c3AABB broadAABB;
	glm::vec3 displacement;

	for (int i = 0; i < boxes; ++i)
	{
		GPED::convertFromCollisionPrimitive(boxData[i], broadAABB);
		displacement = boxData[i].body->getPosition() - boxData[i].body->getLastFramePosition();
		SecondBroadPhase.UpdateProxy(boxData[i].proxyId, broadAABB, displacement);
	}

	for (int i = 0; i < ammoRounds; ++i)
		if (ammo[i].m_shotType != ShotType::SHOT_UNUSED)
		{
			GPED::convertFromCollisionPrimitive(ammo[i], broadAABB);
			displacement = ammo[i].body->getPosition() - ammo[i].body->getLastFramePosition();
			SecondBroadPhase.UpdateProxy(ammo[i].proxyId, broadAABB, displacement);
		}
}

void CGProj::SimpleTerrainDemo::broadPhase()
{
	SecondResult.vPairs.clear();
	// Perform Tree Queries for all moving proxies
	// on this step, we gather the pairs and then 
	// pass the pairs to the generateContacts process for narrow phase
	SecondBroadPhase.UpdatePairs(&SecondResult);
}

void CGProj::SimpleTerrainDemo::generateContacts(CGContactManager & cData)
{
	// Manual Plane
	CGCollisionPlane planeGround;
	planeGround.direction = glm::vec3(0, 1, 0);
	planeGround.offset = -5;


	// Set up the collision data structure
	cData.reset();
	cData.friction = 0.9;
	cData.restitution = 0.1;
	cData.tolerance = 0.1;

	// we will generate contacts from the pairs detected by broadphase
	// In addition, we will generate contacts manually with planes
	const std::vector<std::pair<CGCollisionPrimitive*, CGCollisionPrimitive*>>& t_pair
		= SecondResult.vPairs;
	for (int i = 0; i < t_pair.size(); ++i)
	{
		cAlgo.findNarrowAlgorithmAndProcess(t_pair[i].first, t_pair[i].second, &cData);
	}

	for (int i = 0; i < boxes; ++i)
	{
		if (!boxData[i].body->getAwake()) continue;
		CGCollisionNarrow::OBBAndHalfSpace(boxData[i], planeGround, &cData);
	}
}

void CGProj::SimpleTerrainDemo::fire()
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
	ammo[shotIndex].proxyId = SecondBroadPhase.CreateProxy(broadAABB, &ammo[shotIndex]);
}