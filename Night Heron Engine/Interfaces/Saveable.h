#pragma once

//Handles objects which are able to be saved to or loaded from disk

#include "Util.h"
#include "Types.h"

class Saveable {
public:
	Saveable(CMString a_FilePath) {
		m_FilePath.SetPath(a_FilePath);
	};

	virtual bool Load() = 0;
	virtual void Save() = 0;

	CMFilePath m_FilePath;
	//Hash of the data in the file
	uchar m_Hash[HASH_LENGTH] = {0};
};