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

#include "Generated/Objects_Night_Heron_Engine.h"

ADD_OBJ(TestingOBJ)
class TestingOBJ : public Object {
public:
	TestingOBJ() {
		printf("TESTING_OBJ");
	}

};

int WINAPI WinMain(_In_ HINSTANCE hInstance,
				   _In_opt_ HINSTANCE hPrevInstance,
				   _In_ LPSTR lpCmdLine,
				   _In_ int nShowCmd)
{
	_CArguments = new Arguments();
	_CArguments->Generate(lpCmdLine);




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

	REGISTER_OBJ(TestingOBJ);
	CMString loadFromFileTest = "TestingOBJ";
	Object* obj;
	obj = (TestingOBJ*)GET_OBJ(loadFromFileTest.Get());


	if (!graphics->CreateWindowSetUpAPI()) {
		return -1;
	}

	SingletonManager::CreateSingletons();

	_CGraphics->SetUpGraphics();

	Camera mainCamera;

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

	Texture * testTexture = _CTextureManager->GetTexture("peacock-2.jpg");
	if (testTexture == nullptr) {
		testTexture = graphics->CreateTexture();
		testTexture->LoadTexture("peacock-2.jpg");
		testTexture->SetDebugObjName("Test Texture");
	}

	//UNIFORMS
	TestUniformStruct testUniformStructObj;
	TestUniformStruct2 colorTest;
	CommonDataStruct commonPerFrameData;
	colorTest.Color = glm::vec4(1, 0, 1, 1);
	ShaderUniformBlock* testUniform = graphics->CreateBuffer(&testUniformStructObj, sizeof(TestUniformStruct));
	testUniform->SetDebugObjName("MVP Buffer");
	_CManager->RegisterShaderUniform(testUniform, "Vertex_Data");
	ShaderUniformBlock* testUniform2 = graphics->CreateBuffer(&colorTest, sizeof(TestUniformStruct2));
	testUniform2->SetDebugObjName("Color Test Buffer");
	_CManager->RegisterShaderUniform(testUniform2, "shader_data");
	ShaderUniformBlock* commonDataBlock = graphics->CreateBuffer(&commonPerFrameData, sizeof(CommonDataStruct));
	commonDataBlock->SetDebugObjName("Common Data Buffer");
	_CManager->RegisterShaderUniform(commonDataBlock, "CommonData");

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
			testShader->AddBuffer(testUniform2, "shader_data");
			testShader->AddBuffer(commonDataBlock, "CommonData");

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
			treeShader->AddBuffer(testUniform2, "shader_data");
			treeShader->AddBuffer(commonDataBlock, "CommonData");

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

	Model testModel;
	testModel.LoadModel("Models/Low Poly Forest Decoration Pack/Trees/FBX Files/Tree 1.2/Tree1.2.fbx");
	//testModel.LoadModel("Models/nanosuit.obj");

	Material treeModelMat1("Material/TreeModelMat1.mat");
	if (!treeModelMat1.Load()) {
		treeModelMat1.SetDebugObjName("Tree Model Mat 1");
		treeModelMat1.m_Shader = treeShader;
		treeModelMat1.AddTexture(_CGraphics->m_WhiteTexture, 0);
		treeModelMat1.Save();
	}
	Material treeModelMat2("Material/TreeModelMat2.mat");
	if (!treeModelMat2.Load()) {
		treeModelMat2.SetDebugObjName("Tree Model Mat 2");
		treeModelMat2.m_Shader = testShader;
		treeModelMat2.AddTexture(testTexture, 0);
		treeModelMat2.Save();
	}
	testModel.SetMaterial(&treeModelMat1, 0);
	testModel.SetMaterial(&treeModelMat2, 1);
	squareModel->SetMaterial(&treeModelMat2, 0);

	//for (uint i = 0; i < _CManager->m_Materials.Length(); i++) {
	//	if (_CManager->m_Materials[i]->m_CreatedShader) {
	//		Shader* shader = _CManager->m_Materials[i]->m_Shader;
	//		shader->AddBuffer(testUniform, "Vertex_Data");
	//		shader->AddBuffer(testUniform2, "shader_data");
	//		shader->AddBuffer(commonDataBlock, "CommonData");
	//	}
	//}

	//todo remove this
	_CManager->tempPVMUniform = testUniform;

	graphics->ImGuiInit();

	std::chrono::high_resolution_clock timer;
	float deltaTime = 0;
	float currentTime = 0;

	float fov = 90;

	mainCamera.SetFov(fov);
	mainCamera.SetAspectRatio(graphics->m_Window->GetAspect());

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
			if (ImGui::DragFloat("FOV", &fov, 1, 0, 180)) {
				mainCamera.SetFov(fov);
			}
			if (testRT) {
				ImGui::Image(testRT->GetTexture()->getTexturePtr(), ImVec2(200, 200), _CGraphics->GetImGuiImageUV0(), _CGraphics->GetImGuiImageUV1());
			}
			ImGui::Checkbox("Rotate Camera", &RotateCamera);
			ImGui::DragFloat3("Camera Pos", &CameraPos.x, 0.25f);
			ImGui::End();

			commonPerFrameData.time = currentTime;
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

			testUniformStructObj.MatrixModelTest = treeObj.m_Transform.GetModelMatrix();
			testUniform->UpdateBuffer(&testUniformStructObj);
			testModel.Draw();

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

	return 0;
}
