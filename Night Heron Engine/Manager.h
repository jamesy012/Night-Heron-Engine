#pragma once

#include "Types.h"

class Object;
class Model;
class Material;

class Manager {
public:
	Manager();
	~Manager();

	void ImGuiWindow();

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
};

