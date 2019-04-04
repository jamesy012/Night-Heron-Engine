#include "ShaderSpirvManager.h"

#include "Graphics/ShaderSpirvData.h"
#include "Graphics/API/Shader.h"

#include "ImGui/imgui.h"
#include "Util.h"

#include "Managers/ShaderManager.h"

#include "Debug.h"

ShaderSpirvManager::ShaderSpirvManager() {
	m_WindowName = "Shader Spirv Manager";
}

ShaderSpirvManager::~ShaderSpirvManager() {
}

void ShaderSpirvManager::ImGuiData() {
	static CMString ShaderCompileInfo;

	if (m_ChangeToIndex != -1) {
		m_NodeSelected = m_ChangeToIndex;
		m_ChangeToIndex = -1;
	}
	{
		static ImGuiTextFilter filter;
		//float size = ImGui::GetWindowContentRegionWidth() * 0.5f;
		static float size = 150;
		//ImGui::DragFloat("Size", &size, 1.0f);
		ImGui::BeginChild("Selector", ImVec2(size, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		filter.Draw("", size);

		for (uint i = 0; i < m_Paths.Length(); i++) {
			CMString text = m_Paths[i];
			if (filter.PassFilter(text.Get())) {
				if (ImGui::Selectable(text.Get(), m_NodeSelected == i)) {
					m_NodeSelected = i;
					ShaderCompileInfo = "";
				}
			}
		}

		ImGui::EndChild();
	}
	ImGui::SameLine();
	{
		ImGui::BeginChild("Details", ImVec2(0, 0), true);
		if (m_NodeSelected != -1) {
			ShaderSpirvData* selected = m_ShaderObjects[m_NodeSelected];
			CMString selectedPath = m_Paths[m_NodeSelected];
			ImGui::Text("Selected shader: %i (%s)", m_NodeSelected, selectedPath.Get());

			if (ImGui::Button("Reload Shader")) {
				ShaderLoadRes res = m_ShaderObjects[m_NodeSelected]->Reload();
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

			ImGui::Text("Attached Shaders: %i", selected->m_AttachedShaders.Length());
			if (ImGui::TreeNode("Attached Shaders")) {
				for (uint i = 0; i < selected->m_AttachedShaders.Length(); i++) {
					CMString text = CMString::IntToString(i) + ": " + selected->m_AttachedShaders[i]->GetDebugObjName().Get();
					//ImGui::Text("%i: %s", i, ssd->m_AttachedShaders[i]->GetDebugObjName().Get());
					if (ImGui::Selectable(text.Get())) {
						_CShaderManager->m_NodeSelected =_CShaderManager->FindShader(selected->m_AttachedShaders[i]);
						_CShaderManager->m_ChangeToTab = 1;
					}
				}
				ImGui::TreePop();
			}

		}

		ImGui::EndChild();
	}
}

uint ShaderSpirvManager::FindElement(CMString a_FilePath) {
	CMASSERT(true);
	return -1;
}

ShaderSpirvData * ShaderSpirvManager::GetShaderPart(CMString a_FilePath) {
	if (a_FilePath.Length() > 2 && a_FilePath.At(0) != '/') {
		a_FilePath = '/' + a_FilePath;
	}

	for (uint i = 0; i < m_Paths.Length(); i++) {
		if (m_Paths[i].Compare(a_FilePath)) {
			return m_ShaderObjects[i];
		}
	}

	return nullptr;
}

bool ShaderSpirvManager::IsFileAOurType(CMString a_FilePath) {
	uchar fileHash[HASH_LENGTH] = { 0 };
	a_FilePath.SubStrFindFromEnd('.').ToLower().Hash(fileHash);

	for (uint i = 0; i < m_FileTypes.Length(); i++) {
		if (memcmp(m_FileTypes[i].m_ExtenstionHash, fileHash, HASH_LENGTH) == 0) {
			return true;
		}
	}

	return false;
}

void ShaderSpirvManager::AddFromPath_Internal(CMString a_FilePath) {
	ShaderSpirvData* ssd = new ShaderSpirvData();
	ssd->LoadFromFile(a_FilePath.Get());
	m_ShaderObjects.Add(ssd);
}
