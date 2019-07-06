#pragma once

#include "Types.h"

class Shader;

enum ShaderType {
	SHADER_VERTEX,
	SHADER_FRAGMENT,
	SHADER_INCLUDE,
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
	void RemoveShader(Shader* a_Shader);

	bool m_HasBeenLoaded = false;

	CMFilePath m_FilePath;

	CMString m_SourceFile;

	CMArray<Shader*> m_AttachedShaders;

	//todo: possibly have another format or use this class for includes too. as they need to have a hash
	CMArray<ShaderSpirvData*> m_IncludeList;
private:
	void GetTypeFromFilePath();
	unsigned int ShaderTypeToEShLanguage();

	bool GenerateGLSlangData();
	bool GenerateSpirvData();

	void SaveInfoFile(bool a_DidFail);

	glslang::TShader* m_Shader;
	glslang::TProgram* m_Program;

	unsigned char m_Hash[16];
};
