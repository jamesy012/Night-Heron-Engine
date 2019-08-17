#pragma once

#include "SimpleMath.h"

#include "CMString.h"

class CMFilePath {
public:
	CMFilePath() {
	}

	CMFilePath(CMString a_Input) {
		SetPath(a_Input);
	}

	CMFilePath(const char* a_Input) {
		SetPath(a_Input);
	}

	void SetPath(CMString a_Input) {
		if (a_Input.Length() < 2) {
			return;
		}
		//if (a_Input.at(0) != '/' && a_Input.at(1) != ':') {
		//	a_Input = "/" + a_Input;
		//}
		m_FilePath = a_Input;

		uint lastFolderIndex = m_FilePath.FindFromEnd('/') + 1;
		//if (a_Input.at(0) != '/' && a_Input.at(1) != ':') {
		//	lastFolderIndex = 0;
		//}

		m_FileLocation = m_FilePath.SubStr(0, lastFolderIndex);
		m_FileName = m_FilePath.SubStr(lastFolderIndex, m_FilePath.Size() - lastFolderIndex);
	}

	//Full Path
	CMString m_FilePath;
	//Folder Path only
	CMString m_FileLocation;
	//File Name Only
	CMString m_FileName;
};