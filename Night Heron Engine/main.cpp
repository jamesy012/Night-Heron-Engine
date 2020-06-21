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

#include "Managers/TimeManager.h"

#include "Managers/IniFile.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include "jsonConverter/JCglm.h"

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

#include "nlohmann/json.hpp"

CREATE_BUFFER_UNIFORM(TestUniformStruct2,
	glm::vec4 Color;
)

CREATE_BUFFER_UNIFORM(CommonDataStruct,
	float time;
	glm::vec2 screenSize;
	float pad;
	)

	CREATE_BUFFER_UNIFORM_WITH_JSON(LightDataStruct,
						  glm::vec4 lightPos;
						  glm::vec4 lightColor;
						  glm::vec3 padding;
						  )


	void to_json(nlohmann::json& j, const LightDataStruct& p) {
		j = nlohmann::json{ { "Color", p.lightColor }, { "Pos", p.lightPos } };
	}

	void from_json(const nlohmann::json& j, LightDataStruct& p) {
		j.at("Color").get_to(p.lightColor);
		j.at("Pos").get_to(p.lightPos);
	}



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
	//{
	//	ShaderDefines.Add({ "NUM_POINTLIGHTS", MAX_NUM_POINTLIGHTS });
	//	ShaderDefines.Add({ "NUM_SPOT_LIGHTS", MAX_NUM_SPOTLIGHTS });
	//	ShaderDefines.Add({ "NUM_DIRECTIONAL_LIGHTS", MAX_NUM_DIRECTIONALLIGHTS });
	//	ShaderDefines.Add({ "NUM_SHADOWS", NUM_SHADOWS });
	//}


	SingletonManager::CreateSingletons();

	_CGraphics->SetUpGraphics();

	_CInput->Startup(graphics->m_Window);

	//TEXTURES
	//Texture* testTexture = _CTextureManager->GetTexture("peacock-2.jpg");
	//if (testTexture == nullptr) {
	//	testTexture = graphics->CreateTexture();
	//	testTexture->LoadTexture("peacock-2.jpg");
	//	testTexture->SetDebugObjName("Test Texture");
	//}

	//SHADERS
	Shader* testShader = _CShaderManager->GetShader("Shaders/TestShader.shader");
	if (testShader == nullptr) {
		testShader = graphics->CreateShader();
		testShader->m_FilePath = "Shaders/TestShader.shader";

		if (!testShader->Load()) {
			testShader->SetDebugObjName("Test Shader");
			testShader->m_ShouldPrintCode = true;
			testShader->AddShader(_CShaderSpirvManager->GetShaderPart("test.vert"));
			testShader->AddShader(_CShaderSpirvManager->GetShaderPart("test.frag"));
			testShader->LinkShaders();

			testShader->Save();
		}
	}

	Shader* normalShader = _CShaderManager->GetShader("Simple Shader.shader");

	Shader* SpecShader = _CShaderManager->GetShader("Shaders/SpecShader.shader");
	if (SpecShader == nullptr) {
		SpecShader = graphics->CreateShader();
		SpecShader->m_FilePath = "Shaders/SpecShader.shader";

		if (!SpecShader->Load()) {
			SpecShader->SetDebugObjName("Spec Shader");
			SpecShader->m_ShouldPrintCode = true;
			SpecShader->AddShader(_CShaderSpirvManager->GetShaderPart("Simple.vert"));
			SpecShader->AddShader(_CShaderSpirvManager->GetShaderPart("SpecVisualizer.frag"));
			SpecShader->LinkShaders();

			SpecShader->Save();
			_CShaderManager->AddShader(SpecShader);
		}
	}

	Shader* BumpShader = _CShaderManager->GetShader("Shaders/BumpShader.shader");
	if (BumpShader == nullptr) {
		BumpShader = graphics->CreateShader();
		BumpShader->m_FilePath = "Shaders/BumpShader.shader";

		if (!BumpShader->Load()) {
			BumpShader->SetDebugObjName("Bump Shader");
			BumpShader->m_ShouldPrintCode = true;
			BumpShader->AddShader(_CShaderSpirvManager->GetShaderPart("Simple.vert"));
			BumpShader->AddShader(_CShaderSpirvManager->GetShaderPart("BumpVisualizer.frag"));
			BumpShader->LinkShaders();

			BumpShader->Save();
			_CShaderManager->AddShader(BumpShader);
		}
	}

	//MATERIALS
	//Material SimpleMaterial("Material/SimpleMat.mat");
	//if (!SimpleMaterial.Load()) {
	//	SimpleMaterial.SetDebugObjName("Simple");
	//	SimpleMaterial.m_Shader = _CShaderManager->GetShader("Simple Shader.shader");
	//	SimpleMaterial.Save();
	//}

	//Assimp::DefaultLogger::create();

	//MODELS
	Model* squareModel = new Model();
	squareModel->CreateSquare();

	Model* planeModel = new Model();
	planeModel->CreatePlane();

	//Model testBistro = Model();
	//testBistro.LoadModel("Models/Bistro v4 Update/Bistro_v4/Bistro_Interior.fbx");

	Model testModel;
	Model groundModel;
	Model pathModel;
	//testModel.LoadModel("Models/sponza/sponza.obj");
	//testModel.LoadModel("Models/sponzaDetailed/sponza.obj");
	testModel.LoadModel("Models/syntystudios_Dont upload/SM_Env_Tree_01.fbx");
	groundModel.LoadModel("Models/syntystudios_Dont upload/SM_Env_Path_Dirt_01.fbx");
	pathModel.LoadModel("Models/syntystudios_Dont upload/SM_Env_Path_Tile_01.fbx");
	//testModel.LoadModel("Models/nanosuit.obj");

	//testModel.SetMaterial(&SimpleMaterial, 0);

	//UNIFORMS
	CameraUniformStruct cameraUniformData;
	ShaderUniformBlock* cameraUniformBlock = graphics->CreateBuffer(&cameraUniformData, sizeof(CameraUniformStruct));
	cameraUniformBlock->SetDebugObjName("Camera Data");
	_CManager->RegisterShaderUniform(cameraUniformBlock, "Camera_Data");

	ObjectUniformStruct ObjectUniformStructObj;
	ShaderUniformBlock* testUniform = graphics->CreateBuffer(&ObjectUniformStructObj, sizeof(ObjectUniformStruct));
	testUniform->SetDebugObjName("MVP Buffer");
	_CManager->RegisterShaderUniform(testUniform, "Object_Data");


	CommonDataStruct commonPerFrameData;
	ShaderUniformBlock* commonDataBlock = graphics->CreateBuffer(&commonPerFrameData, sizeof(CommonDataStruct));
	commonDataBlock->SetDebugObjName("Common Data Buffer");
	_CManager->RegisterShaderUniform(commonDataBlock, "Common_Data");

	LightDataStruct lightData;
	{
		nlohmann::json& j = _CIniFileManager->getIniData()["General"];
		if (j.contains("LightData")) {
			lightData.LoadData_Internal(j["LightData"]);
		} else {

			lightData.lightColor = glm::vec4(1);
			lightData.lightPos = glm::vec4(0, 4, 5, 1);
		}
	}
	ShaderUniformBlock* lightDataBlock = graphics->CreateBuffer(&lightData, sizeof(LightDataStruct));
	lightDataBlock->SetDebugObjName("Light Data Buffer");
	_CManager->RegisterShaderUniform(lightDataBlock, "Light_Data");

	TestUniformStruct2 colorTest;
	colorTest.Color = glm::vec4(1, 0, 1, 1);
	ShaderUniformBlock* testUniform2 = graphics->CreateBuffer(&colorTest, sizeof(TestUniformStruct2));
	testUniform2->SetDebugObjName("Color Test Buffer");
	_CManager->RegisterShaderUniform(testUniform2, "Shader_Data");

	//CAMERA
	FlyCamera mainCamera;

	//SCENE
	Scene scene;
	scene.m_FilePath = "Scenes/AssetTest.scene";
	scene.m_Camera = &mainCamera;
	scene.Load();


	Scene shadowScene;

	_CManager->m_CurrentScene = &scene;

	//todo remove this
	_CManager->tempPVMUniform = testUniform;

	graphics->ImGuiInit();

	std::chrono::high_resolution_clock timer;
	float deltaTime = 0;
	float currentTime = 0;

	float fov = 90;

	mainCamera.SetFov(fov);
	mainCamera.SetAspectRatio(graphics->m_Window->GetAspect());

	if (!_CArguments->IsArgument("DontUseIniCamera")) {
		nlohmann::json& j = _CIniFileManager->getIniData()["General"];
		if (j.contains("Camera")) {
			mainCamera.LoadData_Internal(j["Camera"]);
		}
	}

	fov = mainCamera.GetFov();

	//SCENE TEST
	if(false){


		ObjectDrawTest* tree = (ObjectDrawTest*)scene.GetObjectByName("tree");
		if (tree == nullptr) {
			tree = new ObjectDrawTest();
			tree->m_Name = "tree";
			tree->m_Color = Vector3(1, 1, 1);
			tree->m_ObjectModel = &testModel;
			tree->m_ModelUniform = testUniform;
			tree->m_ColorUniform = testUniform2;
			tree->m_Transform.SetScale(Vector3(0.05f));
			tree->m_Transform.SetPosition(glm::vec3(0, -5, 0));
			scene.AddObject(tree);
		}

		ObjectDrawTest* groundPlane = (ObjectDrawTest*)scene.GetObjectByName("ground");
		if (groundPlane == nullptr) {
			groundPlane = new ObjectDrawTest();
			groundPlane->m_Name = "ground";
			groundPlane->m_Color = Vector3(1, 1, 1);
			groundPlane->m_ObjectModel = &groundModel;
			groundPlane->m_ModelUniform = testUniform;
			groundPlane->m_ColorUniform = testUniform2;
			groundPlane->m_Transform.SetScale(Vector3(0.1f));
			groundPlane->m_Transform.SetPosition(glm::vec3(-15, -5, 15));
			scene.AddObject(groundPlane);
		}

		ObjectDrawTest* pathObj = (ObjectDrawTest*)scene.GetObjectByName("pathObj");
		if (pathObj == nullptr) {
			pathObj = new ObjectDrawTest();
			pathObj->m_Name = "pathObj";
			pathObj->m_Color = Vector3(1, 1, 1);
			pathObj->m_ObjectModel = &pathModel;
			pathObj->m_ModelUniform = testUniform;
			pathObj->m_ColorUniform = testUniform2;
			pathObj->m_Transform.SetScale(Vector3(0.1f));
			pathObj->m_Transform.SetPosition(glm::vec3(-15, -5, -15));
			scene.AddObject(pathObj);
		}

	}

	testShader->FindUnlinkedUniforms();

	//Simple Shader.shader
	{
		Shader* shader = _CShaderManager->GetShader("Simple Shader.shader");
		if (shader) {
			shader->FindUnlinkedUniforms();
		}
	}
	SpecShader->FindUnlinkedUniforms();
	BumpShader->FindUnlinkedUniforms();

	scene.Start();

	{
		CMLOG_SCOPED_NAME("START");
		CMLOG_SCOPED_INDENT();
		CMLOG_SCOPED_INDENT();
		CMLOG("\n\nProgram starting!\n\n");
	}

	glm::vec4 ClearColour = glm::vec4(glm::vec3(0.1f),1.0f);
	nlohmann::json& j = _CIniFileManager->getIniData()["General"];
	if (j.contains("ClearColour")) {
		ClearColour = j["ClearColour"].get<glm::vec4>();
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

			if (!_CManager->m_ShowingModal) {
			_CInput->Update();
			}

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

			ImGui::Begin("Stats");
			ImGui::Text("Current Time: %f", currentTime);
			ImGui::Text("Delta Time: %f", deltaTime);
			ImGui::Text("fps: %f", ImGui::GetIO().Framerate);
			ImGui::Text("Mouse Pos: (x=%f, y=%f)", _CInput->GetMousePos().x, _CInput->GetMousePos().y);
			ImGui::Text("Keys: %s", _CInput->GetKeysDown().Get());
			if (ImGui::DragFloat("FOV", &fov, 1, 0, 180)) {
				mainCamera.SetFov(fov);
			}
			ImGui::DragFloat("Camera Speed", &mainCamera.MovementSpeed, 0.1f);

			{
				bool lightingChange = false;
				lightingChange |= ImGui::DragFloat3("Light Pos", &lightData.lightPos.x, 0.1f);
				lightingChange |= ImGui::ColorEdit4("Light Color", &lightData.lightColor.x, 0.1f);
				if (lightingChange) {
					lightDataBlock->UpdateBuffer(&lightData);
				}
			}
			ImGui::ColorEdit4("Clear Color", &ClearColour.x, 0.1f);

			static int ShaderVisualizer = 0;
			ImGui::Combo("Visualize", &ShaderVisualizer, "Normal\0Spec\0Bump\0");
			ImGui::End();


			mainCamera.m_MouseMovement = _CInput->IsMouseKeyDown(IMOUSEKEY_RIGHTBUTTON);
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

			graphics->PushDebugGroup("Main Render");
			graphics->SetClearColor(ClearColour.r, ClearColour.g, ClearColour.b, ClearColour.a);
			graphics->Clear();

			switch (ShaderVisualizer) {
				default:
				case 0:
					normalShader->Use();
					break;
				case 1:
					SpecShader->Use();
					break;
				case 2:
					BumpShader->Use();
					break;
			}

			scene.Draw();

			graphics->UnbindTexture(0);

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

	delete testUniform;
	delete testUniform2;
	delete commonDataBlock;
	delete squareModel;
	//delete testTexture;
	delete testShader;
	delete graphics;

	{//save camera to ini
		nlohmann::json& j = _CIniFileManager->getIniData()["General"]["Camera"];
		mainCamera.SaveData_Internal(j);

		//lightData.SaveData_Internal(_CIniFileManager->getIniData()["General"]["LightData"]);
		_CIniFileManager->getIniData()["General"]["LightData"] = lightData;
		_CIniFileManager->getIniData()["General"]["ClearColour"] = ClearColour;
	}


	scene.Save();

	_CManager->UpdateIniFile();
	_CIniFileManager->SaveIni();

	return 0;
}


