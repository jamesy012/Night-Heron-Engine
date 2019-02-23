#pragma once

#include <string>
#include <vector>

enum ShaderTypes {
	SHADER_VERTEX,
	SHADER_FRAGMENT,
	SHADERCOUNT,
};

namespace glslang {
	class TShader;
};

class ShaderUniformBlock {
public:
	//unsigned int m_Slot = 0;
	unsigned int m_Size = 0;

	virtual void UpdateBuffer(void* a_Object) = 0;
};

class Shader {
public:
	std::string Path;

	void AddShader(ShaderTypes a_Type, std::string a_Path);
	void LinkShaders();
	virtual void Use() = 0;

	virtual void AddBuffer(ShaderUniformBlock* a_Block, std::string a_StructName) = 0;
	virtual void BindTexture(std::string a_Name, unsigned int a_Index) = 0;

	std::vector<unsigned int> loadSpirvFromPath(std::string a_Path);
	std::string loadTextFromPath(std::string a_Path);//move to Util class???

	bool m_ShouldPrintCode = false;

protected:
	virtual void AddShader_Internal(ShaderTypes a_Type, std::vector<unsigned int> a_Code) = 0;
	//virtual void AddShader_Internal(ShaderTypes a_Type, std::string a_Path) = 0;
	virtual void Link_Internal() = 0;

	unsigned int ShaderTypeToEShLanguage(ShaderTypes a_Type);
	ShaderTypes EShLanguageToShaderType(unsigned int a_Type);


private:
	struct ShaderInfo {
		bool m_HasBeenGenerated;
		bool m_IsUsed;
		glslang::TShader* m_Shader;
		bool m_HasBeenChanged;
		unsigned char m_Hash[16];
		std::string m_Path;
	};
	ShaderInfo m_Shaders[(int)ShaderTypes::SHADERCOUNT] = { false, false, 0, true, {0}, "" };
	//glslang::TShader* m_Shaders[(int)ShaderTypes::SHADERCOUNT] = {0};
	

};