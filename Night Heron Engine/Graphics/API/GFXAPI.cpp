#include "stdafx.h"
#include "GFXAPI.h"

#include <Dependency/ImGui/imgui.h>
#include <Window.h>

#include "Texture.h"
#include "Managers/TextureManager.h"

#include "Managers/Arguments.h"

#include "RenderTarget.h"

void GFX::ImGuiInit() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	if (_CArguments->IsArgument("ImGuiPath")) {
		io.IniFilename = _CArguments->GetArgumentValue("ImGuiPath").Get();
	}

	ImGui::StyleColorsDark();

	m_Window->ImGuiInit();
	InitImGui_Internal();
}

void GFX::UseRenderTarget(RenderTarget* a_Rt, SimpleBox a_ViewPort) {
	a_Rt->Bind();
	SetViewPort(a_ViewPort);
}

void GFX::UseRenderTarget(RenderTarget* a_Rt) {
	UseRenderTarget(a_Rt, SimpleBox(0, 0, a_Rt->GetWidth(), a_Rt->GetHeight()));
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
