#include "Object.h"


Object::Object() {
	m_DefineName = "ADD_OBJ";
	GenFileName("Objects");
}


Object::~Object() {
}

unsigned int hash_c_string(const char* p) {
	unsigned int result = 0;
	const size_t prime = 31;
	unsigned int i = 0;
	while(p[i] != 0){
	//for (size_t i = 0; i < s; ++i) {
		result = p[i++] + (result * prime);
	}
	return result;
}

void Object::GetFromFile(std::string fileName, std::string& fileData) {
	if (fileName.find(m_SaveFileName) != fileName.npos) {
		return;
	}

	size_t posFile = 0;

	static const int NumTexts = 2;
	std::string texts[NumTexts] = { m_DefineName, m_DefineName+"_BASE" };
	int currentText = 0;

	while (currentText != NumTexts) {
		posFile = fileData.find(texts[currentText] + "(", posFile + 1);
		if (posFile == fileData.npos) {
			posFile = 0;
			currentText++;
			continue;
		}
		posFile += texts[currentText].size() + 1;
		size_t size = (fileData.find(')', posFile) - posFile);
		std::string hashText = fileData.substr(posFile, size);
		//unsigned int hash = std::hash<std::string>{}(hashText);
		//unsigned int hash = std::hash<const char*>{}(hashText.c_str());
		unsigned int hash = hash_c_string(hashText.c_str());

		if (m_ObjectMap.find(hash) != m_ObjectMap.end()) {
			printf("dup\n");
		}
		Obj obj;
		obj.hash = hash;
		obj.name = hashText;
		obj.filename = fileName;
		m_ObjectMap[hash] = obj;
	}
}

void Object::SaveFile() {
	SaveFileH();
	SaveFileCPP();
}

void Object::SaveFileH() {
	std::string finalFile;

	finalFile = R"(//Do not edit - automated file
#pragma once
#include <algorithm>
#include <map>
#include <iostream>

#define )" + m_DefineName + R"((text)  \
public: \
virtual const char* GetObjName() override {return #text;};

#define )" + m_DefineName + R"(_BASE(text)  \
public: \
virtual const char* GetObjName() {return #text;};

#define REGISTER_OBJ(text) GENERATED_OBJ::g_factory->register_class<text>(#text);
enum _PROG_OBJ_ENUM {
)";

	//findAndReplaceAll(finalFile, "DEFINE", m_DefineName);

	//for (auto it = m_ObjectMap.begin(); it != m_ObjectMap.end(); it++) {
	//	printf("Hash: (%i): %s\n", it->first, it->second.name.c_str());
	//	finalFile += "\t_PROG_HASH_" + it->second.name + " = " + std::to_string(it->first) + ",\n";
	//}

	finalFile += "};\n\n";

	//for (auto it = m_ObjectMap.begin(); it != m_ObjectMap.end(); it++) {
	//	finalFile += "//#include \"" + it->second.filename + "\" //" + it->second.name + " \n";
	//}

	std::string buf2 = R"(
namespace GENERATED_OBJ{

template <class T> void* constructor() { return (void*)new T(); }

struct GEN_OBJ_FACTORY
{
   typedef void*(*constructor_t)();
   typedef std::map<std::string, constructor_t> map_type;
   map_type m_classes;

   template <class T>
   void register_class(std::string const& n)
   { m_classes.insert(std::make_pair(n, &constructor<T>)); }

   void* construct(std::string const& n)
   {
      map_type::iterator i = m_classes.find(n);
      if (i == m_classes.end()) return 0; // or throw or whatever you want
      return i->second();
   }
};

extern GEN_OBJ_FACTORY* g_factory;

	//const std::map<unsigned int,char*> OBJ_MAP = {)";

	if(false){
		bool firstRun = true;
		for (auto it = m_ObjectMap.begin(); it != m_ObjectMap.end(); it++) {
			if (!firstRun) {
				buf2 += ",";
			}
			buf2 += "{" + std::to_string(it->first) + ", \"" + it->second.name + "\" }";
			firstRun = false;
		}
	}

	buf2 += R"(};

	unsigned int hash_c_string(const char* p);

	void RegisterClasses();

};)";

	buf2 += R"(
static void* GET_OBJ(const char* a_Obj) {
void* obj = GENERATED_OBJ::g_factory->construct(a_Obj);
if(obj == nullptr){
std::cout << "Failed to get Object(" << a_Obj << ") from string, class is not in a .h file (if it's from cpp then please call REGISTER_OBJ on it) " << std::endl;
}
//std::cout << "Get OBJ " << a_Obj << " - " << std::hash<const char*>{}(a_Obj) << " - " << (242207216 == GENERATED_OBJ::hash_c_string(a_Obj)) << "\n";
	return obj;
}

//static char* GET_OBJ_NAME(const unsigned int a_Hash) {
//	return GENERATED_OBJ::OBJ_MAP[a_Hash];
//}

)";

	finalFile += "\n" + buf2;

	std::string directory = std::string(data->args[1]) + m_SaveFileName + ".h";

	std::ifstream fileStream(directory);
	std::stringstream fileBuffer;
	fileBuffer << fileStream.rdbuf();//get text from file into buffer
	std::string fileData = fileBuffer.str();

	if (fileData == finalFile) {
		std::cout << "\n" << directory << " - File Not changed" << "\n";
		return;
	}

	CreateDirectoryA((data->args[1] + data->args[3]).c_str(), NULL);

	std::cout << "\n" << "Output: " << directory << "\n";
	{
		std::ofstream myfile;
		myfile.open(directory.c_str());
		if (myfile.is_open()) {
			myfile << finalFile;
			myfile.close();
		}
	}
}

void Object::SaveFileCPP() {
	std::string finalFile;

	finalFile = R"(//Do not edit - automated file
)";
	finalFile += "#include \"stdafx.h\"\n\n";

	finalFile += "#include \"" + m_Name+ ".h\"\n\n";

	for (auto it = m_ObjectMap.begin(); it != m_ObjectMap.end(); it++) {
		if (it->second.filename.find(".h") != std::string::npos) {
			finalFile += "#include \"" + it->second.filename + "\" //" + it->second.name + " \n";
		}
	}

	finalFile += R"(
namespace GENERATED_OBJ{

	GEN_OBJ_FACTORY* g_factory = new GEN_OBJ_FACTORY();
)";

	finalFile += R"(

	unsigned int hash_c_string(const char* p) {
		unsigned int result = 0;
		const size_t prime = 31;
		unsigned int i = 0;
		while(p[i] != 0){
		//for (size_t i = 0; i < s; ++i) {
			result = p[i++] + (result * prime);
		}
		return result;
	}

	void RegisterClasses() {
)";

	for (auto it = m_ObjectMap.begin(); it != m_ObjectMap.end(); it++) {
		if (it->second.filename.find(".h") != std::string::npos) {
			finalFile += "REGISTER_OBJ("+ it->second.name +");\n";
		}
	}

finalFile += R"(
	}

};)";

	std::string directory = std::string(data->args[1]) + m_SaveFileName + ".cpp";

	std::ifstream fileStream(directory);
	std::stringstream fileBuffer;
	fileBuffer << fileStream.rdbuf();//get text from file into buffer
	std::string fileData = fileBuffer.str();

	if (fileData == finalFile) {
		std::cout << "\n" << directory << " - File Not changed" << "\n";
		return;
	}

	CreateDirectoryA((data->args[1] + data->args[3]).c_str(), NULL);

	std::cout << "\n" << "Output: " << directory << "\n";
	{
		std::ofstream myfile;
		myfile.open(directory.c_str());
		if (myfile.is_open()) {
			myfile << finalFile;
			myfile.close();
		}
	}
}
