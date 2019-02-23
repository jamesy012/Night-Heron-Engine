
#define NOMINMAX 
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <chrono>

#include "Graphics/OpenGL/GFXOpenGL.h"
#include "Graphics/DX11/GFXDX11.h"

#include "Graphics/API/Shader.h"
#include "Graphics/API/Mesh.h"
#include "Graphics/API/Texture.h"
#include "Window.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include <imgui-master\imgui.h>

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

	Shader* testShader = graphics->CreateShader();
	testShader->m_ShouldPrintCode = true;
	Mesh* testMesh = graphics->CreateMesh();
	Texture* testTexture = graphics->CreateTexture();

	testTexture->loadTexture("peacock-2.jpg");

	testShader->AddShader(ShaderTypes::SHADER_VERTEX, "test.vert");
	testShader->AddShader(ShaderTypes::SHADER_FRAGMENT, "test.frag");
	//testShader->AddShader(ShaderTypes::SHADER_VERTEX, "simple.vert");
	//testShader->AddShader(ShaderTypes::SHADER_FRAGMENT, "simple.frag");
	testShader->LinkShaders();

	testMesh->CreateTriangle();
	testMesh->Bind();

	TestUniformStruct testUniformStructObj;
	TestUniformStruct2 colorTest;
	colorTest.Color = glm::vec4(1, 0, 1, 1);

	testUniformStructObj.MatrixModelTest = glm::mat4(1.0f);

	//testUniformStructObj.MatrixView = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0), glm::vec3(0, 1, 0));
	//projection = glm::perspective(90.0f, (float)(graphics->m_Window->m_WindowWidth / graphics->m_Window->m_WindowHeight), 0.1f, 100.0f);
	//testUniformStructObj.MatrixProjection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 100.0f);
	//testUniformStructObj.MatrixProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.0f, 100.0f);

	testUniformStructObj.MatrixModelTest = glm::translate(testUniformStructObj.MatrixModelTest, glm::vec3(1, 1, -1));
	ShaderUniformBlock* testUniform = graphics->CreateBuffer(&testUniformStructObj, sizeof(TestUniformStruct));
	testShader->AddBuffer(testUniform, "Vertex_Data");
	testUniform->UpdateBuffer(&testUniformStructObj);

	ShaderUniformBlock* testUniform2 = graphics->CreateBuffer(&colorTest, sizeof(TestUniformStruct2));
	testShader->AddBuffer(testUniform2, "shader_data");
	testUniform2->UpdateBuffer(&colorTest);

	graphics->ImGuiInit();



	std::chrono::high_resolution_clock timer;
	float deltaTime = 0;
	float currentTime = 0;

	while (true) {
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			auto timerStart = timer.now();
			currentTime += deltaTime;


			graphics->ImGuiNewFrame();

			ImGui::Begin("Stats");
			ImGui::Text("Current Time: %f", currentTime);
			ImGui::Text("Delta Time: %f", deltaTime);
			ImGui::End();

			static int counter = 0;
			//counter = (counter + 1) % 512;
			counter++;

			float x = glm::sin(counter / 64.0f) * 5;
			float y = glm::cos(counter / 82.0f) * 3;
			//
			testUniformStructObj.MatrixProjection = glm::perspective(glm::radians(45.0f), (float)graphics->m_Window->m_WindowWidth / graphics->m_Window->m_WindowHeight, 1.0f, 100.0f);
			//
			testUniformStructObj.MatrixView = glm::lookAt(glm::vec3(x, y, 5), glm::vec3(0), glm::vec3(0, 1, 0));
			testUniformStructObj.MatrixPV = testUniformStructObj.MatrixProjection * testUniformStructObj.MatrixView;
			//
			//
			////projection = glm::perspective(counter / 128.0f, 4.0f/3.0f, 0.1f, 100.0f);
			////testUniformStructObj.MatrixView = glm::translate(testUniformStructObj.MatrixView, glm::vec3(0, 0.1f, 0));
			//
			////testUniformStructObj.colorTest.g = (counter > 255 ? 512 - counter : counter) / 255.0f;
			//
			graphics->SetClearColor(1.0f, 1.0f, 0.0f, 1.0f);
			graphics->Clear();

			testShader->Use();

			testTexture->bind(0);
			testShader->BindTexture("textureTest", 0);

			testUniformStructObj.MatrixModelTest = glm::mat4(1.0f);
			testUniformStructObj.MatrixModelTest = glm::translate(testUniformStructObj.MatrixModelTest, glm::vec3(-2.0f, 0, 0));
			testUniform->UpdateBuffer(&testUniformStructObj);
			colorTest.Color = glm::vec4(1, 0, 0, 1);
			testUniform2->UpdateBuffer(&colorTest);
			testMesh->Draw();

			testUniformStructObj.MatrixModelTest = glm::mat4(1.0f);
			testUniform->UpdateBuffer(&testUniformStructObj);
			colorTest.Color = glm::vec4(0, 1, 0, 1);
			testUniform2->UpdateBuffer(&colorTest);
			testMesh->Draw();

			testUniformStructObj.MatrixModelTest = glm::mat4(1.0f);
			testUniformStructObj.MatrixModelTest = glm::translate(testUniformStructObj.MatrixModelTest, glm::vec3(2.0f, 0, 0));
			testUniform->UpdateBuffer(&testUniformStructObj);
			colorTest.Color = glm::vec4(0, 0, 1, 1);
			testUniform2->UpdateBuffer(&colorTest);
			testMesh->Draw();

			graphics->ImGuiDraw();

			graphics->SwapBuffer();

			//Sleep(1.0 / 60.0f * 1000);

			auto timerEnd = timer.now();
			deltaTime = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1, 1>>>(timerEnd - timerStart).count();
		}

	}

	graphics->DestroyMainWindow();

	delete graphics;

	return 0;
}
