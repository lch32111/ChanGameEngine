#include "TryFirst.h"

#include <vector>

#include <Imgui/imgui.h>
#include <GPED/GPED_random.h>
#include <Graphics/GLTextureUtility.h>


void CGProj::TryFirst::initGraphics()
{
	simpleShader = Shader("ShaderFolder/simpleRender.vs", "ShaderFolder/simpleRender.fs");
	simpleShader.loadShader();
	simpleShader.use();
	simpleShader.setInt("texture1", 0);

	wireShader = Shader("ShaderFolder/wireRender.vs", "ShaderFolder/wireRender.fs");
	wireShader.loadShader();

	woodTexture = TextureFromFile("ImageFolder/fieldGrass.jpg", false);
	containerTexture = TextureFromFile("ImageFolder/container2.png", false);

	// BigBallistic Demo
	cData.contacts = cData.contactArray = contacts;
	resolver = GPED::ContactResolver(maxContacts * 8);

	// Initialise the box
	GPED::real z = 20.0f;
	GPED::Random ranGen(334455);
	for (Box *box = boxData; box < boxData + boxes; box++)
	{
		float xRan = ranGen.randomReal(-10, 10);
		float yRan = ranGen.randomReal(3, 20);
		float zRan = ranGen.randomReal(0, 50);
		box->setState(xRan, yRan, zRan);
		box->proxyId = FirstBroadPhase.CreateProxy(GPED::convertFromCollisionPrimitive(*box), box);
	}
	currentShotType = ShotType::SHOT_PISTOL;

	bRender.connectTree(FirstBroadPhase.getTree());
	bRender.setColor(glm::vec3(1, 0, 0), glm::vec3(1, 1, 0));
	bRender.setLineWidth(1.5f, 1.f);
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
	ImGui::TextColored(ImVec4(0.99, 0.4, 0.37, 1.0), "Press Tab Button to convert GAME/UI Mode");
	if (GameControl) ImGui::TextColored(ImVec4(0.78, 0.17, 0.54, 1.0), "GAME mode");
	else ImGui::TextColored(ImVec4(0.11, 0.7, 0.81, 1.0), "UI mode");

	if (ImGui::Button("Simulation Start / End")) start = start ? false : true; ImGui::SameLine();
	
	if (start) ImGui::TextColored(ImVec4(1.0, 0.89, 0, 1.0), "Simulating...");
	else ImGui::TextColored(ImVec4(1.0, 0, 0, 1.0), "Not simulating!");

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
		generateContacts(cData); // narrow phase from broadphase
		resolver.resolveContacts(cData.contactArray, cData.contactCount, deltaTime);
	}
}

void CGProj::TryFirst::display(int width, int height)
{
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
	bRender.draw(&wireShader, &projection, &view);
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
			lastX = xpos;
			lastY = ypos;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	
}

void CGProj::TryFirst::scroll(double yoffset)
{
	if (GameControl)
	{
		camera.ProcessMouseScroll(yoffset);
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
	for (int i = 0; i < boxes; ++i)
		if(boxData[i].body->getAwake())
			FirstBroadPhase.UpdateProxy(boxData[i].proxyId, GPED::convertFromCollisionPrimitive(boxData[i]));

	for (int i = 0; i < ammoRounds; ++i)
		if (ammo[i].m_shotType != ShotType::SHOT_UNUSED)
			FirstBroadPhase.UpdateProxy(ammo[i].proxyId, GPED::convertFromCollisionPrimitive(ammo[i]));
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
void CGProj::TryFirst::generateContacts(GPED::CollisionData & cData)
{
	GPED::CollisionPlane planeGround;
	planeGround.direction = glm::vec3(0, 1, 0);
	planeGround.offset = 0;

	GPED::CollisionPlane planeZWall;
	planeZWall.direction = glm::vec3(0, 0, -1);
	planeZWall.offset = -50;

	// Set up the collision data structure
	cData.reset(maxContacts);
	cData.friction = contactFriction;
	cData.restitution = contactRestitution;
	cData.tolerance = contactRestitution;

	// we will generate contacts from the pairs detected by broadphase
	// In addition, we will generate contacts manually with planes
	const std::vector<std::pair<GPED::CollisionPrimitive*, GPED::CollisionPrimitive*>>& t_pair
		= firstResult.vPairs;
	// std::cout << t_pair.size() << '\n';
	for (int i = 0; i < t_pair.size(); ++i)
	{
		GPED::CollisionDetector::collision(t_pair[i].first, t_pair[i].second, &cData);
	}

	for (int i = 0; i < boxes; ++i)
	{
		if (!boxData[i].body->getAwake()) continue;
		GPED::CollisionDetector::boxAndHalfSpace(boxData[i], planeGround, &cData);
		GPED::CollisionDetector::boxAndHalfSpace(boxData[i], planeZWall, &cData);
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
	ammo[shotIndex].setState(currentShotType, camera);
	ammo[shotIndex].proxyId = 
		FirstBroadPhase.CreateProxy(GPED::convertFromCollisionPrimitive(ammo[shotIndex]), &ammo[shotIndex]);
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

	GPED::Random ranGen(glfwGetTime());
	float x;
	float y;
	float z = 0;

	for(int i = index, j = 0; i < ammoRounds && j < 10; ++i)
	{
		x = ranGen.randomBinomial(50);
		y = ranGen.randomReal(0, 5);
		if (ammo[i].m_shotType == ShotType::SHOT_UNUSED)
		{
			ammo[i].setState(ShotType::SHOT_ARTILLERY, glm::vec3(x, y, z), glm::vec3(0, 3, 40));
			ammo[i].proxyId = FirstBroadPhase.CreateProxy(GPED::convertFromCollisionPrimitive(ammo[i]), &ammo[i]);
			++j;
			x += 0.6;
		}
	}
}

void CGProj::TryFirst::reset()
{
	GPED::Random ranGen(glfwGetTime());
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