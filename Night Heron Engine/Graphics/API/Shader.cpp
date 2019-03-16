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

#define ShaderCachePath "ShaderCache\\"

extern Shader* _CCurrentShader = nullptr;


Shader::Shader() {
	_CShaderManager->m_Shaders.Add(this);
}


Shader::~Shader() {
	_CShaderManager->m_Shaders.Remove(this);
}

void Shader::AddBuffer(ShaderUniformBlock * a_Block, CMString a_StructName) {
	m_AttachedUniforms.push_back({a_Block, a_StructName});
	AddBuffer_Internal(a_Block, a_StructName);
}

std::vector<unsigned int> Shader::loadSpirvFromPath(std::string a_Path) {
	size_t shaderSize;
	std::vector<unsigned int> code;


	std::ifstream is(a_Path, std::ios::binary | std::ios::in | std::ios::ate);

	if (is.is_open()) {
		shaderSize = is.tellg();
		is.seekg(0, std::ios::beg);
		// Copy file contents into a buffer
		code.resize(shaderSize / (sizeof(unsigned int) / sizeof(char)));
		is.read((char*)&code[0], shaderSize);
		is.close();
		assert(shaderSize > 0);
	}

	return code;
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
