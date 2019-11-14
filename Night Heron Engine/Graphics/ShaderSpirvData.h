#pragma once

#include "Types/CMString.h"
#include "Types/CMFilePath.h"
#include "Types/CMArray.h"

class Shader;

struct SHADERDEFINES {
public:
	SHADERDEFINES() : m_Name(""), m_Value(0) {
	};
	/*constexpr*/ SHADERDEFINES(CMString a_Name, int a_Value) : m_Name(a_Name), m_Value(a_Value) {
	};

	CMString m_Name;
	int m_Value;
};

extern CMArray<SHADERDEFINES> ShaderDefines;

enum ShaderType {
	SHADER_VERTEX,
	SHADER_FRAGMENT,
	SHADER_COMPUTE,
	SHADERCOUNT,
	SHADER_INCLUDE,
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
	ShaderType m_ShaderType = ShaderType::SHADERCOUNT;

	void AddShader(Shader* a_Shader);
	void RemoveShader(Shader* a_Shader);

	void AddDefine(SHADERDEFINES* a_Define);

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

	bool m_HasPermutation;

	CMArray<SHADERDEFINES*> m_ShaderDefines;


	unsigned char m_Hash[16];
};
