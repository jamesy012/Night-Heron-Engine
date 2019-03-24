#include "Saveable.h"


#include <fstream> //std::ifstream
#include <sstream> //std::stringstream
#include <string> //std::string

#include <windows.h>

bool Saveable::Load() {
	CMString data = Util::LoadTextFromPath(m_FilePath.m_FilePath);
	if (data.Length() < 5) {
		return false;
	}

	data = data.SubStr(17, data.Length() - 17);
	CMArray<CMString> splits = data.Split('\n');


	return Load_Internal(splits);
}

void Saveable::Save() {
	CreateDirectory(m_FilePath.m_FileLocation.Get(), NULL);
	std::ofstream infoFile(m_FilePath.m_FilePath);
	CMString data;
	if (infoFile.is_open()) {

		data = GetData_Internal();

		data.Hash(m_Hash);
		for (int q = 0; q < HASH_LENGTH; q++) {
			infoFile << m_Hash[q];
		}
		infoFile << "\n";
		infoFile << data;
		infoFile.close();
	}
}