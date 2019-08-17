#pragma once


#include "Types/CMString.h"

#include <cassert>

#define CMASSERT_MSG(Expression, Message) assert(!(Expression) && Message);
#define CMASSERT(Expression) CMASSERT_MSG(Expression, "");


namespace CMLogger {
	extern int PrintIndent;
	extern CMString LogCat;

	void LogMessage(const char* a_Message, ...);

	class CMLogScopedIndent {
	public:
		CMLogScopedIndent() {
			CMLogger::PrintIndent++;
		}
		~CMLogScopedIndent() {
			CMLogger::PrintIndent--;
		}
	};

	class CMLogScopedName {
	public:
		CMLogScopedName(CMString a_Name) {
			PreviousName = CMLogger::LogCat;
			CMLogger::LogCat = a_Name;
		}
		~CMLogScopedName() {
			CMLogger::LogCat = PreviousName;

		}
	private:
		CMString PreviousName;
	};

};

#define CMLOG_INDENT(x) CMLogger::CMLogScopedIndent x;
#define CMLOG_NAME(x) CMLogger::CMLogScopedName INTERNAL_CMLogName(x);

#define CMLOG(...) CMLogger::LogMessage(__VA_ARGS__);
#define CMLOG_LIT(Message) CMLogger::LogMessage("%s\n", #Message);
#define CMLOG_LINE(Message) CMLogger::LogMessage("(%s #%i) %s", __FUNCTION__, __LINE__, Message);
