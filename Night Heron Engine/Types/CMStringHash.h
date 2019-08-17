#pragma once

#include "SimpleMath.h"

#include "CMString.h"

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
