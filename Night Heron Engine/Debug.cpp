#include "Debug.h"

#include <stdio.h>
#include <stdarg.h>

namespace CMLogger {
	int PrintIndent = 0;
	CMString LogCat = CMString();
};

void CMLogger::LogMessage(const char* a_Message, ...) {
	//Catergory
	if (!LogCat.IsEmpty()) {
		printf("%s%s", LogCat.Get(), ":\t");
	}

	//indent
	CMString indent;
	for (int i = 0; i < PrintIndent; ++i) {
		indent += '\t';
	}
	printf(indent.Get());

	//message/arugments
	va_list argptr;
	va_start(argptr, a_Message);
	vprintf(a_Message, argptr);
	va_end(argptr);
}