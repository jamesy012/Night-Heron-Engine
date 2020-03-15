#pragma once


#include "Types/CMString.h"

#include <cassert>

#define CMASSERT_MSG(Expression, Message) assert(!(Expression) && Message);
//ASSERTS WHEN TRUE
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

#define _INTERNAL_CMLOG_INDENT_NAMED(x,y)			CMLogger::CMLogScopedIndent x##y;
#define CMLOG_SCOPED_INDENT_NAMED(x)				_INTERNAL_CMLOG_INDENT_NAMED(ScopedLogIndent_,x)
#define CMLOG_SCOPED_INDENT()						CMLOG_SCOPED_INDENT_NAMED( __LINE__ )

#define _INTERNAL_CMLOG_NAME_NAMED(var, var2, name)	CMLogger::CMLogScopedName var##var2 (name);
#define CMLOG_SCOPED_NAME_NAMED(var, name)			_INTERNAL_CMLOG_NAME_NAMED(CMLogName_,var, name)
#define CMLOG_SCOPED_NAME(name)						CMLOG_SCOPED_NAME_NAMED(__LINE__, name)
//#define CMLOG_NAME(name)							CMLogger::CMLogScopedName CMLogName (name);

#define CMLOG_GET_NAME ((const CMString)CMLogger::LogCat)

#define CMLOG(...) CMLogger::LogMessage(__VA_ARGS__);
#define CMLOG_LIT(Message, ...) CMLogger::LogMessage("%s\n", #Message, __VA_ARGS__);
#define CMLOG_LINE(Message) CMLogger::LogMessage("(%s #%i) %s", __FUNCTION__, __LINE__, Message);
