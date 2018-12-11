#include "DeferredRenderer.h"

#include <Imgui/imgui.h>

#include <Graphics/GLTextureUtility.h>
#include <Graphics/GLPrimitiveUtil.h>

#include <GPED/CGPhysicsUtil.h>
#include <GPED/GPED_random.h>

void CGProj::DeferredRenderer::initGraphics(int width, int height)
{
	assetManager.assetInit();
	camera.Position = glm::vec3(10, 10, 10);
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
	for (unsigned i = 0; i < 5; ++i)
		Deferred_Second_Shader->setInt("dirShadowMap[" + std::to_string(i) + "]", 5 + i);

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	
	// Emissive Buffer
	glGenTextures(1, &gEmissive);
	glBindTexture(GL_TEXTURE_2D, gEmissive);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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
	for (unsigned i = 0; i < 2; ++i)
	{
		editProxies.push_back(CGEditProxyObject());
		editProxies[i].connectBroadPhase(&dBroadPhase);
		editProxies[i].setBroadPhaseId(dBroadPhase.CreateProxy(editProxies[i].getFitAABB(), &editProxies[i]));
		editProxies[i].setDefShader(Deferred_First_Shader);
		
		editProxies[i].setCMorLM(true);
		
		editProxies[i].setDiffuseFlag(true);
		editProxies[i].setDiffuseTexture(assetManager.getTexture(TEXTURE_CONTAINER_DIFFUSE, true));

		editProxies[i].setSpecularFlag(true);
		editProxies[i].setSpecularTexture(assetManager.getTexture(TEXTURE_CONTAINER_SPECULAR, true));

		editProxies[i].setEmissiveFlag(true);
		editProxies[i].setEmissiveTexture(assetManager.getTexture(TEXTURE_BLUE_MATRIX, true));
	}

	GPED::Random random(331);
	editLights.reserve(400);
	editLights.push_back(CGEditLightObject(assetManager));
	editLights[0].setLightType(EDIT_DIRECTION_LIGHT);
	editLights[0].setScale(0.3);
	editLights[0].connectBroadPhase(&dBroadPhase);
	editLights[0].setBroadPhaseId(dBroadPhase.CreateProxy(editLights[0].getFitAABB(), &editLights[0]));
	editLights[0].setDefShader(Deferred_Second_Shader);
	editLights[0].setForwardShader(Simple_Shader);
	editLights[0].setLightDirection(glm::vec3(-0.2f, -1.0f, -0.3f));
	editLights[0].setAmbientColor(glm::vec3(0.05));
	editLights[0].setDiffuseColor(glm::vec3(0.4));
	editLights[0].setSpecularColor(glm::vec3(0.5));

	// Point Light
	for (unsigned i = 1; i < 11; ++i)
	{
		editLights.push_back(CGEditLightObject(assetManager));
		editLights[i].setObjectType(EDIT_OBJECT_LIGHT);
		editLights[i].setScale(0.3);
		editLights[i].connectBroadPhase(&dBroadPhase);
		editLights[i].setBroadPhaseId(dBroadPhase.CreateProxy(editLights[i].getFitAABB(), &editLights[i]));
		editLights[i].setDefShader(Deferred_Second_Shader);
		editLights[i].setForwardShader(Simple_Shader);

		editLights[i].setPosition(random.randomVector(glm::vec3(-20, -5, -20), glm::vec3(20, 5, 20)));
		editLights[i].setAmbientColor(random.randomVector(glm::vec3(0, 0, 0) ,glm::vec3(1, 1, 1)));
		editLights[i].setDiffuseColor(random.randomVector(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
		editLights[i].setSpecularColor(random.randomVector(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
		editLights[i].setAttnLinear(0.7);
		editLights[i].setAttnQuadratic(0.5);
	}
	// Object Manual Setting + Light Manual Setting


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

	// Shadow Mapping Test
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		assert(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float near_plane = 1.0f, far_plane = 7.5f;
	lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);
	
	
	depthMapShader = Shader("ShaderFolder/CGDepthMap.vs", "ShaderFolder/CGDepthMap.fs");
	depthMapShader.loadShader();
	// Shadow Mapping Test
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

	// Shadow Mapping Pass
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	{
		lightView = glm::lookAt(
			editLights[0].getPosition() - editLights[0].getLightDirection() * 5.f,
			editLights[0].getPosition() + editLights[0].getLightDirection(),
			glm::vec3(0, 1, 0));
		lightSpaceMatrix = lightProjection * lightView;


		glClear(GL_DEPTH_BUFFER_BIT);
		depthMapShader.use();
		depthMapShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glCullFace(GL_FRONT);
		for (unsigned i = 0; i < editProxies.size(); ++i)
		{
			glm::mat4 model(1.0);
			model = glm::translate(model, editProxies[i].getPosition());
			model = glm::scale(model, editProxies[i].getScale());
			depthMapShader.setMat4("model", model);
			renderCube();
		}
		glCullFace(GL_BACK);
	}
	// Shadow Mapping Pass

	// First Pass
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, gFBO);
	{
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
		model = glm::scale(model, glm::vec3(25));
		Deferred_First_Shader->setBool("material.CMorLM", true);
		Deferred_First_Shader->setBool("material.isLMdiffuse", true);
		Deferred_First_Shader->setBool("material.isLMspecular", false);
		Deferred_First_Shader->setBool("material.isLMemissive", false);
		Deferred_First_Shader->setMat4("projection", projection);
		Deferred_First_Shader->setMat4("view", view);
		Deferred_First_Shader->setMat4("model", model);
		Deferred_First_Shader->setMat3("ModelNormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, assetManager.getTexture(TEXTURE_WOOD_PANEL, true));
		renderQuad();
	}
	// First Pass

	// Second Pass + Post Process
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	{
		if (wireDraw)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // This quad always fills the screen plane!

		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		for (unsigned i = 0; i < editLights.size(); ++i)
		{
			EditLightType type = editLights[i].getLightType();

			switch (type)
			{
			case EDIT_DIRECTION_LIGHT:
			{
				editLights[i].setLightPropertyOnShader(num_dir_light++, camera.Position);

				Deferred_Second_Shader->use();
				Deferred_Second_Shader->setMat4("dirLightSpace[" + std::to_string(0) + "]", lightSpaceMatrix);
				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, depthMap);
				break;
			}
			case EDIT_POINT_LIGHT:
				editLights[i].setLightPropertyOnShader(num_point_light++, camera.Position);
				break;
			case EDIT_SPOT_LIGHT:
				editLights[i].setLightPropertyOnShader(num_spot_light++, camera.Position);
				break;
			}
		}
		Deferred_Second_Shader->setInt("DIR_USED_NUM", num_dir_light);
		Deferred_Second_Shader->setInt("POINT_USED_NUM", num_point_light);
		Deferred_Second_Shader->setInt("SPOT_USED_NUM", num_spot_light);
		num_dir_light = 0;
		num_point_light = 0;
		num_spot_light = 0;

		renderScreenQuad();
	}
	// Second Pass + Post Process

	// Debug Drawing and UI Render like forward processing
	{
		if (wireDraw) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, gFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (lightDraw)
			for (unsigned int i = 0; i < editLights.size(); ++i)
				editLights[i].forwardRender(view, projection);

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
		if (pickedEditBox != nullptr)
		{
			gizmoTest.setAxisLength(pickedEditBox->getScale().x + 1.0f);
			gizmoTest.setEditObject(pickedEditBox);
			gizmoTest.renderGizmo(view, projection);
			gizmoTest.renderGizmoBox(view, projection);
		}
	}
	// Debug Drawing and UI Render like forward processing
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

				pickedEditBox = (CGEditObject*)raycastWrapper.userData;
			}
			else
			{
				pickedEditBox = nullptr;
				gizmoTest.setEditObject(nullptr);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// Emissive Buffer
	glBindTexture(GL_TEXTURE_2D, gEmissive);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

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