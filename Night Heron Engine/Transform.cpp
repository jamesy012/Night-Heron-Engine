#include "Transform.h"

#include <glm\ext.hpp>

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

void Transform::SetScale(Vector3 a_NewScale) {
	m_Scale = a_NewScale;
	SetDirty();
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
