#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Interfaces/Jsonable.h"
#include "Generated/Objects_Night_Heron_Engine.h"


typedef glm::vec3 Vector3;
typedef glm::mat4 Matrix;

class Transform : public Jsonable {
ADD_OBJ_BASE(Transform)
public:
	Transform();
	~Transform();

	void SetPosition(Vector3 a_NewPosition);
	void SetRotation(Vector3 a_NewRotation);
	void SetRotation(glm::quat a_NewRotation);
	void SetScale(Vector3 a_NewScale);

	void SetLookAt(Vector3 a_Pos, Vector3 a_At, Vector3 a_Up);

	Matrix GetModelMatrix();

	inline void SetDirty() {
		m_IsDirty = true;
	}

	void Reset();

	Vector3 m_Position;
	Vector3 m_Rotation;
	Vector3 m_Scale;

	// Inherited via Jsonable
	virtual bool LoadData_Internal(nlohmann::json& a_Json) override;
	virtual void SaveData_Internal(nlohmann::json& a_Json) override;
protected:
	virtual void UpdateModelMatrix();

	bool m_IsDirty = true;
	Matrix m_ModelMatrix;
};
