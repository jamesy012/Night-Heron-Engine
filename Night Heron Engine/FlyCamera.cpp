#include "FlyCamera.h"

#include "Input/InputHandler.h"
#include "Managers/TimeManager.h"
#include "glm/glm.hpp"

#include "ImGui/imgui.h"

FlyCamera::FlyCamera() {
	MovementSpeed = 30.0f;
	RotationSpeed = 100.0f;
}


FlyCamera::~FlyCamera() {
}


void FlyCamera::UpdateInput() {

	float yaw = m_Rotation.y;
	float pitch = m_Rotation.x;
	float deltaTime = _CTimeManager->m_DeltaTime;

	glm::vec3 movement = glm::vec3(0);

	if (_CInput->IsKeyDown(IKEY_W)) {
		movement.z -= MovementSpeed * deltaTime;
	}
	if (_CInput->IsKeyDown(IKEY_S)) {
		movement.z += MovementSpeed * deltaTime;
	}
	if (_CInput->IsKeyDown(IKEY_Q)) {
		movement.y -= MovementSpeed * deltaTime;
	}
	if (_CInput->IsKeyDown(IKEY_E)) {
		movement.y += MovementSpeed * deltaTime;
	}
	if (_CInput->IsKeyDown(IKEY_A)) {
		movement.x -= MovementSpeed * deltaTime;
	}
	if (_CInput->IsKeyDown(IKEY_D)) {
		movement.x += MovementSpeed * deltaTime;
	}

	m_Position = glm::translate(GetModelMatrix(), movement)[3];


	if (_CInput->IsKeyDown(IKEY_LeftArrow)) {
		m_Rotation.y += RotationSpeed * deltaTime;
	}
	if (_CInput->IsKeyDown(IKEY_RightArrow)) {
		m_Rotation.y -= RotationSpeed * deltaTime;
	}
	if (_CInput->IsKeyDown(IKEY_DownArrow)) {
		m_Rotation.x -= RotationSpeed * deltaTime;
	}
	if (_CInput->IsKeyDown(IKEY_UpArrow)) {
		m_Rotation.x += RotationSpeed * deltaTime;
	}

	SetDirty();

	//SetLookAt(m_Position, glm::vec3(0), glm::vec3(0, 1, 0));

	//if (CameraLookAt) {
//	if (RotateCamera) {
//		mainCamera.SetLookAt(CameraPos + glm::vec3(x, y, 0), glm::vec3(0), glm::vec3(0, 1, 0));
//	} else {
//		mainCamera.SetLookAt(CameraPos, glm::vec3(0), glm::vec3(0,1,0));
//	}
//}else {
//	mainCamera.SetPosition(CameraPos);
//	mainCamera.SetRotation(glm::vec3(0));
//}

}