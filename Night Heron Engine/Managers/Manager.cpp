#include "Manager.h"

#include <ImGui/imgui.h>
#include "Singletons.h"
#include "TextureManager.h"
#include "ShaderManager.h"

#include "Graphics/API/GFXAPI.h"
#include "Graphics/API/RenderTarget.h"
#include "Graphics/API/Texture.h"
#include "Graphics/API/Shader.h"

#include "Object.h"
#include "Scene.h"
#include "Graphics/Model.h"
#include "Graphics/Material.h"
#include "Graphics/API/Shader.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include "Generated/Objects_Night_Heron_Engine.h"

#include "nlohmann/json.hpp"

Manager::Manager() {
	_CManager = this;
}

Manager::~Manager() {
	delete m_CommonRT;
}

void Manager::ImGuiWindow() {
	if (m_CommonRT == nullptr) {
		m_CommonRT = _CGraphics->CreateRenderTarget(256, 256);
		m_CommonRT->SetupRenderTarget_Internal();
		m_CommonRT->SetDebugObjName("Manager RT");
	}

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Managers")) {
			ImGui::MenuItem("Objects", NULL, &m_ShowObjects);
			ImGui::MenuItem("Models", NULL, &m_ShowModels);
			ImGui::MenuItem("Materials", NULL, &m_ShowMaterials);
			ImGui::MenuItem("Textures", NULL, &m_ShowTextures);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	ImGuiObjects();
	ImGuiModels();
	ImGuiMaterials();
	_CTextureManager->ImGuiWindow(&m_ShowTextures);
}

void Manager::RegisterShaderUniform(ShaderUniformBlock* a_Uniform, CMString a_SlotName) {
	if (GetShaderUniform(a_SlotName) == nullptr) {
		a_Uniform->m_Registered = true;
		m_ShaderUniforms.Add({ a_Uniform, a_SlotName });
	}
}

ShaderUniformBlock* Manager::GetShaderUniform(CMString a_SlotName) {
	for (int i = 0; i < m_ShaderUniforms.Length(); i++) {
		if (m_ShaderUniforms[i].a_Name == a_SlotName) {
			return m_ShaderUniforms[i].a_Block;
		}
	}
	return nullptr;
}

void Manager::FindAllTextures() {
}

void Manager::AddModel(Model * a_Model) {
	m_Models.Add(a_Model);
}

Model * Manager::GetModel(CMString a_FileName) {
	for (int i = 0; i < m_Models.Length(); i++) {
		if (m_Models[i]->m_FilePath == a_FileName) {
			return m_Models[i];
		}
	}
	return nullptr;
}

void Manager::ImGuiObjects() {
	if (!m_ShowObjects && m_CurrentScene != nullptr) {
		return;
	}

	static float m_Scale = 0.25f;

	//static int selection_mask = (1 << 2);
	static int nodeSelected = -1;

	enum class MODALS {
		NONE = -1,
		CREATE = 1,
		CHANGE_MODEL,
		CHANGE_NAME,
		CHANGE_TYPE,
	};
	static MODALS modal = MODALS::NONE;
	static bool showingModal = false;

	ImGui::Begin("Object Menu", &m_ShowObjects);

	{
		static float size = 150;
		ImGui::BeginChild("Objects", ImVec2(size,0));
		//float size = ImGui::GetWindowContentRegionWidth() * 0.5f;
		//ImGui::DragFloat("Size", &size, 1.0f);
		if (ImGui::Button("Save")) {
			m_CurrentScene->Save();
		}

		ImGui::BeginChild("Selector", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
		for (uint i = 0; i < m_CurrentScene->m_Objects.Length(); i++) {
			CMString text = m_CurrentScene->m_Objects[i]->m_Name.Get();
			if (ImGui::Selectable(text.Get(), nodeSelected == i)) {
				nodeSelected = i;
			}
		}
		ImGui::EndChild();
		
		ImGui::BeginChild("Buttons");

		if (ImGui::Button("Create")) {
			modal = MODALS::CREATE;
		}

		ImGui::EndChild();
		ImGui::EndChild();
	}
	ImGui::SameLine();
	{
		ImGui::BeginChild("Details", ImVec2(0, 0), true);
		if (nodeSelected >= 0) {			

			Object* obj = m_CurrentScene->m_Objects[nodeSelected];
			ImGui::Text("Selected Object: %s", obj->m_Name.Get());
			ImGui::SameLine();

			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			if (ImGui::Button("Change Name")) {
				modal = MODALS::CHANGE_NAME;
			}
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
			
			ImGui::SameLine();

			if (ImGui::Button("Change Model")) {
				modal = MODALS::CHANGE_MODEL;
			}

			if (ImGui::Button("Change Type")) {
				modal = MODALS::CHANGE_TYPE;
			}

			obj->ImGuiExtras();
		}

		ImGui::EndChild();
	}

	//MODALS
	{
		static int Selected = 0;
		static ImGuiTextFilter filter;
		if (showingModal == false && modal != MODALS::NONE) {
			switch (modal) {
				case MODALS::CREATE:
					ImGui::OpenPopup("Create Scene Object");
					break;
				case MODALS::CHANGE_NAME:
					ImGui::OpenPopup("Change Object Name");
					break;
				case MODALS::CHANGE_MODEL:
					ImGui::OpenPopup("Change Object Model");
					break;
				case MODALS::CHANGE_TYPE:
					ImGui::OpenPopup("Change Object Type");
					break;
			}
			showingModal = true;
			Selected = -1;
			modal = MODALS::NONE;
		}

		if (ImGui::BeginPopupModal("Create Scene Object", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			static CMString ObjectSelected;
			ImGui::BeginChild("Selector", ImVec2(0, 100), false, ImGuiWindowFlags_HorizontalScrollbar);
			filter.Draw("", 100);
			int index = 0;
			for (auto it = GENERATED_OBJ::g_factory->m_classes.begin(); it != GENERATED_OBJ::g_factory->m_classes.end(); it++, index++) {
				if (filter.PassFilter(it->first.c_str())) {
					if (ImGui::Selectable(it->first.c_str(), Selected == index)) {
						Selected = index;
						ObjectSelected = it->first;
					}
				}
			}
			ImGui::EndChild();


			if (ImGui::Button("Add")) {
				if (Selected != -1) {
					Object* obj = (Object*)GET_OBJ(ObjectSelected.Get());
					if (obj->m_Name == "") {
						obj->m_Name = ObjectSelected + "_" + CMString::IntToString(m_CurrentScene->m_Objects.Length());
					}
					m_CurrentScene->AddObject(obj);

					obj->Start();

					ImGui::CloseCurrentPopup();
					showingModal = false;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) {
				ImGui::CloseCurrentPopup();
				showingModal = false;
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupModal("Change Object Name", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			const uint nameMaxLength = 32;
			static char name[nameMaxLength] = "";
			static bool getNewName = true;

			if (getNewName) {
				strcpy_s(name, m_CurrentScene->m_Objects[nodeSelected]->m_Name.Get());
				getNewName = false;
			}
			
			bool EnterPressed = false;
			if (ImGui::InputText("", name, nameMaxLength, ImGuiInputTextFlags_EnterReturnsTrue)) {
				EnterPressed = true;
			}

			if (ImGui::Button("OK") || EnterPressed) {
				getNewName = true;

				m_CurrentScene->m_Objects[nodeSelected]->m_Name = name;

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
		if (ImGui::BeginPopupModal("Change Object Model", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			static CMString ObjectSelected;
			ImGui::BeginChild("Selector", ImVec2(400, 100), false, ImGuiWindowFlags_HorizontalScrollbar);
			filter.Draw("", 100);
			for (int i = 0; i < m_Models.Length();i++) {
				if (filter.PassFilter(m_Models[i]->m_FilePath.c_str())) {
					if (ImGui::Selectable(m_Models[i]->m_FilePath.c_str(), Selected == i)) {
						Selected = i;
					}
				}
			}
			ImGui::EndChild();


			if (ImGui::Button("Change")) {
				if (Selected != -1) {
					m_CurrentScene->m_Objects[nodeSelected]->m_ObjectModel = m_Models[Selected];

					ImGui::CloseCurrentPopup();
					showingModal = false;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) {
				ImGui::CloseCurrentPopup();
				showingModal = false;
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupModal("Change Object Type", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			static CMString ObjectSelected;
			ImGui::BeginChild("Selector", ImVec2(0, 100), false, ImGuiWindowFlags_HorizontalScrollbar);
			filter.Draw("", 100);
			int index = 0;
			for (auto it = GENERATED_OBJ::g_factory->m_classes.begin(); it != GENERATED_OBJ::g_factory->m_classes.end(); it++, index++) {
				if (filter.PassFilter(it->first.c_str())) {
					if (ImGui::Selectable(it->first.c_str(), Selected == index)) {
						Selected = index;
						ObjectSelected = it->first;
					}
				}
			}
			ImGui::EndChild();


			if (ImGui::Button("OK")) {
				if (Selected != -1) {
					Object* obj = (Object*)GET_OBJ(ObjectSelected.Get());
					nlohmann::json j;
					m_CurrentScene->m_Objects[nodeSelected]->SaveData_Internal(j);
					obj->LoadData_Internal(j);
					m_CurrentScene->m_Objects.RemoveAt(nodeSelected);
					m_CurrentScene->AddObject(obj);

					obj->Start();

					ImGui::CloseCurrentPopup();
					showingModal = false;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) {
				ImGui::CloseCurrentPopup();
				showingModal = false;
			}
			ImGui::EndPopup();
		}
	}

	ImGui::End();
}

void Manager::ImGuiModels() {
	if (!m_ShowModels) {
		return;
	}
	//static int selection_mask = (1 << 2);
	static int nodeSelected = -1;

	if (m_IsRtDirty && nodeSelected != -1) {
		struct {
		public:
			glm::mat4 MatrixView = glm::mat4();
			glm::mat4 MatrixProjection = glm::mat4();
			glm::mat4 MatrixModelTest = glm::mat4();
			glm::mat4 MatrixPV = glm::mat4();
		}testUniformStructObj;

		_CGraphics->PushDebugGroup("Manager Render Target");
		testUniformStructObj.MatrixProjection = glm::perspective(glm::radians(m_RTFov), 1.4f, 0.1f, 100.0f);
		testUniformStructObj.MatrixView = glm::lookAt(glm::vec3(m_RTPos.v1, m_RTPos.v2, m_RTPos.v3), glm::vec3(0), glm::vec3(0, 1, 0));
		testUniformStructObj.MatrixPV = testUniformStructObj.MatrixProjection * testUniformStructObj.MatrixView;
		testUniformStructObj.MatrixModelTest = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		_CGraphics->UseRenderTarget(m_CommonRT);
		_CGraphics->SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		_CGraphics->Clear();

		tempPVMUniform->UpdateBuffer(&testUniformStructObj);

		m_Models[nodeSelected]->Draw();

		_CGraphics->ResetRenderTarget();

		_CGraphics->PopDebugGroup();

		m_IsRtDirty = false;
	}

	ImGui::Begin("Model Menu", &m_ShowModels);
	{
		//float size = ImGui::GetWindowContentRegionWidth() * 0.5f;
		static float size = 150;
		//ImGui::DragFloat("Size", &size, 1.0f);
		ImGui::BeginChild("Selector", ImVec2(size, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		for (uint i = 0; i < m_Models.Length(); i++) {
			CMString text = m_Models[i]->GetDebugObjName().Get();
			if (ImGui::Selectable(text.Get(), nodeSelected == i)) {
				nodeSelected = i;
				m_IsRtDirty = true;
			}
		}

		ImGui::EndChild();
	}
	ImGui::SameLine();
	{
		ImGui::BeginChild("Details", ImVec2(0, 0), true);
		InlineImGoiRenderTargetSettings();

		if (nodeSelected >= 0) {
			Model* model = m_Models[nodeSelected];
			ImGui::Text("Selected Model: %s", model->GetDebugObjName().Get());
			ImGui::Text("Mesh's: %s", model->GetDebugObjName().Get());
			if (model->NumMeshs() > 0) {
				for (uint i = 0; i < model->NumMeshs(); i++) {
					Model::ModelMeshHolder* mmh = m_Models[nodeSelected]->GetMeshHolder(i);

					ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
					if (ImGui::TreeNode(CMString::IntToString(i).Get())) {
						ImGui::Text("Mesh: %s", mmh->m_ObjName.Get());
						ImGui::Text("Material: %i", mmh->m_Material ? mmh->m_Material->m_ID : -1);
						int id = -1;
						if (mmh->m_Material) {
							id = (int)mmh->m_Material->m_ID;
						}
						if (ImGui::DragInt("Mat", &id, 0.1f, -1, m_Materials.Length() - 1)) {
							if (id == -1) {
								mmh->m_Material = nullptr;
							} else {
								mmh->m_Material = m_Materials.At(id);
							}
							m_IsRtDirty = true;
						}
						ImGui::TreePop();
					}
				}
			}

			ImGui::Image(m_CommonRT->GetTexture()->getTexturePtr(), ImVec2(200, 200), _CGraphics->GetImGuiImageUV0(), _CGraphics->GetImGuiImageUV1());
		}

		ImGui::EndChild();
	}

	ImGui::End();
}

void Manager::ImGuiMaterials() {
	if (!m_ShowMaterials) {
		return;
	}
	static CMArray<uint> unsaved;
	//static int selection_mask = (1 << 2);
	static int nodeSelected = -1;
	ImGui::Begin("Materials Menu", &m_ShowMaterials);

	{
		//float size = ImGui::GetWindowContentRegionWidth() * 0.5f;
		static float size = 150;
		//ImGui::DragFloat("Size", &size, 1.0f);
		ImGui::BeginChild("Selector", ImVec2(size, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		for (uint i = 0; i < m_Materials.Length(); i++) {
			CMString text = m_Materials[i]->GetDebugObjName().Get();
			for (int q = 0; q < unsaved.Length(); q++) {
				if (unsaved[q] == i) {
					text += " *";
				}
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

		if (nodeSelected != -1) {
			Material* material = m_Materials[nodeSelected];
			ImGui::Text("Selected Material: %s", material->GetDebugObjName().Get());

			if (ImGui::Button("Save")) {
				material->Save();
				unsaved.Remove(nodeSelected);
			}
			{
				static uint CurrShader;

				ImGui::Text("Shader: %s", material->m_Shader ? material->m_Shader->GetDebugObjName().Get() : "None");

				ImGui::SameLine();

				if (ImGui::Button("Change...#100")) {
					ImGui::OpenPopup("Change Material Shader");
					CurrShader = _CShaderManager->FindElement(material->m_Shader->m_FilePath.m_FilePath.Get());
				}

				if (ImGui::BeginPopupModal("Change Material Shader", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
					ImGui::Text(material->m_Shader->GetDebugObjName().Get());

					_CShaderManager->ImGuiSelector(&CurrShader, SimpleVec2(300, 150));

					if (ImGui::Button("OK")) {
						unsaved.Add(nodeSelected);

						material->m_Shader = _CShaderManager->GetShader(CurrShader);
						ImGui::CloseCurrentPopup();
					}
					ImGui::SameLine();
					if (ImGui::Button("Close")) {
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
			}
			{
				static uint TextureID;
				static uint CurrTex;
				static int SlotID;
				ImGui::Text("Textures:");
				for (uint i = 0; i < material->m_TextureOverloads.Length(); i++) {
					ImGui::Text("Slot: %i, Path: %s", material->m_TextureOverloads[i].m_Slot, material->m_TextureOverloads[i].m_Tex->GetPath().Get());

					ImGui::SameLine();

					if (ImGui::Button("Change...#200")) {
						ImGui::OpenPopup("Change Material Texture");
						TextureID = i;
						SlotID = material->m_TextureOverloads[i].m_Slot;
						CurrTex = _CTextureManager->FindElement(material->m_TextureOverloads[i].m_Tex->GetPath());
					}
				}

				if (ImGui::BeginPopupModal("Change Material Texture", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
					ImGui::DragInt("Slot:", &SlotID, 1, 0, 32);

					ImGui::BeginChild("Selector", ImVec2(0, 100), false, ImGuiWindowFlags_HorizontalScrollbar);
					for (uint q = 0; q < _CTextureManager->m_Paths.Length(); q++) {
						CMString text = _CTextureManager->m_Paths[q];
						//if (popupFilter.PassFilter(text.Get())) {
						if (ImGui::Selectable(text.Get(), CurrTex == q)) {
							CurrTex = q;
						}
						//}
					}
					ImGui::EndChild();

					if (ImGui::Button("OK")) {
						unsaved.Add(nodeSelected);

						material->m_TextureOverloads[TextureID].m_Slot = SlotID;
						material->m_TextureOverloads[TextureID].m_Tex = _CTextureManager->m_Textures[CurrTex].m_Texture;
						ImGui::CloseCurrentPopup();
					}
					ImGui::SameLine();
					if (ImGui::Button("Close")) {
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
			}
		}

		ImGui::EndChild();
	}

	ImGui::End();
}

void Manager::InlineImGoiRenderTargetSettings() {
	if (ImGui::TreeNode("Render Target Settings")) {
		if (ImGui::DragFloat3("Position", &m_RTPos.v1)) {
			m_IsRtDirty = true;
		}
		if (ImGui::DragFloat("Fov", &m_RTFov)) {
			m_IsRtDirty = true;
		}

		ImGui::TreePop();
	}
}
