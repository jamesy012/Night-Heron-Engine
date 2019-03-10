#pragma once

#include <string>
#include <vector>
#include "SimpleMath.h"

//todo use own thing here
class CMString : public std::string {
public:
	CMString()  {
	};

	CMString(const char* a_Input) : std::string(a_Input) {
	};

	CMString(std::string a_String) : std::string(a_String) {
	};

	//CMString operator+(const const char*& rhs) {
	//	CMString string;
	//	string.append(this);
	//	return this->append(rhs);
	//}

	const char* Get() {
		return c_str();
	};

	uint Size() {
		return size();
	};

	uint FindFromEnd(char a_Character) {
		return find_last_of(a_Character);
	}

	CMString SubStr(uint a_From, uint a_NumCharacters) {
		return substr(a_From, a_NumCharacters);
	}

	static CMString IntToString(int a_Value) {
		char buffer[33];
		_itoa_s(a_Value, buffer, 10);
		return CMString(buffer);
	}
};

template <class T>
class CMArray : public std::vector<T> {


};
