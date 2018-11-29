#include "DeferredRenderer.h"

#include <Imgui/imgui.h>

#include <Graphics/GLTextureUtility.h>
#include <Graphics/GLPrimitiveUtil.h>

#include <GPED/CGPhysicsUtil.h>
#include <GPED/GPED_random.h>

void CGProj::DeferredRenderer::initGraphics(int width, int height)
{
	assetManager.assetInit();

	// Shader Setup
	Deferred_First_Shader = assetManager.getShader(SHADER_DEFERRED_FIRST);
	Deferred_First_Shader->use();
	Deferred_First_Shader->setInt("material.LMdiffuse", 0);
	Deferred_First_Shader->setInt("material.LMspecular", 1);
	Deferred_First_Shader->setInt("material.LMemissive", 2);

	Deferred_Second_Shader = assetManager.getShader(SHADER_DEFERRED_SECOND);
	Deferred_Second_Shader->use();
	Deferred_Second_Shader->setInt("gPosition", 0);
	Deferred_Second_Shader->setInt("gNormal", 1);
	Deferred_Second_Shader->setInt("gAlbedoSpec", 2);
	Deferred_Second_Shader->setInt("gEmissive", 3);
	Deferred_Second_Shader->setInt("gBool", 4);

	Simple_Shader = assetManager.getShader(SHADER_SIMPLE_COLOR_RENDER);
	wireShader = assetManager.getShader(SHADER_WIRE_RENDER);
	// Shader Setup

	// First Pass Setup For Deferred Rendering
	glGenFramebuffers(1, &gFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, gFBO);

	// Position Buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// Normal Buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// Albedo + specular Buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// Emissive Buffer
	glGenTextures(1, &gEmissive);
	glBindTexture(GL_TEXTURE_2D, gEmissive);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gEmissive, 0);

	// Boolean Buffer
	glGenTextures(1, &gBool);
	glBindTexture(GL_TEXTURE_2D, gBool);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gBool, 0);

	// Tell OpenGL which color attachments we'll use (of this framebuffeer) for rendering
	unsigned attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, 
		GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, attachments);

	// attach the depth component with renderbuffer
	glGenRenderbuffers(1, &gRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, gRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gRBO);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		assert(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// First Pass Setup For Deferred Rendering

	// Object Manual Setting + Light Manual Setting

	editProxies.reserve(20); // prevent STL from reallocating dynamically because of broad phase user data
	for (unsigned i = 0; i < 15; ++i)
	{
		editProxies.push_back(CGEditProxyObject());
		editProxies[i].connectBroadPhase(&dBroadPhase);
		editProxies[i].setBroadPhaseId(dBroadPhase.CreateProxy(editProxies[i].getFitAABB(), &editProxies[i]));
		editProxies[i].setFirstPassDefShader(Deferred_First_Shader);
		
		editProxies[i].setCMorLM(true);
		
		editProxies[i].setDiffuseFlag(true);
		editProxies[i].setDiffuseTexture(assetManager.getTexture(TEXTURE_CONTAINER_DIFFUSE, true));

		editProxies[i].setSpecularFlag(true);
		editProxies[i].setSpecularTexture(assetManager.getTexture(TEXTURE_CONTAINER_SPECULAR, true));

		editProxies[i].setEmissiveFlag(true);
		editProxies[i].setEmissiveTexture(assetManager.getTexture(TEXTURE_BLUE_MATRIX, true));
	}


	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	for (unsigned int i = 0; i < NR_LIGHTS; ++i)
	{
		float lightMax = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
		float radius =
			(-linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax)))
			/ (2 * quadratic);
		lightRadius.push_back(radius);
	}

	// Debug Render Setting
	bRender.connectTree(dBroadPhase.getTree());
	bRender.setColor(glm::vec3(1, 0, 0), glm::vec3(1, 1, 0));
	bRender.setLineWidth(1.5f, 1.f);
	lineRen = CGRenderLine("ShaderFolder/CGLineShader.vs", "ShaderFolder/CGLineShader.fs");
	rayRen = CGRenderLine("ShaderFolder/CGLineShader.vs", "ShaderFolder/CGLineShader.fs");
	orinentLineRen = CGRenderLine("ShaderFolder/CGLineShader.vs", "ShaderFolder/CGLineShader.fs");
	// Debug Render Setting

	// Gizmo Setting
	gizmoTest.initGizmo();
	gizmoTest.setAxisWidth(5.0);
	// Gizmo Setting
}

void CGProj::DeferredRenderer::initImgui()
{
}

void CGProj::DeferredRenderer::deinit()
{
}

void CGProj::DeferredRenderer::updateImgui()
{
	GLFWwindow* app_window = (GLFWwindow*)ImGui::GetIO().ClipboardUserData;

	ImGui::Begin("Deferred Lighting Demo"); // Create a window called " " and append into it.

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Camera Position %.1f %.1f %.1f", camera.Position.x, camera.Position.y, camera.Position.z);

	ImGui::TextColored(ImVec4(0.99, 0.4, 0.37, 1.0), "Press Tab Button to convert GAME/UI Mode");
	if (GameControl) ImGui::TextColored(ImVec4(0.78, 0.17, 0.54, 1.0), "GAME mode");
	else ImGui::TextColored(ImVec4(0.11, 0.7, 0.81, 1.0), "UI mode");
	
	ImGui::Checkbox("Light Box Render", &lightDraw);
	ImGui::Checkbox("Broad Phase Debug Render", &BroadDebug);
	ImGui::Checkbox("Wire Mode", &wireDraw);
	ImGui::Checkbox("Click Ray Render", &clickDraw); ImGui::SameLine();
	if(ImGui::Button("Click Ray Reset")) rayCollector.clear();

	ImGui::Checkbox("Hit Ray Render", &rayHitDraw); ImGui::SameLine();
	if (ImGui::Button("Hit Ray Reset")) hitCollector.clear();
	
	ImGui::End();

	if (pickedEditBox)
	{
		pickedEditBox->UIrender(assetManager);
	}
}

void CGProj::DeferredRenderer::updateSimulation(float deltaTime, float lastFrame)
{

}

void CGProj::DeferredRenderer::display(int width, int height)
{
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 1000.f);
	glm::mat4 model(1.0);

	// First Pass
	glBindFramebuffer(GL_FRAMEBUFFER, gFBO);
	if (wireDraw) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (unsigned i = 0; i < editProxies.size(); ++i)
	{
		editProxies[i].render(view, projection);
	}

	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0, -5, 0));
	model = glm::scale(model, glm::vec3(10));
	Deferred_First_Shader->setBool("material.CMorLM", true);
	Deferred_First_Shader->setBool("material.isLMdiffuse", true);
	Deferred_First_Shader->setBool("material.isLMspecular", false);
	Deferred_First_Shader->setBool("material.isLMemissive", false);
	Deferred_First_Shader->setMat4("viewModel", view * model);
	Deferred_First_Shader->setMat3("MVNormalMatrix", glm::mat3(glm::transpose(glm::inverse(view * model))));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, assetManager.getTexture(TEXTURE_WOOD_PANEL, true));
	renderQuad();
	// First Pass

	// Second Pass + Post Process
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	if(wireDraw)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // This quad always fills the screen plane!

	Deferred_Second_Shader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gEmissive);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gBool);
	for (unsigned int i = 0; i < lightPositions.size(); ++i)
	{
		Deferred_Second_Shader->setVec3("lights[" + std::to_string(i) + "].Position", glm::vec3(view * glm::vec4(lightPositions[i], 1.0)));
		Deferred_Second_Shader->setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
		Deferred_Second_Shader->setFloat("lights[" + std::to_string(i) + "].Radius", lightRadius[i]);
		Deferred_Second_Shader->setFloat("lights[" + std::to_string(i) + "].Linear", linear);
		Deferred_Second_Shader->setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
	}
	renderScreenQuad();
	// Second Pass + Post Process

	// Debug Drawing like forward processing
	if (wireDraw) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, gFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	if (lightDraw)
	{
		Simple_Shader->use();
		Simple_Shader->setMat4("view", view);
		Simple_Shader->setMat4("projection", projection);
		for (unsigned int i = 0; i < lightPositions.size(); ++i)
		{
			glm::mat4 t_model(1.0);
			t_model = glm::translate(t_model, lightPositions[i]);
			t_model = glm::scale(t_model, glm::vec3(0.25f));
			Simple_Shader->setMat4("model", t_model);
			Simple_Shader->setVec3("Color", lightColors[i]);
			renderCube();
		}
	}

	// Broad Phase Debug Rendering
	if (BroadDebug)
		bRender.draw(wireShader, &projection, &view);

	if (clickDraw)
	{
		for (unsigned i = 0; i < rayCollector.size(); ++i)
			lineRen.insertLine(rayCollector[i].first, rayCollector[i].second, glm::vec4(1.0, .0, .0, 1.));
		lineRen.renderLine(view, projection, 0.5);
	}
	
	if (rayHitDraw)
	{
		for (unsigned i = 0; i < hitCollector.size(); ++i)
			rayRen.insertLine(hitCollector[i].first, hitCollector[i].second, glm::vec4(1.0, 1.0, 0.0, 1.0));
		rayRen.renderLine(view, projection, 1.8);
	}

	// Picked One
	if (pickedEditBox)
	{
		gizmoTest.setAxisLength(pickedEditBox->getScale().x + 1.0f);
		gizmoTest.setEditProxyObject(pickedEditBox);
		gizmoTest.renderGizmo(view, projection);
		gizmoTest.renderGizmoBox(view, projection);
	}
}

void CGProj::DeferredRenderer::key(GLFWwindow * app_window, float deltaTime)
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

void CGProj::DeferredRenderer::mouse(double xpos, double ypos)
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
	else
	{
		float xoffset = xpos - UILastX;
		float yoffset = UILastY - ypos;
		UILastX = xpos;
		UILastY = ypos;

		if (mouseClick) // Mouse Holding and Moving
		{
			if (gizmoTest.isActivated() && gizmoTest.isHitActivated())
			{
				gizmoTest.translate(xoffset, yoffset, camera);
			}
		}
	}
}

void CGProj::DeferredRenderer::mouseButton(GLFWwindow * app_window, 
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
			
			glm::vec3 rayFrom = camera.Position;
			glm::vec3 rayTo = GetRayTo((int)x, (int)y, &camera, screen_width, screen_height);

			// Place the line a little ahead of camera to 
			// enable a user to watch the line right after making a line.
			// Click Ray Collector
			// the line position is not correct position for the purpose above
			// But don't worry for this, the collision between ray and object is working well.
			rayCollector.push_back({ rayFrom - camera.Front, rayTo }); 

			// Make Ray
			GPED::c3RayInput rayInput(rayFrom, rayTo);

			// Manual Gizmo Intersection
			if (gizmoTest.isActivated())
				if (gizmoTest.rayOverlapBoxes(rayInput))
				{
					// Do not Find other objects since user 
					// clicks the gizmo Box
					return;
				}
			
			// Find Proxy with ray Input
			BroadClosestRayCast raycastWrapper;
			raycastWrapper.broadPhase = &dBroadPhase;
			dBroadPhase.RayCast(&raycastWrapper, rayInput);
			
			if (raycastWrapper.userData != nullptr)
			{
				hitCollector.push_back({ 
					raycastWrapper.rayOutput.startPoint - camera.Front,
					raycastWrapper.rayOutput.hitPoint });

				pickedEditBox = (CGEditProxyObject*)raycastWrapper.userData;
			}
			else
			{
				pickedEditBox = nullptr;
				gizmoTest.setEditProxyObject(nullptr);
			}
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		mouseClick = false;
	}
}

void CGProj::DeferredRenderer::scroll(double yoffset)
{
	// Exit out if mouse clicks on Imgui GUI
	if (ImGui::IsMouseHoveringAnyWindow()) return;

	camera.ProcessMouseScroll(yoffset);
}

void CGProj::DeferredRenderer::resize(int width, int height)
{
	if (width <= 0 || height <= 0) return; // Ignore window minimization case

	// First Pass Setup For Deferred Rendering
	glBindFramebuffer(GL_FRAMEBUFFER, gFBO);

	// Position Buffer
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

	// Normal Buffer
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

	// Albedo + specular Buffer
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// Emissive Buffer
	glBindTexture(GL_TEXTURE_2D, gEmissive);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// Boolean Buffer
	glBindTexture(GL_TEXTURE_2D, gBool);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	// attach the depth component with renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, gRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		assert(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}