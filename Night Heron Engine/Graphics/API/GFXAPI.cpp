#include "GFXAPI.h"

#include <Dependency/ImGui/imgui.h>
#include <Window.h>


//namespace Constants {
	extern GFX* m_CurrentGraphics = nullptr;
//}

	void GFX::ImGuiInit() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		
		ImGui::StyleColorsDark();
		
		m_Window->ImGuiInit();
		InitImGui_Internal();
	}

	void GFX::ResizeWindow(int a_Width, int a_Height) {
		ResizeWindow_Internal(a_Width, a_Height);

		//callbacks

	}