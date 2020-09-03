#include "stdafx.h"
#include "ShaderManager.h"

#include "Graphics/ShaderSpirvData.h"
#include "Graphics/API/Shader.h"
#include "Graphics/API/GFXAPI.h"

#include "ImGui/imgui.h"
#include "Util.h"

#include "Managers/ShaderSpirvManager.h"

#include "Debug.h"

#include "Helpers/ImGuiHelper.h"

ShaderManager::ShaderManager() {
}

ShaderManager::~ShaderManager() {
	for (uint i = 0; i < m_Shaders.Length(); i++) {
		if (!m_Shaders[i].m_PassedThrough) {
			delete m_Shaders[i].m_Shader;
		}
	}
}

uint ShaderManager::FindShader(Shader* a_Shader) {
	for (uint i = 0; i < m_Shaders.Length(); i++) {
		if (m_Shaders[i].m_Shader == a_Shader) {
			return i;
		}
	}
	return -1;
}

uint ShaderManager::FindElement(CMString a_FilePath) {
	for (uint i = 0; i < m_Shaders.Length(); i++) {
		if (m_Shaders[i].m_Shader->m_FilePath.m_FilePath.Compare(a_FilePath)) {
			return i;
		}
	}
	return -1;
}

bool ShaderManager::IsFileAOurType(CMString a_FilePath) {
	uchar fileHash[HASH_LENGTH] = { 0 };
	a_FilePath.SubStrFindFromEnd('.').ToLower().Hash(fileHash);

	for (uint i = 0; i < m_FileTypes.Length(); i++) {
		if (memcmp(m_FileTypes[i].m_ExtenstionHash, fileHash, HASH_LENGTH) == 0) {
			return true;
		}
	}

	return false;
}

void ShaderManager::AddFromPath_Internal(CMString a_FilePath) {
	Shader* shader = _CGraphics->CreateShader();
	if (a_FilePath[0] == '\\' || a_FilePath[0] == '/') {
		shader->m_FilePath = a_FilePath.SubStr(1, a_FilePath.Length());
	} else {
		shader->m_FilePath = a_FilePath;
	}

	if (shader->Load()) {
		m_Shaders.Add({ shader, false });
		m_UnsavedShaders.Add(false);
	} else {
		CMASSERT_MSG(true, "Failed To Load shader!");
	}
}

Shader* ShaderManager::GetShader(CMString a_FilePath) {
	for (uint i = 0; i < m_Shaders.Length(); i++) {
		if (m_Shaders[i].m_Shader->m_FilePath.m_FilePath.Compare(a_FilePath)) {
			return m_Shaders[i].m_Shader;
		}
	}

	return nullptr;
}

Shader* ShaderManager::GetShader(uint a_Index) {
	return m_Shaders[a_Index].m_Shader;
}

void ShaderManager::AddShader(Shader* a_Shader) {
	m_Shaders.Add({ a_Shader, true });
	m_UnsavedShaders.Add(false);
}

void ShaderManager::RemoveShader(Shader* a_Shader) {
	for (uint i = 0; i < m_Shaders.Length(); i++) {
		if (m_Shaders[i].m_Shader == a_Shader) {
			m_Shaders.RemoveAt(i);
		}
	}
}

void ShaderManager::ImGuiSelector(uint* a_ID, SimpleVec2 a_Size) {
	ImGui::BeginChild("Selector", a_Size, false, ImGuiWindowFlags_HorizontalScrollbar);
	for (uint q = 0; q < m_Paths.Length(); q++) {
		CMString text = m_Paths[q];
		//if (popupFilter.PassFilter(text.Get())) {
		if (ImGui::Selectable(text.Get(), (*a_ID) == q)) {
			(*a_ID) = q;
		}
		//}
	}
	ImGui::EndChild();
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
				ImGuiData();
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Shader Files", 0, (m_ChangeToTab == 0) ? ImGuiTabItemFlags_SetSelected : 0)) {
			if (m_ChangeToTab != -1) {
				m_ChangeToTab = -1;
			} else {
				_CShaderSpirvManager->ImGuiData();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void ShaderManager::ImGuiData() {
	static CMString ShaderCompileInfo;
	if (m_ChangeToIndex != -1) {
		m_NodeSelected = m_ChangeToIndex;
		m_ChangeToIndex = -1;
	}

	const uint nameMaxLength = 32;
	static char shaderName[nameMaxLength] = "";

	enum class MODALS {
		NONE = -1,
		SAVE_AS = 1
	};
	static MODALS modal = MODALS::NONE;
	static bool showingModal = false;

	static bool ForceSave = false;

	{
		//float size = ImGui::GetWindowContentRegionWidth() * 0.5f;
		static float size = 150;
		//ImGui::DragFloat("Size", &size, 1.0f);
		ImGui::BeginChild("Selector", ImVec2(size, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		CMArray<CMString> names;
		for (uint i = 0; i < m_Shaders.Length(); i++) {
			CMString name = m_Shaders[i].m_Shader->GetDebugObjName();
			if (m_UnsavedShaders[i]) {
				name += " *";
			}
			names.Add(name);
		}
		if (ImGuiHelper::Selectable(&m_NodeSelected, names)) {
			ShaderCompileInfo = "";
			strcpy_s(shaderName, m_Shaders[m_NodeSelected].m_Shader->GetDebugObjName().Get());
		}


		if (ImGui::Button("Add Shader")) {
			Shader* shader = _CGraphics->CreateShader();
			AddShader(shader);
			shader->SetDebugObjName("New Shader.");
		}

		ImGui::EndChild();
	}
	ImGui::SameLine();
	{
		if (m_Shaders.Length() == 0) {
			return;
		}
		ImGui::BeginChild("Details", ImVec2(0, 0), true);
		Shader* selected = m_Shaders[m_NodeSelected].m_Shader;

		if (selected && m_NodeSelected != -1) {
			//ImGui::Text("Selected shader: %i (%s)", m_NodeSelected, selected->GetDebugObjName().Get());
			if (ImGui::InputText("", shaderName, nameMaxLength)) {
				m_UnsavedShaders[m_NodeSelected] = true;
			}

			bool forceReload = false;

			if (selected->m_FilePath.m_FilePath != "") {
				if (ImGui::Button("Save")) {
					ForceSave = true;
				}

			}
			ImGui::SameLine();
			if (ImGui::Button("Save as...")) {
				modal = MODALS::SAVE_AS;

			}
			ImGui::SameLine();
			ImGui::Text("%s", selected->m_FilePath.m_FilePath.Get());

			if (ForceSave) {
				selected->SetDebugObjName(shaderName);
				//HashHolder temp = selected->m_Hash;
				selected->Save();
				m_UnsavedShaders[m_NodeSelected] = false;
				//if (!temp.Comp(selected->m_Hash)) {
				forceReload = true;
				//}

				ForceSave = false;
			}

			if (ImGui::Button("Reload Shader") || forceReload) {
				ShaderLoadRes finalRes = ShaderLoadRes::SHADERLOAD_LOAD;
				for (uint i = 0; i < selected->m_ShaderFileObjects.Length(); i++) {
					ShaderLoadRes res = selected->m_ShaderFileObjects[i]->Reload();
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
				selected->FindUnlinkedUniforms();
			}
			ImGui::SameLine();
			ImGui::Text(ShaderCompileInfo.Get());

			if (ImGui::Button("Add Shader")) {
				selected->m_ShaderFileObjects.Add(new ShaderSpirvData());
			}

			{
				static uint newNode = -1;
				static uint ChangingNode = -1;
				static ImGuiTextFilter popupFilter;

				ImGui::Text("Attached Shaders: %i", selected->m_ShaderFileObjects.Length());
				if (ImGui::TreeNode("Attached Shaders")) {
					for (uint i = 0; i < selected->m_ShaderFileObjects.Length(); i++) {
						ImGui::PushID(i);
						ShaderSpirvData* ssd = selected->m_ShaderFileObjects[i];
						//ImGui::Text("%i: %s", i, ssd->m_FilePath.m_FileName.Get());
						//ImGui::Text("%i: %s", i, ssd->m_AttachedShaders[i]->GetDebugObjName().Get());

						CMString text = CMString::IntToString(i) + ": " + ssd->m_FilePath.m_FileName.Get();
						if (ImGui::Selectable(text.Get(), false, ImGuiSelectableFlags_None, ImVec2(150, 0))) {
							_CShaderSpirvManager->m_NodeSelected = _CShaderSpirvManager->m_ShaderObjects.FindElement(ssd);
							m_ChangeToTab = 0;
						}

						ImGui::SameLine();

						if (ImGui::Button("Change...")) {
							ImGui::OpenPopup("Change Shader");
							newNode = _CShaderSpirvManager->m_ShaderObjects.FindElement(ssd);
							ChangingNode = i;

							popupFilter = ImGuiTextFilter(ssd->m_FilePath.m_FilePath.SubStrFindFromEnd('.').Get());
							popupFilter.Build();
						}

						ImGui::SameLine();

						if (ImGui::Button("Remove")) {
							selected->m_ShaderFileObjects.RemoveAt(i);
						}

						//popup selector
						if (ChangingNode == i && ImGui::BeginPopupModal("Change Shader", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
							ImGui::Text("Current: %s", selected->m_ShaderFileObjects[ChangingNode]->m_FilePath.m_FileName.Get());

							static float size = 150;

							popupFilter.Draw("", size);

							ImGui::BeginChild("Selector", ImVec2(size, 100), false, ImGuiWindowFlags_HorizontalScrollbar);
							for (uint q = 0; q < _CShaderSpirvManager->m_Paths.Length(); q++) {
								CMString text = _CShaderSpirvManager->m_Paths[q];
								if (popupFilter.PassFilter(text.Get())) {
									if (ImGui::Selectable(text.Get(), newNode == q)) {
										newNode = q;
									}
								}
							}
							ImGui::EndChild();

							if (ImGui::Button("OK")) {
								m_UnsavedShaders[m_NodeSelected] = true;

								selected->RemoveShader(ssd);
								selected->AddShader(_CShaderSpirvManager->m_ShaderObjects[newNode]);
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

	//MODALS
	{
		static int Selected = 0;
		static ImGuiTextFilter filter;
		if (showingModal == false && modal != MODALS::NONE) {
			switch (modal) {
				case MODALS::SAVE_AS:
					ImGui::OpenPopup("Save As");
					break;
			}
			showingModal = true;
			Selected = -1;
			modal = MODALS::NONE;
		}

		if (ImGui::BeginPopupModal("Save As", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			const uint nameMaxLength = 32;
			static char name[nameMaxLength] = "";
			static bool getNewName = true;

			if (getNewName) {
				strcpy_s(name, m_Shaders[m_NodeSelected].m_Shader->m_FilePath.m_FilePath.Get());
				getNewName = false;
			}

			bool EnterPressed = false;
			if (ImGui::InputText("", name, nameMaxLength, ImGuiInputTextFlags_EnterReturnsTrue)) {
				EnterPressed = true;
			}

			if (ImGui::Button("OK") || EnterPressed) {
				getNewName = true;

				CMString fileName = name;
				if (!fileName.ToLower().Contains(".shader")) {
					fileName += ".shader";
				}

				m_Shaders[m_NodeSelected].m_Shader->m_FilePath = fileName;
				m_UnsavedShaders[m_NodeSelected] = true;

				ImGui::CloseCurrentPopup();
				showingModal = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				getNewName = true;
				ImGui::CloseCurrentPopup();
				showingModal = false;
			}
			ImGui::EndPopup();
		}
	}
}
