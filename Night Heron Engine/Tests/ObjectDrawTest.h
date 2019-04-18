#pragma once

#include "Object.h"

class ShaderUniformBlock;

ADD_OBJ(ObjectDrawTest)
class ObjectDrawTest : public Object {
public:
	ObjectDrawTest() {};
	ObjectDrawTest(CMString a_String) : Object(a_String) {};

	virtual void Start() override;
	virtual void PreDraw() override;
	virtual void Draw() override;

	virtual CMString GetObjectClassName() { return "ObjectDrawTest"; };

	virtual void ImGuiExtras() override;
	// Inherited via Jsonable
	virtual bool LoadData_Internal(nlohmann::json& a_Json) override;
	virtual void SaveData_Internal(nlohmann::json& a_Json) override;

	ShaderUniformBlock* m_ModelUniform = nullptr;
	ShaderUniformBlock* m_ColorUniform = nullptr;

	Vector3 m_Color = Vector3(1);
};
