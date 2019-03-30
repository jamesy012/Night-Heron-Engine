#include "ShaderManager.h"

#include "Graphics/ShaderSpirvData.h"
#include "Graphics/API/Shader.h"

#include "ImGui/imgui.h"
#include "Util.h"

ShaderManager::ShaderManager() {
}


ShaderManager::~ShaderManager() {
}


bool ShaderManager::IsFileAOurType(CMString a_FilePath) {
	uchar fileHash[HASH_LENGTH] = { 0 };
	a_FilePath.SubStrFindFromEnd('.').ToLower().Hash(fileHash);

	for (uint i = 0; i < m_ShaderFileTypes.Length(); i++) {
		if (memcmp(m_ShaderFileTypes[i].m_ExtenstionHash, fileHash, HASH_LENGTH) == 0) {
			return true;
		}
	}

	return false;
}

void ShaderManager::AddFromPath_Internal(CMString a_FilePath) {
	ShaderSpirvData* ssd = new ShaderSpirvData();
	ssd->LoadFromFile(a_FilePath.Get());
	m_ShaderObjects.Add(ssd);
	m_UnsavedShaders.Add(false);
}


ShaderSpirvData * ShaderManager::GetShaderPart(CMString a_FilePath) {
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

Shader * ShaderManager::GetShader(CMString a_FilePath) {
	for (uint i = 0; i < m_Shaders.Length(); i++) {
		if (m_Shaders[i]->m_FilePath.m_FilePath.Compare(a_FilePath)) {
			return m_Shaders[i];
		}
	}

	return nullptr;
}

void ShaderManager::ImGuiWindow(bool* a_Open) {
	if (!*a_Open) {
		return;
	}
	ImGui::Begin("Shader Menu", a_Open);

	if (ImGui::BeginTabBar("Tab Bar", ImGuiTabBarFlags_None)) {

		if (ImGui::BeginTabItem("Shader Objects", 0, (m_ChangeToTab == 1) ? ImGuiTabItemFlags_SetSelected : 0)) {
			if (m_ChangeToTab != -1) {
				m_ChangeToTab = -1;
			} else {
				ImGuiWindowTab2();
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Shader Files", 0, (m_ChangeToTab == 0) ? ImGuiTabItemFlags_SetSelected : 0)) {
			if (m_ChangeToTab != -1) {
				m_ChangeToTab = -1;
			} else {
				ImGuiWindowTab1();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void ShaderManager::ImGuiWindowTab1() {
	static CMString ShaderCompileInfo;

	static int nodeSelected = -1;
	if (m_ChangeToIndex != -1) {
		nodeSelected = m_ChangeToIndex;
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
				if (ImGui::Selectable(text.Get(), nodeSelected == i)) {
					nodeSelected = i;
						ShaderCompileInfo = "";
				}
			}
		}

		ImGui::EndChild();
	}
	ImGui::SameLine();
	{
		ImGui::BeginChild("Details", ImVec2(0, 0), true);
		if (nodeSelected != -1) {
			ShaderSpirvData* selected = m_ShaderObjects[nodeSelected];
			CMString selectedPath = m_Paths[nodeSelected];
			ImGui::Text("Selected shader: %i (%s)", nodeSelected, selectedPath.Get());

			if (ImGui::Button("Reload Shader")) {
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

			ImGui::Text("Attached Shaders: %i", selected->m_AttachedShaders.Length());
			if (ImGui::TreeNode("Attached Shaders")) {
				for (uint i = 0; i < selected->m_AttachedShaders.Length(); i++) {
					CMString text = CMString::IntToString(i) + ": " + selected->m_AttachedShaders[i]->GetDebugObjName().Get();
					//ImGui::Text("%i: %s", i, ssd->m_AttachedShaders[i]->GetDebugObjName().Get());
					if (ImGui::Selectable(text.Get())) {
						m_ChangeToIndex = m_Shaders.FindElement(selected->m_AttachedShaders[i]);
						m_ChangeToTab = 1;
					}
				}
				ImGui::TreePop();
			}

		}

		ImGui::EndChild();
	}
}

void ShaderManager::ImGuiWindowTab2() {
	static CMString ShaderCompileInfo;
	static int nodeSelected = -1;
	if (m_ChangeToIndex != -1) {
		nodeSelected = m_ChangeToIndex;
		m_ChangeToIndex = -1;
	}

	{
		static ImGuiTextFilter filter;
		//if (m_UnsavedShaders.Length() != m_Shaders.Length()) {
		//	m_UnsavedShaders.Resize(m_Shaders.Length())
		//}
		
		//float size = ImGui::GetWindowContentRegionWidth() * 0.5f;
		static float size = 150;		
		//ImGui::DragFloat("Size", &size, 1.0f);
		ImGui::BeginChild("Selector", ImVec2(size, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		filter.Draw("", size);

		for (uint i = 0; i < m_Shaders.Length(); i++) {
			CMString text = m_Shaders[i]->GetDebugObjName();
			if (filter.PassFilter(text.Get())) {
				if (m_UnsavedShaders[i]) {
					text += " *";
				}
				if (ImGui::Selectable(text.Get(), nodeSelected == i)) {
					nodeSelected = i;
						ShaderCompileInfo = "";
				}
			}
		}

		ImGui::EndChild();
	}
	ImGui::SameLine();
	{
		ImGui::BeginChild("Details", ImVec2(0, 0), true);
		Shader* selected = m_Shaders[nodeSelected];

		if (selected && nodeSelected != -1) {

			ImGui::Text("Selected shader: %i (%s)", nodeSelected, selected->GetDebugObjName().Get());
			
			bool forceReload = false;

			if (selected->m_FilePath.m_FilePath != "") {
				if (ImGui::Button("Save")) {
					HashHolder temp = selected->m_Hash;
					selected->Save();
						m_UnsavedShaders[nodeSelected] = false;
					if (!temp.Comp(selected->m_Hash)) {
						forceReload = true;
					}
				}
				ImGui::SameLine();
				ImGui::Text("%s", selected->m_FilePath.m_FilePath.Get());
			}


			if (ImGui::Button("Reload Shader") || forceReload) {
				ShaderLoadRes finalRes = ShaderLoadRes::SHADERLOAD_LOAD;
				for (uint i = 0; i < m_Shaders[nodeSelected]->m_ShaderFileObjects.Length(); i++) {
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

			{
				static uint newNode = -1;
				static uint ChangingNode = -1;
				static ImGuiTextFilter popupFilter;


				ImGui::Text("Attached Shaders: %i", m_Shaders[nodeSelected]->m_ShaderFileObjects.Length());
				if (ImGui::TreeNode("Attached Shaders")) {
					for (uint i = 0; i < m_Shaders[nodeSelected]->m_ShaderFileObjects.Length(); i++) {

						ImGui::PushID(i);
						ShaderSpirvData* ssd = m_Shaders[nodeSelected]->m_ShaderFileObjects[i];
						//ImGui::Text("%i: %s", i, ssd->m_FilePath.m_FileName.Get());
						//ImGui::Text("%i: %s", i, ssd->m_AttachedShaders[i]->GetDebugObjName().Get());

						CMString text = CMString::IntToString(i) + ": " + ssd->m_FilePath.m_FileName.Get();
						if (ImGui::Selectable(text.Get(),false, ImGuiSelectableFlags_None, ImVec2(150,0))) {
							m_ChangeToIndex = m_ShaderObjects.FindElement(ssd);
							m_ChangeToTab = 0;
						}
						
						ImGui::SameLine();

						if (ImGui::Button("Change...")) {
							ImGui::OpenPopup("Change Shader");
							newNode = m_ShaderObjects.FindElement(ssd);
							ChangingNode = i;
							
							popupFilter = ImGuiTextFilter(ssd->m_FilePath.m_FilePath.SubStrFindFromEnd('.').Get());
							popupFilter.Build();
						}

						//popup selector
						if (ChangingNode == i && ImGui::BeginPopupModal("Change Shader", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

							ImGui::Text("Current: %s", m_Shaders[nodeSelected]->m_ShaderFileObjects[ChangingNode]->m_FilePath.m_FileName.Get());


							static float size = 150;

							popupFilter.Draw("", size);

							ImGui::BeginChild("Selector", ImVec2(size, 100), false, ImGuiWindowFlags_HorizontalScrollbar);
							for (uint q = 0; q < m_Paths.Length(); q++) {
								CMString text = m_Paths[q];
								if (popupFilter.PassFilter(text.Get())) {
									if (ImGui::Selectable(text.Get(), newNode == q)) {
										newNode = q;
									}
								}
							}
							ImGui::EndChild();
							
							if (ImGui::Button("OK")) {
								m_UnsavedShaders[nodeSelected] = true;

								selected->RemoveShader(ssd);
								selected->AddShader(m_ShaderObjects[newNode]);
								ChangingNode = -1;
								ImGui::CloseCurrentPopup();
							}
							ImGui::SameLine();
							if (ImGui::Button("Close")) {
								ChangingNode = -1;
								ImGui::CloseCurrentPopup();
							}
							ImGui::EndPopup();
						}
						ImGui::PopID();
					}
					ImGui::TreePop();
				}
				
				//

			}
		}

		ImGui::EndChild();
	}
}
