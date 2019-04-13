#pragma once
#include "BaseType.h"
class Object : public BaseType {
public:
	Object();
	~Object();

	// Inherited via BaseType
	virtual void GetFromFile(std::string fileName, std::string& fileData) override;
	virtual void SaveFile() override;

private:
	void SaveFileH();
	void SaveFileCPP();

	struct Obj {
		unsigned int hash;
		std::string name;
		std::string filename;
	};
	
	std::map<unsigned int, Obj> m_ObjectMap;

};
