#include "Object.h"

#include "nlohmann/json.hpp"
#include "Graphics/Model.h"

#include "ImGui/imgui.h"

#include "Managers/Manager.h"

Object::Object() {
}

Object::Object(CMString a_Name, Transform a_Transform) : m_Name(a_Name), m_Transform(a_Transform) {
}
Object::Object(CMString a_Name) : m_Name(a_Name) {
}

Object::~Object() {
}

void Object::ImGuiExtras() {
	float scale = 0.25f;
	if (ImGui::DragFloat3("Position", &m_Transform.m_Position.x, scale)) {
		m_Transform.SetDirty();
	}
	if (ImGui::DragFloat3("Rotation", &m_Transform.m_Rotation.x, scale)) {
		m_Transform.SetDirty();
	}
	if (ImGui::DragFloat3("Scale", &m_Transform.m_Scale.x, scale)) {
		m_Transform.SetDirty();
	}
	ImGui::Separator();
}

bool Object::LoadData_Internal(nlohmann::json & a_Json) {
	//nlohmann::json& json = (*a_Json);
	//json["Type"] = "Object";	
	if (!m_Transform.LoadData_Internal(a_Json["Transform"])) {
		return false;
	}
	if (a_Json.contains("Model")) {
		if (a_Json["Model"].is_string()) {
			//m_ObjectModel = new Model();
			//m_ObjectModel->LoadModel(a_Json["Model"].get<CMString>());
			m_ObjectModel = _CManager->GetModel(a_Json["Model"].get<CMString>());
		}
	}
	if (a_Json.contains("Name")) {
		m_Name = a_Json["Name"].get<CMString>();
	}
	return true;
}

void Object::SaveData_Internal(nlohmann::json & a_Json) {
	a_Json["Type"] = GetObjectClassName();
	m_Transform.SaveData_Internal(a_Json["Transform"]);
	if (m_ObjectModel) {
		if (m_ObjectModel->m_FilePath.Length() > 5) {
			a_Json["Model"] = m_ObjectModel->m_FilePath;
		}
	}
	a_Json["Name"] = m_Name;
}

#include "Managers/TimeManager.h"
void SinObject::Update() {
	float offset = glm::sin(_CTimeManager->m_CurrentTime);
	Vector3 pos = m_Transform.m_Position;
	pos.y = offset;
	m_Transform.SetPosition(pos);
}
