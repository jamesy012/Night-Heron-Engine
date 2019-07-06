#include "ShaderSpirvData.h"

#include <glslang-master/glslang/Public/ShaderLang.h>
#include <glslang-master/glslang/MachineIndependent/localintermediate.h>
#include <glslang-master/SPIRV/disassemble.h>
#include <glslang-master/SPIRV/GlslangToSpv.h>

#include "Debug.h"
#include "Util.h"

#include <fstream> //std::ifstream
#include <sstream> //std::stringstream
#include <string> //std::string

#include "API/Shader.h"
#include "Util.h"

#include "Singletons.h"
#include "API/GFXAPI.h"

#include <windows.h>

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

class OurIncluder : public glslang::TShader::Includer
{
public:
	OurIncluder(ShaderSpirvData* a_Owner) : m_Owner(a_Owner)
	{

	}
	// For the "system" or <>-style includes; search the "system" paths.
	virtual IncludeResult* includeSystem(const char* headerName,
										 const char* includerName,
										 size_t inclusionDepth) override
	{
		printf("Including System file header name (%s) IncluderName (%s) inclusionDepth (%i)\n", headerName, includerName, inclusionDepth);
		

		return LoadFile(CMString("Programs/") + headerName);
	}

	// For the "local"-only aspect of a "" include. Should not search in the
	// "system" paths, because on returning a failure, the parser will
	// call includeSystem() to look in the "system" locations.
	virtual IncludeResult* includeLocal(const char* headerName,
										const char* includerName,
										size_t inclusionDepth) override
	{
		printf("Including local file header name (%s) IncluderName (%s) inclusionDepth (%i)\n", headerName, includerName, inclusionDepth);
		CMString includer = includerName;
		CMString path;
		if (!includer.IsEmpty()) {
			int folderPos = includer.FindFromEnd('/');
			path = includer.SubStr(0, folderPos);
			path += "/";
		}
		path += headerName;

		return LoadFile(path);
	}

	// Signals that the parser will no longer use the contents of the
	// specified IncludeResult.
	virtual void releaseInclude(IncludeResult* result) override {
		if (result != nullptr)
		{
			delete[] static_cast<char*>(result->userData);
			delete result;
		}
	};
	virtual ~OurIncluder() override {}

private:
	IncludeResult* LoadFile(CMString a_Path)
	{
		CMString file = Util::LoadTextFromPath(a_Path);

		if (!file.IsEmpty())
		{
			char* content = new char[file.Length() + 1];
			strcpy_s(content, file.Length() + 1, file.Get());
			m_Owner->m_IncludeList.AddUnique(a_Path);
			return new glslang::TShader::Includer::IncludeResult(a_Path, content, file.Length(), content);
		}
		return nullptr;
	}

	ShaderSpirvData* m_Owner;
};

ShaderSpirvData::ShaderSpirvData() {
}

ShaderLoadRes ShaderSpirvData::LoadFromFile(CMString a_FilePath) {
	if (m_Shader) {
		CMLog("Shader already exists");
		return ShaderLoadRes::SHADERLOAD_ERROR;
	}
	m_FilePath = a_FilePath;
	GetTypeFromFilePath();

	CMString shaderFile = Util::LoadTextFromPath(m_FilePath.m_FilePath);

	if (shaderFile.Length() <= 5) {
		SaveInfoFile(true);
		return ShaderLoadRes::SHADERLOAD_ERROR;
	}


	std::string infoFile = Util::LoadTextFromPath(ShaderCachePath + m_FilePath.m_FilePath + ".info");
	if (infoFile.size() != 0) {
		shaderFile.Hash(m_Hash);

		//todo: also check included files hash's here.

		int res = memcmp(m_Hash, &infoFile[0], HASH_LENGTH);

		if (res == 0) {
			printf("Shader: Using cached data: %s\n", m_FilePath.m_FilePath.c_str());
			m_HasBeenLoaded = true;
			return ShaderLoadRes::SHADERLOAD_LOAD;
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
		return ShaderLoadRes::SHADERLOAD_ERROR;
	}

	//Generate SPIRV From GLSlang
	if (!GenerateSpirvData()) {
		CMASSERT_MSG(true, "Failed To Generate Spirv Data from glslang");
		delete m_Program;
		delete m_Shader;
		return ShaderLoadRes::SHADERLOAD_ERROR;
	}

	m_HasBeenLoaded = true;

	delete m_Program;
	delete m_Shader;
	m_Program = nullptr;
	m_Shader = nullptr;

	return ShaderLoadRes::SHADERLOAD_COMPILED;
}

ShaderLoadRes ShaderSpirvData::Reload() {
	m_HasBeenLoaded = false;

	ShaderLoadRes res = LoadFromFile(m_FilePath.m_FilePath);

	if (res != ShaderLoadRes::SHADERLOAD_ERROR) {
		for (uint i = 0; i < m_AttachedShaders.Length(); i++) {
			m_AttachedShaders[i]->Reload();
		}
	}

	_CGraphics->ResetShader();

	return res;
}

void ShaderSpirvData::AddShader(Shader * a_Shader) {
	m_AttachedShaders.Add(a_Shader);
}

void ShaderSpirvData::RemoveShader(Shader * a_Shader) {
	m_AttachedShaders.Remove(a_Shader);
}

void ShaderSpirvData::GetTypeFromFilePath() {
	const CMArray<CMStringHash> hashs = { ".vert", ".frag" };

	uchar* fileNameHash = m_FilePath.m_FileName.SubStrFindFromEnd('.').ToLower().HashAlloc();

	m_ShaderType = ShaderType::SHADERCOUNT;

	for (uint i = 0; i < hashs.Length(); i++) {
		if (memcmp(hashs[i].m_ExtenstionHash, fileNameHash, HASH_LENGTH) == 0) {
			m_ShaderType = (ShaderType)i;
			break;
		}
	}

	CMASSERT_MSG(m_ShaderType == ShaderType::SHADERCOUNT, "Could not find Type of shader!");

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
	//Find/set file version
	CMString versionText = "#version 450\n";
	if (a_Code.ToLower().Contains("#version") && !a_Code.SubStr(0,3).Contains("//")) {
		int firstNewLine = a_Code.FindFromStart('\n');
		const uint VersionTextSize = sizeof("#version");
		versionText = a_Code.SubStr(VersionTextSize, firstNewLine - VersionTextSize);
		versionText = "#version " + versionText + "\n";
		a_Code = a_Code.SubStr(firstNewLine + 1, -1);
	}

	//and add this extension to allow glslang to use #include

	const char* s[3] = { versionText.Get(), "#extension GL_GOOGLE_include_directive : enable\n", a_Code.Get() };


	EShLanguage language = (EShLanguage)ShaderTypeToEShLanguage();

	TBuiltInResource Resources = DefaultTBuiltInResource;
	glslang::InitializeProcess();
	glslang::TShader* newShader = new glslang::TShader(language);

	OurIncluder includer(this);

	newShader->setStrings(s, 3);
	newShader->setEnvInput(glslang::EShSourceGlsl, language, glslang::EShClientOpenGL, 450);
	newShader->setEnvClient(glslang::EShClientOpenGL, glslang::EShTargetOpenGL_450);
	newShader->setEnvTarget(glslang::EshTargetSpv, glslang::EShTargetSpv_1_0);
	//newShader->getIntermediate()->setUseStorageBuffer();
	newShader->setAutoMapLocations(true);
	newShader->setAutoMapBindings(true);
	//newShader->setHlslIoMapping(true);
	//newShader->setEnvTargetHlslFunctionality1();

	//Contains final string of shader code
	//std::string output;
	//newShader->preprocess(&Resources, 450, ENoProfile, false, false, EShMessages::EShMsgDefault, &output, includer);

	newShader->parse(&Resources, 450, false, EShMessages::EShMsgDefault, includer);
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

		glslang::OutputSpvBin(spirv, (folderPath + m_FilePath.m_FileName + ".spirv").c_str());
	}

	SaveInfoFile(didFail);

	return !didFail;
}

void ShaderSpirvData::SaveInfoFile(bool a_DidFail) {
	//Update the info file with the new hash
//CreateDirectory(ShaderCachePath, NULL);

	//todo: Change shader info files over to json

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
		infoFile << m_FilePath.m_FilePath + "\n";

		//todo: maybe also include the include's hash here?
		for (uint i = 0; i < m_IncludeList.Length(); i++) {
			infoFile << m_IncludeList[i] + "\n";
		}

		infoFile.close();
	}
}
