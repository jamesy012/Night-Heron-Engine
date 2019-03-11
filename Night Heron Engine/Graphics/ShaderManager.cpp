#include "ShaderManager.h"

#include "ShaderSpirvData.h"
#include "Graphics/API/Shader.h"

#include <iostream>
#include <filesystem>
namespace fs = std::experimental::filesystem;

#include "ImGui/imgui.h"

ShaderManager::ShaderManager() {
}


ShaderManager::~ShaderManager() {
}

void ShaderManager::FindAllShaders() {
	m_CurrentPath = fs::current_path().generic_string();
	SeachFolder(m_CurrentPath);

	for (int i = 0; i < m_ShaderPaths.Length(); i++) {
		ShaderSpirvData* ssd = new ShaderSpirvData();
		ssd->LoadFromFile(m_ShaderPaths[i].Get());
		m_Shaders.push_back(ssd);
	}
}

void ShaderManager::ImGuiWindow(bool* a_Open) {
	if (!*a_Open) {
		return;
	}
	//static int selection_mask = (1 << 2);
	static int nodeSelected = -1;
	ImGui::Begin("Shader Menu", a_Open);

	ImGui::Text("Selected shader: %i (%s)", nodeSelected, nodeSelected >=0 ? m_ShaderPaths[nodeSelected].Get():"None");

	if(ImGui::Button("Reload Shader") && nodeSelected != -1) {
		m_Shaders[nodeSelected]->Reload();
	}

	if (nodeSelected >= 0) {
		ShaderSpirvData* ssd = m_Shaders[nodeSelected];
		ImGui::Text("Attached Shaders: %i", ssd->m_AttachedShaders.Length());
		if (ImGui::TreeNode("Attached Shaders")) {
			for (int i = 0; i < ssd->m_AttachedShaders.Length(); i++) {
				ImGui::Text("%i: %s", i, ssd->m_AttachedShaders[i]->GetDebugObjName().Get());
			}
			ImGui::TreePop();
		}
	}

	if (ImGui::TreeNode("Current Shaders")) {
		for (int i = 0; i < m_ShaderPaths.Length(); i++) {
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ((nodeSelected == i) ? ImGuiTreeNodeFlags_Selected : 0);
			//ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ((nodeSelected == i) ? ImGuiTreeNodeFlags_Selected : 0);

			ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Shader %s", m_ShaderPaths[i].Get());
			if (ImGui::IsItemClicked()) {
				nodeSelected = i;
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	ImGui::End();
}

ShaderSpirvData * ShaderManager::GetShader(CMString a_FilePath) {
	if (a_FilePath.Length() > 2 && a_FilePath.At(0) != '/') {
		a_FilePath = '/' + a_FilePath;
	}

	for (int i = 0; i < m_ShaderPaths.Length(); i++) {
		if (m_ShaderPaths[i].Compare(a_FilePath)) {
			return m_Shaders[i];
		}
	}

	return nullptr;
}

void ShaderManager::SeachFolder(CMString a_Path) {
	for (const fs::v1::directory_entry & entry : fs::directory_iterator(a_Path)) {
		CMString filePath = entry.path().generic_string();
		if (fs::is_directory(entry.status())) {
			SeachFolder(filePath);
			continue;
		}
		CMString relativePath = filePath.SubStr(m_CurrentPath.Length(), -1);
		if (IsFileAShader(relativePath)) {
			m_ShaderPaths.push_back(relativePath);
		}
	}
}

bool ShaderManager::IsFileAShader(CMString a_FilePath) {
	uchar fileHash[16] = { 0 };
	a_FilePath.SubStrFindFromEnd('.').ToLower().Hash(fileHash);

	for (int i = 0; i < m_FileTypes.Length(); i++) {
		if (memcmp(m_FileTypes[i].m_ExtenstionHash, fileHash, 16) == 0) {
			return true;
		}
	}

	return false;
}
