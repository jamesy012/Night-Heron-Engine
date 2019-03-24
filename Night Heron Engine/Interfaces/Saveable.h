#pragma once

//Handles objects which are able to be saved to or loaded from disk

#include "Util.h"
#include "Types.h"

class Saveable {
public:
	Saveable() {
	}
	Saveable(CMString a_FilePath) {
		m_FilePath.SetPath(a_FilePath);
	};

	virtual bool Load();
	virtual void Save();

	CMFilePath m_FilePath;
	//Hash of the data in the file
	uchar m_Hash[HASH_LENGTH] = {0};
protected:
	virtual bool Load_Internal(CMArray<CMString> a_Splits) = 0;
	virtual CMString GetData_Internal() = 0;
};