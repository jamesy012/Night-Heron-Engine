#pragma once

#include "Types.h"
class ShaderSpirvData;
class Shader;

class ManagerBase {
public:

	virtual void FindAllInFiles();

	virtual void ImGuiWindow(bool* a_Open) = 0;

protected:
	virtual bool IsFileAOurType(CMString a_FilePath) = 0;
	virtual void AddFromPath_Internal(CMString a_FilePath) = 0;
	
	CMArray<CMString> m_Paths;
private:
	virtual void SeachFolder(CMString a_Path);

	CMString m_CurrentPath = "";

};