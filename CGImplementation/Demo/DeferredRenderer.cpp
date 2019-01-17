#include "DeferredRenderer.h"

#include <Imgui/imgui.h>

#include <CGErrorLogger.h>

#include <Graphics/GLTextureUtility.h>
#include <Graphics/GLPrimitiveUtil.h>
#include <Graphics/CGDefSecondUtil.h>

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
	Deferred_First_Shader->setInt("material.NormalMap", 3);
	Deferred_First_Shader->setInt("material.DepthMap", 4);

	Deferred_Second_Shader = assetManager.getShader(SHADER_DEFERRED_SECOND);
	Deferred_Second_Shader->use();
	Deferred_Second_Shader->setInt("gPosition", 0);
	Deferred_Second_Shader->setInt("gNormal", 1);
	Deferred_Second_Shader->setInt("gAlbedoSpec", 2);
	Deferred_Second_Shader->setInt("gEmissive", 3);
	Deferred_Second_Shader->setInt("gBool", 4);
	for (unsigned i = 0; i < NR_DIR_SHADOWS; ++i)
		Deferred_Second_Shader->setInt("dirShadowMap[" + std::to_string(i) + "]", NR_GBUFFER_TEXTURES + i);
	for (unsigned i = 0; i < NR_POINT_SHADOWS; ++i)
		Deferred_Second_Shader->setInt("pointShadowMap[" + std::to_string(i) + "]", NR_GBUFFER_TEXTURES + NR_DIR_SHADOWS + i);
	for (unsigned i = 0; i < NR_SPOT_SHADOWS; ++i)
		Deferred_Second_Shader->setInt("spotShadowMap[" + std::to_string(i) + "]", NR_GBUFFER_TEXTURES + NR_DIR_SHADOWS + NR_POINT_SHADOWS + i);

	// Shadow shader setting
	Shader* DirdepthMapShader = assetManager.getShader(SHADER_DIR_SHADOW_MAP);
	Shader* DirdepthMapDebugShader = assetManager.getShader(SHADER_DIR_SHADOW_MAP_DEBUG_RENDER);
	DirdepthMapDebugShader->use();
	DirdepthMapDebugShader->setInt("depthMap", 0);

	Shader* PointdepthMapDebugShader = assetManager.getShader(SHADER_POINT_SHADOW_MAP_DEBUG_RENDER);
	PointdepthMapDebugShader->use();
	PointdepthMapDebugShader->setInt("depthMap", 0);

	Shader* SpotdepthMapDebugShader = assetManager.getShader(SHADER_SPOT_SHADOW_MAP_DEBUG_RENDER);
	SpotdepthMapDebugShader->use();
	SpotdepthMapDebugShader->setInt("depthMap", 0);
	// Shadow shader setting

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
	glCheckError();
	// First Pass Setup For Deferred Rendering

	// Object Manual Setting + Light Manual Setting
	GPED::Random prandom(554);
	editProxies.reserve(20); // prevent STL from reallocating dynamically because of broad phase user data
	for (unsigned i = 0; i < 2; ++i)
	{
		editProxies.push_back(CGEditProxyObject(assetManager));
		editProxies[i].connectBroadPhase(&dBroadPhase);
		editProxies[i].setBroadPhaseId(dBroadPhase.CreateProxy(editProxies[i].getFitAABB(), &editProxies[i]));
		
		editProxies[i].setCMorLM(true);
		
		editProxies[i].setDiffuseFlag(true);
		editProxies[i].setDiffuseTexture(assetManager.getTexture(TEXTURE_CONTAINER_DIFFUSE, true));

		editProxies[i].setSpecularFlag(true);
		editProxies[i].setSpecularTexture(assetManager.getTexture(TEXTURE_CONTAINER_SPECULAR, true));

		editProxies[i].setEmissiveFlag(true);
		editProxies[i].setEmissiveTexture(assetManager.getTexture(TEXTURE_BLUE_MATRIX, true));

		editProxies[i].setPosition(prandom.randomVector(glm::vec3(-7, -5, 0), glm::vec3(-4, -4, 5)));
	}
	editProxies[0].setModelData(true);
	editProxies[0].setModel(assetManager.getModelData(MODEL_SPONZA_BUILDING));
	editProxies[0].setScale(0.01f);
	editProxies[1].setModelData(true);
	editProxies[1].setModel(assetManager.getModelData(MODEL_NANO_SUIT));
	editProxies[1].setScale(0.5f);


	GPED::Random random(331);
	editLights.reserve(400); // prevent the memory address of editLights from not changing because of dBroadPhase userProxy pointer!
	editLights.push_back(CGEditLightObject());
	editLights[0].initialize(assetManager);
	editLights[0].setLightType(EDIT_DIRECTION_LIGHT);
	editLights[0].setScale(0.3f);
	editLights[0].connectBroadPhase(&dBroadPhase);
	editLights[0].setBroadPhaseId(dBroadPhase.CreateProxy(editLights[0].getFitAABB(), &editLights[0]));
	editLights[0].setLightDirection(glm::vec3(-0.2f, -1.0f, -0.3f));
	editLights[0].setAmbientColor(glm::vec3(0.05f));
	editLights[0].setDiffuseColor(glm::vec3(0.4f));
	editLights[0].setSpecularColor(glm::vec3(0.5f));

	// Point Light
	for (unsigned i = 1; i < 5; ++i)
	{
		editLights.push_back(CGEditLightObject());
		editLights[i].initialize(assetManager);
		editLights[i].setLightType(EDIT_POINT_LIGHT);
		editLights[i].setScale(0.3f);
		editLights[i].connectBroadPhase(&dBroadPhase);
		editLights[i].setBroadPhaseId(dBroadPhase.CreateProxy(editLights[i].getFitAABB(), &editLights[i]));

		editLights[i].setPosition(random.randomVector(glm::vec3(-20, -5, -20), glm::vec3(20, 5, 20)));
		editLights[i].setAmbientColor(random.randomVector(glm::vec3(0, 0, 0) ,glm::vec3(1, 1, 1)));
		editLights[i].setDiffuseColor(random.randomVector(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
		editLights[i].setSpecularColor(random.randomVector(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
		editLights[i].setAttnLinear(0.7f);
		editLights[i].setAttnQuadratic(0.5);
	}
	// Object Manual Setting + Light Manual Setting

	// Debug Render Setting
	bRender.connectTree(dBroadPhase.getTree());
	bRender.setColor(glm::vec3(1, 0, 0), glm::vec3(1, 1, 0));
	bRender.setLineWidth(1.5f, 1.f);
	lineRen.setShader(assetManager.getShader(SHADER_CG_LINE));
	rayRen.setShader(assetManager.getShader(SHADER_CG_LINE));
	orinentLineRen.setShader(assetManager.getShader(SHADER_CG_LINE));
	// Debug Render Setting

	// Gizmo Setting
	gizmoTest.initGizmo(assetManager);
	gizmoTest.setAxisWidth(5.0);
	// Gizmo Setting
}

void CGProj::DeferredRenderer::initImgui()
{
}

void CGProj::DeferredRenderer::deinit()
{
	assetManager.destroy();
}

void CGProj::DeferredRenderer::updateImgui()
{
	GLFWwindow* app_window = (GLFWwindow*)ImGui::GetIO().ClipboardUserData;

	ImGui::Begin("Deferred Lighting Demo"); // Create a window called " " and append into it.

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Camera Position %.1f %.1f %.1f", camera.Position.x, camera.Position.y, camera.Position.z);

	ImGui::TextColored(ImVec4(0.99f, 0.4f, 0.37f, 1.0f), "Press Tab Button to convert GAME/UI Mode");
	if (GameControl) ImGui::TextColored(ImVec4(0.78f, 0.17f, 0.54f, 1.0f), "GAME mode");
	else ImGui::TextColored(ImVec4(0.11f, 0.7f, 0.81f, 1.0f), "UI mode");
	
	ImGui::Checkbox("Light Box Render", &lightDraw);
	ImGui::Checkbox("Broad Phase Debug Render", &BroadDebug);
	ImGui::Checkbox("Wire Mode", &wireDraw);
	ImGui::Checkbox("Click Ray Render", &clickDraw); ImGui::SameLine();
	if(ImGui::Button("Click Ray Reset")) rayCollector.clear();

	ImGui::Checkbox("Hit Ray Render", &rayHitDraw); ImGui::SameLine();
	if (ImGui::Button("Hit Ray Reset")) hitCollector.clear();
	
	if (ImGui::InputFloat("Shadow Bias", &shadowBias))
	{
		Deferred_Second_Shader->setFloat("shadowBias", shadowBias);
	}

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
	{
		// TODO : use the shadow light array in case of lots of lights
		for (unsigned i = 0; i < editLights.size(); ++i)
		{
			if (editLights[i].getIsShadowRender() == false)
				continue;

			editLights[i].renderShadowMap(editProxies);
		}
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

			editProxies[i].render(view, projection, camera.Position);
		}

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0, -5, 0));
		// model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1, 0, 0));
		model = glm::scale(model, glm::vec3(25));
		Deferred_First_Shader->setBool("material.CMorLM", true);
		Deferred_First_Shader->setBool("material.isLMdiffuse", true);
		Deferred_First_Shader->setBool("material.isLMspecular", false);
		Deferred_First_Shader->setBool("material.isLMemissive", false);
		Deferred_First_Shader->setBool("material.isNormalMap", false);
		Deferred_First_Shader->setBool("material.isDepthMap", false);
		Deferred_First_Shader->setMat4("projection", projection);
		Deferred_First_Shader->setMat4("view", view);
		Deferred_First_Shader->setMat4("model", model);
		Deferred_First_Shader->setMat3("ModelNormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
		Deferred_First_Shader->setBool("IsUseTangentSpace", false);
		Deferred_First_Shader->setVec3("cameraPos", camera.Position);
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
				editLights[i].setLightPropertyOnShader(num_dir_light++, num_dir_shadow,camera.Position);
				if (editLights[i].getIsShadowRender()) ++num_dir_shadow;
				break;
			}
			case EDIT_POINT_LIGHT:
				editLights[i].setLightPropertyOnShader(num_point_light++, num_point_shadow, camera.Position);
				if (editLights[i].getIsShadowRender()) ++num_point_shadow;
				break;
			case EDIT_SPOT_LIGHT:
				editLights[i].setLightPropertyOnShader(num_spot_light++, num_spot_shadow, camera.Position);
				if (editLights[i].getIsShadowRender()) ++num_spot_shadow;
				break;
			}
		}
		Deferred_Second_Shader->setInt("DIR_USED_NUM", num_dir_light);
		Deferred_Second_Shader->setInt("POINT_USED_NUM", num_point_light);
		Deferred_Second_Shader->setInt("SPOT_USED_NUM", num_spot_light);
		Deferred_Second_Shader->setFloat("shadowBias", shadowBias);
		num_dir_light = num_point_light = num_spot_light = 0;
		num_dir_shadow = num_point_shadow = num_spot_shadow = 0;

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

		// Broad Phase Debug Rendering
		if (BroadDebug)
			bRender.draw(assetManager.getShader(SHADER_WIRE_RENDER), &projection, &view);

		if (clickDraw)
		{
			for (unsigned i = 0; i < rayCollector.size(); ++i)
				lineRen.insertLine(rayCollector[i].first, rayCollector[i].second, glm::vec4(1.0, .0, .0, 1.));
			lineRen.renderLine(view, projection, 0.5f);
		}

		if (rayHitDraw)
		{
			for (unsigned i = 0; i < hitCollector.size(); ++i)
				rayRen.insertLine(hitCollector[i].first, hitCollector[i].second, glm::vec4(1.0, 1.0, 0.0, 1.0));
			rayRen.renderLine(view, projection, 1.8f);
		}

		if (lightDraw)
			for (unsigned int i = 0; i < editLights.size(); ++i)
				editLights[i].forwardRender(view, projection);

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

	camera.ProcessMouseScroll((float)yoffset);
}

void CGProj::DeferredRenderer::resize(int width, int height)
{
	if (width <= 0 || height <= 0) return; // Ignore window minimization case

	// First Pass Setup For Deferred Rendering
	glBindFramebuffer(GL_FRAMEBUFFER, gFBO);

	// Position Buffer
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

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