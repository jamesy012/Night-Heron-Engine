#pragma once

#include <iostream>
#include <fstream> //std::ifstream
#include <sstream> //std::stringstream
#include <string> //std::string

#include <functional>
#include <algorithm>
#include <map>
#include <vector>

#include "Helpers.h"
#include <windows.h>

class BaseType {
public:
	virtual void GetFromFile(std::string fileName, std::string& fileData) = 0;
	virtual void SaveFile() = 0;

	//Data* data;
protected:
	void GenFileName(std::string a_Name) {
		m_Name = a_Name + "_" + data->args[4];
		std::replace(m_Name.begin(), m_Name.end(), ' ', '_');
		m_SaveFileName = data->args[3] + m_Name;
		//std::replace(m_SaveFileName.begin(), m_SaveFileName.end(), ' ', '_');
	}

	std::string m_DefineName = "...";
	std::string m_SaveFileName = "...";
	std::string m_Name = "...";
};