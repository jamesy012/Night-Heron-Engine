#pragma once

#include "..\API\Shader.h"
#include "..\API\Mesh.h"

#include <d3d11.h>

#include <vector>

class ShaderUniformBlockDX11 : public ShaderUniformBlock {
public:
	struct ID3D11Buffer* m_Buffer;

	virtual void UpdateBuffer(void * a_Object) override;

};

class ShaderDX11 : public Shader {
public:

	void AddShader_Internal(ShaderTypes a_Type, std::vector<unsigned int> a_Code) override;
	//virtual void AddShader_Internal(ShaderTypes a_Type, std::string a_Path) = 0;

	virtual void BindTexture(std::string a_Name, unsigned int a_Index) override;


	void Link_Internal() override;
	void Use() override;

	// Inherited via Shader


	virtual void AddBuffer(ShaderUniformBlock* a_Block, std::string a_StructName) override;
private:
	struct ID3D10Blob* VS_Buffer;
	struct ID3D10Blob* PS_Buffer;

	struct ID3D11VertexShader* VS;
	struct ID3D11PixelShader* PS;

	struct ID3D11InputLayout* vertLayout;

	struct DX11CBufferData {
		ShaderUniformBlockDX11* m_Uniform;
		//ShaderUniformBlock::UniformSlotData m_Data;
		unsigned int m_VertexSlot = -1;
		unsigned int m_PixelSlot = -1;
	};

	std::vector<DX11CBufferData> m_CBuffers;


};