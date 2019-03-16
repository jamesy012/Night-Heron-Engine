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
private:
	void ImGuiObjects();
	void ImGuiModels();
	void ImGuiMaterials();
};

