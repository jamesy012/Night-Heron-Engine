#pragma once

#include "SimpleMath.h"

#include <initializer_list>
#include <cstring>

template <typename  T>
class CMArray /*: public std::vector<T>*/ {
public:
	CMArray() {
	};

	CMArray(std::initializer_list<T> a_Input) {
		auto node = a_Input.begin();
		for (uint i = 0; i < a_Input.size(); i++) {
			Add(*node++);
		}
	};

	CMArray(const CMArray<T>& a_Input) {
		m_Size = a_Input.m_Size;
		m_ArraySize = a_Input.m_ArraySize;
		m_Array = new T[m_ArraySize];
		for (uint i = 0; i < m_Size; i++) {
			m_Array[i] = a_Input.m_Array[i];
		}
	};

	//CMArray & operator=(const CMArray& a) { return *this; }

	~CMArray() {
		//todo work this out..
		if (m_Array) {
			delete[] m_Array;
		}
	}

	uint Length() const {
		return m_Size;
	}

	void Clear() {
		if (m_Array) {
			delete[] m_Array;
		}
		m_Size = m_ArraySize = 0;
		m_Array = nullptr;
	}

	void Add(T a_Object) {
		ResizeIfNeeded(1);
		m_Array[m_Size] = a_Object;
		//memcpy(&m_Array[m_Size], &a_Object, sizeof(T));
		m_Size++;
		//this->push_back(a_Object);
	}

	void AddUnique(T a_Object) {
		if (FindElement(a_Object) == -1) {
			Add(a_Object);
		}
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

	void RemoveAt(uint a_Index) {
		if (a_Index == -1) {
			return;
		}

		T* newArray = new T[m_ArraySize];

		//memcpy(newArray, m_Array, sizeof(T) * a_Index);
		//memcpy(newArray[index], m_Array[index], sizeof(T) * ((m_Size - a_Index)));
		for (uint i = 0; i < a_Index; i++) {
			newArray[i] = m_Array[i];
		}
		for (uint i = a_Index + 1; i < m_Size; i++) {
			newArray[i - 1] = m_Array[i];
		}

		if (m_Array) {
			delete[] m_Array;
		}
		m_Array = newArray;
		m_Size--;
	}

	void Remove(T a_Object) {
		RemoveAt(FindElement(a_Object));
	}

	uint FindElement(T a_Object) {
		for (uint i = 0; i < m_Size; i++) {
			if (m_Array[i] == a_Object) {
				//if (memcmp(a_Object, m_Array[i], sizeof(T)) == 0) {
				return i;
			}
		}
		return -1;
	}

	CMArray<T>& operator=(const CMArray<T>&);
	T& operator[](int);
	T operator[](int) const;

private:

	void ResizeIfNeeded(int a_NumElements);
	void ResizeForElements();

	T* m_Array = nullptr;
	uint m_Size = 0;
	uint m_ArraySize = 0;
};

/*
T& operator=(const T& other) // copy assignment
{
	if (this != &other) { // self-assignment check expected
		if (other.size != size) {         // storage cannot be reused
			delete[] mArray;              // destroy storage in this
			size = 0;
			mArray = nullptr;             // preserve invariants in case next line throws
			mArray = new int[other.size]; // create storage in this
			size = other.size;
		}
		std::copy(other.mArray, other.mArray + other.size, mArray);
	}
	return *this;
}
*/

template<typename T>
CMArray<T>& CMArray<T>::operator=(const CMArray<T>& a_Input) {
	m_Size = a_Input.m_Size;
	m_ArraySize = a_Input.m_ArraySize;
	m_Array = new T[m_ArraySize];
	//for (uint i = 0; i < m_Size; i++) {
	//	m_Array[i] = a_Input.m_Array[i];
	//}
	memcpy(m_Array, a_Input.m_Array, sizeof(T) * m_Size);
	return *this;
}

template<typename T>
T& CMArray<T>::operator[](int a_Index) {
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
