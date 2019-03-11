#pragma once

#include <string>
#include <vector>

#include "GFXObj.h"

#include "Graphics/ShaderSpirvData.h"

namespace glslang {
	class TShader;
};

class ShaderUniformBlock : public GFXObj {
public:
	//unsigned int m_Slot = 0;
	unsigned int m_Size = 0;

	virtual void UpdateBuffer(void* a_Object) = 0;
};

class Shader : public GFXObj {
public:

	std::string Path;

	void AddShader(ShaderSpirvData* a_Shader);
	void AddShader(ShaderType a_Type, std::string a_Path);
	void LinkShaders();
	virtual void Use() = 0;

	virtual void Reload() = 0;

	virtual void AddBuffer(ShaderUniformBlock* a_Block, CMString a_StructName);
	virtual void BindTexture(std::string a_Name, unsigned int a_Index) = 0;

	std::vector<unsigned int> loadSpirvFromPath(std::string a_Path);
	std::string loadTextFromPath(std::string a_Path);//move to Util class???

	bool m_ShouldPrintCode = false;
	bool m_ShoudRegenerateCode = false;

protected:
	virtual void AddBuffer_Internal(ShaderUniformBlock* a_Block, CMString a_StructName) = 0;
	virtual void AddShader_Internal(ShaderType a_Type, std::vector<unsigned int> a_Code) = 0;
	//virtual void AddShader_Internal(ShaderTypes a_Type, std::string a_Path) = 0;
	virtual void Link_Internal() = 0;

	unsigned int ShaderTypeToEShLanguage(ShaderType a_Type);
	ShaderType EShLanguageToShaderType(unsigned int a_Type);

	//has this shader been linked yet?
	bool m_IsLinked;

	struct UniformBufferObject {
		ShaderUniformBlock* m_Block;
		CMString m_Name;
	};

	CMArray<UniformBufferObject> m_AttachedUniforms;

private:
	struct ShaderInfo {
		bool m_IsUsed;
		bool m_HasBeenLoaded;
		std::string m_Path;
	};
	ShaderInfo m_Shaders[(int)ShaderType::SHADERCOUNT] = { false, false, "" };
	//glslang::TShader* m_Shaders[(int)ShaderTypes::SHADERCOUNT] = {0};

};