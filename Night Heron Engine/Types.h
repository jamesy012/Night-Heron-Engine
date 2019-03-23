#pragma once

#include <string>
//#include <vector>
#include "SimpleMath.h"


template <typename  T>
class CMArray /*: public std::vector<T>*/ {
public:
	CMArray() {

	};

	CMArray(std::initializer_list<T> a_Input) {
		auto node = a_Input.begin();
		for (int i = 0; i < a_Input.size(); i++) {
			Add(*node++);
		}
	};

	//~CMArray() {
	//	//todo work this out..
	//	delete[] m_Array;
	//}

	uint Length() const {
		return m_Size;
	}

	void Add(T a_Object) {
		ResizeIfNeeded(1);
		m_Array[m_Size] = a_Object;
		//memcpy(&m_Array[m_Size], &a_Object, sizeof(T));
		m_Size++;
		//this->push_back(a_Object);
	}

	//void AddList(T a_Element, uint a_Count) {
	//	for (uint i = 0; i < a_Count; i++) {
	//		Add(a_Element[i]);
	//	}
	//}

	T At(int a_Index) {
		return m_Array[a_Index];
	}

	T* First() {
		return &m_Array[0];
	}

	T Last() {
		return m_Array[m_Size - 1];
	}

	void Reserve(uint a_Size) {
		ResizeIfNeeded(a_Size);
	}

	void Remove(T a_Object) {
		uint index = FindElement(a_Object);
		if (index == -1) {
			return;
		}

		T* newArray = new T[m_ArraySize];


		//memcpy(newArray, m_Array, sizeof(T) * index);
		//memcpy(newArray[index], m_Array[index], sizeof(T) * ((m_Size - index)));
		for (uint i = 0; i < index; i++) {
			newArray[i] = m_Array[i];
		}
		for (uint i = index + 1; i < m_Size; i++) {
			newArray[i - 1] = m_Array[i];
		}

		if (m_Array) {
			delete m_Array;
		}
		m_Array = newArray;
		m_Size--;
	}

	uint FindElement(T a_Object) {
		for (int i = 0; i < Length(); i++) {
			if (m_Array[i] == a_Object) {
				return i;
			}
		}
		return -1;
	}

	const T &operator=(const T &);
	T &operator[](int);
	T operator[](int) const;

private:

	void ResizeIfNeeded(int a_NumElements);
	void ResizeForElements();

	T* m_Array;
	uint m_Size;
	uint m_ArraySize;

};


template<typename T>
const T & CMArray<T>::operator=(const T &) {
	// TODO: insert return statement here
}

template<typename T>
T & CMArray<T>::operator[](int a_Index) {
	return m_Array[a_Index];
}

template<typename T>
T CMArray<T>::operator[](int a_Index) const {
	return m_Array[a_Index];
}

template<typename  T>
void CMArray<T>::ResizeIfNeeded(int a_NumElements) {
	if (m_Size + a_NumElements > m_ArraySize) {
		m_ArraySize = m_Size + a_NumElements + (int)((float)m_Size * 0.35f) + 1;
		ResizeForElements();
	}
}

template<typename  T>
inline void CMArray<T>::ResizeForElements() {
	//uint TSize = sizeof(T) / sizeof(char*);
	uint TSize = sizeof(T);
	if (TSize > 1'000'000) {
		return;
	}
	//char* newArray = new char[m_ArraySize * TSize];
	T* newArray = new T[m_ArraySize];

	//memcpy(&newArray, &m_Array, TSize * m_Size);
	//memcpy(&newArray, &m_Array, m_Size);
	//memcpy(newArray, m_Array, m_Size * TSize);
	//memcpy(newArray, m_Array, m_Size * TSize);
	//memmove(newArray, m_Array, TSize * m_Size);
	for (uint i = 0; i < m_Size; i++) {
		newArray[i] = m_Array[i];
	}

	if (m_Array) {
		delete[] m_Array;
	}
	m_Array = (T*)newArray;
}

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
			splits.Add(SubStr(index, nextChar- index));
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
	static int StringToInt(CMString a_Value) {
		int value = atoi(a_Value.Get());
		return value;
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
		//if (a_Input.at(0) != '/' && a_Input.at(1) != ':') {
		//	lastFolderIndex = 0;
		//}

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

struct CMStringHash {
	CMStringHash() {
	}
	CMStringHash(const CMString a_Text) {
		CMString(a_Text).Hash(m_ExtenstionHash);
	}
	void Hash(const CMString a_Text) {
		CMString(a_Text).Hash(m_ExtenstionHash);
	}
	CMStringHash(const char* a_Text) : CMStringHash(CMString(a_Text)) {}
	uchar m_ExtenstionHash[16] = { 0 };
};