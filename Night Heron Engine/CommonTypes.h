#pragma once

#include <string>

//todo use own thing here
class CMString : public std::string {
public:
	CMString(const char* a_Input) {
		assign(a_Input);
	}
};
