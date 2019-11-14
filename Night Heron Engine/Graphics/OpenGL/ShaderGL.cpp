#include "ShaderGL.h"

#include <GL/glew.h>
#include "Managers/Arguments.h"

//#include "..\SPIRV-Cross-master\spirv_glsl.hpp"
#include <fstream>

#include <SPIRV-Cross-master/spirv_glsl.hpp>

unsigned int getOpenglShaderType(ShaderType a_Type) {
	switch (a_Type) {
		case ShaderType::SHADER_VERTEX:
			return GL_VERTEX_SHADER;
		case ShaderType::SHADER_FRAGMENT:
			return GL_FRAGMENT_SHADER;
		case ShaderType::SHADER_COMPUTE:
			return GL_COMPUTE_SHADER;
		default:
			CMASSERT_MSG(true, "Shader not defined");
			return 0;
	}
}

ShaderGL::~ShaderGL() {
	DeleteShaders();
	if (m_Program != 0) {
		glDeleteProgram(m_Program);
		m_Program = 0;
	}
}

void ShaderGL::AddShader_Internal(ShaderType a_Type, std::vector<unsigned int> a_Code) {
	CMLOG_SCOPED_NAME(m_FilePath.m_FileName + " - " + GetShaderTypeString(a_Type));
	CMLOG("Shader: %s\n", m_DebugName.Get());
	CMLOG_SCOPED_INDENT;

	spirv_cross::CompilerGLSL glsl(a_Code);

	spirv_cross::ShaderResources resources = glsl.get_shader_resources();
	// Get all sampled images in the shader.
	for (auto& resource : resources.sampled_images) {
		unsigned set = glsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
		unsigned binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
		unsigned location = glsl.get_decoration(resource.id, spv::DecorationLocation);
		CMLOG("Image %s at set = %u, binding = %u, location = %u\n", resource.name.c_str(), set, binding, location);

		// Modify the decoration to prepare it for GLSL.
		glsl.unset_decoration(resource.id, spv::DecorationDescriptorSet);
		//glsl.unset_decoration(resource.id, spv::DecorationBinding);
		//glsl.set_decoration(resource.id, spv::DecorationBinding, location);
		glsl.unset_decoration(resource.id, spv::DecorationLocation);

		// Some arbitrary remapping if we want.
		//glsl.set_decoration(resource.id, spv::DecorationBinding, set * 16 + binding);

		m_HasTextureForSlot[location] = true;
	}

	for (auto& resource : resources.uniform_buffers) {
		unsigned set = glsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
		unsigned binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
		unsigned int location = glsl.get_decoration(resource.id, spv::DecorationLocation);
		CMLOG("uniform %s at set = %u, binding = %u, location %u\n", resource.name.c_str(), set, binding, location);

		// Modify the decoration to prepare it for GLSL.
		//hlsl.unset_decoration(resource.id, spv::DecorationDescriptorSet);
		//
		//// Some arbitrary remapping if we want.
		//hlsl.set_decoration(resource.id, spv::DecorationBinding, set * 16 + binding);
		glsl.unset_decoration(resource.id, spv::DecorationLocation);

		if (binding != 0 && false) {
			CMLOG_SCOPED_INDENT;
			glsl.set_decoration(resource.id, spv::DecorationLocation, binding);

			binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
			location = glsl.get_decoration(resource.id, spv::DecorationLocation);
			CMLOG("\tuniform %s moved to at set = %u, binding = %u, location %u\n", resource.name.c_str(), set, binding, location);
		}
		CMLOG_SCOPED_NAME(CMLOG_GET_NAME + " " + resource.name);
		AddBuffer(resource.name);
	}

	// Set some options.
	spirv_cross::CompilerGLSL::Options options;
	options.version = 450;
	glsl.set_common_options(options);

	// Compile to GLSL, ready to give to GL driver.
	std::string source = glsl.compile();

	if (_CArguments->IsArgument("PrintShaderCode")) {
		CMLOG("Final Source: glsl (TYPE:%i)\n%s\n", a_Type, source.c_str());
	}

	unsigned int index = glCreateShader(getOpenglShaderType(a_Type));

	const char* shaderCode = source.c_str();
	const char* const* shaderCodePtr = &shaderCode;

	glShaderSource(index, 1, shaderCodePtr, 0);
	glCompileShader(index);

	GLint success;
	glGetShaderiv(index, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetShaderInfoLog(index, 512, NULL, infoLog);
		CMLOG("ERROR::SHADER::_%u_::%s\n%s\n", index, "COMPILE ERROR", infoLog);
	}

	m_GLShaderIndex[a_Type] = index;
}

void ShaderGL::Link_Internal() {
	m_Program = glCreateProgram();

	for (int i = 0; i < ShaderType::SHADERCOUNT; i++) {
		if (m_GLShaderIndex[i] != 0) {
			glAttachShader(m_Program, m_GLShaderIndex[i]);
		}
	}

	glLinkProgram(m_Program);

	GLint success;
	glGetProgramiv(m_Program, GL_LINK_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetProgramInfoLog(m_Program, 512, NULL, infoLog);
		if (infoLog[0] == -52) {
			memcpy(infoLog, "No useful data.", 16);
		}
		CMLOG("ERROR::SHADER::_%u_::%s\n%s\n", m_Program, "Linking ERROR", infoLog);
		DeleteShaders();
		glDeleteProgram(m_Program);
		glUseProgram(0);
		return;
	}

	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 32; // maximum name length
	GLchar name[bufSize]; // variable name in GLSL
	GLsizei length; // name length
	glGetProgramiv(m_Program, GL_ACTIVE_UNIFORMS, &count);

	//goes through each uniform and creates a ShaderUniformData object to store and allow
	//modification of uniforms
	//for (int i = 0; i < count; i++) {
	//	//gets uniform name and type
	//	glGetActiveUniform(m_Program, (GLuint)i, bufSize, &length, &size, &type, name);
	//	//printf("Uniform: %s\n", name);
	//}

	DeleteShaders();

	m_IsLinked = true;
	glUseProgram(0);
}

void ShaderGL::Use() {
	if (m_Program != 0 && m_IsLinked) {
		_CCurrentShader = this;
		glUseProgram(m_Program);
	}
}

void ShaderGL::Reload() {
	DeleteShaders();
	if (m_Program != 0) {
		glDeleteProgram(m_Program);
		m_Program = 0;
	}
	LinkShaders();

	for (uint i = 0; i < m_AttachedUniforms.Length(); i++) {
		if (m_AttachedUniforms[i].m_Block != nullptr) {
			AddBuffer_Internal(m_AttachedUniforms[i].m_Block, m_AttachedUniforms[i].m_Name);
		}
	}
}

void ShaderGL::AddBuffer_Internal(ShaderUniformBlock* a_Block, CMString a_StructName) {
	unsigned int uniformBlock = glGetUniformBlockIndex(m_Program, a_StructName.Get());
	if (uniformBlock != -1) {
		glUniformBlockBinding(m_Program, uniformBlock, ((ShaderUniformBlockGL*)a_Block)->m_SlotID);
	}
}

void ShaderGL::SetDebugObjName_Internal() {
	if (m_Program) {
		glObjectLabel(GL_PROGRAM, m_Program, -1, m_DebugName.Get());
	}
}

void ShaderGL::BindTexture(std::string a_Name, unsigned int a_Index) {
	const GLuint uniformID = glGetUniformLocation(m_Program, a_Name.c_str());
	glUniform1i(uniformID, a_Index); // set it manually
}

ShaderUniformBlockGL::~ShaderUniformBlockGL() {
	if (m_ID != 0) {
		glDeleteBuffers(1, &m_ID);
		m_ID = 0;
	}
}

void ShaderUniformBlockGL::UpdateBuffer(void* a_Object) {
	glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
	glBufferData(GL_UNIFORM_BUFFER, m_Size, a_Object, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShaderUniformBlockGL::SetDebugObjName_Internal() {
	if (m_ID) {
		glObjectLabel(GL_BUFFER, m_ID, -1, m_DebugName.Get());
	}
}

void ShaderGL::DeleteShaders() {
	for (int i = 0; i < ShaderType::SHADERCOUNT; i++) {
		if (m_GLShaderIndex[i] != 0) {
			glDetachShader(m_Program, m_GLShaderIndex[i]);
			glDeleteShader(m_GLShaderIndex[i]);
			m_GLShaderIndex[i] = 0;
		}
	}
}
