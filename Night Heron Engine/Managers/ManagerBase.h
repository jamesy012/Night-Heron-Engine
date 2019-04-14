#pragma once

#include "Types.h"
class ShaderSpirvData;
class Shader;

class ManagerBase {
public:

	virtual void FindAllInFiles();

	//code for opening up the window
	virtual void ImGuiWindow(bool* a_Open);
	//where ImGui::BeginChild... goes
	virtual void ImGuiData() = 0;

	virtual uint FindElement(CMString a_FilePath);

protected:
	virtual bool IsFileAOurType(CMString a_FilePath) = 0;
	virtual void AddFromPath_Internal(CMString a_FilePath) = 0;

	CMArray<CMString> m_Paths;

	CMString m_WindowName = "Manager";
private:
	virtual void SeachFolder(CMString a_Path);

	CMString m_CurrentPath = "";
};
