#include "GFXAPI.h"

#include <Dependency/ImGui/imgui.h>
#include <Window.h>

#include "Texture.h"
#include "Managers/TextureManager.h"

void GFX::ImGuiInit() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	m_Window->ImGuiInit();
	InitImGui_Internal();
}

void GFX::SetUpGraphics() {
	m_WhiteTexture = CreateTexture();
	m_WhiteTexture->CreateTexture(1, 1);
	m_WhiteTexture->SetDebugObjName("White Texture");
	m_WhiteTexture->m_Path = "GEN_White Texture";
	_CTextureManager->AddTexture(m_WhiteTexture);
}

void GFX::ResizeWindow(int a_Width, int a_Height) {
	ResizeWindow_Internal(a_Width, a_Height);

	//callbacks

}