#include "stdafx.h"
#include "IniFile.h"

#include <fstream> //std::ifstream
#include <sstream> //std::stringstream
#include <string> //std::string

#include "Util.h"
#include "nlohmann/json.hpp"

#include <windows.h>

static nlohmann::json JsonData;

IniFile::IniFile() {
	JsonData;
	m_Location = "iniFile.ini";
}


IniFile::~IniFile() {
}

void IniFile::LoadIni() {
	CMString data = Util::LoadTextFromPath(m_Location.m_FilePath);
	if (data.Length() < 5) {
		return;
	}

	JsonData = nlohmann::json::parse(data);
}

void IniFile::SaveIni() {
	CreateDirectory(m_Location.m_FileLocation.Get(), NULL);
	std::ofstream infoFile(m_Location.m_FilePath, std::ofstream::out);

	if (infoFile.is_open()) {
		infoFile << JsonData.dump(4);
		infoFile.close();
	}
}

void IniFile::SetIniPath(CMFilePath a_Location) {
	m_Location = a_Location;
}

nlohmann::json& IniFile::getIniData() {
	return JsonData;
}