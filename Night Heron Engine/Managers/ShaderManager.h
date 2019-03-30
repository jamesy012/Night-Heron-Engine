#pragma once

#include "ManagerBase.h"

#include "Types.h"

class ShaderSpirvData;
class Shader;

extern class ShaderManager* _CShaderManager;

class ShaderManager : public ManagerBase {
public:
	ShaderManager();
	~ShaderManager();


	void ImGuiWindow(bool* a_Open) override;

	ShaderSpirvData* GetShaderPart(CMString a_FilePath);
	Shader* GetShader(CMString a_FilePath);


	CMArray<Shader*> m_Shaders;
private:
	bool IsFileAOurType(CMString a_FilePath) override;
	void AddFromPath_Internal(CMString a_FilePath) override;

	CMArray<ShaderSpirvData*> m_ShaderObjects;

	const CMArray<CMStringHash> m_ShaderFileTypes = { ".vert", ".frag" };

	void ImGuiWindowTab1();
	void ImGuiWindowTab2();

	int m_ChangeToTab = -1;
	int m_ChangeToIndex = -1;

	CMArray<bool> m_UnsavedShaders;
};

