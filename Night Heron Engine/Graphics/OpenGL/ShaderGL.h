#pragma once

#include "..\API\Shader.h"

class ShaderUniformBlockGL : public ShaderUniformBlock {
public:
	unsigned int m_ID = 0;
	unsigned int m_SlotID = 0;

	virtual void UpdateBuffer(void * a_Object) override;
};

class ShaderGL : public Shader {




	// Inherited via Shader
	virtual void AddShader_Internal(ShaderTypes a_Type, std::vector<unsigned int> a_Code) override;
	//virtual void AddShader_Internal(ShaderTypes a_Type, std::string a_Path) = 0;

	void BindTexture(std::string a_Name, unsigned int a_Index);

	virtual void Link_Internal() override;

	virtual void Use() override;

	virtual void AddBuffer(ShaderUniformBlock* a_Block, std::string a_StructName) override;

private:

	unsigned int m_GLShaderIndex[(int)ShaderTypes::SHADERCOUNT] = { 0 };

	unsigned int m_Program = 0;

};
