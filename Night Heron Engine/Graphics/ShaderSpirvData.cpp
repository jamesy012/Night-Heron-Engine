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
#include <vector>

#include "API/Shader.h"
#include "Util.h"

#include "Managers/ShaderSpirvManager.h"
#include "API/GFXAPI.h"

#include <windows.h>

#include "nlohmann/json.hpp"
//json object for this class.
static nlohmann::json SSD_JsonHolder;
CMArray<SHADERDEFINES> ShaderDefines;


#define ShaderCachePath "ShaderCache\\"
#define ShaderJsonFileVersion 1

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
		CMLOG("Including System file header name (%s) IncluderName (%s) inclusionDepth (%i)\n", headerName, includerName, inclusionDepth);


		return LoadFile(CMString("Programs/") + headerName);
	}

	// For the "local"-only aspect of a "" include. Should not search in the
	// "system" paths, because on returning a failure, the parser will
	// call includeSystem() to look in the "system" locations.
	virtual IncludeResult* includeLocal(const char* headerName,
										const char* includerName,
										size_t inclusionDepth) override
	{
		CMLOG("Including local file header name (%s) IncluderName (%s) inclusionDepth (%i)\n", headerName, includerName, inclusionDepth);
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
		ShaderSpirvData* includeFile = _CShaderSpirvManager->GetShaderPart(a_Path);
		if (includeFile) {

			if (m_Owner->m_IncludeList.FindElement(includeFile) != -1) {
				CMLOG("\tAuto #pragma once kicked in, not including\n");
				const uint length = 0;
				char* content = new char[length + 1];
				return new glslang::TShader::Includer::IncludeResult(a_Path, content, length, content);
			}

			CMString includeString = includeFile->m_SourceFile;

			//process include defines
			{
				CMString defines;
				CMArray<SHADERDEFINES*> shaderDefineRef;
				//search for them
				for (int i = 0; i < ShaderDefines.Length(); i++) {
					if (includeString.Contains(ShaderDefines[i].m_Name)) {
						m_Owner->AddDefine(&ShaderDefines[i]);
						shaderDefineRef.Add(&ShaderDefines[i]);


					}
				}
				//set up string
				for (int i = 0; i < shaderDefineRef.Length(); i++) {
					defines += "#define " + shaderDefineRef[i]->m_Name + " " + CMString::IntToString(shaderDefineRef[i]->m_Value) + "\n";
				}
				//add to file
				includeString = defines + includeString;
			}

			const uint length = includeFile->m_SourceFile.Length();
			char* content = new char[length + 1];
			strcpy_s(content, length + 1, includeFile->m_SourceFile.Get());

			//tell the current file that we have included something
			m_Owner->m_IncludeList.AddUnique(includeFile);

			return new glslang::TShader::Includer::IncludeResult(a_Path, content, length, content);
		}
		return nullptr;
	}

	ShaderSpirvData* m_Owner;
};

ShaderSpirvData::ShaderSpirvData() {
}

ShaderLoadRes ShaderSpirvData::LoadFromFile(CMString a_FilePath) {
	if (m_Shader) {
		CMLOG("Shader already exists");
		return ShaderLoadRes::SHADERLOAD_ERROR;
	}
	m_FilePath = a_FilePath;
	GetTypeFromFilePath();

	m_SourceFile = Util::LoadTextFromPath(m_FilePath.m_FilePath);

	if (m_SourceFile.Length() <= 5) {
		SaveInfoFile(true);
		return ShaderLoadRes::SHADERLOAD_ERROR;
	}

	m_SourceFile.Hash(m_Hash);

	std::string infoFile = Util::LoadTextFromPath(ShaderCachePath + m_FilePath.m_FilePath + ".info");
	if (infoFile.size() != 0) {
		int res = -1;
		if (infoFile[0] == '{') {
			SSD_JsonHolder = nlohmann::json::parse(infoFile);

			const bool correctVerion = SSD_JsonHolder.contains("SpirvJsonVersion") ? SSD_JsonHolder.at("SpirvJsonVersion").get<int>() == ShaderJsonFileVersion : false;

			const bool errored = SSD_JsonHolder.contains("Error");
			//LastCompiedAPI
			m_HasPermutation = SSD_JsonHolder.contains("Permutation") ? SSD_JsonHolder.at("Permutation").get<bool>() : false;
			const bool lastCompiedAPIMatch = (SSD_JsonHolder.contains("LastCompiedAPI") ? SSD_JsonHolder.at("LastCompiedAPI").get<int>() == (int)_CGraphics->GetGraphicsType() : false);

			bool failedDefines = false;
			//Check for different defines
			if (m_HasPermutation && SSD_JsonHolder.contains("Defines")) {
				nlohmann::json objects = SSD_JsonHolder.at("Defines");
				//search through each define that was included and see if it's different
				for (nlohmann::json::iterator it = objects.begin(); it != objects.end(); ++it) {
					auto inc = *it;
					CMString name = inc["Name"].get<CMString>();
					//find it
					for (int i = 0; i < ShaderDefines.Length(); i++) {
						if (name == ShaderDefines[i].m_Name) {
							int value = inc["Value"].get<int>();
							if (ShaderDefines[i].m_Value != value) {
								//value didnt match, error
								failedDefines = true;
							}
							break;
						}
					}
				}
			}


			if (correctVerion && !errored && !(m_HasPermutation && !lastCompiedAPIMatch) && !failedDefines) {

				char output[HASH_LENGTH];
				int index = 0;

				//check file hash
				nlohmann::json j = SSD_JsonHolder.at("Hash");
				for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
					output[index++] = (*it).get<char>();
				}
				index = 0;

				res = memcmp(m_Hash, output, HASH_LENGTH);

				//check include hash's
				nlohmann::json objects = SSD_JsonHolder.at("Includes");
				for (nlohmann::json::iterator it = objects.begin(); it != objects.end(); ++it) {
					auto inc = *it;
					nlohmann::json j = inc.at("Hash");

					ShaderSpirvData* includeFile = _CShaderSpirvManager->GetShaderPart(inc["FilePath"].get<CMString>());
					if (includeFile) {

						for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
							output[index++] = (*it).get<char>();
						}
						index = 0;

						res |= memcmp(includeFile->m_Hash, output, HASH_LENGTH);
					}
					else {
						res = -1;
						break;
					}
				}

			}
		}


		if (res == 0) {
			CMLOG("Shader: Using cached data: %s\n", m_FilePath.m_FilePath.c_str());
			m_HasBeenLoaded = true;
			return ShaderLoadRes::SHADERLOAD_LOAD;
		} else {
			//delete old files
			remove((ShaderCachePath + m_FilePath.m_FilePath + ".info").c_str());
			remove((ShaderCachePath + m_FilePath.m_FilePath + ".spirv").c_str());
			CMLOG("Shader: Old code didnt match, deleteing cached data %s\n", m_FilePath.m_FilePath.c_str());
		}
	}
	CMLOG("Shader: Generating Code: %s\n", m_FilePath.m_FilePath.c_str());

	m_HasPermutation = m_SourceFile.Contains("#if WITH_OPENGL");
	m_HasPermutation |= m_SourceFile.Contains("#if WITH_DIRECTX");

	//find any defines we need
	for (int i = 0; i < ShaderDefines.Length(); i++) {
		if (m_SourceFile.Contains(ShaderDefines[i].m_Name)) {
			AddDefine(&ShaderDefines[i]);
		}
	}

	if (m_ShaderType == ShaderType::SHADER_INCLUDE) {
		SaveInfoFile(false);
	}else{
		//Generate GLSlang from file
		if (!GenerateGLSlangData()) {
			SaveInfoFile(true);
			CMASSERT_MSG(true, "Failed To Generate GLSLang");
			delete m_Shader;
			return ShaderLoadRes::SHADERLOAD_ERROR;
		}

		for (uint i = 0; i < m_IncludeList.Length(); i++) {
			m_HasPermutation |= m_IncludeList[i]->m_HasPermutation;
		}

		//Generate SPIRV From GLSlang
		if (!GenerateSpirvData()) {
			CMASSERT_MSG(true, "Failed To Generate Spirv Data from glslang");
			delete m_Program;
			delete m_Shader;
			return ShaderLoadRes::SHADERLOAD_ERROR;
		}

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

void ShaderSpirvData::AddDefine(SHADERDEFINES* a_Define) {
	m_ShaderDefines.AddUnique(a_Define);
	m_HasPermutation = true;
}


void ShaderSpirvData::GetTypeFromFilePath() {
	const CMArray<CMStringHash> hashs = { ".vert", ".frag","SHADERCOUNT", ".inc" };

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

bool ShaderSpirvData::GenerateGLSlangData() {
	//Find/set file version
	CMString versionText = "#version 450\n";
	if (m_SourceFile.ToLower().Contains("#version") && !m_SourceFile.SubStr(0,3).Contains("//")) {
		int firstNewLine = m_SourceFile.FindFromStart('\n');
		const uint VersionTextSize = sizeof("#version");
		versionText = m_SourceFile.SubStr(VersionTextSize, firstNewLine - VersionTextSize);
		versionText = "#version " + versionText + "\n";
		m_SourceFile = m_SourceFile.SubStr(firstNewLine + 1, -1);
	}

	//and add this extension to allow glslang to use #include
	CMString preProcessorString = R"(
		#extension GL_GOOGLE_include_directive : enable
		)";

	if (_CGraphics->GetGraphicsType() == GraphicsAPITypes::OPENGL4) {
		preProcessorString += "#define WITH_OPENGL 1\n";
		preProcessorString += "#define WITH_DIRECTX 0\n";
	} else {
		preProcessorString += "#define WITH_OPENGL 0\n";
		preProcessorString += "#define WITH_DIRECTX 1\n";
	}

	//Add our defines to the final file
	for (int i = 0; i < m_ShaderDefines.Length(); i++) {
		preProcessorString += "#define " + m_ShaderDefines[i]->m_Name + " " + CMString::IntToString(m_ShaderDefines[i]->m_Value) + "\n";
	}

	const char* s[3] = { versionText.Get(), preProcessorString.Get(), m_SourceFile.Get() };


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
	//m_IncludeList.Clear();

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

	CMString folderPath = (CMString(ShaderCachePath) + m_FilePath.m_FileLocation);
	CreateDirectory(folderPath.Get(), NULL);
	std::ofstream infoFile(folderPath + m_FilePath.m_FileName + ".info");
	if (infoFile.is_open()) {
		//start a new object
		SSD_JsonHolder = nlohmann::json();

		SSD_JsonHolder["SpirvJsonVersion"] = ShaderJsonFileVersion;
		if (a_DidFail) {
			SSD_JsonHolder["Error"] = true;
		} else {
			std::vector<char> vec;
			vec.insert(vec.end(), m_Hash, m_Hash + 16);
			SSD_JsonHolder["Hash"] = vec;

			SSD_JsonHolder["FilePath"] = m_FilePath.m_FilePath;

			SSD_JsonHolder["LastCompiedAPI"] = (int)_CGraphics->GetGraphicsType();
			SSD_JsonHolder["Permutation"] = m_HasPermutation;

			nlohmann::json& includes = SSD_JsonHolder["Includes"];
			for (uint i = 0; i < m_IncludeList.Length(); i++) {
				vec.clear();
				vec.insert(vec.end(), m_Hash, m_Hash + 16);
				SSD_JsonHolder["Hash"] = vec;
				includes[i]["Hash"] = m_IncludeList[i]->m_Hash;
				includes[i]["FilePath"] = m_IncludeList[i]->m_FilePath.m_FilePath;
			}

			nlohmann::json& defines = SSD_JsonHolder["Defines"];
			for (uint i = 0; i < m_ShaderDefines.Length(); i++) {
				defines[i]["Name"] = m_ShaderDefines[i]->m_Name;
				defines[i]["Value"] = m_ShaderDefines[i]->m_Value;
			}

		}

		infoFile << SSD_JsonHolder.dump(1, '\t', false, nlohmann::detail::error_handler_t::replace);

		infoFile.close();
	}
}
