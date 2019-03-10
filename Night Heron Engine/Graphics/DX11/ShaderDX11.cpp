#include "ShaderDX11.h"

#include "GFXDX11.h"

#include "d3dcompiler.h"
#include <d3d11shader.h>

#include <fstream>
#include <SPIRV-Cross-master/spirv_hlsl.hpp>


//D3D11_INPUT_ELEMENT_DESC VertexLayout[] = {
//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, m_Color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
//};
D3D11_INPUT_ELEMENT_DESC VertexLayout[] = {
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, m_UV), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, m_Color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
UINT numVertexLayoutElements = ARRAYSIZE(VertexLayout);


ShaderDX11::~ShaderDX11() {
	if (VS_Buffer) {
		VS_Buffer->Release();
	}
	if (VS) {
		VS->Release();
	}
	if (PS_Buffer) {
		PS_Buffer->Release();
	}
	if (PS) {
		PS->Release();
	}
	if (vertLayout) {
		vertLayout->Release();
	}
	m_CBuffers.clear();
}

void ShaderDX11::AddShader_Internal(ShaderTypes a_Type, std::vector<unsigned int> a_Code) {

	spirv_cross::CompilerHLSL hlsl(a_Code);

	spirv_cross::ShaderResources resources = hlsl.get_shader_resources();
	//// Get all sampled images in the shader.
	for (auto &resource : resources.sampled_images) {
		unsigned set = hlsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
		unsigned binding = hlsl.get_decoration(resource.id, spv::DecorationBinding);
		printf("Image %s at set = %u, binding = %u\n", resource.name.c_str(), set, binding);
	
		// Modify the decoration to prepare it for GLSL.
		//hlsl.unset_decoration(resource.id, spv::DecorationDescriptorSet);
		//
		//// Some arbitrary remapping if we want.
		//hlsl.set_decoration(resource.id, spv::DecorationBinding, set * 16 + binding);
		unsigned int location = hlsl.get_decoration(resource.id, spv::DecorationLocation);
		hlsl.set_decoration(resource.id, spv::DecorationBinding, location);

	}

	// Set some options.
	spirv_cross::CompilerHLSL::Options options;
	options.shader_model = 50;
	hlsl.set_hlsl_options(options);

	// Compile to GLSL, ready to give to GL driver.
	std::string source = hlsl.compile();

	if (m_ShouldPrintCode) {
		printf("Final Source: hlsl (TYPE:%i)\n%s\n", a_Type, source.c_str());
	}

	HRESULT hr;

	ID3D10Blob* shaderError;
	switch (a_Type) {
		case ShaderTypes::SHADER_VERTEX:
			hr = D3DCompile(source.c_str(), source.size(), NULL, nullptr, nullptr, "main", "vs_5_0", 0, 0, &VS_Buffer, &shaderError);
			if (FAILED(hr)) {
				printf("VERTEX Test SHADER ERROR\n%s",(char*)(shaderError->GetBufferPointer()));
				return;
			}
			GFXDX11::GetCurrentContex()->m_Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
			if (FAILED(hr)) {
				printf("PIXEL Test SHADER ERROR CreateVertexShader\n%s", (char*)(shaderError->GetBufferPointer()));
				return;
			}
			return;
		case ShaderTypes::SHADER_FRAGMENT:
			hr = D3DCompile(source.c_str(), source.size(), NULL, nullptr, nullptr, "main", "ps_5_0", 0, 0, &PS_Buffer, &shaderError);
			if (FAILED(hr)) {
				printf("PIXEL Test SHADER ERROR\n%s", (char*)(shaderError->GetBufferPointer()));
				return;
			}
			GFXDX11::GetCurrentContex()->m_Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);
			if (FAILED(hr)) {
				printf("PIXEL Test SHADER ERROR CreatePixelShader\n%s", (char*)(shaderError->GetBufferPointer()));
				return;
			}
			return;

	}
}

void ShaderDX11::Link_Internal() {
	HRESULT hr;
	if (VS_Buffer) {
		hr = GFXDX11::GetCurrentContex()->m_Device->CreateInputLayout(VertexLayout, numVertexLayoutElements, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &vertLayout);
		if (FAILED(hr)) {
			printf("CreateInputLayout ERROR %ld\n", hr);
			return;
		}
		m_IsLinked = true;
	}
}

void ShaderDX11::Use() {
	GFXDX11::GetCurrentContex()->m_DevCon->IASetInputLayout(vertLayout);
	GFXDX11::GetCurrentContex()->m_DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (int i = 0; i < m_CBuffers.size(); i++) {
		if (m_CBuffers[i].m_VertexSlot != -1) {
			GFXDX11::GetCurrentContex()->m_DevCon->VSSetConstantBuffers(m_CBuffers[i].m_VertexSlot, 1, &m_CBuffers[i].m_Uniform->m_Buffer);
		}
		if (m_CBuffers[i].m_PixelSlot != -1) {
			GFXDX11::GetCurrentContex()->m_DevCon->PSSetConstantBuffers(m_CBuffers[i].m_PixelSlot, 1, &m_CBuffers[i].m_Uniform->m_Buffer);
		}

	}

	GFXDX11::GetCurrentContex()->m_DevCon->VSSetShader(VS, 0, 0);
	GFXDX11::GetCurrentContex()->m_DevCon->PSSetShader(PS, 0, 0);
}

void ShaderDX11::AddBuffer(ShaderUniformBlock* a_Block, std::string a_StructName) {
	if (!m_IsLinked) {
		return;
	}
	DX11CBufferData bd;
	//bd.m_Data = a_Slot;
	bd.m_Uniform = (ShaderUniformBlockDX11*)a_Block;

	ID3D11ShaderReflection* reflector;
	D3D11_SHADER_DESC shaderDesc;
	D3D11_SHADER_BUFFER_DESC desc;
	{
		D3DReflect(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflector);
		ZeroMemory(&shaderDesc, sizeof(D3D11_SHADER_DESC));
		reflector->GetDesc(&shaderDesc);
		
		for (int i = 0; i < shaderDesc.ConstantBuffers; i++) {

			ID3D11ShaderReflectionConstantBuffer* cbuffer = reflector->GetConstantBufferByIndex(i);
			ZeroMemory(&desc, sizeof(D3D11_SHADER_BUFFER_DESC));
			cbuffer->GetDesc(&desc);
			if (desc.Name == a_StructName) {
				bd.m_VertexSlot = i;
				break;
			}
		}

		reflector->Release();
	}
	{
		D3DReflect(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflector);

		reflector->GetDesc(&shaderDesc);

		for (int i = 0; i < shaderDesc.ConstantBuffers; i++) {

			ID3D11ShaderReflectionConstantBuffer* cbuffer = reflector->GetConstantBufferByIndex(i);
			ZeroMemory(&desc, sizeof(D3D11_SHADER_BUFFER_DESC));
			cbuffer->GetDesc(&desc);
			if (desc.Name == a_StructName) {
				bd.m_PixelSlot = i;
				break;
			}
		}

		reflector->Release();
	}

	m_CBuffers.push_back(bd);

}

void ShaderDX11::SetDebugObjName_Internal() {
	if (VS) {
		CMString temp = m_DebugName + " VS Shader";
		VS->SetPrivateData(WKPDID_D3DDebugObjectName, temp.Size(), temp.Get());
	}
	if (PS) {
		CMString temp = m_DebugName + " PS Shader";
		PS->SetPrivateData(WKPDID_D3DDebugObjectName, temp.Size(), temp.Get());
	}
	if (vertLayout) {
		CMString temp = m_DebugName + " Vert layout";
		vertLayout->SetPrivateData(WKPDID_D3DDebugObjectName, temp.Size(), temp.Get());
	}
}

void ShaderDX11::BindTexture(std::string a_Name, unsigned int a_Index) {
}


ShaderUniformBlockDX11::~ShaderUniformBlockDX11() {
	if (m_Buffer) {
		m_Buffer->Release();
	}
}

void ShaderUniformBlockDX11::UpdateBuffer(void * a_Object) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	GFXDX11::GetCurrentContex()->m_DevCon->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//  Update the vertex buffer here.
	memcpy(mappedResource.pData, a_Object, m_Size);
	//  Reenable GPU access to the vertex buffer data.
	GFXDX11::GetCurrentContex()->m_DevCon->Unmap(m_Buffer, 0);
}

void ShaderUniformBlockDX11::SetDebugObjName_Internal() {
	if (m_Buffer) {
		m_Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, m_DebugName.Size(), m_DebugName.Get());
	}
}
