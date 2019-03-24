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

#include "Graphics/ShaderManager.h"
#include "Manager.h"

#define ShaderCachePath "ShaderCache\\"

extern Shader* _CCurrentShader = nullptr;


Shader::Shader() {
	_CShaderManager->m_Shaders.Add(this);
}


Shader::~Shader() {
	_CShaderManager->m_Shaders.Remove(this);
}

void Shader::AddBuffer(ShaderUniformBlock * a_Block, CMString a_StructName) {
	if (a_Block != nullptr) {
		m_AttachedUniforms.Add({ a_Block, a_StructName, true });
		AddBuffer_Internal(a_Block, a_StructName);
	} else {
		m_AttachedUniforms.Add({ a_Block, a_StructName, false });
	}
}

void Shader::FindUnlinkedUniforms() {
	for (uint i = 0; i < m_AttachedUniforms.Length(); i++) {
		if (!m_AttachedUniforms[i].m_HasLinked) {
			ShaderUniformBlock* block = _CManager->GetShaderUniform(m_AttachedUniforms[i].m_Name);
			if (block) {
				AddBuffer_Internal(block, m_AttachedUniforms[i].m_Name);
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
		is.read((char*)&code[0], shaderSize);
		is.close();
		assert(shaderSize > 0);
	}

	return code;
}

bool Shader::Load_Internal(CMArray<CMString> a_Splits) {
	uint line = 0;
	uint stage = 0;
	while (line < a_Splits.Length() - 1) {
		stage++;
		if (stage == 1) {
			SetDebugObjName(a_Splits[line++]);
		}
		if (stage == 2) {
			int shaders = CMString::StringToInt(a_Splits[line++]);
			for (int i = 0; i < shaders; i++) {
				AddShader(_CShaderManager->GetShaderPart(a_Splits[line++]));
			}
			LinkShaders();
		}
		if (stage == 3) {
			int uniforms = CMString::StringToInt(a_Splits[line++]);
			for (int i = 0; i < uniforms; i++) {
				ShaderUniformBlock* uniform = _CManager->GetShaderUniform(a_Splits[line]);
				AddBuffer(uniform, a_Splits[line++]);
			}
		}
	}
	return true;
}

CMString Shader::GetData_Internal() {
	CMString data;

	data += m_DebugName + "\n";
	{
		data += CMString::IntToString(m_ShaderFileObjects.Length()) + '\n';
		for (uint i = 0; i < m_ShaderFileObjects.Length(); i++) {
			data += m_ShaderFileObjects[i]->m_FilePath.m_FilePath + '\n';
		}
	}
	{
		CMString uniformData = "";
		int numUniforms = 0;
		for (uint i = 0; i < m_AttachedUniforms.Length(); i++) {
			if (m_AttachedUniforms[i].m_HasLinked && (m_AttachedUniforms[i].m_Block && m_AttachedUniforms[i].m_Block->m_Registered)) {
				uniformData += m_AttachedUniforms[i].m_Name + '\n';
				numUniforms++;
			}
		}
		data += CMString::IntToString(numUniforms) + '\n';
		data += uniformData;
	}

	return data;
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

void Shader::LinkShaders() {
	for (int i = 0; i < ShaderType::SHADERCOUNT; i++) {
		if (m_ShaderObjects[i].m_HasBeenLoaded && m_ShaderObjects[i].m_IsUsed) {
			AddShader_Internal((ShaderType)i, loadSpirvFromPath(ShaderCachePath + m_ShaderObjects[i].m_Path + ".spirv"));
		}
	}

	Link_Internal();
	SetDebugObjName_Internal();
}
