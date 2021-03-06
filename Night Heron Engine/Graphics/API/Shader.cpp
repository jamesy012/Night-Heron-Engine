#include "stdafx.h"
#include "Shader.h"

#include <fstream> //std::ifstream
#include <sstream> //std::stringstream
#include <string> //std::string

#include <glslang-master/glslang/Public/ShaderLang.h>
#include <glslang-master/glslang/MachineIndependent/localintermediate.h>
#include <glslang-master/SPIRV/disassemble.h>
#include <glslang-master/SPIRV/GlslangToSpv.h>

#include <Windows.h>
#include <wincrypt.h>

#include "Managers/ShaderSpirvManager.h"
#include "Managers/Manager.h"

#include "nlohmann/json.hpp"


#define ShaderCachePath "ShaderCache\\"

extern Shader* _CCurrentShader = nullptr;

Shader::Shader() {
}

Shader::~Shader() {
}

void Shader::AddBuffer(ShaderUniformBlock* a_Block, CMString a_StructName) {
	if (a_Block != nullptr) {
		m_AttachedUniforms.Add({ a_Block, a_StructName, true });
		AddBuffer_Internal(a_Block, a_StructName);
	} else {
		m_AttachedUniforms.Add({ a_Block, a_StructName, false });
	}
}

bool Shader::AddBuffer(CMString a_StructName) {
	ShaderUniformBlock* uniform = _CManager->GetShaderUniform(a_StructName);
	AddBuffer(uniform, a_StructName);
	return uniform != nullptr;
}

void Shader::FindUnlinkedUniforms() {
	//CMASSERT_MSG(true, "FindUnlinkedUniforms unused");
	for (uint i = 0; i < m_AttachedUniforms.Length(); i++) {
		if (!m_AttachedUniforms[i].m_HasLinked) {
			ShaderUniformBlock* block = _CManager->GetShaderUniform(m_AttachedUniforms[i].m_Name);
			if (block) {
				AddBuffer_Internal(block, m_AttachedUniforms[i].m_Name);
				m_AttachedUniforms[i].m_Block = block;
				m_AttachedUniforms[i].m_HasLinked = true;
			}
		}
	}
}

std::vector<unsigned int> Shader::loadSpirvFromPath(std::string a_Path) {
	uint shaderSize;
	std::vector<unsigned int> code;

	std::ifstream is(a_Path, std::ios::binary | std::ios::in | std::ios::ate);

	if (is.is_open()) {
		shaderSize = (uint)is.tellg();
		is.seekg(0, std::ios::beg);
		// Copy file contents into a buffer
		code.resize(shaderSize / (sizeof(unsigned int) / sizeof(char)));
		is.read((char*)& code[0], shaderSize);
		is.close();
		assert(shaderSize > 0);
	}

	return code;
}

bool Shader::LoadData_Internal(nlohmann::json & a_Json) {
	SetDebugObjName(a_Json["Debug Name"].get<CMString>());

	if (a_Json.contains("Shaders")) {
		auto shaders = a_Json["Shaders"];
		for (nlohmann::json::iterator it = shaders.begin(); it != shaders.end(); ++it) {
			auto obj = *it;
			AddShader(_CShaderSpirvManager->GetShaderPart(obj.get<CMString>()));
		}
		LinkShaders();
	}
	//if (a_Json.contains("Uniforms")) {
	//	auto uniforms = a_Json["Uniforms"];
	//	for (nlohmann::json::iterator it = uniforms.begin(); it != uniforms.end(); ++it) {
	//		auto obj = *it;
	//		CMString uniformName = obj.get<CMString>();
	//		ShaderUniformBlock* uniform = _CManager->GetShaderUniform(uniformName);
	//		AddBuffer(uniform, uniformName);
	//	}
	//}
	return true;
}

void Shader::SaveData_Internal(nlohmann::json & a_Json) {
	a_Json["Debug Name"] = m_DebugName;
	{
		auto& sfo = a_Json["Shaders"];
		for (uint i = 0; i < m_ShaderFileObjects.Length(); i++) {
			sfo[i] = m_ShaderFileObjects[i]->m_FilePath.m_FilePath;
		}
	}
	{
		auto& uniforms = a_Json["Uniforms"];
		//for (uint i = 0; i < m_AttachedUniforms.Length(); i++) {
		//	if (m_AttachedUniforms[i].m_HasLinked && (m_AttachedUniforms[i].m_Block && m_AttachedUniforms[i].m_Block->m_Registered)) {
		//		uniforms[i] = m_AttachedUniforms[i].m_Name;
		//	}
		//}
		uniforms[0] = "Todo";
	}
}

CMString Shader::GetShaderTypeString(ShaderType a_Type) {
	switch (a_Type) {
		case ShaderType::SHADER_VERTEX:
			return "Vertex";
		case ShaderType::SHADER_FRAGMENT:
			return "Fragment";
	}
	return CMString();
}

void Shader::AddShader(ShaderSpirvData * a_Shader) {
	ShaderInfo* info = &m_ShaderObjects[a_Shader->m_ShaderType];
	info->m_Path = a_Shader->m_FilePath.m_FilePath;
	info->m_HasBeenLoaded = a_Shader->m_HasBeenLoaded;
	info->m_IsUsed = true;

	a_Shader->AddShader(this);
	m_ShaderFileObjects.Add(a_Shader);
}

void Shader::RemoveShader(ShaderSpirvData * a_Shader) {
	ShaderInfo* info = &m_ShaderObjects[a_Shader->m_ShaderType];
	info->m_Path = "";
	info->m_HasBeenLoaded = false;
	info->m_IsUsed = false;

	a_Shader->RemoveShader(this);
	m_ShaderFileObjects.Remove(a_Shader);
}

void Shader::LinkShaders() {
	for (int i = 0; i < ShaderType::SHADERCOUNT; i++) {
		if (m_ShaderObjects[i].m_HasBeenLoaded && m_ShaderObjects[i].m_IsUsed) {
			AddShader_Internal((ShaderType)i, loadSpirvFromPath(ShaderCachePath + m_ShaderObjects[i].m_Path + ".spirv"));
		}
	}

	Link_Internal();
	SetDebugObjName_Internal();
}
