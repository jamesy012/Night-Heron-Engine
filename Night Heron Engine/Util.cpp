#include "Util.h"

#include <fstream> //std::ifstream
#include <sstream> //std::stringstream

CMString Util::LoadTextFromPath(CMString a_Path) {
	if (a_Path.Length() > 1 && a_Path.at(0) == '/') {
		a_Path = a_Path.SubStr(1, a_Path.Length());
	}
	std::ifstream textFile(a_Path);
	if (textFile.bad()) {
		return CMString();
	}
	std::stringstream fileBuffer;
	fileBuffer << textFile.rdbuf();//get text from file into buffer
	return fileBuffer.str();
}

bool Util::IsANumber(char a_Char) {
	return a_Char >= '0' && a_Char <= '9';
}
