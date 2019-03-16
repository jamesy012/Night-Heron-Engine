#include "ShaderManager.h"

#include "ShaderSpirvData.h"
#include "Graphics/API/Shader.h"

#include <iostream>
#include <filesystem>
namespace fs = std::experimental::filesystem;

#include "ImGui/imgui.h"
#include "Util.h"

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
		m_ShaderObjects.push_back(ssd);
	}
}

void ShaderManager::ImGuiWindow(bool* a_Open) {
	if (!*a_Open) {
		return;
	}

	static CMString ShaderCompileInfo;
	ImGui::Begin("Shader Menu", a_Open);
	if (ImGui::BeginTabBar("Tab Bar", ImGuiTabBarFlags_None)) {
		if (ImGui::BeginTabItem("Shader Files")) {
			
			static int nodeSelected = -1;
			{
				//float size = ImGui::GetWindowContentRegionWidth() * 0.5f;
				static float size = 150;
				//ImGui::DragFloat("Size", &size, 1.0f);
				ImGui::BeginChild("Selector", ImVec2(size, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
				for (uint i = 0; i < m_ShaderPaths.Length(); i++) {
					CMString text = m_ShaderPaths[i].Get();
					if (ImGui::Selectable(text.Get(), nodeSelected == i)) {
						nodeSelected = i;
						ShaderCompileInfo = "";
					}
				}

				ImGui::EndChild();
			}
			ImGui::SameLine();
			{
				ImGui::BeginChild("Details", ImVec2(0, 0), true);

				ImGui::Text("Selected shader: %i (%s)", nodeSelected, nodeSelected >= 0 ? m_ShaderPaths[nodeSelected].Get() : "None");

				if (ImGui::Button("Reload Shader") && nodeSelected != -1) {
					ShaderLoadRes res = m_ShaderObjects[nodeSelected]->Reload();
					switch (res) {
						case ShaderLoadRes::SHADERLOAD_COMPILED:
							ShaderCompileInfo = "Compiled from source";
							break;
						case ShaderLoadRes::SHADERLOAD_ERROR:
							ShaderCompileInfo = "Error compiling";
							break;
						case ShaderLoadRes::SHADERLOAD_LOAD:
							ShaderCompileInfo = "shader unchanged";
							break;
					}
				}
				ImGui::SameLine();
				ImGui::Text(ShaderCompileInfo.Get());

				if (nodeSelected >= 0) {
					ShaderSpirvData* ssd = m_ShaderObjects[nodeSelected];
					ImGui::Text("Attached Shaders: %i", ssd->m_AttachedShaders.Length());
					if (ImGui::TreeNode("Attached Shaders")) {
						for (int i = 0; i < ssd->m_AttachedShaders.Length(); i++) {
							ImGui::Text("%i: %s", i, ssd->m_AttachedShaders[i]->GetDebugObjName().Get());
						}
						ImGui::TreePop();
					}
				}

				ImGui::EndChild();
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Shader Objects")) {
			
			static int nodeSelected = -1;
			{
				//float size = ImGui::GetWindowContentRegionWidth() * 0.5f;
				static float size = 150;
				//ImGui::DragFloat("Size", &size, 1.0f);
				ImGui::BeginChild("Selector", ImVec2(size, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
				for (uint i = 0; i < m_Shaders.Length(); i++) {
					CMString text = m_Shaders[i]->GetDebugObjName().Get();
					if (ImGui::Selectable(text.Get(), nodeSelected == i)) {
						nodeSelected = i;
						ShaderCompileInfo = "";
					}
				}

				ImGui::EndChild();
			}
			ImGui::SameLine();
			{
				ImGui::BeginChild("Details", ImVec2(0, 0), true);

				ImGui::Text("Selected shader: %i (%s)", nodeSelected, nodeSelected >= 0 ? m_Shaders[nodeSelected]->GetDebugObjName().Get() : "None");

				if (ImGui::Button("Reload Shader") && nodeSelected != -1) {
					ShaderLoadRes finalRes = ShaderLoadRes::SHADERLOAD_LOAD;
					for (int i = 0; i < m_Shaders[nodeSelected]->m_ShaderFileObjects.Length(); i++) {
						ShaderLoadRes res = m_Shaders[nodeSelected]->m_ShaderFileObjects[i]->Reload();
						if (res == ShaderLoadRes::SHADERLOAD_COMPILED && finalRes == ShaderLoadRes::SHADERLOAD_LOAD) {
							finalRes = ShaderLoadRes::SHADERLOAD_COMPILED;
						}
						if (res == ShaderLoadRes::SHADERLOAD_ERROR) {
							finalRes = ShaderLoadRes::SHADERLOAD_ERROR;
						}
					}
					switch (finalRes) {
						case ShaderLoadRes::SHADERLOAD_COMPILED:
							ShaderCompileInfo = "Compiled from source";
							break;
						case ShaderLoadRes::SHADERLOAD_ERROR:
							ShaderCompileInfo = "Error compiling";
							break;
						case ShaderLoadRes::SHADERLOAD_LOAD:
							ShaderCompileInfo = "shader unchanged";
							break;
					}
				}
				ImGui::SameLine();
				ImGui::Text(ShaderCompileInfo.Get());

				if (nodeSelected >= 0) {
						ImGui::Text("Attached Shaders: %i", m_Shaders[nodeSelected]->m_ShaderFileObjects.Length());
						if (ImGui::TreeNode("Attached Shaders")) {
					for (int i = 0; i < m_Shaders[nodeSelected]->m_ShaderFileObjects.Length(); i++) {
						ShaderSpirvData* ssd = m_Shaders[nodeSelected]->m_ShaderFileObjects[i];
								ImGui::Text("%i: %s", i, ssd->m_FilePath.m_FileName.Get());
						}
							ImGui::TreePop();
					}
				}

				ImGui::EndChild();
			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}

ShaderSpirvData * ShaderManager::GetShader(CMString a_FilePath) {
	if (a_FilePath.Length() > 2 && a_FilePath.At(0) != '/') {
		a_FilePath = '/' + a_FilePath;
	}

	for (int i = 0; i < m_ShaderPaths.Length(); i++) {
		if (m_ShaderPaths[i].Compare(a_FilePath)) {
			return m_ShaderObjects[i];
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
	uchar fileHash[HASH_LENGTH] = { 0 };
	a_FilePath.SubStrFindFromEnd('.').ToLower().Hash(fileHash);

	for (int i = 0; i < m_ShaderFileTypes.Length(); i++) {
		if (memcmp(m_ShaderFileTypes[i].m_ExtenstionHash, fileHash, HASH_LENGTH) == 0) {
			return true;
		}
	}

	return false;
}
