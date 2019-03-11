#include "ShaderSpirvData.h"

#include <glslang-master/glslang/Public/ShaderLang.h>
#include <glslang-master/glslang/MachineIndependent/localintermediate.h>
#include <glslang-master/SPIRV/disassemble.h>
#include <glslang-master/SPIRV/GlslangToSpv.h>

#include <Windows.h>
#include <wincrypt.h>

#include "Debug.h"
#include "Util.h"

#include <fstream> //std::ifstream
#include <sstream> //std::stringstream
#include <string> //std::string

#include "API/Shader.h"

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


ShaderSpirvData::ShaderSpirvData() {
}

void ShaderSpirvData::LoadFromFile(CMString a_FilePath) {
	if (m_Shader) {
		CMLog("Shader already exists");
		return;
	}
	m_FilePath = a_FilePath;
	GetTypeFromFilePath();

	CMString shaderFile = Util::LoadTextFromPath(m_FilePath.m_FilePath);

	if (shaderFile.Length() <= 5) {
		SaveInfoFile(true);
		return;
	}
	
	HCRYPTPROV hProv;
	HCRYPTHASH hHash;
	CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);
	CryptHashData(hHash, (BYTE*)shaderFile.c_str(), shaderFile.length(), 0);
	DWORD cbHash = HASH_LENGTH;
	uchar rgbHash[HASH_LENGTH];
	CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0);
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	memcpy(m_Hash, rgbHash, HASH_LENGTH);
	


	std::string infoFile = Util::LoadTextFromPath(ShaderCachePath + m_FilePath.m_FilePath + ".info");
	if (infoFile.size() != 0) {

		int res = memcmp(m_Hash, &infoFile[0], HASH_LENGTH);

		if (res == 0) {
			printf("Shader: Using cached data: %s\n", m_FilePath.m_FilePath.c_str());
			m_HasBeenLoaded = true;
			return;
		} else {
			//delete old files
			remove((ShaderCachePath + m_FilePath.m_FilePath + ".info").c_str());
			remove((ShaderCachePath + m_FilePath.m_FilePath + ".spirv").c_str());
			printf("Shader: Old code didnt match, deleteing cached data %s\n", m_FilePath.m_FilePath.c_str());
		}
	}
	printf("Shader: Generating Code: %s\n", m_FilePath.m_FilePath.c_str());

	//Generate GLSlang from file
	if (!GenerateGLSlangData(shaderFile)) {
		CMASSERT_MSG(true, "Failed To Generate GLSLang");
		delete m_Shader;
		return;
	}

	//Generate SPIRV From GLSlang
	if (!GenerateSpirvData()) {
		CMASSERT_MSG(true, "Failed To Generate Spirv Data from glslang");
		delete m_Program;
		delete m_Shader;
		return;
	}

	m_HasBeenLoaded = true;

	delete m_Program;
	delete m_Shader;
	m_Program = nullptr;
	m_Shader = nullptr;
}

void ShaderSpirvData::Reload() {
	m_HasBeenLoaded = false;

	LoadFromFile(m_FilePath.m_FilePath);

	for (int i = 0; i < m_AttachedShaders.Length(); i++) {
		m_AttachedShaders[i]->Reload();

	}
}

void ShaderSpirvData::AddShader(Shader * a_Shader) {
	m_AttachedShaders.push_back(a_Shader);
}

void ShaderSpirvData::GetTypeFromFilePath() {
	const CMArray<uchar*> hashs = { CMString(".vert").HashAlloc(), CMString(".frag").HashAlloc() };

	uchar* fileNameHash = m_FilePath.m_FileName.SubStrFindFromEnd('.').ToLower().HashAlloc();

	m_ShaderType = ShaderType::SHADERCOUNT;

	for (int i = 0; i < hashs.Length(); i++) {
		if (memcmp(hashs[i], fileNameHash, HASH_LENGTH) == 0) {
			m_ShaderType = (ShaderType)i;
			break;
		}
	}

	CMASSERT_MSG(m_ShaderType == ShaderType::SHADERCOUNT, "Could not find Type of shader!");

	for (int i = 0; i < hashs.Length(); i++) {
		delete[] hashs[i];
	}
	delete fileNameHash;
}

unsigned int ShaderSpirvData::ShaderTypeToEShLanguage() {
	switch (m_ShaderType) {
		default:
		case ShaderType::SHADER_VERTEX:
			return EShLangVertex;
		case ShaderType::SHADER_FRAGMENT:
			return EShLangFragment;
	}
}

bool ShaderSpirvData::GenerateGLSlangData(CMString a_Code) {
	const char* s[1] = { a_Code.Get() };
	EShLanguage language = (EShLanguage)ShaderTypeToEShLanguage();

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
	//newShader->getIntermediate()->setSourceFile(m_FilePath.m_FilePath.c_str());//debug

	//check if it failed:
	if (newShader->getInfoLog() && newShader->getInfoLog()[0]) {
		std::cout << "Shader Error: " << newShader->getInfoLog() << std::endl;
		//delete newShader;
		return false;
	}

	m_Shader = newShader;

	glslang::FinalizeProcess();

	return true;
}

bool ShaderSpirvData::GenerateSpirvData() {
	bool didFail = false;
	m_Program = new glslang::TProgram();

	m_Program->addShader(m_Shader);

	m_Program->link(EShMessages::EShMsgDefault);

	m_Program->mapIO();

	m_Program->buildReflection();
	//program.dumpReflection();

	if (m_Program->getInfoLog() && m_Program->getInfoLog()[0]) {
		std::cout << "linking: " << m_Program->getInfoLog() << std::endl;
		didFail = true;
	}

	glslang::TIntermediate* shaderStage = m_Program->getIntermediate((EShLanguage)ShaderTypeToEShLanguage());
	std::vector<unsigned int> spirv;
	spv::SpvBuildLogger logger;
	glslang::SpvOptions spvOptions;
	spvOptions.generateDebugInfo = false;
	spvOptions.disableOptimizer = true;
	spvOptions.validate = true;
	spvOptions.optimizeSize = true;
	glslang::GlslangToSpv(*shaderStage, spirv, &logger, &spvOptions);
	
	if (logger.getAllMessages().size() != 0) {
		std::cout << "Logger Messages:" << logger.getAllMessages() << std::endl;
		didFail = true;
	}
	
	if (!didFail) {
		//spv::Disassemble(std::cout, spirv);
		CMString folderPath = (CMString(ShaderCachePath) + m_FilePath.m_FileLocation);
		CreateDirectory(folderPath.Get(), NULL);
		std::ofstream infoFile(folderPath + m_FilePath.m_FileName + ".info");

		glslang::OutputSpvBin(spirv, (folderPath + m_FilePath.m_FileName + ".spirv").c_str());
	
	}

	SaveInfoFile(didFail);

	return !didFail;
}

void ShaderSpirvData::SaveInfoFile(bool a_DidFail) {
	//Update the info file with the new hash
//CreateDirectory(ShaderCachePath, NULL);
	CMString folderPath = (CMString(ShaderCachePath) + m_FilePath.m_FileLocation);
	CreateDirectory(folderPath.Get(), NULL);
	std::ofstream infoFile(folderPath + m_FilePath.m_FileName + ".info");
	if (infoFile.is_open()) {
		if (a_DidFail) {
			infoFile << "Failed.\n";
		} else {
			for (int q = 0; q < HASH_LENGTH; q++) {
				infoFile << m_Hash[q];
			}
			infoFile << "\n";
		}
		infoFile << m_FilePath.m_FilePath;
		infoFile.close();
	}
}
