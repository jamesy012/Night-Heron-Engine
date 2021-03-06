#include "stdafx.h"
#include "Scene.h"

#include "Generated/Objects_Night_Heron_Engine.h"

#include "Object.h"

#include "Camera.h"

#include "nlohmann/json.hpp"

void Scene::Start() {
	for (uint i = 0; i < m_Objects.Length(); i++) {
		m_Objects[i]->Start();
	}
}

void Scene::Update() {
	for (uint i = 0; i < m_Objects.Length(); i++) {
		m_Objects[i]->Update();
	}
}

void Scene::Draw() {
	for (uint i = 0; i < m_Objects.Length(); i++) {
		m_Objects[i]->PreDraw();
		m_Objects[i]->Draw();
	}
}

bool Scene::LoadData_Internal(nlohmann::json & a_Json) {
	//j.parse(data);
	a_Json.at("Name").get_to(m_Name);
	nlohmann::json objects = a_Json.at("Objects");

	for (nlohmann::json::iterator it = objects.begin(); it != objects.end(); ++it) {
		auto json = *it;
		//std::cout << it.key() << " : " << it.value() << "\n";
		Object* obj;
		if (json.contains("Type")) {
			obj = (Object*)GET_OBJ(json["Type"].get<CMString>().Get());
		} else {
			return false;
		}
		//obj->m_Name = it.key();
		if (!obj->LoadData_Internal(json)) {
			m_Objects.Clear();
			return false;
		}
		m_Objects.Add(obj);
	}

	if (m_Camera) {
		m_Camera->LoadData_Internal(a_Json["Camera"]);
	}

	return true;
}

void Scene::SaveData_Internal(nlohmann::json & a_Json) {
	a_Json["Name"] = m_Name;
	nlohmann::json& objs = a_Json["Objects"];
	for (uint i = 0; i < m_Objects.Length(); i++) {
		m_Objects[i]->SaveData_Internal(objs[i]);
	}
	m_Camera->SaveData_Internal(a_Json["Camera"]);
}

void Scene::AddObject(Object* a_Object) {
	m_Objects.Add(a_Object);
}

Object * Scene::GetObjectByName(const CMString a_Name) const {
	for (uint i = 0; i < m_Objects.Length(); i++) {
		if (m_Objects[i]->m_Name == a_Name) {
			return m_Objects[i];
		}
	}
	return nullptr;
}
