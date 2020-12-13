#include <CGPrecompiled.h>
#include "DeferredRenderer.h"

#include <Imgui/imgui.h>

#include <CGErrorLogger.h>

#include <Graphics/GLTextureUtility.h>
#include <Graphics/GLPrimitiveUtil.h>
#include <Graphics/CGDefSecondUtil.h>
#include <Graphics/GLInstancePrimitiveUtil.h>

#include <GPED/CGPhysicsUtil.h>
#include <GPED/GPED_random.h>

/****************************************************************************************/
/* ### Graphics Demo ### */
void CGProj::GraphicsDemo::OnInitialize()
{
	glfwSwapInterval(0); // Turn off Vsync and measure the FPS
	glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	test2 = new DeferredRenderer();
	test2->initGraphics(m_width, m_height);
	test2->initImgui();
}

void CGProj::GraphicsDemo::OnFinalize()
{
	test2->deinit();
	delete test2;
}

void CGProj::GraphicsDemo::Update(float deltaTime, float lastFrame)
{
	test2->key(app_window, deltaTime);
	test2->updateImgui();
	test2->updateSimulation(deltaTime, lastFrame);
}

void CGProj::GraphicsDemo::Display()
{
	test2->display(m_width, m_height);
}

void CGProj::GraphicsDemo::MouseMoveCallback(double xpos, double ypos)
{
	test2->mouse(xpos, ypos);
}

void CGProj::GraphicsDemo::MouseButtonCallback(int button, int action, int mods)
{
	// Application::mouseButton(button, action, mods);

	test2->mouseButton(app_window, button, action, mods, m_width, m_height);
}

void CGProj::GraphicsDemo::ScrollCallback(double yoffset)
{
	test2->scroll(yoffset);
}

void CGProj::GraphicsDemo::ResizeWindowCallback(int width, int height)
{
	Application::m_width = width;
	Application::m_height = height;
	glViewport(0, 0, width, height);

	test2->resize(width, height);
}

/* ### Graphics Demo ### */
/****************************************************************************************/

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
	Deferred_Second_Shader->setInt("ssaoTexture", 5);
	for (unsigned i = 0; i < NR_DIR_SHADOWS; ++i)
		Deferred_Second_Shader->setInt("dirShadowMap[" + std::to_string(i) + "]", NR_GBUFFER_TEXTURES + i);
	for (unsigned i = 0; i < NR_POINT_SHADOWS; ++i)
		Deferred_Second_Shader->setInt("pointShadowMap[" + std::to_string(i) + "]", NR_GBUFFER_TEXTURES + NR_DIR_SHADOWS + i);
	for (unsigned i = 0; i < NR_SPOT_SHADOWS; ++i)
		Deferred_Second_Shader->setInt("spotShadowMap[" + std::to_string(i) + "]", NR_GBUFFER_TEXTURES + NR_DIR_SHADOWS + NR_POINT_SHADOWS + i);

	Deferred_Post_Shader = assetManager.getShader(SHADER_DEFERRED_POST);
	Deferred_Post_Shader->use();
	Deferred_Post_Shader->setInt("currentScene", 0);
	Deferred_Post_Shader->setInt("bloomedScene", 1);

	Deferred_STD140.initialize("Matrices");
	Deferred_STD140.addShader(Deferred_First_Shader);
	
	pGamma = 2.2f; Deferred_Post_Shader->setFloat("gamma", pGamma);
	pExposure = 1.0; Deferred_Post_Shader->setFloat("exposure", pExposure);
	
	isBloom = false;
	Shader* tempBlurInit = assetManager.getShader(SHADER_GAUSSIAN_BLUR);
	tempBlurInit->use();
	tempBlurInit->setInt("image", 0);
	myBloom.Initialize(assetManager, width, height, 10);
	myBloom.m_BrightnessExtractor = glm::vec3(0.2126, 0.7152, 0.0722);
	myBloom.m_BrightnessThreShold = 1.f;

	isSSAO = false;
	isSSAODebug = false;
	Shader* tempSSAOInit = assetManager.getShader(SHADER_SSAO_EFFECT);
	tempSSAOInit->use();
	tempSSAOInit->setInt("gPosition", 0);
	tempSSAOInit->setInt("gNormal", 1);
	tempSSAOInit->setInt("texNoise", 2);
	tempSSAOInit = assetManager.getShader(SHADER_SSAO_BLUR);
	tempSSAOInit->use();
	tempSSAOInit->setInt("ssaoInput", 0);
	SSAODebugShader = assetManager.getShader(SHADER_SSAO_DEBUG);
	SSAODebugShader->use();
	SSAODebugShader->setInt("SSAO", 0);
	mySSAO.Initialize(assetManager, 123456, 16, 0.5f, 0.025f, 1.0f, width, height);

	// Shadow shader setting
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
		CGassert();
	glCheckError();
	// First Pass Setup For Deferred Rendering

	// Second Pass Setup For Deferred Rendering
	glGenFramebuffers(1, &dSecondFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, dSecondFBO);
	
	glGenTextures(1, &dSecondHDRColorBuffer);
	glBindTexture(GL_TEXTURE_2D, dSecondHDRColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dSecondHDRColorBuffer, 0);

	glGenTextures(1, &dSecondHDRBloomBuffer);
	glBindTexture(GL_TEXTURE_2D, dSecondHDRBloomBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, dSecondHDRBloomBuffer, 0);

	unsigned SecondAttachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, SecondAttachments);

	/*
	glGenRenderbuffers(1, &dSecondHDRDepthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, dSecondHDRDepthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dSecondHDRDepthRBO);
	*/
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		CGassert();
	glCheckError();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Second Pass Setup For Deferred Rendering

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
	editProxies[0].setModel(assetManager.getModelData(MODEL_OLD_HOUSE, 1));
	editProxies[0].setScale(1.f);
	editProxies[1].setModelData(false);
	// editProxies[1].setModel(assetManager.getModelData(MODEL_NANO_SUIT, 100));
	editProxies[1].setScale(0.5f);

	srand((unsigned)glfwGetTime()); // initialize random seed
	float radius = 10.0f;
	float offset = 2.5f;

	int modelNumb = 100;
	modelMatrices.reserve(modelNumb);
	worldNormalMatrices.reserve(modelNumb);
	int leftZ = 5, rightZ = -9;
	int downX = 60, UpX = -70;
	float step = (float)(-UpX + downX) / (modelNumb / 2) + 1.5f;
	for (int i = 0; i < modelNumb / 2; ++i)
	{
		glm::mat4 model(1.0);
		model = glm::translate(model, glm::vec3(downX - step * i , 0, leftZ));
		model = glm::rotate(model, glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));


		modelMatrices.push_back(model);
		worldNormalMatrices.push_back(glm::transpose(glm::inverse(model)));
	}

	for (int i = 0; i < modelNumb / 2; ++i)
	{
		glm::mat4 model(1.0);
		model = glm::translate(model, glm::vec3(downX - step * i, 0, rightZ));
		// model = glm::rotate(model, glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));

		modelMatrices.push_back(model);
		worldNormalMatrices.push_back(glm::transpose(glm::inverse(model)));
	}


	GPED::Random random(331);
	editLights.reserve(400); // prevent the memory address of editLights from not changing because of dBroadPhase userProxy pointer!
	editLights.push_back(CGEditLightObject());
	editLights[0].initialize(assetManager);
	editLights[0].setLightType(EDIT_DIRECTION_LIGHT);
	editLights[0].setPosition(0.f, 95.f, 0.f);
	editLights[0].setScale(0.3f);
	editLights[0].connectBroadPhase(&dBroadPhase);
	editLights[0].setBroadPhaseId(dBroadPhase.CreateProxy(editLights[0].getFitAABB(), &editLights[0]));
	editLights[0].setLightDirection(glm::vec3(-0.2f, -1.0f, -0.3f));
	editLights[0].setAmbientColor(glm::vec3(0.05f));
	editLights[0].setDiffuseColor(glm::vec3(0.4f));
	editLights[0].setSpecularColor(glm::vec3(0.5f));

	// Point Light
	for (unsigned i = 1; i < 50; ++i)
	{
		editLights.push_back(CGEditLightObject());
		editLights[i].initialize(assetManager);
		editLights[i].setLightType(EDIT_POINT_LIGHT);
		editLights[i].setScale(0.3f);
		editLights[i].connectBroadPhase(&dBroadPhase);
		editLights[i].setBroadPhaseId(dBroadPhase.CreateProxy(editLights[i].getFitAABB(), &editLights[i]));
		
		glm::vec3 pos = random.randomVector(glm::vec3(60, 0, 5), glm::vec3(-70, 10, -9));
		editLights[i].setPosition(pos);
		editLights[i].setAmbientColor(random.randomVector(glm::vec3(0, 0, 0) ,glm::vec3(1, 1, 1)));
		editLights[i].setDiffuseColor(random.randomVector(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
		editLights[i].setSpecularColor(random.randomVector(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
		editLights[i].setAttnLinear(0.7f);
		editLights[i].setAttnQuadratic(0.5);
		editLights[i].setLightIntensity(3.8f);
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
	myBloom.Destroy();
	mySSAO.Destroy();
	Deferred_STD140.Destroy();
	CGInstancePrimitiveUtil::destroy();

	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gAlbedoSpec);
	glDeleteTextures(1, &gEmissive);
	glDeleteTextures(1, &gBool);
	glDeleteRenderbuffers(1, &gRBO);

	glDeleteTextures(1, &dSecondHDRColorBuffer);
	glDeleteTextures(1, &dSecondHDRBloomBuffer);

	glDeleteFramebuffers(1, &gFBO);
	glDeleteFramebuffers(1, &dSecondFBO);
}

void CGProj::DeferredRenderer::updateImgui()
{
	GLFWwindow* app_window = (GLFWwindow*)ImGui::GetIO().ClipboardUserData;

	ImGui::Begin("Deferred Lighting Demo"); // Create a window called " " and append into it.
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Camera Position %.1f %.1f %.1f", camera.Position.x, camera.Position.y, camera.Position.z);

		ImGui::TextColored(ImVec4(0.99f, 0.4f, 0.37f, 1.0f), "Press Tab Button to convert GAME/UI Mode");
		if (GameControl) ImGui::TextColored(ImVec4(0.78f, 0.17f, 0.54f, 1.0f), "GAME mode");
		else ImGui::TextColored(ImVec4(0.11f, 0.7f, 0.81f, 1.0f), "UI mode");

		ImGui::Checkbox("Light Box Render", &lightDraw);
		ImGui::Checkbox("Broad Phase Debug Render", &BroadDebug);
		ImGui::Checkbox("Wire Mode", &wireDraw);
		ImGui::Checkbox("Click Ray Render", &clickDraw); ImGui::SameLine();
		if (ImGui::Button("Click Ray Reset")) rayCollector.clear();

		ImGui::Checkbox("Hit Ray Render", &rayHitDraw); ImGui::SameLine();
		if (ImGui::Button("Hit Ray Reset")) hitCollector.clear();

		// Post-Processing Control
		ImGui::InputFloat("Gamma", &pGamma, 0.02f, 5.0f, "%.3f");
		ImGui::InputFloat("Exposure", &pExposure, 0.02f, 5.0f, "%.3f");

		ImGui::Checkbox("Bloom", &isBloom);
		if (isBloom)
		{
			ImGui::InputFloat3("Extractor Vector", &myBloom.m_BrightnessExtractor[0]);
			ImGui::InputFloat("Extractor Threshold", &myBloom.m_BrightnessThreShold);

			int iteration = (unsigned)myBloom.getIteration();
			if (ImGui::InputInt("Bloom Iteration", &iteration, 1, 1))
				myBloom.setIteration((unsigned)iteration);
		}

		ImGui::Checkbox("SSAO", &isSSAO);
		if (isSSAO)
		{
			float ssaoTemp = mySSAO.getRadius();
			if (ImGui::InputFloat("SSAO radius", &ssaoTemp, 0.02f, 5.0f, "%.3f"))
				mySSAO.setRadius(ssaoTemp);

			ssaoTemp = mySSAO.getBias();
			if (ImGui::InputFloat("SSAO bias", &ssaoTemp, 0.02f, 5.0f, "%.3f"))
				mySSAO.setBias(ssaoTemp);

			ssaoTemp = mySSAO.getPower();
			if (ImGui::InputFloat("SSAO power", &ssaoTemp, 0.02f, 5.0f, "%.3f"))
				mySSAO.setPower(ssaoTemp);

			int ssaoNoise = (int)mySSAO.getNoiseNum();
			if (ImGui::InputInt("SSAO Noise Numb", &ssaoNoise, 2, 1))
				mySSAO.setNoiseNum((unsigned)ssaoNoise);

			ImGui::Text("SSAO Kernel Numb : %d", NR_SSAO_KERNEL);

			bool blurState = mySSAO.getBlurState();
			if (ImGui::Checkbox("SSAO Blur", &blurState)) mySSAO.setBlurState(blurState);

			ImGui::Checkbox("SSAO Debug", &isSSAODebug);
		}
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
	glClearColor(0.11f, 0.11f, 0.11f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 1000.f);
	glm::mat4 model(1.0);

	// Get/Set Proxy Model, Normal Data
	// Now it is done by manual setting
	// TODO : make a structure to manage this kind of process
	std::vector<glm::mat4> tModel, tNormalMatrix;
	model = glm::translate(model, glm::vec3(0.f));
	model = glm::scale(model, glm::vec3(0.05f));
	tModel.push_back(model); tNormalMatrix.push_back(glm::transpose(glm::inverse(model)));

	editProxies[0].setInstanceData(tModel, tNormalMatrix);
	editProxies[1].setInstanceData(modelMatrices, worldNormalMatrices);


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

		Deferred_STD140.populateBuffer(view, projection);

		editProxies[0].render(camera.Position);
		editProxies[1].render(camera.Position);
		/*
		for (unsigned i = 0; i < editProxies.size(); ++i)
		{
			editProxies[i].render(camera.Position);
		}
		*/

		
		Deferred_First_Shader->setBool("material.CMorLM", true);
		Deferred_First_Shader->setBool("material.isLMdiffuse", true);
		Deferred_First_Shader->setBool("material.isLMspecular", false);
		Deferred_First_Shader->setBool("material.isLMemissive", false);
		Deferred_First_Shader->setBool("material.isNormalMap", false);
		Deferred_First_Shader->setBool("material.isDepthMap", false);
		
		Deferred_First_Shader->setBool("IsUseTangentSpace", false);
		Deferred_First_Shader->setVec3("cameraPos", camera.Position);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, assetManager.getTexture(TEXTURE_WOOD_PANEL, true));

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0, -5, 0));
		// model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1, 0, 0));
		model = glm::scale(model, glm::vec3(25));
		std::vector<glm::mat4> tModel, tNormal;
		tModel.push_back(model);
		tNormal.push_back(glm::transpose(glm::inverse(model)));
		CGInstancePrimitiveUtil::setQuadOneInstanceData(tModel, tNormal);
		CGInstancePrimitiveUtil::renderQuad();

		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, assetManager.getTexture(TEXTURE_WOOD_PANEL, true));
		model = model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0, -5, -10));
		model = glm::rotate(model, glm::radians(90.f), glm::vec3(1, 0, 0));
		model = glm::scale(model, glm::vec3(25));
		tModel[0] = model;
		tNormal[0] = glm::transpose(glm::inverse(model));
		CGInstancePrimitiveUtil::setQuadOneInstanceData(tModel, tNormal);
		CGInstancePrimitiveUtil::renderQuad();
		*/
	}
	// First Pass

	// SSAO between Deferred Rendering Pass
	unsigned ssaoResult;
	if (isSSAO)
		ssaoResult = mySSAO.getSSAOTexture(gPosition, gNormal, view, projection);

	// Second Pass
	glBindFramebuffer(GL_FRAMEBUFFER, dSecondFBO);
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
		
		// SSAO
		Deferred_Second_Shader->setBool("useSSAO", isSSAO);
		if (isSSAO)
		{
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, ssaoResult);
		}

		// Bloom
		Deferred_Second_Shader->setBool("useBloom", isBloom);
		if (isBloom)
		{
			Deferred_Second_Shader->setVec3("brightExtractor", myBloom.m_BrightnessExtractor);
			Deferred_Second_Shader->setFloat("brightThreshold", myBloom.m_BrightnessThreShold);
		}

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
		num_dir_light = num_point_light = num_spot_light = 0;
		num_dir_shadow = num_point_shadow = num_spot_shadow = 0;

		renderScreenQuad();
	}
	// Second Pass

	// Post Processing
	{
		unsigned result = 0;
		if(isBloom) result = myBloom.getBloomedTexture(dSecondHDRBloomBuffer);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Deferred_Post_Shader->use();
		Deferred_Post_Shader->setFloat("gamma", pGamma);
		Deferred_Post_Shader->setFloat("exposure", pExposure);
		Deferred_Post_Shader->setBool("useBloom", isBloom);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dSecondHDRColorBuffer);
		glActiveTexture(GL_TEXTURE1);
		if(isBloom)	glBindTexture(GL_TEXTURE_2D, result);
		renderScreenQuad();

		if (isSSAODebug && isSSAO)
		{
			glViewport(width / 4 * 3, 0, width / 4, height / 4);
			glScissor(width / 4 * 3, 0, width / 4, height / 4);
			glEnable(GL_SCISSOR_TEST);
			glClear(GL_DEPTH_BUFFER_BIT);

			SSAODebugShader->use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, ssaoResult);
			renderScreenQuad();

			glDisable(GL_SCISSOR_TEST);
			glViewport(0, 0, width, height);
		}
	}
	// Post Processing

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

	glCheckError();

	// Second Pass Setup For Deferred Rendering

	glBindTexture(GL_TEXTURE_2D, dSecondHDRColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, dSecondHDRBloomBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

	glCheckError();
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	myBloom.setTextureDimension(width, height);
	mySSAO.setTextureDimension(width, height);
}