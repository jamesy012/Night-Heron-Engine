#pragma once

#include "nlohmann/json_fwd.hpp"

#include "Types.h"

extern class IniFile* _CIniFileManager;

class IniFile {
public:
	IniFile();
	~IniFile();

	void LoadIni();
	void SaveIni();
	void SetIniPath(CMFilePath a_Location);

	nlohmann::json& getIniData();

private:
	CMFilePath m_Location;
};

