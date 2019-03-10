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

#define ShaderCachePath "ShaderCache\\"

const TBuiltInResource DefaultTBuiltInResource = {
	/* .MaxLights = */ 32,
	/* .MaxClipPlanes = */ 6,
	/* .MaxTextureUnits = */ 32,
	/* .MaxTextureCoords = */ 32,
	/* .MaxVertexAttribs = */ 64,
	/* .MaxVertexUniformComponents = */ 4096,
	/* .MaxVaryingFloats = */ 64,
	/* .MaxVertexTextureImageUnits = */ 32,
	/* .MaxCombinedTextureImageUnits = */ 80,
	/* .MaxTextureImageUnits = */ 32,
	/* .MaxFragmentUniformComponents = */ 4096,
	/* .MaxDrawBuffers = */ 32,
	/* .MaxVertexUniformVectors = */ 128,
	/* .MaxVaryingVectors = */ 8,
	/* .MaxFragmentUniformVectors = */ 16,
	/* .MaxVertexOutputVectors = */ 16,
	/* .MaxFragmentInputVectors = */ 15,
	/* .MinProgramTexelOffset = */ -8,
	/* .MaxProgramTexelOffset = */ 7,
	/* .MaxClipDistances = */ 8,
	/* .MaxComputeWorkGroupCountX = */ 65535,
	/* .MaxComputeWorkGroupCountY = */ 65535,
	/* .MaxComputeWorkGroupCountZ = */ 65535,
	/* .MaxComputeWorkGroupSizeX = */ 1024,
	/* .MaxComputeWorkGroupSizeY = */ 1024,
	/* .MaxComputeWorkGroupSizeZ = */ 64,
	/* .MaxComputeUniformComponents = */ 1024,
	/* .MaxComputeTextureImageUnits = */ 16,
	/* .MaxComputeImageUniforms = */ 8,
	/* .MaxComputeAtomicCounters = */ 8,
	/* .MaxComputeAtomicCounterBuffers = */ 1,
	/* .MaxVaryingComponents = */ 60,
	/* .MaxVertexOutputComponents = */ 64,
	/* .MaxGeometryInputComponents = */ 64,
	/* .MaxGeometryOutputComponents = */ 128,
	/* .MaxFragmentInputComponents = */ 128,
	/* .MaxImageUnits = */ 8,
	/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
	/* .MaxCombinedShaderOutputResources = */ 8,
	/* .MaxImageSamples = */ 0,
	/* .MaxVertexImageUniforms = */ 0,
	/* .MaxTessControlImageUniforms = */ 0,
	/* .MaxTessEvaluationImageUniforms = */ 0,
	/* .MaxGeometryImageUniforms = */ 0,
	/* .MaxFragmentImageUniforms = */ 8,
	/* .MaxCombinedImageUniforms = */ 8,
	/* .MaxGeometryTextureImageUnits = */ 16,
	/* .MaxGeometryOutputVertices = */ 256,
	/* .MaxGeometryTotalOutputComponents = */ 1024,
	/* .MaxGeometryUniformComponents = */ 1024,
	/* .MaxGeometryVaryingComponents = */ 64,
	/* .MaxTessControlInputComponents = */ 128,
	/* .MaxTessControlOutputComponents = */ 128,
	/* .MaxTessControlTextureImageUnits = */ 16,
	/* .MaxTessControlUniformComponents = */ 1024,
	/* .MaxTessControlTotalOutputComponents = */ 4096,
	/* .MaxTessEvaluationInputComponents = */ 128,
	/* .MaxTessEvaluationOutputComponents = */ 128,
	/* .MaxTessEvaluationTextureImageUnits = */ 16,
	/* .MaxTessEvaluationUniformComponents = */ 1024,
	/* .MaxTessPatchComponents = */ 120,
	/* .MaxPatchVertices = */ 32,
	/* .MaxTessGenLevel = */ 64,
	/* .MaxViewports = */ 16,
	/* .MaxVertexAtomicCounters = */ 0,
	/* .MaxTessControlAtomicCounters = */ 0,
	/* .MaxTessEvaluationAtomicCounters = */ 0,
	/* .MaxGeometryAtomicCounters = */ 0,
	/* .MaxFragmentAtomicCounters = */ 8,
	/* .MaxCombinedAtomicCounters = */ 8,
	/* .MaxAtomicCounterBindings = */ 1,
	/* .MaxVertexAtomicCounterBuffers = */ 0,
	/* .MaxTessControlAtomicCounterBuffers = */ 0,
	/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
	/* .MaxGeometryAtomicCounterBuffers = */ 0,
	/* .MaxFragmentAtomicCounterBuffers = */ 1,
	/* .MaxCombinedAtomicCounterBuffers = */ 1,
	/* .MaxAtomicCounterBufferSize = */ 16384,
	/* .MaxTransformFeedbackBuffers = */ 4,
	/* .MaxTransformFeedbackInterleavedComponents = */ 64,
	/* .MaxCullDistances = */ 8,
	/* .MaxCombinedClipAndCullDistances = */ 8,
	/* .MaxSamples = */ 4,
	/* .maxMeshOutputVerticesNV = */ 256,
	/* .maxMeshOutputPrimitivesNV = */ 512,
	/* .maxMeshWorkGroupSizeX_NV = */ 32,
	/* .maxMeshWorkGroupSizeY_NV = */ 1,
	/* .maxMeshWorkGroupSizeZ_NV = */ 1,
	/* .maxTaskWorkGroupSizeX_NV = */ 32,
	/* .maxTaskWorkGroupSizeY_NV = */ 1,
	/* .maxTaskWorkGroupSizeZ_NV = */ 1,
	/* .maxMeshViewCountNV = */ 4,

	/* .limits = */{
	/* .nonInductiveForLoops = */ 1,
	/* .whileLoops = */ 1,
	/* .doWhileLoops = */ 1,
	/* .generalUniformIndexing = */ 1,
	/* .generalAttributeMatrixVectorIndexing = */ 1,
	/* .generalVaryingIndexing = */ 1,
	/* .generalSamplerIndexing = */ 1,
	/* .generalVariableIndexing = */ 1,
	/* .generalConstantMatrixVectorIndexing = */ 1,
} };

unsigned int Shader::ShaderTypeToEShLanguage(ShaderTypes a_Type) {
	switch (a_Type) {
		default:
		case ShaderTypes::SHADER_VERTEX:
			return EShLangVertex;
		case ShaderTypes::SHADER_FRAGMENT:
			return EShLangFragment;
	}
}

ShaderTypes Shader::EShLanguageToShaderType(unsigned int a_Type) {
	switch (a_Type) {
		default:
		case EShLangVertex:
			return ShaderTypes::SHADER_VERTEX;
		case EShLangFragment:
			return ShaderTypes::SHADER_FRAGMENT;
	}
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

std::string Shader::loadTextFromPath(std::string a_Path) {
	std::ifstream textFile(a_Path);
	if (textFile.bad()) {
		return std::string();
	}
	std::stringstream fileBuffer;
	fileBuffer << textFile.rdbuf();//get text from file into buffer
	return fileBuffer.str();
}

void Shader::AddShader(ShaderTypes a_Type, std::string a_Path) {
	ShaderInfo* info = &m_Shaders[a_Type];

	if (info->m_HasBeenGenerated) {
		std::cout << "Shader Already has a shader of type: " << (int)a_Type << std::endl;
		return;
	}

	info->m_IsUsed = true;
	info->m_Path = a_Path;

	std::string shaderFile = loadTextFromPath(a_Path);

	HCRYPTPROV hProv;
	HCRYPTHASH hHash;
	CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);
	CryptHashData(hHash, (BYTE*)shaderFile.c_str(), shaderFile.length(), 0);
	DWORD cbHash = 16;
	BYTE rgbHash[16];
	CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0);
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	memcpy(info->m_Hash, rgbHash, 16);


	std::string infoFile = loadTextFromPath(ShaderCachePath + a_Path + ".info");
	if (infoFile.size() != 0 && !m_ShoudRegenerateCode) {
		BYTE fileHash[16];
		//for (int i = 0; i < 16; i++) {
		//	fileHash[i] = (BYTE)infoFile[i];
		//}
		memcpy(fileHash, &infoFile[0], 16);
		int res = memcmp(rgbHash, fileHash, 16);
		if (res == 0) {
			printf("Shader: Using cached data: %s\n", a_Path.c_str());
			info->m_HasBeenChanged = false;
			return;
		} else {
			//delete old files
			remove((ShaderCachePath + a_Path + ".info").c_str());
			remove((ShaderCachePath + a_Path + ".spirv").c_str());
			printf("Shader: Old code didnt match, deleteing cached data %s\n", a_Path.c_str());
		}
	}
	printf("Shader: Generating Code: %s\n", a_Path.c_str());
	info->m_HasBeenChanged = true;



	const char* s[1] = { shaderFile.c_str() };
	EShLanguage language = (EShLanguage)ShaderTypeToEShLanguage(a_Type);

	TBuiltInResource Resources = DefaultTBuiltInResource;
	glslang::InitializeProcess();
	glslang::TShader* newShader = new glslang::TShader(language);
	newShader->setStrings(s, 1);
	newShader->setEnvInput(glslang::EShSourceGlsl, language, glslang::EShClientOpenGL, 450);
	newShader->setEnvClient(glslang::EShClientOpenGL, glslang::EShTargetOpenGL_450);
	newShader->setEnvTarget(glslang::EshTargetSpv, glslang::EShTargetSpv_1_0);
	//newShader->getIntermediate()->setUseStorageBuffer();
	newShader->setAutoMapLocations(true);
	newShader->setAutoMapBindings(true);
	//newShader->setHlslIoMapping(true);
	//newShader->setEnvTargetHlslFunctionality1();
	newShader->parse(&Resources, 450, false, EShMessages::EShMsgDefault);
	newShader->getIntermediate()->setSourceFile(a_Path.c_str());//debug

	//check if it failed:
	if (newShader->getInfoLog() && newShader->getInfoLog()[0]) {
		std::cout << "Shader Error: " << newShader->getInfoLog() << std::endl;
		info->m_IsUsed = false;
		//delete newShader;
		return;
	}

	info->m_Shader = newShader;

	//m_Shaders[(int)a_Type] = info;

	glslang::FinalizeProcess();
}

void Shader::LinkShaders() {
	bool shouldLink = false;
	glslang::TProgram program;
	for (int i = 0; i < ShaderTypes::SHADERCOUNT; i++) {
		if (m_Shaders[i].m_IsUsed) {
			if (m_Shaders[i].m_HasBeenChanged) {
				if (m_Shaders[i].m_Shader != 0) {
					program.addShader(m_Shaders[i].m_Shader);
					shouldLink = true;
				}

				//Update the info file with the new hash
				CreateDirectory(ShaderCachePath, NULL);
				std::ofstream infoFile(ShaderCachePath + m_Shaders[i].m_Path + ".info");
				if (infoFile.is_open()) {
					for (int q = 0; q < 16; q++) {
						infoFile << m_Shaders[i].m_Hash[q];
					}
					infoFile << "\n";
					infoFile << m_Shaders[i].m_Path;
					infoFile.close();
				}
			} else {
				//file hasent changed since last time, so just load it's spirv code
				AddShader_Internal((ShaderTypes)i, loadSpirvFromPath(ShaderCachePath + m_Shaders[i].m_Path + ".spirv"));
				//continue;
			}

		}
	}

	if (shouldLink) {
		program.link(EShMessages::EShMsgDefault);

		program.mapIO();

		program.buildReflection();
		//program.dumpReflection();

		if (program.getInfoLog() && program.getInfoLog()[0]) {
			std::cout << "linking: " << program.getInfoLog() << std::endl;
		}


		for (int stage = 0; stage < EShLangCount; ++stage) {
			if (program.getIntermediate((EShLanguage)stage)) {
				glslang::TIntermediate* shaderStage = program.getIntermediate((EShLanguage)stage);
				TIntermNode* root = shaderStage->getTreeRoot();
				std::vector<unsigned int> spirv;
				spv::SpvBuildLogger logger;
				glslang::SpvOptions spvOptions;
				spvOptions.generateDebugInfo = false;
				spvOptions.disableOptimizer = true;
				spvOptions.validate = true;
				spvOptions.optimizeSize = true;
				glslang::GlslangToSpv(*shaderStage, spirv, &logger, &spvOptions);


				//OUTPUT TO BINARY:
				//if (m_ShouldOutputToBinary) {
				//}

				bool didFail = false;
				if (logger.getAllMessages().size() != 0) {
					std::cout << "Logger Messages:" << logger.getAllMessages() << std::endl;
					didFail = true;
				}

				if (!didFail) {
					//spv::Disassemble(std::cout, spirv);
					glslang::OutputSpvBin(spirv, (ShaderCachePath + std::string(shaderStage->getSourceFile()) + ".spirv").c_str());

					AddShader_Internal(EShLanguageToShaderType((EShLanguage)stage), spirv);
				} else {
					std::ofstream infoFile(ShaderCachePath + std::string(shaderStage->getSourceFile()) + ".info");
					if (infoFile.is_open()) {
						infoFile << "Failed.\n";
						infoFile.close();
					}
				}
			}
		}
	}

	Link_Internal();
	SetDebugObjName_Internal();
}
