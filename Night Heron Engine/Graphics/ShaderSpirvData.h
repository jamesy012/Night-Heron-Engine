#pragma once

#include "Types.h"

class Shader;

enum ShaderType {
	SHADER_VERTEX,
	SHADER_FRAGMENT,
	SHADERCOUNT,
};

enum class ShaderLoadRes {
	//compiled from supplied file
	SHADERLOAD_COMPILED,
	//file not changed, we used the old compiled source
	SHADERLOAD_LOAD,
	//error loading/compiling shader
	SHADERLOAD_ERROR
};

namespace glslang {
	class TShader;
	class TProgram;
};


class ShaderSpirvData {
public:
	ShaderSpirvData();

	ShaderLoadRes LoadFromFile(CMString a_FilePath);
	ShaderLoadRes Reload();
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