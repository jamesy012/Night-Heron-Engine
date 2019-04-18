#include "Transform.h"

#include <glm\ext.hpp>

#include "nlohmann/json.hpp"

Transform::Transform() {
	Reset();
}

Transform::~Transform() {
}

void Transform::SetPosition(Vector3 a_NewPosition) {
	m_Position = a_NewPosition;
	SetDirty();
}

void Transform::SetRotation(Vector3 a_NewRotation) {
	m_Rotation = a_NewRotation;
	SetDirty();
}

void Transform::SetRotation(glm::quat a_NewRotation) {
#ifdef USE_QUATERNIONS
	m_Rotation = a_Quat;
#else
	m_Rotation = glm::degrees(glm::eulerAngles(a_NewRotation));
#endif // USE_QUATERNIONS

	SetDirty();
}

void Transform::SetScale(Vector3 a_NewScale) {
	m_Scale = a_NewScale;
	SetDirty();
}

void Transform::SetLookAt(Vector3 a_Pos, Vector3 a_At, Vector3 a_Up) {
	glm::mat4 lookAt = glm::lookAt(a_Pos, a_At, a_Up);

	glm::quat rotation = glm::quat(glm::inverse(lookAt));

#ifdef USE_QUATERNIONS
	setRotation(rotation);
#else
	glm::vec3 quatRot = glm::degrees(glm::eulerAngles(rotation));
	if (abs(quatRot.z) > 90) {//if 180??!
		glm::vec3 euler;
		euler.x = quatRot.x - 180;
		euler.y = 180 - quatRot.y;
		euler.z = 0;
		SetRotation(euler);
	} else {
		SetRotation(rotation);
	}
#endif // USE_QUATERNIONS

	SetPosition(a_Pos);
	SetScale(Vector3(1));
}

Matrix Transform::GetModelMatrix() {
	if (m_IsDirty) {
		UpdateModelMatrix();
	}
	return m_ModelMatrix;
}

void Transform::Reset() {
	m_Position = Vector3(0);
	m_Rotation = Vector3(0);
	m_Scale = Vector3(1);
}

bool Transform::LoadData_Internal(nlohmann::json & a_Json) {
	if (a_Json.contains("Pos")) {
		a_Json["Pos"]["x"].get_to(m_Position.x);
		a_Json["Pos"]["y"].get_to(m_Position.y);
		a_Json["Pos"]["z"].get_to(m_Position.z);
	} else {
		return false;
	}
	if (a_Json.contains("Rot")) {
		a_Json["Rot"]["x"].get_to(m_Rotation.x);
		a_Json["Rot"]["y"].get_to(m_Rotation.y);
		a_Json["Rot"]["z"].get_to(m_Rotation.z);
	} else {
		return false;
	}
	if (a_Json.contains("Scale")) {
		a_Json["Scale"]["x"].get_to(m_Scale.x);
		a_Json["Scale"]["y"].get_to(m_Scale.y);
		a_Json["Scale"]["z"].get_to(m_Scale.z);
	} else {
		return false;
	}
	SetDirty();
	return true;
}

void Transform::SaveData_Internal(nlohmann::json & a_Json) {
	a_Json["Pos"]["x"] = m_Position.x;
	a_Json["Pos"]["y"] = m_Position.y;
	a_Json["Pos"]["z"] = m_Position.z;

	a_Json["Rot"]["x"] = m_Rotation.x;
	a_Json["Rot"]["y"] = m_Rotation.y;
	a_Json["Rot"]["z"] = m_Rotation.z;
	
	a_Json["Scale"]["x"] = m_Scale.x;
	a_Json["Scale"]["y"] = m_Scale.y;
	a_Json["Scale"]["z"] = m_Scale.z;
}

void Transform::UpdateModelMatrix() {
	m_ModelMatrix = glm::translate(glm::mat4(1), m_Position);

	glm::mat4 rotmat = glm::mat4(1);
	rotmat = glm::rotate(rotmat, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
	rotmat = glm::rotate(rotmat, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
	rotmat = glm::rotate(rotmat, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
	m_ModelMatrix *= rotmat;

	m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);

	m_IsDirty = false;
}
