#pragma once

#include "ManagerBase.h"

#include "Types/CMString.h"
#include "Types/CMStringHash.h"
#include "Types/CMArray.h"

class ShaderSpirvData;
class Shader;

class ShaderSpirvManager;

extern class ShaderManager* _CShaderManager;

//Will error if the sizes are not correct
#define CREATE_BUFFER_UNIFORM(name,x) \
struct name { \
public: \
x \
};\
static_assert(sizeof(name) % 16 == 0,"Buffer size must be multiple of 16 '" #name "'");

class ShaderManager : public ManagerBase {
public:
	ShaderManager();
	~ShaderManager();

	void ImGuiWindow(bool* a_Open) override;
	void ImGuiData();

	uint FindElement(CMString a_FilePath) override;

	Shader* GetShader(CMString a_FilePath);
	Shader* GetShader(uint a_Index);

	void AddShader(Shader* a_Shader);
	void RemoveShader(Shader* a_Shader);

	void ImGuiSelector(uint* a_ID, SimpleVec2 a_Size = SimpleVec2(0, 100));

protected:
	friend ShaderSpirvManager;

	struct ShaderHolder {
		Shader* m_Shader;
		bool m_PassedThrough;
	};

	uint FindShader(Shader* a_Shader);

	CMArray<ShaderHolder> m_Shaders;
	int m_ChangeToTab = -1;
	int m_NodeSelected = -1;
	int m_ChangeToIndex = -1;
private:
	bool IsFileAOurType(CMString a_FilePath) override;
	void AddFromPath_Internal(CMString a_FilePath) override;

	const CMArray<CMStringHash> m_FileTypes = { ".shader" };

	CMArray<bool> m_UnsavedShaders;
};
