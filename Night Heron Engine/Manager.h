#pragma once

#include "Types.h"

class Object;
class Model;
class Material;
class ShaderUniformBlock;

class Manager {
public:
	Manager();
	~Manager();

	void ImGuiWindow();
	
	void RegisterShaderUniform(ShaderUniformBlock* a_Uniform, CMString a_SlotName);
	ShaderUniformBlock* GetShaderUniform(CMString a_SlotName);

	CMArray<Object*> m_Objects;
	CMArray<Model*> m_Models;
	CMArray<Material*> m_Materials;

	bool m_ShowObjects;
	bool m_ShowModels;
	bool m_ShowMaterials;

	//todo: remove this, get this another way
	class ShaderUniformBlock* tempPVMUniform;
private:
	void ImGuiObjects();
	void ImGuiModels();
	void ImGuiMaterials();

	class RenderTarget* m_CommonRT;

	struct ShaderUniform {
		ShaderUniformBlock* a_Block;
		CMString a_Name;
	};

	CMArray<ShaderUniform> m_ShaderUniforms;

};

extern class Manager* _CManager;
