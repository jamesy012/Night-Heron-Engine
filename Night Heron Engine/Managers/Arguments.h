#pragma once

#include "Types.h"

extern class Arguments* _CArguments;

class Arguments {
public:
	void Generate(CMString a_Arguments);
	CMString GetArgumentValue(CMString a_Argument);
	bool IsArgument(CMString a_Argument);
private:
	struct Argument {
		CMString m_Arg;
		CMString m_Value;
	};

	Argument* GetArgument(CMString a_Argument);

	CMArray<Argument> m_Arguments;
};