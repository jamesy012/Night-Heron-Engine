#pragma once

#include "Types.h"
class ShaderSpirvData;

class ShaderManager {
public:
	ShaderManager();
	~ShaderManager();

	void FindAllShaders();

	void ImGuiWindow(bool* a_Open);

	ShaderSpirvData* GetShader(CMString a_FilePath);

private:
	void SeachFolder(CMString a_Path);
	bool IsFileAShader(CMString a_FilePath);

	CMArray<CMString> m_ShaderPaths;
	CMArray<ShaderSpirvData*> m_Shaders;

	struct ShaderFile {
		ShaderFile(const char* a_FileExtention) {			
			CMString(a_FileExtention).Hash(m_ExtenstionHash);
		}
		uchar m_ExtenstionHash[16] = { 0 };
	};


	CMArray<ShaderFile> m_FileTypes = { ".vert", ".frag" };

	CMString m_CurrentPath = "";
};

