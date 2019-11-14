#pragma once

#include "ManagerBase.h"

#include "Types/CMString.h"
#include "Types/CMStringHash.h"
#include "Types/CMArray.h"

class ShaderSpirvData;
class Shader;

class ShaderManager;

extern class ShaderSpirvManager* _CShaderSpirvManager;

class ShaderSpirvManager : public ManagerBase {
public:
	ShaderSpirvManager();
	~ShaderSpirvManager();

	void ImGuiData() override;

	uint FindElement(CMString a_FilePath) override;

	ShaderSpirvData* GetShaderPart(CMString a_FilePath);

	const CMArray<CMStringHash> m_FileTypes = { ".vert", ".frag", ".comp","SHADERCOUNT", ".inc" };

protected:
	friend ShaderManager;

	CMArray<ShaderSpirvData*> m_ShaderObjects;
	int m_ChangeToIndex = -1;
	int m_NodeSelected = -1;
private:
	bool IsFileAOurType(CMString a_FilePath) override;
	void AddFromPath_Internal(CMString a_FilePath) override;
};
