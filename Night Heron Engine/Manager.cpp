#include "Manager.h"

#include <ImGui/imgui.h>
#include "Singletons.h"

#include "Graphics/API/GFXAPI.h"
#include "Graphics/API/RenderTarget.h"
#include "Graphics/API/Texture.h"

#include "Object.h"
#include "Graphics/Model.h"
#include "Graphics/Material.h"
#include "Graphics/API/Shader.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>


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
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	ImGuiObjects();
	ImGuiModels();
	ImGuiMaterials();
}

void Manager::ImGuiObjects() {
	if (!m_ShowObjects) {
		return;
	}

	static float m_Scale = 0.25f;

	//static int selection_mask = (1 << 2);
	static int nodeSelected = -1;
	ImGui::Begin("Object Menu", &m_ShowObjects);

	{
		//float size = ImGui::GetWindowContentRegionWidth() * 0.5f;
		static float size = 150;
		//ImGui::DragFloat("Size", &size, 1.0f);
		ImGui::BeginChild("Selector", ImVec2(size, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		for (uint i = 0; i < m_Objects.Length(); i++) {
			CMString text = m_Objects[i]->m_Name.Get();
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
			Object* obj = m_Objects[nodeSelected];
			ImGui::Text("Selected Object: %s", obj->m_Name.Get());

			if (ImGui::DragFloat3("Position", &obj->m_Transform.m_Position.x, m_Scale)) {
				obj->m_Transform.SetDirty();
			}
			if (ImGui::DragFloat3("Rotation", &obj->m_Transform.m_Rotation.x, m_Scale)) {
				obj->m_Transform.SetDirty();
			}
			if (ImGui::DragFloat3("Scale", &obj->m_Transform.m_Scale.x, m_Scale)) {
				obj->m_Transform.SetDirty();
			}
		}


		ImGui::EndChild();
	}

	ImGui::End();
}

void Manager::ImGuiModels() {
	if (!m_ShowModels) {
		return;
	}
	//static int selection_mask = (1 << 2);
	static int nodeSelected = -1;
	static bool isRtDirty = true;

	if (isRtDirty && nodeSelected != -1) {
		struct {
		public:
			glm::mat4 MatrixView = glm::mat4();
			glm::mat4 MatrixProjection = glm::mat4();
			glm::mat4 MatrixModelTest = glm::mat4();
			glm::mat4 MatrixPV = glm::mat4();
		}testUniformStructObj;

		_CGraphics->PushDebugGroup("Manager Render Target");
		testUniformStructObj.MatrixProjection = glm::perspective(glm::radians(60.0f), 1.4f, 0.1f, 100.0f);
		testUniformStructObj.MatrixView = glm::lookAt(glm::vec3(-4, 4, 10.0f), glm::vec3(0), glm::vec3(0, 1, 0));
		testUniformStructObj.MatrixPV = testUniformStructObj.MatrixProjection * testUniformStructObj.MatrixView;
		testUniformStructObj.MatrixModelTest = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		_CGraphics->UseRenderTarget(m_CommonRT);
		_CGraphics->SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		_CGraphics->Clear();

		tempPVMUniform->UpdateBuffer(&testUniformStructObj);

		_CGraphics->BindTexture(_CGraphics->m_WhiteTexture, 0);

		m_Models[nodeSelected]->Draw();

		_CGraphics->ResetRenderTarget();

		_CGraphics->UnbindTexture(0);

		_CGraphics->PopDebugGroup();
		
		isRtDirty = false;
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
				isRtDirty = true;
			}
		}

		ImGui::EndChild();
	}
	ImGui::SameLine();
	{
		ImGui::BeginChild("Details", ImVec2(0, 0), true);

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
	//static int selection_mask = (1 << 2);
	static int nodeSelected = -1;
	ImGui::Begin("Materials Menu", &m_ShowMaterials);

	{
		//float size = ImGui::GetWindowContentRegionWidth() * 0.5f;
		static float size = 150;
		//ImGui::DragFloat("Size", &size, 1.0f);
		ImGui::BeginChild("Selector", ImVec2(size, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		for (uint i = 0; i < m_Materials.Length(); i++) {
			CMString text = CMString::IntToString(i) + m_Materials[i]->GetDebugObjName().Get();
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

			ImGui::Text("Shader: %s", material->m_Shader ? material->m_Shader->GetDebugObjName().Get() : "None");
		}

		ImGui::EndChild();
	}

	ImGui::End();
}

