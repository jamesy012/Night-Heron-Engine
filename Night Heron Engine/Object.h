#pragma once

#include "Types/CMString.h"
#include "Transform.h"

#include "Interfaces/Jsonable.h"

#include "Generated/Objects_Night_Heron_Engine.h"

class Model;

ADD_OBJ(Object)
class Object : public Jsonable {
public:
	Object();
	Object(CMString a_Name);
	Object(CMString a_Name, Transform a_Transform);
	~Object();

	virtual void Start() {};
	virtual void Update() {};
	virtual void PreDraw() {};
	virtual void Draw() {};

	virtual void ImGuiExtras();

	//automate this part with the generated code
	virtual CMString GetObjectClassName() { return "Object"; };

	CMString m_Name;
	Transform m_Transform;
	Model* m_ObjectModel;

	// Inherited via Jsonable
	virtual bool LoadData_Internal(nlohmann::json& a_Json) override;
	virtual void SaveData_Internal(nlohmann::json& a_Json) override;
};


ADD_OBJ(SinObject)
class SinObject : public Object {
public:
	virtual void Update() override;
};
