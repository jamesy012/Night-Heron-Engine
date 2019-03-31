#include "ManagerBase.h"

#include <iostream>
#include <filesystem>
namespace fs = std::experimental::filesystem;

#include "ImGui/imgui.h"


void ManagerBase::FindAllInFiles() {
	m_CurrentPath = fs::current_path().generic_string();
	SeachFolder(m_CurrentPath);

	for (uint i = 0; i < m_Paths.Length(); i++) {
		AddFromPath_Internal(m_Paths[i]);
	}
}

void ManagerBase::ImGuiWindow(bool * a_Open) {
	if (!a_Open) {
		return;
	}

	ImGui::Begin(m_WindowName.Get(), a_Open);

	ImGuiData();

	ImGui::End();
}

void ManagerBase::SeachFolder(CMString a_Path) {
	for (const fs::v1::directory_entry & entry : fs::directory_iterator(a_Path)) {
		CMString filePath = entry.path().generic_string();
		if (fs::is_directory(entry.status())) {
			SeachFolder(filePath);
			continue;
		}
		CMString relativePath = filePath.SubStr(m_CurrentPath.Length(), -1);
		if (IsFileAOurType(relativePath)) {
			m_Paths.Add(relativePath);
		}
	}
}
