#pragma once

#include "Types/CMString.h"

#define HASH_LENGTH 16

class Util {
public:

	static CMString LoadTextFromPath(CMString a_Path);
	static bool IsANumber(char a_Text);
	static char ToLower(char a_Letter);
};

class HashHolder {
public:
	HashHolder() {};
	HashHolder(HashHolder& a_Other) {
		memcpy(m_Hash, a_Other.m_Hash, HASH_LENGTH);
	};
	HashHolder(uchar* a_Other) {
		memcpy(m_Hash, a_Other, HASH_LENGTH);
	};

	bool Comp(HashHolder a_Other) {
		return memcmp(m_Hash, &a_Other.m_Hash[0], HASH_LENGTH) == 0;
	}

	uchar* m_Hash[HASH_LENGTH] = { 0 };
};
