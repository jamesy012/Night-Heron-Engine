#include "Saveable.h"

#include <fstream> //std::ifstream
#include <sstream> //std::stringstream
#include <string> //std::string

#include <windows.h>

#include "nlohmann/json.hpp"

bool Saveable::Load() {
	CMString data = Util::LoadTextFromPath(m_FilePath.m_FilePath);
	if (data.Length() < 5) {
		return false;
	}

	m_JsonData = data;
	nlohmann::json j = nlohmann::json::parse(data);

	if (j["Version"] != m_Version) {
		return false;
	}

	return LoadData_Internal(j["Object"]);
}

void Saveable::Save() {
	CreateDirectory(m_FilePath.m_FileLocation.Get(), NULL);
	std::ofstream infoFile(m_FilePath.m_FilePath);

	nlohmann::json j = nlohmann::json::parse(m_JsonData);
	j["Version"] = m_Version;
	if (infoFile.is_open()) {

		SaveData_Internal(j["Object"]);

		infoFile << j.dump(4);
		infoFile.close();
	}
}
