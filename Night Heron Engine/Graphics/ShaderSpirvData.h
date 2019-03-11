#pragma once

#include "Types.h"

class Shader;

enum ShaderType {
	SHADER_VERTEX,
	SHADER_FRAGMENT,
	SHADERCOUNT,
};

namespace glslang {
	class TShader;
	class TProgram;
};


class ShaderSpirvData {
public:
	ShaderSpirvData();

	void LoadFromFile(CMString a_FilePath);
	void Reload();
	ShaderType m_ShaderType;

	void AddShader(Shader* a_Shader);

	bool m_HasBeenLoaded = false;

	CMFilePath m_FilePath;

	CMArray<Shader*> m_AttachedShaders;
private:
	void GetTypeFromFilePath();
	unsigned int ShaderTypeToEShLanguage();

	bool GenerateGLSlangData(CMString a_Code);
	bool GenerateSpirvData();

	void SaveInfoFile(bool a_DidFail);

	glslang::TShader* m_Shader;
	glslang::TProgram* m_Program;

	unsigned char m_Hash[16];

};