#include "stdafx.h"
#include "TextureManager.h"

#include "Util.h"
#include "ImGui/imgui.h"

#include "Graphics/API/GFXAPI.h"
#include "Graphics/API/Texture.h"

void TextureManager::ImGuiWindow(bool* a_Open) {
	if (!*a_Open) {
		return;
	}

	ImGui::Begin("Texture Menu", a_Open);
	static int nodeSelected = -1;
	{
		//float size = ImGui::GetWindowContentRegionWidth() * 0.5f;
		static float size = 150;
		//ImGui::DragFloat("Size", &size, 1.0f);
		ImGui::BeginChild("Selector", ImVec2(size, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		for (uint i = 0; i < m_Textures.Length(); i++) {
			CMString text;
			if (m_Textures[i].m_PassedThrough) {
				text = m_Textures[i].m_Texture->GetDebugObjName();
			} else {
				text = m_Textures[i].m_FilePath.Get();
			}
			if (ImGui::Selectable(text.Get(), nodeSelected == i)) {
				nodeSelected = i;
			}
		}

		ImGui::EndChild();
	}
	ImGui::SameLine();
	{
		ImGui::BeginChild("Details", ImVec2(0, 0), true);

		if (nodeSelected >= 0) {
			TextureRef obj = m_Textures[nodeSelected];
			Texture* texture = m_Textures[nodeSelected].m_Texture;

			CMString text;
			if (obj.m_PassedThrough) {
				text = texture->GetDebugObjName();
			} else {
				text = obj.m_FilePath.Get();
			}
			ImGui::Text("Selected Texture: %s", text.Get());

			{
				if (obj.m_PassedThrough) {
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				if (ImGui::Button("Load Texture")) {
					texture = m_Textures[nodeSelected].m_Texture = _CGraphics->CreateTexture();
					texture->LoadTexture(obj.m_FilePath);
				}

				if (obj.m_PassedThrough) {
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}
			}

			if (texture != nullptr) {
				ImGui::Image(texture->getTexturePtr(), ImVec2(200, 200), ImVec2(0, 0), ImVec2(1, 1));
			}
		}

		ImGui::EndChild();
	}

	ImGui::End();
}

void TextureManager::ImGuiData() {
}

Texture* TextureManager::GetTexture(CMString a_Path) {
	if (a_Path[0] != '/') {
		a_Path = '/' + a_Path;
	}
	for (uint i = 0; i < m_Paths.Length(); i++) {
		if (m_Paths[i].Compare(a_Path)) {
			if (m_Textures[i].m_Texture == nullptr) {
				Texture* tex = m_Textures[i].m_Texture = _CGraphics->CreateTexture();
				tex->LoadTexture(m_Textures[i].m_FilePath);
			}
			return m_Textures[i].m_Texture;
		}
	}
	return nullptr;
}

void TextureManager::AddTexture(Texture* a_Texture) {
	m_Textures.Add({ a_Texture, true, a_Texture->GetDebugObjName() });
	if (a_Texture->GetPath()[0] != '/') {
		m_Paths.Add("/" + a_Texture->GetPath());
	} else {
		m_Paths.Add(a_Texture->GetPath());
	}
}

bool TextureManager::IsFileAOurType(CMString a_FilePath) {
	uchar fileHash[HASH_LENGTH] = { 0 };
	a_FilePath.SubStrFindFromEnd('.').ToLower().Hash(fileHash);

	for (uint i = 0; i < m_FileTypes.Length(); i++) {
		if (memcmp(m_FileTypes[i].m_ExtenstionHash, fileHash, HASH_LENGTH) == 0) {
			return true;
		}
	}

	return false;
}

void TextureManager::AddFromPath_Internal(CMString a_FilePath) {
	if (a_FilePath[0] == '\\' || a_FilePath[0] == '/') {
		m_Textures.Add({ nullptr, false, a_FilePath.SubStr(1, a_FilePath.Length()) });
	} else {
		m_Textures.Add({ nullptr, false, a_FilePath });
	}
}
