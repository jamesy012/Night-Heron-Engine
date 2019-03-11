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

	CMString ToLower();

	void Hash(uchar* a_Output) const;
	//runs Hash but also new's the output
	//besure to delete it
	uchar* HashAlloc() const;

	uint FindFromEnd(char a_Character) const {
		return find_last_of(a_Character);
	}

	CMString SubStr(uint a_From, uint a_NumCharacters) const {
		return substr(a_From, a_NumCharacters);
	}

	CMString SubStrFindFromEnd(char a_Character) const {
		const uint index = FindFromEnd(a_Character);
		return substr(index, Length() - index);
	}

	static CMString IntToString(int a_Value) {
		char buffer[33];
		_itoa_s(a_Value, buffer, 10);
		return CMString(buffer);
	}
};

class CMFilePath {
public:
	CMFilePath() {

	}

	CMFilePath(CMString a_Input) {
		SetPath(a_Input);
	}

	void SetPath(CMString a_Input) {
		if (a_Input.Length() < 2) {
			return;
		}
		//if (a_Input.at(0) != '/' && a_Input.at(1) != ':') {
		//	a_Input = "/" + a_Input;
		//}
		m_FilePath = a_Input;

		uint lastFolderIndex = m_FilePath.FindFromEnd('/') + 1;
		if (a_Input.at(0) != '/' && a_Input.at(1) != ':') {
			lastFolderIndex = 0;
		}

		m_FileLocation = m_FilePath.SubStr(0, lastFolderIndex);
		m_FileName = m_FilePath.SubStr(lastFolderIndex, m_FilePath.Size() - lastFolderIndex);
		
	}

	//Full Path
	CMString m_FilePath;
	//Folder Path only
	CMString m_FileLocation;
	//File Name Only
	CMString m_FileName;
};

template <class T>
class CMArray : public std::vector<T> {
public:
	CMArray() : std::vector<T>() {

	};

	CMArray(std::initializer_list<T> a_Input) : std::vector<T>(a_Input) {
	};

	uint Length() const {
		return this->size();
	}

};
