#pragma once

#include "Types.h"

#define CMLog(Message) printf(Message);

#define HASH_LENGTH 16

class Util {
public:

	static CMString LoadTextFromPath(CMString a_Path);
	static bool IsANumber(char a_Text);

};

