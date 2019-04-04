#pragma once

#include "..\API\Shader.h"

class ShaderUniformBlockGL : public ShaderUniformBlock {
public:
	~ShaderUniformBlockGL();
	unsigned int m_ID = 0;
	unsigned int m_SlotID = 0;

	virtual void UpdateBuffer(void* a_Object) override;
private:
	virtual void SetDebugObjName_Internal() override;
};

class ShaderGL : public Shader {
public:
	~ShaderGL();

	// Inherited via Shader
	virtual void AddShader_Internal(ShaderType a_Type, std::vector<unsigned int> a_Code) override;
	//virtual void AddShader_Internal(ShaderType a_Type, std::string a_Path) = 0;

	void BindTexture(std::string a_Name, unsigned int a_Index);

	virtual void Link_Internal() override;

	virtual void Use() override;
	virtual void Reload() override;

private:
	virtual void AddBuffer_Internal(ShaderUniformBlock* a_Block, CMString a_StructName) override;
	virtual void SetDebugObjName_Internal() override;

	unsigned int m_GLShaderIndex[(int)ShaderType::SHADERCOUNT] = { 0 };

	void DeleteShaders();

	unsigned int m_Program = 0;
};
