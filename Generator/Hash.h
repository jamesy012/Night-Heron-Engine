#pragma once

#include "BaseType.h"

class Hash : public BaseType{
public:
	Hash();
	~Hash();

	// Inherited via BaseType
	virtual void GetFromFile(std::string fileName, std::string& fileData) override;
	virtual void SaveFile() override;
protected:
	std::map<unsigned int, std::string> hashMap;
	//std::string hashDefineName = "COMPILE_HASH";
	//std::string hashSaveFileName = "...";

};

