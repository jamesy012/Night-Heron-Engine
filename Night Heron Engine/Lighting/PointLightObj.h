#pragma once
#include "Object.h"


ADD_OBJ(PointLightObj)
class PointLightObj : public Object {
public:
	PointLightObj() {};
	PointLightObj(CMString a_String) : Object(a_String) {};
	/*
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

	Vector3 m_Color = Vector3(1);*/
};
