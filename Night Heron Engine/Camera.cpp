#include "Camera.h"

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
}

void Camera::SetFarClip(float a_FarClip) {
}
