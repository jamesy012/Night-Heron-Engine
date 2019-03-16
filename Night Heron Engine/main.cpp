
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

#include "Graphics/ShaderManager.h"
#include "Manager.h"
#include "Singletons.h"

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

struct TestUniformStruct {
public:
	glm::mat4 MatrixView = glm::mat4();
	glm::mat4 MatrixProjection = glm::mat4();
	glm::mat4 MatrixModelTest = glm::mat4();
	glm::mat4 MatrixPV = glm::mat4();
};

struct TestUniformStruct2 {
public:
	glm::vec4 Color;
};

struct CommonDataStruct {
public:
	float time;
	glm::vec3 pad;
};



int WINAPI WinMain(HINSTANCE   hInstance,              // Instance
				   HINSTANCE   hPrevInstance,              // Previous Instance
				   LPSTR       lpCmdLine,              // Command Line Parameters
				   int     nCmdShow)               // Window Show State

{
	//create console
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

	std::string arguments = lpCmdLine;

	//pick graphics api
	GFX* graphics = nullptr;

	if (arguments.find("-DX11") != std::string::npos) {
		graphics = new GFXDX11();
	}
	if (arguments.find("-OpenGL") != std::string::npos) {
		graphics = new GFXOpenGL();
	}

	if (graphics == nullptr) {
		graphics = new GFXOpenGL();
		//graphics = new GFXDX11();
	}

	if (!graphics->CreateWindowSetUpAPI()) {
		return -1;
	}

	SingletonManager::CreateSingletons();

	Shader* testShader = graphics->CreateShader();
	Shader* treeShader = graphics->CreateShader();
	testShader->SetDebugObjName("Test Shader");
	testShader->m_ShouldPrintCode = true;
	treeShader->SetDebugObjName("tree Shader");
	treeShader->m_ShouldPrintCode = true;
	//testShader->m_ShoudRegenerateCode = true;
	Texture* testTexture = graphics->CreateTexture();
	Texture* whiteTexture = graphics->CreateTexture();
	RenderTarget* testRT = graphics->CreateRenderTarget(256, 256);

	Model* squareModel = new Model();
	squareModel->CreateSquare();

	Object square1("Square1"), square2("Square2"), square3("Square3"), square4("Square4"), treeObj("WorldTree");
	square1.m_Transform.SetPosition(glm::vec3(-2, 0, 0));
	square2.m_Transform.SetPosition(glm::vec3(0, 0, 0));
	square3.m_Transform.SetPosition(glm::vec3(2, 0, 0));
	square4.m_Transform.SetPosition(glm::vec3(2, 2, 0));
	treeObj.m_Transform.SetPosition(glm::vec3(-6.5f, 5.5f, -5.5f));
	treeObj.m_Transform.SetRotation(glm::vec3(-55.0f, 0, 55.0f));

	_CManager->m_Objects.Add(&square1);
	_CManager->m_Objects.Add(&square2);
	_CManager->m_Objects.Add(&square3);
	_CManager->m_Objects.Add(&square4);
	_CManager->m_Objects.Add(&treeObj);

	testTexture->LoadTexture("peacock-2.jpg");
	testTexture->SetDebugObjName("Test Texture");

	whiteTexture->CreateTexture(1, 1);
	whiteTexture->SetDebugObjName("White Texture");

	testShader->AddShader(_CShaderManager->GetShader("test.vert"));
	testShader->AddShader(_CShaderManager->GetShader("test.frag"));
	treeShader->AddShader(_CShaderManager->GetShader("test2.vert"));
	treeShader->AddShader(_CShaderManager->GetShader("test2.frag"));
	testShader->LinkShaders();
	treeShader->LinkShaders();

	TestUniformStruct testUniformStructObj;
	TestUniformStruct2 colorTest;
	CommonDataStruct commonPerFrameData;
	colorTest.Color = glm::vec4(1, 0, 1, 1);

	//testUniformStructObj.MatrixModelTest = glm::mat4(1.0f);

	//testUniformStructObj.MatrixView = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0), glm::vec3(0, 1, 0));
	//projection = glm::perspective(90.0f, (float)(graphics->m_Window->m_WindowWidth / graphics->m_Window->m_WindowHeight), 0.1f, 100.0f);
	//testUniformStructObj.MatrixProjection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 100.0f);
	//testUniformStructObj.MatrixProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.0f, 100.0f);

	//testUniformStructObj.MatrixModelTest = glm::translate(testUniformStructObj.MatrixModelTest, glm::vec3(1, 1, -1));

	ShaderUniformBlock* testUniform = graphics->CreateBuffer(&testUniformStructObj, sizeof(TestUniformStruct));
	testUniform->SetDebugObjName("MVP Buffer");
	testShader->AddBuffer(testUniform, "Vertex_Data");
	treeShader->AddBuffer(testUniform, "Vertex_Data");

	ShaderUniformBlock* testUniform2 = graphics->CreateBuffer(&colorTest, sizeof(TestUniformStruct2));
	testUniform2->SetDebugObjName("Color Test Buffer");
	testShader->AddBuffer(testUniform2, "shader_data");
	treeShader->AddBuffer(testUniform2, "shader_data");

	ShaderUniformBlock* commonDataBlock = graphics->CreateBuffer(&commonPerFrameData, sizeof(CommonDataStruct));
	commonDataBlock->SetDebugObjName("Common Data Buffer");
	testShader->AddBuffer(commonDataBlock, "CommonData");
	treeShader->AddBuffer(commonDataBlock, "CommonData");

	if (testRT) {
		testRT->SetupRenderTarget_Internal();
		testRT->SetDebugObjName("Test RT");
	}


	Model testModel;
	testModel.LoadModel("Models/Low Poly Forest Decoration Pack/Trees/FBX Files/Tree 1.2/Tree1.2.fbx");
	//testModel.LoadModel("Models/nanosuit.obj");

	Material treeModelMat1;
	treeModelMat1.SetDebugObjName("Tree Model Mat 1");
	Material treeModelMat2;
	treeModelMat2.SetDebugObjName("Tree Model Mat 2");
	treeModelMat1.m_Shader = treeShader;
	treeModelMat2.m_Shader = testShader;
	
	testModel.SetMaterial(&treeModelMat1, 0);
	testModel.SetMaterial(&treeModelMat2, 1);
	squareModel->SetMaterial(&treeModelMat2, 0);

	graphics->ImGuiInit();

	std::chrono::high_resolution_clock timer;
	float deltaTime = 0;
	float currentTime = 0;

	float fov = 90;

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

			graphics->ImGuiNewFrame();

			static bool DemoWindow = false;
			static bool ShaderMenu = true;


			if (ImGui::BeginMainMenuBar()) {
				if (ImGui::BeginMenu("Windows")) {
					ImGui::MenuItem("Demo", NULL, &DemoWindow);
					ImGui::MenuItem("Shaders", NULL, &ShaderMenu);
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}

			_CShaderManager->ImGuiWindow(&ShaderMenu);
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
			ImGui::DragFloat("FOV", &fov, 1, 0, 180);
			if (testRT) {
				ImGui::Image(testRT->GetTexture()->getTexturePtr(), ImVec2(200, 200), _CGraphics->GetImGuiImageUV0(), _CGraphics->GetImGuiImageUV1());
			}
			ImGui::Checkbox("Rotate Camera", &RotateCamera);
			ImGui::DragFloat3("Camera Pos", &CameraPos.x, 0.25f);
			ImGui::End();

			bool ShowMetrics = true;

			commonPerFrameData.time = currentTime;
			commonDataBlock->UpdateBuffer(&commonPerFrameData);

			static int counter = 0;
			//counter = (counter + 1) % 512;
			counter++;

			float x = glm::sin(counter / 64.0f) * 5;
			float y = glm::cos(counter / 82.0f) * 3;
			//
			testUniformStructObj.MatrixProjection = glm::perspective(glm::radians(fov), (float)graphics->m_Window->GetAspect(), 0.1f, 100.0f);

			if (testRT) {
				graphics->PushDebugGroup("Render Target");
				testUniformStructObj.MatrixView = glm::lookAt(glm::vec3(-x, y, 10.0f), glm::vec3(0), glm::vec3(0, 1, 0));
				testUniformStructObj.MatrixPV = testUniformStructObj.MatrixProjection * testUniformStructObj.MatrixView;

				_CGraphics->UseRenderTarget(testRT);
				graphics->SetClearColor(1.0f, 0.0f, 1.0f, 1.0f);
				graphics->Clear();
				//treeShader->Use();
				//treeShader->BindTexture("textureTest", 1);

				testUniformStructObj.MatrixModelTest = glm::translate(glm::mat4(1.0f), glm::vec3(0, 2.0f, 0));
				testUniformStructObj.MatrixModelTest = glm::rotate(testUniformStructObj.MatrixModelTest, 90.0f, glm::vec3(0, 1, 1));
				testUniform->UpdateBuffer(&testUniformStructObj);
				colorTest.Color = glm::vec4(1, 1, 1, 1);
				testUniform2->UpdateBuffer(&colorTest);
				//testMesh->Draw();

				graphics->BindTexture(whiteTexture, 0);

				testModel.Draw();

				_CGraphics->ResetRenderTarget();

				graphics->UnbindTexture(0);

				graphics->PopDebugGroup();
			}



			if (RotateCamera) {
				testUniformStructObj.MatrixView = glm::lookAt(CameraPos + glm::vec3(x, y, 0), glm::vec3(0), glm::vec3(0, 1, 0));
			} else {
				testUniformStructObj.MatrixView = glm::inverse(glm::translate(glm::mat4(1), CameraPos));
			}
			testUniformStructObj.MatrixPV = testUniformStructObj.MatrixProjection * testUniformStructObj.MatrixView;
			//
			//
			////projection = glm::perspective(counter / 128.0f, 4.0f/3.0f, 0.1f, 100.0f);
			////testUniformStructObj.MatrixView = glm::translate(testUniformStructObj.MatrixView, glm::vec3(0, 0.1f, 0));
			//
			////testUniformStructObj.colorTest.g = (counter > 255 ? 512 - counter : counter) / 255.0f;
			//

			graphics->PushDebugGroup("Main Render");
			graphics->SetClearColor(1.0f, 1.0f, 0.0f, 1.0f);
			graphics->Clear();
			testShader->Use();


			graphics->BindTexture(testTexture, 0);
			//testShader->BindTexture("textureTest", 1);

			testUniformStructObj.MatrixModelTest = square1.m_Transform.GetModelMatrix();
			testUniform->UpdateBuffer(&testUniformStructObj);
			colorTest.Color = glm::vec4(1, 0, 0, 1);
			testUniform2->UpdateBuffer(&colorTest);
			squareModel->Draw();

			testUniformStructObj.MatrixModelTest = square2.m_Transform.GetModelMatrix();
			testUniform->UpdateBuffer(&testUniformStructObj);
			colorTest.Color = glm::vec4(0, 1, 0, 1);
			testUniform2->UpdateBuffer(&colorTest);
			squareModel->Draw();

			testUniformStructObj.MatrixModelTest = square3.m_Transform.GetModelMatrix();
			testUniform->UpdateBuffer(&testUniformStructObj);
			colorTest.Color = glm::vec4(0, 0, 1, 1);
			testUniform2->UpdateBuffer(&colorTest);
			squareModel->Draw();

			testUniformStructObj.MatrixModelTest = square4.m_Transform.GetModelMatrix();
			testUniform->UpdateBuffer(&testUniformStructObj);
			colorTest.Color = glm::vec4(1, 1, 1, 1);
			testUniform2->UpdateBuffer(&colorTest);
			squareModel->Draw();

			graphics->BindTexture(whiteTexture, 0);

			testUniformStructObj.MatrixModelTest = treeObj.m_Transform.GetModelMatrix();
			testUniform->UpdateBuffer(&testUniformStructObj);
			testModel.Draw();

			graphics->UnbindTexture(0);

			graphics->PopDebugGroup();
			graphics->PushDebugGroup("ImGui Render");
			graphics->ImGuiDraw();
			graphics->PopDebugGroup();

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

	return 0;
}
