#pragma once

#include "SimpleMath.h"

#include "CMArray.h"

//todo remove string include
#include <string>

//todo use own thing here
class CMString : public std::string {
public:
	CMString() {
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

	const char* Get() const {
		return c_str();
	};

	uint Size() const {
		return size();
	};

	uint Length() const {
		return size();
	};

	char At(uint a_Index) const {
		return at(a_Index);
	};

	bool Compare(CMString a_Other) const {
		return compare(a_Other) == 0;
	}

	bool IsEmpty() {
		return Length() == 0;
	}

	CMString ToLower();

	void Hash(uchar* a_Output) const;
	//runs Hash but also new's the output
	//besure to delete it
	uchar* HashAlloc() const;

	uint FindFromEnd(char a_Character) const {
		return find_last_of(a_Character);
	}

	uint FindFromStart(char a_Character) const {
		return find_first_of(a_Character);
	}

	bool Contains(CMString a_String) const {
		return find(a_String) != std::string::npos;
	}

	CMString SubStr(uint a_From, uint a_NumCharacters) const {
		return substr(a_From, a_NumCharacters);
	}

	CMString SubStrFindFromEnd(char a_Character) const {
		const uint index = FindFromEnd(a_Character);
		if (index == -1) {
			return Get();
		}
		return substr(index, Length() - index);
	}

	CMArray<CMString> Split(char a_Char) const {
		CMArray<CMString> splits;
		uint index = 0;
		while (true) {
			uint nextChar = find(a_Char, index);
			if (nextChar == npos) {
				break;
			}
			splits.Add(SubStr(index, nextChar - index));
			index = nextChar + 1;
		}
		splits.Add(SubStr(index, Length() - index));
		return splits;
	}

	static CMString IntToString(int a_Value) {
		char buffer[33];
		_itoa_s(a_Value, buffer, 10);
		return CMString(buffer);
	}

	static CMString FloatToString(float a_Value) {
		char buffer[33];
		sprintf_s(buffer, "%f", a_Value);
		return CMString(buffer);
	}

	static int StringToInt(CMString a_Value) {
		int value = atoi(a_Value.Get());
		return value;
	}

	static float StringToFloat(CMString a_Value) {
		float value = (float)atof(a_Value.Get());
		return value;
	}
};


