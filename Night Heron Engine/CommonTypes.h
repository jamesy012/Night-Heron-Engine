#pragma once

#include <string>
#include <vector>

//todo use own thing here
class CMString : public std::string {
public:
	CMString(const char* a_Input) {
		assign(a_Input);
	}
};

template <class T>
class CMArray : public std::vector<T> {


};
