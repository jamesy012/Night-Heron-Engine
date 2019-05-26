#pragma once

//Handles objects which are able to be saved to or loaded from disk

#include "Util.h"
#include "Types.h"

#include "Jsonable.h"

class Saveable : public Jsonable {
public:
	Saveable() {
	}
	Saveable(CMString a_FilePath) {
		m_FilePath.SetPath(a_FilePath);
	};

	virtual bool Load();
	virtual void Save();

	CMFilePath m_FilePath;
protected:
	float m_Version = 1.0f;

	CMString m_JsonData;
};
