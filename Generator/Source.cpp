#include <iostream>
#include <fstream> //std::ifstream
#include <sstream> //std::stringstream
#include <string> //std::string

#include <functional>
#include <algorithm>
#include <map>
#include <vector>

//#include <direct.h>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */


#include "Helpers.h"

#include "Hash.h"
#include "Object.h"

#include "Generated/Hash_Generator.h"

std::map<unsigned int, std::string> hashMap;
//std::vector<std::string> args;

#include "Generated/Objects_Generator.h"

ADD_OBJ(Testing)
class Testing {
public:
	Testing() {
		std::cout << "Testing Print!!!" << std::endl;
	}

};

//arg0 - file name/path
//arg1 - project file source
//arg2 - project .vcxproj
//arg3 - final file location folder
//arg4 - final file name
int main(int argc, char* argv[]) {

	//Manager::Create();
	//Data* data = new Data();
	std::cout << COMPILE_HASH(TEST) << "\n";

	REGISTER_OBJ(Testing);
	//GENERATED_OBJ::g_factory->register_class<Testing>("Testing");

	Testing* testing = (Testing*)GET_OBJ("Testing");

	for (int i = 0; i < argc; ++i) {
		std::cout << argv[i] << "\n";
		data->args.push_back(argv[i]);
	}

	Hash hash;
	Object object;

	//read .vcxproj
	std::ifstream vcxprojStream(argv[2]);
	std::stringstream fileBuffer;
	fileBuffer << vcxprojStream.rdbuf();//get text from file into buffer
	std::string vcxprojFile = fileBuffer.str();

	size_t pos = 0;
	const char* includeNames[2] = { "ClCompile Include", "ClInclude Include" };
	unsigned int current = 0;
	while (true) {
		pos = vcxprojFile.find(includeNames[current], pos);
		if (pos == vcxprojFile.npos) {
			current++;
			if (current == 1) {
				pos = 0;
				continue;
			}
			break;
		}
		pos += 19;
		size_t size = (vcxprojFile.find('\n', pos) - pos) - 4;

		std::string filename = vcxprojFile.substr(pos, size);

		std::ifstream fileStream(data->args[1] + filename);
		std::stringstream fileBuffer;
		fileBuffer << fileStream.rdbuf();//get text from file into buffer
		std::string fileData = fileBuffer.str();

		printf("Included: %s\n", filename.c_str());

		hash.GetFromFile(filename, fileData);
		object.GetFromFile(filename, fileData);
	}

	hash.SaveFile();
	object.SaveFile();

}