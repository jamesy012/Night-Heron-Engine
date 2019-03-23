#pragma once

#include "Types.h"
class ShaderSpirvData;
class Shader;

extern class ShaderManager* _CShaderManager;

class ShaderManager {
public:
	ShaderManager();
	~ShaderManager();

	void FindAllShaders();

	void ImGuiWindow(bool* a_Open);

	ShaderSpirvData* GetShader(CMString a_FilePath);

	CMArray<Shader*> m_Shaders;
private:
	void SeachFolder(CMString a_Path);
	bool IsFileAShader(CMString a_FilePath);

	CMArray<CMString> m_ShaderPaths;
	CMArray<ShaderSpirvData*> m_ShaderObjects;

	const CMArray<CMStringHash> m_ShaderFileTypes = { ".vert", ".frag" };
	CMString m_CurrentPath = "";

	void ImGuiWindowTab1();
	void ImGuiWindowTab2();

	int m_ChangeToTab = -1;
	int m_ChangeToIndex = -1;

};

