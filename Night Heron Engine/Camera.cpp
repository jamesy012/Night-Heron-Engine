#include "stdafx.h"
#include "Camera.h"

#include "nlohmann/json.hpp"

void Camera::UpdateModelMatrix() {
	Transform::UpdateModelMatrix();
	m_ViewMatrix = glm::inverse(m_ModelMatrix);
	m_ProjectionMatrix = glm::perspective(m_Fov, m_Aspect, m_Near, m_Far);
	m_PV = m_ProjectionMatrix * m_ViewMatrix;
}

Camera::Camera() {
}

Matrix Camera::GetView() {
	if (m_IsDirty) {
		UpdateModelMatrix();
	}
	return m_ViewMatrix;
}

Matrix Camera::GetProjection() {
	if (m_IsDirty) {
		UpdateModelMatrix();
	}
	return m_ProjectionMatrix;
}

Matrix Camera::GetPV() {
	if (m_IsDirty) {
		UpdateModelMatrix();
	}
	return m_PV;
}

void Camera::SetAspectRatio(float a_NewAspect) {
	m_Aspect = a_NewAspect;
	SetDirty();
}

void Camera::SetFov(float a_NewFov) {
	m_Fov = glm::radians(a_NewFov);
	SetDirty();
}

void Camera::SetNearClip(float a_NearClip) {
	m_Near = a_NearClip;
	SetDirty();
}

void Camera::SetFarClip(float a_FarClip) {
	m_Far = a_FarClip;
	SetDirty();
}

float Camera::GetFov() const {
	return glm::degrees(m_Fov);
}

bool Camera::LoadData_Internal(nlohmann::json & a_Json) {
	Transform::LoadData_Internal(a_Json);
	if (a_Json.contains("Fov")) {
		m_Fov = a_Json["Fov"].get<float>();
	} else {
		return false;
	}
	if (a_Json.contains("Far")) {
		m_Far = a_Json["Far"].get<float>();
	} else {
		return false;
	}
	if (a_Json.contains("Near")) {
		m_Near = a_Json["Near"].get<float>();
	} else {
		return false;
	}
	SetDirty();
	return true;
}

void Camera::SaveData_Internal(nlohmann::json & a_Json) {
	Transform::SaveData_Internal(a_Json);
	a_Json["Fov"] = m_Fov;

	a_Json["Far"] = m_Far;

	a_Json["Near"] = m_Near;
}