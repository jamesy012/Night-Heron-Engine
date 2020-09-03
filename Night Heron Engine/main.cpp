#include "stdafx.h"
#define NOMINMAX
#include <Windows.h>
#include <chrono>


#include "Singletons.h"
#include "Managers/IniFile.h"
#include "Managers/Arguments.h"
#include "Managers/TimeManager.h"
#include "Managers/Manager.h"
#include "Generated/Objects_Night_Heron_Engine.h"
#include "Debug.h"

#include "Graphics/OpenGL/GFXOpenGL.h"
#include "Graphics/DX11/GFXDX11.h"

#include "Input/InputHandler.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include "jsonConverter/JCglm.h"

#include <Dependency/ImGui/imgui.h>
#include "nlohmann/json.hpp"


#include "_GameTest/Game1/Game1.h"
//#include "_GameTest/Game2/Game2.h"


AppBase* GameCurrent = nullptr;



int WINAPI WinMain(_In_ HINSTANCE hInstance,
				   _In_opt_ HINSTANCE hPrevInstance,
				   _In_ LPSTR lpCmdLine,
				   _In_ int nShowCmd) {

	_CIniFileManager = new IniFile();

	_CArguments = new Arguments();
	_CArguments->Generate(lpCmdLine);

	GENERATED_OBJ::RegisterClasses();

	if (GameCurrent == nullptr && _CArguments->IsArgument("Game1")) {
		GameCurrent = new Game1();
	}
	if (GameCurrent == nullptr && _CArguments->IsArgument("Game2")) {
		//GameCurrent = new Game2();
	}
	if (GameCurrent == nullptr) {
		GameCurrent = new Game1();
	}

	_CIniFileManager->LoadIni();

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

	_CGraphics->ImGuiInit();


	std::chrono::high_resolution_clock timer;
	float deltaTime = 0;
	float currentTime = 0;

	{
		CMLOG_SCOPED_NAME("START");
		CMLOG_SCOPED_INDENT();
		CMLOG_SCOPED_INDENT();
		CMLOG("\n\nProgram starting!\n\n");
	}

	GameCurrent->Start();

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

			GameCurrent->Update();

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

			GameCurrent->ImguiDraw();


			ImGui::ColorEdit4("Clear Color", &ClearColour.x, 0.1f);

			//static int ShaderVisualizer = 0;
			//ImGui::Combo("Visualize", &ShaderVisualizer, "Normal\0Spec\0Bump\0");
			ImGui::End();

			_CGraphics->ResetRenderTarget();

			graphics->PushDebugGroup("Main Render");
			graphics->SetClearColor(ClearColour.r, ClearColour.g, ClearColour.b, ClearColour.a);
			graphics->Clear();

			GameCurrent->Draw();

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

	GameCurrent->Finish();
	delete GameCurrent;

	graphics->DestroyMainWindow();

	{//save camera to ini
		nlohmann::json& j = _CIniFileManager->getIniData()["General"]["Camera"];

		_CIniFileManager->getIniData()["General"]["ClearColour"] = ClearColour;
	}

	delete graphics;


	_CManager->UpdateIniFile();
	_CIniFileManager->SaveIni();

	return 0;
}


