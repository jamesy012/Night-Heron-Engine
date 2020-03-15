#include "ObjectDrawTest.h"

#include "Graphics/API/Shader.h"
#include "Graphics/Model.h"

#include "Scene.h"
#include "Managers/Manager.h"
#include "Camera.h"

#include "ImGui/imgui.h"
#include "nlohmann/json.hpp"
#include "jsonConverter/JCglm.h"

//for uniforms
#include "Managers/ShaderManager.h"

struct TestUniformStruct2 {
public:
	glm::vec4 Color;
};

void ObjectDrawTest::Start() {
	m_ModelUniform = _CManager->GetShaderUniform("Object_Data");
	m_ColorUniform = _CManager->GetShaderUniform("Shader_Data");
}

void ObjectDrawTest::PreDraw() {
	if (m_ModelUniform) {
		ObjectUniformStruct PVMBuf;
		PVMBuf.ModelMatrix = m_Transform.GetModelMatrix();
		//PVMBuf.MatrixPV = _CManager->m_CurrentScene->m_Camera->GetPV();
		m_ModelUniform->UpdateBuffer(&PVMBuf);
	}
	if (m_ColorUniform) {
		TestUniformStruct2 ColorBuf;
		ColorBuf.Color = glm::vec4(m_Color, 1.0f);
		m_ColorUniform->UpdateBuffer(&ColorBuf);
	}
}

void ObjectDrawTest::Draw() {
	if (m_ObjectModel) {
		m_ObjectModel->Draw();
	}
}

void ObjectDrawTest::ImGuiExtras() {
	Object::ImGuiExtras();
	ImGui::Separator();
	ImGui::DragFloat3("Color", &m_Color.x, 0.1f);
}

bool ObjectDrawTest::LoadData_Internal(nlohmann::json & a_Json) {
	if (!Object::LoadData_Internal(a_Json)) {
		return false;
	}

	if (a_Json.contains("UColor")) {
		m_Color = a_Json["UColor"].get<glm::vec3>();
	}

	return true;
}

void ObjectDrawTest::SaveData_Internal(nlohmann::json & a_Json) {
	Object::SaveData_Internal(a_Json);
	a_Json["UColor"] = m_Color;
}
