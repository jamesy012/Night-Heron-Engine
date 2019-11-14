#define NOMINMAX
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <chrono>

#include "Graphics/OpenGL/GFXOpenGL.h"
#include "Graphics/DX11/GFXDX11.h"

#include "Graphics/API/Shader.h"
#include "Graphics/API/Texture.h"
#include "Graphics/API/RenderTarget.h"
#include "Window.h"
#include "Graphics/Model.h"
#include "Graphics/Material.h"
#include "Util.h"
#include "FlyCamera.h"

#include "Managers/ShaderManager.h"
#include "Managers/ShaderSpirvManager.h"
#include "Managers/TextureManager.h"
#include "Managers/Manager.h"
#include "Managers/Arguments.h"
#include "Singletons.h"

//#include "Lighting/LightManager.h"
#include "Managers/TimeManager.h"

#include "Managers/IniFile.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include <Dependency/ImGui/imgui.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "Graphics/ShaderSpirvData.h"

#include "Transform.h"
#include "Object.h"
#include "Scene.h"

#include "tests/ObjectDrawTest.h"

#include "Input/InputHandler.h"

#include <GL/glew.h>
#include <GL/wglew.h>
#include <cstdlib>
#include <random>


CREATE_BUFFER_UNIFORM(CommonDataStruct,
	float time;
	glm::vec2 screenSize;
	float pad;
)


CREATE_BUFFER_UNIFORM(LightData,
glm::mat4 view;
glm::mat4 projection;
int screenSizeX;
int screenSizeY;
int lightCount;
int pad;
)

CREATE_BUFFER_UNIFORM(LightDebug,
int totalLightCount;
int numberOfTilesX;
int pad;
int pad2;
)


#define NUM_OF_LIGHTS 1024
const float LIGHT_RADIUS = 30.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 300.0f;
const glm::vec3 LIGHT_MIN_BOUNDS = glm::vec3(-135.0f, -20.0f, -60.0f);
const glm::vec3 LIGHT_MAX_BOUNDS = glm::vec3(135.0f, 170.0f, 60.0f);

struct PointLight {
	glm::vec4 color;
	glm::vec4 position;
	glm::vec4 paddingAndRadius;
};

glm::vec3 RandomPosition(std::uniform_real_distribution<> dis, std::mt19937 gen) {
	glm::vec3 position = glm::vec3(0.0);
	for (int i = 0; i < 3; i++) {
		float min = LIGHT_MIN_BOUNDS[i];
		float max = LIGHT_MAX_BOUNDS[i];
		position[i] = (GLfloat)dis(gen) * (max - min) + min;
	}

	return position;
}

struct VisibleIndex {
	int index;
};

int WINAPI WinMain(_In_ HINSTANCE hInstance,
				   _In_opt_ HINSTANCE hPrevInstance,
				   _In_ LPSTR lpCmdLine,
				   _In_ int nShowCmd) {

	_CIniFileManager = new IniFile();
	_CIniFileManager->LoadIni();

	_CArguments = new Arguments();
	_CArguments->Generate(lpCmdLine);

	GENERATED_OBJ::RegisterClasses();

	//pick graphics api
	GFX* graphics = nullptr;

	if (_CArguments->IsArgument("DX11")) {
		graphics = new GFXDX11();
	}
	if (_CArguments->IsArgument("OpenGL")) {
		graphics = new GFXOpenGL();
	}

	if (graphics == nullptr) {
		graphics = new GFXOpenGL();
		//graphics = new GFXDX11();
	}

	if (_CArguments->IsArgument("Console")) {
		//create console
		AllocConsole();
		freopen_s((FILE * *)stdout, "CONOUT$", "w", stdout);
	}

	if (!graphics->CreateWindowSetUpAPI()) {
		return -1;
	}

	//SHADER DEFINES SETUP
	{
		ShaderDefines.Add({ "NUM_POINTLIGHTS", 1 });
		ShaderDefines.Add({ "NUM_SPOT_LIGHTS", 1 });
		ShaderDefines.Add({ "NUM_DIRECTIONAL_LIGHTS", 1 });
		ShaderDefines.Add({ "NUM_SHADOWS", 1 });
	}

	SingletonManager::CreateSingletons();

	_CGraphics->SetUpGraphics();

	_CInput->Startup(graphics->m_Window);

	//TEXTURES
	Texture* testTexture = _CTextureManager->GetTexture("peacock-2.jpg");
	if (testTexture == nullptr) {
		testTexture = graphics->CreateTexture();
		testTexture->LoadTexture("peacock-2.jpg");
		testTexture->SetDebugObjName("Test Texture");
	}

	//SHADERS
	Shader* LightDebugShader = nullptr;// _CShaderManager->GetShader("Shaders/ForwardPlus/LightDebug.shader");
	if (LightDebugShader == nullptr) {
		LightDebugShader = graphics->CreateShader();
		LightDebugShader->m_FilePath = "Shaders/ForwardPlus/LightDebug.shader";

		//if (!LightDebugShader->Load()) {
			LightDebugShader->SetDebugObjName("Light Debug Shader");
			LightDebugShader->m_ShouldPrintCode = true;
			LightDebugShader->AddShader(_CShaderSpirvManager->GetShaderPart("Simple.vert"));
			LightDebugShader->AddShader(_CShaderSpirvManager->GetShaderPart("light_debug.frag"));
			LightDebugShader->LinkShaders();

			LightDebugShader->Save();
		//}
	}


	Shader* compShader = _CShaderManager->GetShader("Shaders/ForwardPlus/CompShader.shader");
	if (compShader == nullptr) {
		compShader = graphics->CreateShader();
		compShader->m_FilePath = "Shaders/ForwardPlus/CompShader.shader";

		if (!compShader->Load()) {
			compShader->SetDebugObjName("Compute Test Shader");
			compShader->m_ShouldPrintCode = true;
			compShader->AddShader(_CShaderSpirvManager->GetShaderPart("light_culling.comp"));
			compShader->LinkShaders();

			compShader->Save();
		}
	}

	Shader* NormalShader = nullptr;//_CShaderManager->GetShader("Shaders/ForwardPlus/Normal.shader");
	if (NormalShader == nullptr) {
		NormalShader = graphics->CreateShader();
		NormalShader->m_FilePath = "Shaders/ForwardPlus/Normal.shader";

		//if (!NormalShader->Load()) {
			NormalShader->SetDebugObjName("NormalShader Shader");
			NormalShader->m_ShouldPrintCode = true;
			NormalShader->AddShader(_CShaderSpirvManager->GetShaderPart("light_accumulation.vert"));
			//NormalShader->AddShader(_CShaderSpirvManager->GetShaderPart("Simple.frag"));
			NormalShader->AddShader(_CShaderSpirvManager->GetShaderPart("light_accumulation.frag"));
			NormalShader->LinkShaders();

			NormalShader->Save();
		//}
	}

	//MATERIALS
	Material LightDebugMat("Material/ForwardPlus/LightDebug.mat");
	if (!LightDebugMat.Load()) {
		LightDebugMat.SetDebugObjName("Tree Model Mat 1");
		LightDebugMat.m_Shader = LightDebugShader;//_CShaderManager->GetShader("Shaders/ForwardPlus/LightDebug.shader");
		//LightDebugMat.AddTexture(_CGraphics->m_WhiteTexture, 0);
		LightDebugMat.Save();
	}


	//Assimp::DefaultLogger::create();

	//MODELS
	Model* squareModel = new Model();
	squareModel->CreateSquare();

	Model* planeModel = new Model();
	planeModel->CreatePlane();

	Model sponzaModel = Model();
	sponzaModel.LoadModel("Models/sponza/sponza.obj");

	//testModel.SetMaterial(&treeModelMat1, 0);
	//testModel.SetMaterial(&SimpleMaterial, 1);
	//squareModel->SetMaterial(&treeModelMat2, 0);
	//planeModel->SetMaterial(&treeModelMat2, 0);

	CameraUniformStruct cameraUniformData;
	ShaderUniformBlock* cameraUniformBlock = graphics->CreateBuffer(&cameraUniformData, sizeof(CameraUniformStruct));
	cameraUniformBlock->SetDebugObjName("Camera Data");
	_CManager->RegisterShaderUniform(cameraUniformBlock, "Camera_Data");

	CommonDataStruct commonPerFrameData;
	ShaderUniformBlock* commonDataBlock = graphics->CreateBuffer(&commonPerFrameData, sizeof(CommonDataStruct));
	commonDataBlock->SetDebugObjName("Common Data Buffer");
	_CManager->RegisterShaderUniform(commonDataBlock, "Common_Data");


	ObjectUniformStruct ObjectUniformStructObj;
	ShaderUniformBlock* objectUniform = graphics->CreateBuffer(&ObjectUniformStructObj, sizeof(ObjectUniformStruct));
	objectUniform->SetDebugObjName("MVP Buffer");
	_CManager->RegisterShaderUniform(objectUniform, "Object_Data");

	LightData lightUniformData;
	ShaderUniformBlock* LightBlock = graphics->CreateBuffer(&lightUniformData, sizeof(LightData));
	LightBlock->SetDebugObjName("Light Data Buffer");
	_CManager->RegisterShaderUniform(LightBlock, "ComputeBuffers");

	LightDebug lightDebugData;
	lightDebugData.totalLightCount = NUM_OF_LIGHTS;
	lightDebugData.numberOfTilesX = 64;
	ShaderUniformBlock* LightDebug = graphics->CreateBuffer(&lightDebugData, sizeof(LightDebug));
	LightDebug->SetDebugObjName("Light Debug Buffer");
	_CManager->RegisterShaderUniform(LightDebug, "LightDebug");


	//CAMERA
	FlyCamera mainCamera;
	Camera defaultCamera;

	//SCENE
	Scene screenspaceScene;
	screenspaceScene.m_Camera = &defaultCamera;

	Scene scene;
	scene.m_Camera = &mainCamera;

	_CManager->m_CurrentScene = &scene;


	//RENDER SETUP

	//todo remove this
	_CManager->tempPVMUniform = objectUniform;

	graphics->ImGuiInit();

	std::chrono::high_resolution_clock timer;
	float deltaTime = 0;
	float currentTime = 0;

	float fov = 90;

	mainCamera.SetFov(fov);
	mainCamera.SetAspectRatio(graphics->m_Window->GetAspect());

	//ObjectDrawTest* testObj = (ObjectDrawTest*)scene.GetObjectByName("Model/Uniform Object Test");
	//if (testObj == nullptr) {
	//	testObj = new ObjectDrawTest();
	//	testObj->m_Name = "Model/Uniform Object Test";
	//	testObj->m_Color = Vector3(1, 0, 1);
	//	testObj->m_ObjectModel = planeModel;
	//	testObj->m_ModelUniform = testUniform;
	//	testObj->m_ColorUniform = testUniform2;
	//	scene.AddObject(testObj);
	//}

	ObjectDrawTest* screenObj = new ObjectDrawTest();
	screenObj->m_Name = "Model/plane";
	screenObj->m_Color = Vector3(1, 1, 1);
	screenObj->m_ObjectModel = planeModel;
	screenObj->m_ModelUniform = objectUniform;
	screenObj->m_ColorUniform = nullptr;
	screenObj->m_Transform.SetScale(Vector3(100.0f));
	screenObj->m_Transform.SetPosition(glm::vec3(0, 0, -5));
	screenObj->m_Transform.SetRotation(glm::vec3(90, 0, 0));
	screenspaceScene.AddObject(screenObj);


	ObjectDrawTest* sponza = new ObjectDrawTest();
	sponza->m_Name = "Model/sponza";
	sponza->m_Color = Vector3(1, 1, 1);
	sponza->m_ObjectModel = &sponzaModel;
	sponza->m_ModelUniform = objectUniform;
	sponza->m_ColorUniform = nullptr;
	sponza->m_Transform.SetScale(Vector3(0.1f));
	sponza->m_Transform.SetPosition(glm::vec3(0,0,0));
	scene.AddObject(sponza);

	RenderTarget* textureRender = _CGraphics->CreateRenderTarget(1024, 1024);
	textureRender->SetupRenderTarget_Internal();
	textureRender->SetDebugObjName("textureRender");
	textureRender->Bind();

	RenderTarget* DepthMap = _CGraphics->CreateRenderTarget(_CMainWindow->m_WindowWidth, _CMainWindow->m_WindowHeight);
	DepthMap->AddBuffer(RenderTargetBufferTypes::TEXTURE, RenderTargetBufferFormats::DEPTH, RenderTargetBufferSize::Size16);
	DepthMap->SetDebugObjName("DepthMap");
	DepthMap->Bind();

	// Used for storage buffer objects to hold light data and visible light indicies data
	GLuint lightBuffer = 0;
	GLuint visibleLightIndicesBuffer = 0;
	int workGroupsX = (_CMainWindow->m_WindowWidth + (_CMainWindow->m_WindowWidth % 16)) / 16;
	int workGroupsY = (_CMainWindow->m_WindowHeight + (_CMainWindow->m_WindowHeight % 16)) / 16;
	size_t numberOfTiles = workGroupsX * workGroupsY;
	{
		// Generate our shader storage buffers
		glGenBuffers(1, &lightBuffer);
		glGenBuffers(1, &visibleLightIndicesBuffer);

		// Bind light buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_OF_LIGHTS * sizeof(PointLight), 0, GL_DYNAMIC_DRAW);

		// Bind visible light indices buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibleLightIndicesBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfTiles * sizeof(VisibleIndex) * 1024, 0, GL_STATIC_DRAW);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
		PointLight *pointLights = (PointLight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0, 1);
		for (int i = 0; i < NUM_OF_LIGHTS; i++) {
			PointLight &light = pointLights[i];
			light.position = glm::vec4(RandomPosition(dis, gen), 1.0f);
			light.color = glm::vec4(1.0f + dis(gen), 1.0f + dis(gen), 1.0f + dis(gen), 1.0f);
			light.paddingAndRadius = glm::vec4(glm::vec3(0.0f), LIGHT_RADIUS);
		}

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	LightDebugShader->FindUnlinkedUniforms();
	compShader->FindUnlinkedUniforms();
	NormalShader->FindUnlinkedUniforms();

	scene.Start();

	{
		CMLOG_SCOPED_NAME("START");
		CMLOG_SCOPED_INDENT;
		CMLOG_SCOPED_INDENT;
		CMLOG("\n\nProgram starting!\n\n");
	}

	while (true) {
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			graphics->PushDebugGroup("Frame");
			auto timerStart = timer.now();
			currentTime += deltaTime;
			_CTimeManager->m_CurrentTime = currentTime;
			_CTimeManager->m_DeltaTime = deltaTime;

			_CInput->Update();

			graphics->ImGuiNewFrame();

			static bool DemoWindow = false;

			if (ImGui::BeginMainMenuBar()) {
				if (ImGui::BeginMenu("Windows")) {
					ImGui::MenuItem("Demo", NULL, &DemoWindow);
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}

			_CManager->ImGuiWindow();

			if (DemoWindow) {
				ImGui::ShowDemoWindow(&DemoWindow);
			}

			//static bool RotateCamera = false;
			//static bool CameraLookAt = true;

			ImGui::Begin("Stats");
			ImGui::Text("Current Time: %f", currentTime);
			ImGui::Text("Delta Time: %f", deltaTime);
			ImGui::Text("fps: %f", ImGui::GetIO().Framerate);
			ImGui::Text("Mouse Pos: (x=%f, y=%f)", _CInput->GetMousePos().x, _CInput->GetMousePos().y);
			ImGui::Text("Keys: %s", _CInput->GetKeysDown().Get());
			if (ImGui::DragFloat("FOV", &fov, 1, 0, 180)) {
				mainCamera.SetFov(fov);
			}
			//ImGui::Checkbox("Rotate Camera", &RotateCamera);
			//ImGui::Checkbox("Look at Center?", &CameraLookAt);
			ImGui::DragFloat3("Camera Pos", &mainCamera.m_Position.x, 0.25f);

			ImGui::DragFloat("Camera Movement Speed", &mainCamera.MovementSpeed, 0.1f);
			ImGui::DragFloat("Camera Rotation Speed", &mainCamera.RotationSpeed, 0.1f);

			static float LightVisulationMul = 1.0f;
			ImGui::DragFloat("Light Visulation Mul", &LightVisulationMul, 0.05f,0,1);

			if (textureRender && textureRender->GetTexture()) {
				ImGui::Image(textureRender->GetTexture()->getTexturePtr(), ImVec2(200, 150), _CGraphics->GetImGuiImageUV0(), _CGraphics->GetImGuiImageUV1());
			}

			ImGui::End();

			mainCamera.UpdateInput();

			//UPDATE OUR SCENE
			scene.Update();

			commonPerFrameData.time = currentTime;
			commonPerFrameData.screenSize = glm::vec2(graphics->m_Window->m_WindowWidth, graphics->m_Window->m_WindowHeight);
			commonDataBlock->UpdateBuffer(&commonPerFrameData);


			_CGraphics->ResetRenderTarget();

			cameraUniformData.MatrixPV = mainCamera.GetPV();
			cameraUniformData.pos = mainCamera.m_Position;
			cameraUniformBlock->UpdateBuffer(&cameraUniformData);

			{
				graphics->PushDebugGroup("Compute");

				{
					graphics->PushDebugGroup("Prepass");
					_CGraphics->UseRenderTarget(DepthMap);
					graphics->SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
					graphics->Clear();
					NormalShader->Use();

					scene.Draw();

					graphics->UnbindTexture(0);

					graphics->PopDebugGroup();
				}
				// Bind shader storage buffer objects for the light and indice buffers
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleLightIndicesBuffer);
				{
					lightUniformData.lightCount = NUM_OF_LIGHTS;
					lightUniformData.screenSizeX = _CMainWindow->m_WindowWidth;
					lightUniformData.screenSizeY = _CMainWindow->m_WindowHeight;
					lightUniformData.projection = mainCamera.GetProjection();
					lightUniformData.view = mainCamera.GetView();
					//lightCullingShader.Use();
					LightBlock->UpdateBuffer(&lightUniformData);
					compShader->Use();
					//glUniformMatrix4fv(glGetUniformLocation(lightCullingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
					//glUniformMatrix4fv(glGetUniformLocation(lightCullingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

					// Bind depth map texture to texture location 4 (which will not be used by any model texture)
					_CGraphics->BindTexture(DepthMap->GetTexture(), 3);


					// Dispatch the compute shader, using the workgroup values calculated earlier
					glDispatchCompute(workGroupsX, workGroupsY, 1);

					_CGraphics->UnbindTexture(3);
				}

				{
					graphics->PushDebugGroup("Update");
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
					PointLight *pointLights = (PointLight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

					for (int i = 0; i < NUM_OF_LIGHTS; i++) {
						PointLight &light = pointLights[i];
						float min = LIGHT_MIN_BOUNDS[1];
						float max = LIGHT_MAX_BOUNDS[1];

						light.position.y = fmod((light.position.y + (-4.5f * deltaTime) - min + max), max) + min;
					}

					glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
					graphics->PopDebugGroup();
				}

				if (false) {
					graphics->PushDebugGroup("Debug");
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibleLightIndicesBuffer);
					VisibleIndex * indexs = (VisibleIndex*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

					CMLOG("0: %i, 5: %i, 14: %i\n", indexs[0].index, indexs[5].index, indexs[14].index);

					glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
					graphics->PopDebugGroup();
				}


				{
					graphics->PushDebugGroup("debug lighting Render");

					lightDebugData.totalLightCount = NUM_OF_LIGHTS * LightVisulationMul;
					lightDebugData.numberOfTilesX = workGroupsX;
					LightDebug->UpdateBuffer(&lightDebugData);

					_CGraphics->UseRenderTarget(textureRender);
					graphics->SetClearColor(0.0f, 1.0f, 0.0f, 1.0f);
					graphics->Clear();
					LightDebugShader->Use();

					cameraUniformData.MatrixPV = defaultCamera.GetPV();
					cameraUniformData.pos = defaultCamera.m_Position;
					cameraUniformBlock->UpdateBuffer(&cameraUniformData);

					screenspaceScene.Draw();//replace with plane?!

					cameraUniformData.MatrixPV = mainCamera.GetPV();
					cameraUniformData.pos = mainCamera.m_Position;
					cameraUniformBlock->UpdateBuffer(&cameraUniformData);


					graphics->PopDebugGroup();

				}


				graphics->PopDebugGroup();
			}

			graphics->PushDebugGroup("Main Render");
			_CGraphics->ResetRenderTarget();
			graphics->SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			graphics->Clear();
			NormalShader->Use();

			scene.Draw();

			graphics->UnbindTexture(0);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);

			graphics->PopDebugGroup();
			{
				graphics->PushDebugGroup("ImGui Render");
				graphics->ImGuiDraw();
				graphics->PopDebugGroup();
			}

			graphics->PopDebugGroup();
			graphics->SwapBuffer();

			auto timerEnd = timer.now();
			deltaTime = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1, 1>>>(timerEnd - timerStart).count();
		}
	}

	graphics->DestroyMainWindow();

	//delete testUniform;
	//delete testUniform2;
	delete commonDataBlock;
	delete squareModel;
	delete testTexture;
	//delete testShader;
	delete graphics;

	_CIniFileManager->SaveIni();

	return 0;
}
