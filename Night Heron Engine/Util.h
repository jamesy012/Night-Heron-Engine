#pragma once

#include "Types.h"

#define CMLog(Message) printf(Message);

class Util {
public:
	Util();
	~Util();

	static CMString LoadTextFromPath(CMString a_Path);

};

