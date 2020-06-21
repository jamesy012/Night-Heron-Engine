#pragma once
#include "Camera.h"
class FlyCamera :
	public Camera {
	ADD_OBJ(FlyCamera);
public:
	FlyCamera();
	~FlyCamera();

	void UpdateInput();

	float MovementSpeed;
	float RotationSpeed;
	bool m_MouseMovement = false;
};

