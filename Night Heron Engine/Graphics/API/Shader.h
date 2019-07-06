#pragma once

#include <string>
#include <vector>

#include "GFXObj.h"
#include "Interfaces/Saveable.h"

#include "Graphics/ShaderSpirvData.h"

namespace glslang {
	class TShader;
};

class ShaderUniformBlock : public GFXObj {
public:
	//unsigned int m_Slot = 0;
	unsigned int m_Size = 0;
	bool m_Registered = false;

	virtual void UpdateBuffer(void* a_Object) = 0;
};

class Shader : public GFXObj, public Saveable {
public:
	Shader();
	~Shader();

	void AddShader(ShaderSpirvData* a_Shader);
	void RemoveShader(ShaderSpirvData* a_Shader);
	void LinkShaders();
	virtual void Use() = 0;

	virtual void Reload() = 0;

	virtual void AddBuffer(ShaderUniformBlock* a_Block, CMString a_StructName);
	//finds buffer called a_StructName and adds it to the shader
	//returns true if buffer was found. false if it wasnt
	virtual bool AddBuffer(CMString a_StructName);
	virtual void BindTexture(std::string a_Name, unsigned int a_Index) = 0;

	//if there were any uniforms added where we couldnt find a registered uniform for it
	void FindUnlinkedUniforms();

	std::vector<unsigned int> loadSpirvFromPath(std::string a_Path);

	bool m_ShouldPrintCode = false;
	bool m_ShoudRegenerateCode = false;

	bool m_HasTextureForSlot[32] = { false };

	CMArray<ShaderSpirvData*> m_ShaderFileObjects;

protected:
	virtual void AddBuffer_Internal(ShaderUniformBlock* a_Block, CMString a_StructName) = 0;
	virtual void AddShader_Internal(ShaderType a_Type, std::vector<unsigned int> a_Code) = 0;
	//virtual void AddShader_Internal(ShaderTypes a_Type, std::string a_Path) = 0;
	virtual void Link_Internal() = 0;

	// Inherited via Saveable
	virtual bool LoadData_Internal(nlohmann::json& a_Json) override;
	virtual void SaveData_Internal(nlohmann::json& a_Json) override;

	CMString GetShaderTypeString(ShaderType a_Type);

	//has this shader been linked yet?
	bool m_IsLinked;

	struct UniformBufferObject {
		ShaderUniformBlock* m_Block;
		CMString m_Name;
		bool m_HasLinked;
	};

	CMArray<UniformBufferObject> m_AttachedUniforms;

private:
	struct ShaderInfo {
		bool m_IsUsed;
		bool m_HasBeenLoaded;
		std::string m_Path;
	};
	ShaderInfo m_ShaderObjects[(int)ShaderType::SHADERCOUNT] = { false, false, "" };
	//glslang::TShader* m_ShaderObjects[(int)ShaderTypes::SHADERCOUNT] = {0};
};

extern Shader* _CCurrentShader;
