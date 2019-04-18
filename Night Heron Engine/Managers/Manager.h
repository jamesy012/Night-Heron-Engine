#pragma once

#include "Types.h"
#include "SimpleMath.h"

class Model;
class Material;
class ShaderUniformBlock;
class Scene;

class Manager {
public:
	Manager();
	~Manager();

	void ImGuiWindow();

	void RegisterShaderUniform(ShaderUniformBlock* a_Uniform, CMString a_SlotName);
	ShaderUniformBlock* GetShaderUniform(CMString a_SlotName);

	void FindAllTextures();

	CMArray<Model*> m_Models;
	CMArray<Material*> m_Materials;
	Scene* m_CurrentScene = nullptr;

	bool m_ShowObjects;
	bool m_ShowModels;
	bool m_ShowMaterials;
	bool m_ShowTextures;

	void AddModel(Model* a_Model);
	Model* GetModel(CMString a_FileName);

	//todo: remove this, get this another way
	class ShaderUniformBlock* tempPVMUniform;
private:
	void ImGuiObjects();
	void ImGuiModels();
	void ImGuiMaterials();

	void InlineImGoiRenderTargetSettings();

	class RenderTarget* m_CommonRT;

	struct ShaderUniform {
		ShaderUniformBlock* a_Block;
		CMString a_Name;
	};

	CMArray<ShaderUniform> m_ShaderUniforms;

	bool m_IsRtDirty = true;
	SimpleVec3 m_RTPos = SimpleVec3(4, -4, 10);
	float m_RTFov = 20.0f;
};

extern class Manager* _CManager;
