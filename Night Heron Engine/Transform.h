#pragma once

#include <glm/glm.hpp>

typedef glm::vec3 Vector3;
typedef glm::mat4 Matrix;

class Transform {
public:
	Transform();
	~Transform();
	
	void SetPosition(Vector3 a_NewPosition);
	void SetRotation(Vector3 a_NewRotation);
	void SetScale(Vector3 a_NewScale);

	Matrix GetModelMatrix();

	inline void SetDirty() {
		m_IsDirty = true;
	}

	void Reset();

	Vector3 m_Position;
	Vector3 m_Rotation;
	Vector3 m_Scale;
private:
	void UpdateModelMatrix();

	bool m_IsDirty;
	Matrix m_ModelMatrix;
};