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
#include "Camera.h"

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

//Will error if the sizes are not correct
#define CREATE_BUFFER_UNIFORM(name,x) \
struct name { \
public: \
x \
};\
static_assert(sizeof(name) % 16 == 0,"Buffer size must be multiple of 16 '" #name "'");

CREATE_BUFFER_UNIFORM(TestUniformStruct,
	glm::mat4 MatrixView = glm::mat4();
	glm::mat4 MatrixProjection = glm::mat4();
	glm::mat4 MatrixModelTest = glm::mat4();
	glm::mat4 MatrixPV = glm::mat4();
)

#define NUM_LIGHTS 10
CREATE_BUFFER_UNIFORM(TestUniformArray,
  TestUniformStruct data[NUM_LIGHTS];
)

CREATE_BUFFER_UNIFORM(TestUniformStruct2,
	glm::vec4 Color;
)

CREATE_BUFFER_UNIFORM(CommonDataStruct,
	float time;
	glm::vec2 screenSize;
	float pad;
)


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

	SingletonManager::CreateSingletons();

	_CGraphics->SetUpGraphics();

	Model* squareModel = new Model();
	squareModel->CreateSquare();

	Model testModel;
	testModel.LoadModel("Models/Low Poly Forest Decoration Pack/Trees/FBX Files/Tree 1.2/Tree1.2.fbx");
	//testModel.LoadModel("Models/nanosuit.obj");

	Camera mainCamera;

	Scene scene;
	scene.m_Camera = &mainCamera;

	ObjectDrawTest* square1;
	ObjectDrawTest* square2;
	ObjectDrawTest* square3;
	ObjectDrawTest* square4;
	ObjectDrawTest* treeObj;

	scene.m_Name = "Scene Test";
	scene.m_FilePath = "sceneTest.scene";
	if (!scene.Load()) {
		square1 = new ObjectDrawTest("Square1");
		square2 = new ObjectDrawTest("Square2");
		square3 = new ObjectDrawTest("Square3");
		square4 = new ObjectDrawTest("Square4");
		treeObj = new ObjectDrawTest("WorldTree");

		square1->m_Color = glm::vec4(1, 0, 0, 1);
		square2->m_Color = glm::vec4(0, 1, 0, 1);
		square3->m_Color = glm::vec4(0, 0, 1, 1);
		square4->m_Color = glm::vec4(1, 1, 1, 1);

		square1->m_Transform.SetPosition(glm::vec3(-2, 0, 0));
		square2->m_Transform.SetPosition(glm::vec3(0, 0, 0));
		square3->m_Transform.SetPosition(glm::vec3(2, 0, 0));
		square4->m_Transform.SetPosition(glm::vec3(2, 2, 0));
		treeObj->m_Transform.SetPosition(glm::vec3(-6.5f, 5.5f, -5.5f));
		treeObj->m_Transform.SetRotation(glm::vec3(-55.0f, 0, 55.0f));

		scene.AddObject(square1);
		scene.AddObject(square2);
		scene.AddObject(square3);
		scene.AddObject(square4);
		scene.AddObject(treeObj);
		scene.Save();
	} else {
		square1 = (ObjectDrawTest*)scene.GetObjectByName("Square1");
		square2 = (ObjectDrawTest*)scene.GetObjectByName("Square2");
		square3 = (ObjectDrawTest*)scene.GetObjectByName("Square3");
		square4 = (ObjectDrawTest*)scene.GetObjectByName("Square4");
		treeObj = (ObjectDrawTest*)scene.GetObjectByName("WorldTree");
	}

	_CManager->m_CurrentScene = &scene;

	Texture * testTexture = _CTextureManager->GetTexture("peacock-2.jpg");
	if (testTexture == nullptr) {
		testTexture = graphics->CreateTexture();
		testTexture->LoadTexture("peacock-2.jpg");
		testTexture->SetDebugObjName("Test Texture");
	}

	//UNIFORMS
	TestUniformArray testUniformArray;
	for (int i = 0; i < NUM_LIGHTS; i++) {
		//testUniformArray.data[i].MatrixModelTest = glm::mat4(1);
		//testUniformArray.data[i].MatrixView = glm::mat4(1);
		//testUniformArray.data[i].MatrixProjection = glm::mat4(1);
		//testUniformArray.data[i].MatrixPV = glm::mat4(1);
		testUniformArray.data[i].MatrixView[0][0] = 200.0f / NUM_LIGHTS;
		testUniformArray.data[i].MatrixView[0][1] = 50.0f / NUM_LIGHTS;
		testUniformArray.data[i].MatrixView[0][2] = 255.0f / NUM_LIGHTS;
	}
	ShaderUniformBlock* testUniformArrayBlock = graphics->CreateBuffer(&testUniformArray, sizeof(TestUniformArray));
	testUniformArrayBlock->SetDebugObjName("Color Aray");
	_CManager->RegisterShaderUniform(testUniformArrayBlock, "Lighting_Data");
	//testUniformArrayBlock->UpdateBuffer(&testUniformArray);


	TestUniformStruct testUniformStructObj;
	TestUniformStruct2 colorTest;
	CommonDataStruct commonPerFrameData;
	colorTest.Color = glm::vec4(1, 0, 1, 1);
	ShaderUniformBlock* testUniform = graphics->CreateBuffer(&testUniformStructObj, sizeof(TestUniformStruct));
	testUniform->SetDebugObjName("MVP Buffer");
	_CManager->RegisterShaderUniform(testUniform, "Vertex_Data");
	ShaderUniformBlock* testUniform2 = graphics->CreateBuffer(&colorTest, sizeof(TestUniformStruct2));
	testUniform2->SetDebugObjName("Color Test Buffer");
	_CManager->RegisterShaderUniform(testUniform2, "Shader_Data");
	ShaderUniformBlock* commonDataBlock = graphics->CreateBuffer(&commonPerFrameData, sizeof(CommonDataStruct));
	commonDataBlock->SetDebugObjName("Common Data Buffer");
	_CManager->RegisterShaderUniform(commonDataBlock, "Common_Data");

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

			testShader->AddBuffer(testUniform, "Vertex_Data");
			testShader->AddBuffer(testUniform2, "Shader_Data");
			testShader->AddBuffer(commonDataBlock, "Common_Data");
			testShader->AddBuffer(commonDataBlock, "Lighting_Data");

			testShader->Save();
		}
	}

	Shader* treeShader = _CShaderManager->GetShader("Shaders/Tree.shader");
	if (treeShader == nullptr) {
		treeShader = graphics->CreateShader();
		treeShader->m_FilePath = "Shaders/Tree.shader";
		if (!treeShader->Load()) {
			treeShader->SetDebugObjName("tree Shader");
			treeShader->m_ShouldPrintCode = true;
			treeShader->AddShader(_CShaderSpirvManager->GetShaderPart("test2.vert"));
			treeShader->AddShader(_CShaderSpirvManager->GetShaderPart("test2.frag"));
			treeShader->LinkShaders();

			treeShader->AddBuffer(testUniform, "Vertex_Data");
			treeShader->AddBuffer(testUniform2, "Shader_Data");
			treeShader->AddBuffer(commonDataBlock, "Common_Data");

			treeShader->Save();
		}
	}

	testShader->FindUnlinkedUniforms();
	treeShader->FindUnlinkedUniforms();

	RenderTarget* testRT = graphics->CreateRenderTarget(256, 256);
	if (testRT) {
		testRT->SetupRenderTarget_Internal();
		testRT->SetDebugObjName("Test RT");
	}

	square1->m_ObjectModel = square2->m_ObjectModel = square3->m_ObjectModel = square4->m_ObjectModel = squareModel;
	treeObj->m_ObjectModel = &testModel;

	Material treeModelMat1("Material/TreeModelMat1.mat");
	if (!treeModelMat1.Load()) {
		treeModelMat1.SetDebugObjName("Tree Model Mat 1");
		treeModelMat1.m_Shader = treeShader;
		treeModelMat1.AddTexture(_CGraphics->m_WhiteTexture, 0);
		treeModelMat1.Save();
	}
	Material treeModelMat2("Material/TreeModelMat2.mat");
	if (!treeModelMat2.Load())
	{
		treeModelMat2.SetDebugObjName("Tree Model Mat 2");
		treeModelMat2.m_Shader = testShader;
		treeModelMat2.AddTexture(testTexture, 0);
		treeModelMat2.Save();
	}

	Material SimpleMaterial("Material/SimpleMat.mat");
	if (!SimpleMaterial.Load())
	{
		SimpleMaterial.SetDebugObjName("Simple");
		SimpleMaterial.m_Shader = _CShaderManager->GetShader("Simple Shader.shader");
		SimpleMaterial.Save();
	}

	testModel.SetMaterial(&treeModelMat1, 0);
	testModel.SetMaterial(&SimpleMaterial, 1);
	squareModel->SetMaterial(&treeModelMat2, 0);

	//for (uint i = 0; i < _CManager->m_Materials.Length(); i++) {
	//	if (_CManager->m_Materials[i]->m_CreatedShader) {
	//		Shader* shader = _CManager->m_Materials[i]->m_Shader;
	//		shader->AddBuffer(testUniform, "Vertex_Data");
	//		shader->AddBuffer(testUniform2, "Shader_Data");
	//		shader->AddBuffer(commonDataBlock, "Common_Data");
	//	}
	//}

	InputHandler ih;
	ih.Startup(graphics->m_Window);

	//todo remove this
	_CManager->tempPVMUniform = testUniform;

	graphics->ImGuiInit();

	std::chrono::high_resolution_clock timer;
	float deltaTime = 0;
	float currentTime = 0;

	float fov = 90;

	mainCamera.SetFov(fov);
	mainCamera.SetAspectRatio(graphics->m_Window->GetAspect());

	ObjectDrawTest* testObj = (ObjectDrawTest*)scene.GetObjectByName("Model/Uniform Object Test");
	if (testObj == nullptr) {
		testObj = new ObjectDrawTest();
		testObj->m_Name = "Model/Uniform Object Test";
		testObj->m_Color = Vector3(1, 0, 1);
		testObj->m_ObjectModel = squareModel;
		testObj->m_ModelUniform = testUniform;
		testObj->m_ColorUniform = testUniform2;
		scene.AddObject(testObj);
	}

	scene.Start();
	
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

			ih.Update();

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

			static bool RotateCamera = true;
			static glm::vec3 CameraPos = glm::vec3(0, 0, 5);

			ImGui::Begin("Stats");
			ImGui::Text("Current Time: %f", currentTime);
			ImGui::Text("Delta Time: %f", deltaTime);
			ImGui::Text("fps: %f", ImGui::GetIO().Framerate);
			ImGui::Text("Mouse Pos: (x=%f, y=%f)", ih.GetMousePos().x, ih.GetMousePos().y);
			ImGui::Text("Keys: %s", ih.GetKeysDown().Get());
			if (ImGui::DragFloat("FOV", &fov, 1, 0, 180)) {
				mainCamera.SetFov(fov);
			}
			if (testRT) {
				ImGui::Image(testRT->GetTexture()->getTexturePtr(), ImVec2(200, 200), _CGraphics->GetImGuiImageUV0(), _CGraphics->GetImGuiImageUV1());
			}
			ImGui::Checkbox("Rotate Camera", &RotateCamera);
			ImGui::DragFloat3("Camera Pos", &CameraPos.x, 0.25f);

			static float cameraSpeed = 5.0f;
			ImGui::DragFloat("Camera Speed", &cameraSpeed, 0.1f);
			ImGui::End();

			
			if (ih.IsKeyDown(IKEY_W)) {
				CameraPos.z -= cameraSpeed * deltaTime;
			}
			if (ih.IsKeyDown(IKEY_S)) {
				CameraPos.z += cameraSpeed * deltaTime;
			}
			if (ih.IsKeyDown(IKEY_Q)) {
				CameraPos.y -= cameraSpeed * deltaTime;
			}
			if (ih.IsKeyDown(IKEY_E)) {
				CameraPos.y += cameraSpeed * deltaTime;
			}
			if (ih.IsKeyDown(IKEY_A)) {
				CameraPos.x -= cameraSpeed * deltaTime;
			}
			if (ih.IsKeyDown(IKEY_D)) {
				CameraPos.x += cameraSpeed * deltaTime;
			}



			//UPDATE OUR SCENE
			scene.Update();

			commonPerFrameData.time = currentTime;
			commonPerFrameData.screenSize = glm::vec2(graphics->m_Window->m_WindowWidth, graphics->m_Window->m_WindowHeight);
			commonDataBlock->UpdateBuffer(&commonPerFrameData);

			float x = glm::sin(currentTime * 0.423f) * 5;
			float y = glm::cos(currentTime * 0.72f) * 3;
			//
			testUniformStructObj.MatrixProjection = mainCamera.GetProjection();

			if (testRT) {
				graphics->PushDebugGroup("Render Target");
				mainCamera.SetLookAt(glm::vec3(-x, y, 10.0f), glm::vec3(0), glm::vec3(0, 1, 0));
				testUniformStructObj.MatrixPV = mainCamera.GetPV();

				_CGraphics->UseRenderTarget(testRT);
				graphics->SetClearColor(1.0f, 0.0f, 1.0f, 1.0f);
				graphics->Clear();

				testUniformStructObj.MatrixModelTest = glm::translate(glm::mat4(1.0f), glm::vec3(0, 2.0f, 0));
				testUniformStructObj.MatrixModelTest = glm::rotate(testUniformStructObj.MatrixModelTest, 90.0f, glm::vec3(0, 1, 1));
				testUniform->UpdateBuffer(&testUniformStructObj);
				colorTest.Color = glm::vec4(1, 1, 1, 1);
				testUniform2->UpdateBuffer(&colorTest);

				testModel.Draw();

				_CGraphics->ResetRenderTarget();

				graphics->UnbindTexture(0);

				graphics->PopDebugGroup();
			}

			if (RotateCamera) {
				mainCamera.SetLookAt(CameraPos + glm::vec3(x, y, 0), glm::vec3(0), glm::vec3(0, 1, 0));
			} else {
				mainCamera.SetPosition(CameraPos);
				mainCamera.SetRotation(glm::vec3(0));
			}
			testUniformStructObj.MatrixPV = mainCamera.GetPV();

			graphics->PushDebugGroup("Main Render");
			graphics->SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			graphics->Clear();
			testShader->Use();

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
	delete testTexture;
	delete testShader;
	delete graphics;

	_CIniFileManager->SaveIni();

	return 0;
}
