#include "Hash.h"



Hash::Hash(){
	m_DefineName = "COMPILE_HASH";
	GenFileName("hash");
}


Hash::~Hash() {
}

void Hash::GetFromFile(std::string fileName, std::string& fileData) {
	if (fileName.find(m_SaveFileName) != fileName.npos) {
		return;
	}

	size_t posFile = 0;

	while (true) {
		posFile = fileData.find(m_DefineName + "(", posFile + 1);
		if (posFile == fileData.npos) {
			break;
		}
		posFile += m_DefineName.size() + 1;
		size_t size = (fileData.find(')', posFile) - posFile);
		std::string hashText = fileData.substr(posFile, size);
		unsigned int hash = std::hash<std::string>{}(hashText);
		//printf("Hash Text: %s, (%i)\n", hashText.c_str(), hash);
		if (hashMap.find(hash) != hashMap.end()) {
			printf("dup\n");
		}
		hashMap[hash] = hashText;
	}
}

void Hash::SaveFile() {
	std::string finalFile;

	finalFile = R"(//Do not edit - automated file
//this contains all DEFINE(text) from the program
#define DEFINE(text) _PROG_HASH_##text
enum _PROG_HASH_ENUM {
)";

	findAndReplaceAll(finalFile, "DEFINE", m_DefineName);

	auto it = hashMap.begin();
	for (; it != hashMap.end(); it++) {
		printf("Hash: (%i): %s\n", it->first, it->second.c_str());
		finalFile += "\t_PROG_HASH_" + it->second + " = " + std::to_string(it->first) + ",\n";
	}

	finalFile += "};";

	std::string directory = std::string(data->args[1]) + m_SaveFileName + ".h";

	CreateDirectoryA((data->args[1] + data->args[3]).c_str(), NULL);

	std::cout << "\n" << "Output: " << directory << "\n";

	{
		//std::ofstream myfile(argv[3]);
		std::ofstream myfile;
		//myfile.open("E:\\coding\\c++\\ConstExpr And Compile Time\\_TEST_\\TEST.cpp");
		myfile.open(directory.c_str());
		//if (myfile.is_open()) {
		myfile << finalFile;
		myfile.close();
		//}
	}
}
