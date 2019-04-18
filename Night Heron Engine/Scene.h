#pragma once

#include "Interfaces/Saveable.h"
#include "Types.h"

class Object;
class Manager;
class Camera;

class Scene : public Saveable {
	friend Manager;
public:
	CMString m_Name;

	void Start();
	void Update();
	void Draw();

	// Inherited via Saveable
	virtual bool LoadData_Internal(nlohmann::json& a_Json) override;
	virtual void SaveData_Internal(nlohmann::json& a_Json) override;

	void AddObject(Object* a_Object);
	Object* GetObjectByName(const CMString a_Name) const;

	Camera* m_Camera;
private:
	CMArray<Object*> m_Objects;
};