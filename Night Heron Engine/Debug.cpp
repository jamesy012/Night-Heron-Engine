#include "Debug.h"

#include <stdio.h>
#include <stdarg.h>

#include <Windows.h>

namespace CMLogger {
	int PrintIndent = 0;
	CMString LogCat = CMString();
};

void CMLogger::LogMessage(const char* a_Message, ...) {
	char buffer[512] = { 0 };

	//Category
	if (!LogCat.IsEmpty()) {
		sprintf_s(buffer, "%s%s", LogCat.Get(), ":\t");
		printf(buffer);
		OutputDebugString(buffer);

	}

	FillMemory(buffer, 512, 0);

	//indent
	CMString indent;
	for (int i = 0; i < PrintIndent; ++i) {
		indent += '\t';
	}
	printf(indent.Get());
	OutputDebugString(indent.Get());

	//message/arguments
	va_list argptr;
	va_start(argptr, a_Message);
	vprintf(a_Message, argptr);


	vsprintf_s(buffer, a_Message, argptr);
	OutputDebugString(buffer);

	va_end(argptr);


}